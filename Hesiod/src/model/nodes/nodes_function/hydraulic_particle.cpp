/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/filters.hpp"
#include "highmap/math.hpp"
#include "highmap/multiscale/downscaling.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_particle_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG(node));
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG(node));

  // attribute(s)
  node.add_attr<SeedAttribute>("seed", "seed");

  node.add_attr<FloatAttribute>("particle_density", "particle_density", 1.f, 0.f, 4.f);

  node.add_attr<FloatAttribute>("c_capacity", "c_capacity", 10.f, 0.1f, 100.f);

  node.add_attr<FloatAttribute>("c_erosion", "c_erosion", 0.05f, 0.f, 0.1f);

  node.add_attr<FloatAttribute>("c_deposition", "c_deposition", 0.005f, 0.f, 0.05f);

  node.add_attr<FloatAttribute>("c_inertia", "c_inertia", 0.3f, 0.01f, 2.f);

  node.add_attr<FloatAttribute>("drag_rate", "drag_rate", 0.001f, 0.f, 0.02f);

  node.add_attr<FloatAttribute>("evap_rate", "evap_rate", 0.001f, 0.f, 0.05f);

  node.add_attr<BoolAttribute>("post_filtering", "post_filtering", true);

  node.add_attr<BoolAttribute>("post_filtering_local", "post_filtering_local", false);

  node.add_attr<BoolAttribute>("deposition_only", "deposition_only", false);

  node.add_attr<BoolAttribute>("downscale", "downscale", false);

  node.add_attr<FloatAttribute>("kc", "kc", 512.f, 0.f, FLT_MAX);

  // attribute(s) order
  node.set_attr_ordered_key({"seed",
                             "particle_density",
                             "c_capacity",
                             "c_erosion",
                             "c_deposition",
                             "c_inertia",
                             "drag_rate",
                             "evap_rate",
                             "post_filtering",
                             "post_filtering_local",
                             "deposition_only",
                             "_SEPARATOR_",
                             "downscale",
                             "kc"});
}

void compute_hydraulic_particle_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = node.get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = node.get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = node.get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = node.get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = node.get_value_ref<hmap::Heightmap>("deposition");

    // copy the input heightmap
    *p_out = *p_in;

    // set the bedrock at the input heightmap to prevent any erosion,
    // if requested
    if (node.get_attr<BoolAttribute>("deposition_only"))
      p_bedrock = p_in;

    // number of particles based on the input particle density
    int nparticles = (int)(node.get_attr<FloatAttribute>("particle_density") *
                           p_out->shape.x * p_out->shape.y);

    if (!node.get_attr<BoolAttribute>("downscale"))
    {
      hmap::transform(
          {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
          [&node, &nparticles](std::vector<hmap::Array *> p_arrays)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_bedrock = p_arrays[1];
            hmap::Array *pa_moisture_map = p_arrays[2];
            hmap::Array *pa_mask = p_arrays[3];
            hmap::Array *pa_erosion_map = p_arrays[4];
            hmap::Array *pa_deposition_map = p_arrays[5];

            hmap::gpu::hydraulic_particle(*pa_out,
                                          pa_mask,
                                          nparticles,
                                          node.get_attr<SeedAttribute>("seed"),
                                          pa_bedrock,
                                          pa_moisture_map,
                                          pa_erosion_map,
                                          pa_deposition_map,
                                          node.get_attr<FloatAttribute>("c_capacity"),
                                          node.get_attr<FloatAttribute>("c_erosion"),
                                          node.get_attr<FloatAttribute>("c_deposition"),
                                          node.get_attr<FloatAttribute>("c_inertia"),
                                          node.get_attr<FloatAttribute>("drag_rate"),
                                          node.get_attr<FloatAttribute>("evap_rate"),
                                          node.get_attr<BoolAttribute>("post_filtering"));
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
    else
    {
      // adjust particle counts to the new working resolution
      nparticles = (int)(node.get_attr<FloatAttribute>("kc") / p_out->shape.x *
                         nparticles);

      hmap::transform(
          {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
          [&node, nparticles](std::vector<hmap::Array *> p_arrays,
                              hmap::Vec2<int>            shape,
                              hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_bedrock = p_arrays[1];
            hmap::Array *pa_moisture_map = p_arrays[2];
            hmap::Array *pa_mask = p_arrays[3];
            hmap::Array *pa_erosion_map = p_arrays[4];
            hmap::Array *pa_deposition_map = p_arrays[5];

            auto lambda = [&node,
                           shape,
                           nparticles,
                           pa_mask,
                           pa_bedrock,
                           pa_moisture_map,
                           pa_erosion_map,
                           pa_deposition_map](hmap::Array &x)
            {
              // downscaled shape
              hmap::Vec2<int> shape_coarse = x.shape;

              // generate coarse arrays if needed
              std::vector<hmap::Array>   coarse_arrays = {};
              std::vector<hmap::Array *> p_coarse_arrays = {};

              for (auto pa : {pa_mask,
                              pa_bedrock,
                              pa_moisture_map,
                              pa_erosion_map,
                              pa_deposition_map})
              {
                if (pa)
                  coarse_arrays.push_back(std::move(pa->resample_to_shape(shape_coarse)));
                else
                  coarse_arrays.push_back(hmap::Array());
              }

              int k = 0;
              for (auto pa : {pa_mask,
                              pa_bedrock,
                              pa_moisture_map,
                              pa_erosion_map,
                              pa_deposition_map})
              {
                if (pa)
                  p_coarse_arrays.push_back(&coarse_arrays[k]);
                else
                  p_coarse_arrays.push_back(nullptr);
                k++;
              }

              hmap::gpu::hydraulic_particle(
                  x,
                  p_coarse_arrays[0], // mask
                  nparticles,
                  node.get_attr<SeedAttribute>("seed"),
                  p_coarse_arrays[1], // bedrock
                  p_coarse_arrays[2], // moisture
                  p_coarse_arrays[3], // ero map
                  p_coarse_arrays[4], // depo map
                  node.get_attr<FloatAttribute>("c_capacity"),
                  node.get_attr<FloatAttribute>("c_erosion"),
                  node.get_attr<FloatAttribute>("c_deposition"),
                  node.get_attr<FloatAttribute>("c_inertia"),
                  node.get_attr<FloatAttribute>("drag_rate"),
                  node.get_attr<FloatAttribute>("evap_rate"),
                  node.get_attr<BoolAttribute>("post_filtering"));

              // resample output fields to their original shape
              if (pa_erosion_map)
                *pa_erosion_map = coarse_arrays[3].resample_to_shape_bicubic(shape);
              if (pa_deposition_map)
                *pa_deposition_map = coarse_arrays[4].resample_to_shape_bicubic(shape);
            };

            hmap::downscale_transform(*pa_out,
                                      node.get_attr<FloatAttribute>("kc"),
                                      lambda);
          },
          hmap::TransformMode::SINGLE_ARRAY);
    }

    p_out->smooth_overlap_buffers();

    p_erosion_map->smooth_overlap_buffers();
    p_erosion_map->remap();

    p_deposition_map->smooth_overlap_buffers();
    p_deposition_map->remap();

    // clean-up erosion and deposition surfaces
    if (node.get_attr<BoolAttribute>("post_filtering_local"))
    {
      hmap::transform(
          {p_out, p_erosion_map, p_deposition_map},
          [&node, nparticles](std::vector<hmap::Array *> p_arrays,
                              hmap::Vec2<int>,
                              hmap::Vec4<float>)
          {
            hmap::Array *pa_out = p_arrays[0];
            hmap::Array *pa_erosion_map = p_arrays[1];
            hmap::Array *pa_deposition_map = p_arrays[2];

            // TODO harcoded parameters
            hmap::Array mask = hmap::maximum_smooth(*pa_erosion_map,
                                                    *pa_deposition_map,
                                                    0.05f);
            mask = hmap::pow(mask, 0.5f);
            hmap::gpu::smooth_cpulse(mask, 2);
            hmap::gpu::smooth_cpulse(*pa_out, 32, &mask);
          },
          node.get_config_ref()->hmap_transform_mode_gpu);
    }
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
