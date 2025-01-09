/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/erosion.hpp"
#include "highmap/multiscale/downscaling.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_hydraulic_particle_node(BaseNode *p_node)
{
  LOG->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "input");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "bedrock");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "moisture");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::IN, "mask");
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "output", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "erosion", CONFIG);
  p_node->add_port<hmap::Heightmap>(gnode::PortType::OUT, "deposition", CONFIG);

  // attribute(s)
  p_node->add_attr<SeedAttribute>("seed");
  p_node->add_attr<FloatAttribute>("particle_density",
                                   0.4f,
                                   0.f,
                                   2.f,
                                   "particle_density");
  p_node->add_attr<FloatAttribute>("c_capacity", 10.f, 0.1f, 100.f, "c_capacity");
  p_node->add_attr<FloatAttribute>("c_erosion", 0.05f, 0.f, 0.2f, "c_erosion");
  p_node->add_attr<FloatAttribute>("c_deposition", 0.1f, 0.f, 0.1f, "c_deposition");
  p_node->add_attr<FloatAttribute>("drag_rate", 0.001f, 0.f, 1.f, "drag_rate");
  p_node->add_attr<FloatAttribute>("evap_rate", 0.001f, 0.f, 1.f, "evap_rate");
  p_node->add_attr<BoolAttribute>("post-filtering", true, "post-filtering");
  p_node->add_attr<BoolAttribute>("deposition_only", false, "deposition_only");
  p_node->add_attr<BoolAttribute>("GPU", HSD_DEFAULT_GPU_MODE, "GPU");

  p_node->add_attr<BoolAttribute>("downscale", true, "downscale");
  p_node->add_attr<FloatAttribute>("kc", 512.f, 0.f, 2048.f, "kc");

  // attribute(s) order
  p_node->set_attr_ordered_key({"seed",
                                "particle_density",
                                "c_capacity",
                                "c_erosion",
                                "c_deposition",
                                "drag_rate",
                                "evap_rate",
                                "post-filtering",
                                "deposition_only",
                                "_SEPARATOR_",
                                "downscale",
                                "kc",
                                "_SEPARATOR_",
                                "GPU"});
}

void compute_hydraulic_particle_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  LOG->trace("computing node {}", p_node->get_label());

  hmap::Heightmap *p_in = p_node->get_value_ref<hmap::Heightmap>("input");

  if (p_in)
  {
    hmap::Heightmap *p_bedrock = p_node->get_value_ref<hmap::Heightmap>("bedrock");
    hmap::Heightmap *p_moisture_map = p_node->get_value_ref<hmap::Heightmap>("moisture");
    hmap::Heightmap *p_mask = p_node->get_value_ref<hmap::Heightmap>("mask");

    hmap::Heightmap *p_out = p_node->get_value_ref<hmap::Heightmap>("output");
    hmap::Heightmap *p_erosion_map = p_node->get_value_ref<hmap::Heightmap>("erosion");
    hmap::Heightmap *p_deposition_map = p_node->get_value_ref<hmap::Heightmap>(
        "deposition");

    // copy the input heightmap
    *p_out = *p_in;

    // set the bedrock at the input heightmap to prevent any erosion,
    // if requested
    if (GET("deposition_only", BoolAttribute))
      p_bedrock = p_in;

    // number of particles based on the input particle density
    int nparticles = (int)(GET("particle_density", FloatAttribute) * p_out->shape.x *
                           p_out->shape.y);

    int nparticles_tile = (int)(nparticles / (float)p_out->get_ntiles());

    if (GET("GPU", BoolAttribute))
    {
      if (!GET("downscale", BoolAttribute))
      {
        hmap::transform(
            {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
            [p_node, &nparticles_tile](std::vector<hmap::Array *> p_arrays,
                                       hmap::Vec2<int>,
                                       hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_bedrock = p_arrays[1];
              hmap::Array *pa_moisture_map = p_arrays[2];
              hmap::Array *pa_mask = p_arrays[3];
              hmap::Array *pa_erosion_map = p_arrays[4];
              hmap::Array *pa_deposition_map = p_arrays[5];

              hmap::gpu::hydraulic_particle(*pa_out,
                                            pa_mask,
                                            nparticles_tile,
                                            GET("seed", SeedAttribute),
                                            pa_bedrock,
                                            pa_moisture_map,
                                            pa_erosion_map,
                                            pa_deposition_map,
                                            GET("c_capacity", FloatAttribute),
                                            GET("c_erosion", FloatAttribute),
                                            GET("c_deposition", FloatAttribute),
                                            GET("drag_rate", FloatAttribute),
                                            GET("evap_rate", FloatAttribute),
                                            GET("post-filtering", BoolAttribute));
            },
            hmap::TransformMode::DISTRIBUTED);
      }
      else
      {
        // adjust particle counts to the new working resolution
        nparticles = (int)(GET("kc", FloatAttribute) / p_out->shape.x * nparticles);

        hmap::transform(
            {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
            [p_node, nparticles](std::vector<hmap::Array *> p_arrays,
                                 hmap::Vec2<int>            shape,
                                 hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_bedrock = p_arrays[1];
              hmap::Array *pa_moisture_map = p_arrays[2];
              hmap::Array *pa_mask = p_arrays[3];
              hmap::Array *pa_erosion_map = p_arrays[4];
              hmap::Array *pa_deposition_map = p_arrays[5];

              auto lambda = [p_node,
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
                    coarse_arrays.push_back(
                        std::move(pa->resample_to_shape(shape_coarse)));
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

                hmap::gpu::hydraulic_particle(x,
                                              p_coarse_arrays[0], // mask
                                              nparticles,
                                              GET("seed", SeedAttribute),
                                              p_coarse_arrays[1], // bedrock
                                              p_coarse_arrays[2], // moisture
                                              p_coarse_arrays[3], // ero map
                                              p_coarse_arrays[4], // depo map
                                              GET("c_capacity", FloatAttribute),
                                              GET("c_erosion", FloatAttribute),
                                              GET("c_deposition", FloatAttribute),
                                              GET("drag_rate", FloatAttribute),
                                              GET("evap_rate", FloatAttribute),
                                              GET("post-filtering", BoolAttribute));

                // resample output fields to their original shape
                if (pa_erosion_map)
                  *pa_erosion_map = coarse_arrays[3].resample_to_shape_bicubic(shape);
                if (pa_deposition_map)
                  *pa_deposition_map = coarse_arrays[4].resample_to_shape_bicubic(shape);
              };

              hmap::downscale_transform(*pa_out, GET("kc", FloatAttribute), lambda);
            },
            hmap::TransformMode::SINGLE_ARRAY);
      }
    }
    else
    {
      if (!GET("downscale", BoolAttribute))
      {
        hmap::transform(
            {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
            [p_node, &nparticles_tile](std::vector<hmap::Array *> p_arrays,
                                       hmap::Vec2<int>,
                                       hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_bedrock = p_arrays[1];
              hmap::Array *pa_moisture_map = p_arrays[2];
              hmap::Array *pa_mask = p_arrays[3];
              hmap::Array *pa_erosion_map = p_arrays[4];
              hmap::Array *pa_deposition_map = p_arrays[5];

              hmap::hydraulic_particle(*pa_out,
                                       pa_mask,
                                       nparticles_tile,
                                       GET("seed", SeedAttribute),
                                       pa_bedrock,
                                       pa_moisture_map,
                                       pa_erosion_map,
                                       pa_deposition_map,
                                       GET("c_capacity", FloatAttribute),
                                       GET("c_erosion", FloatAttribute),
                                       GET("c_deposition", FloatAttribute),
                                       GET("drag_rate", FloatAttribute),
                                       GET("evap_rate", FloatAttribute),
                                       GET("post-filtering", BoolAttribute));
            },
            hmap::TransformMode::DISTRIBUTED);
      }
      else
      {
        // adjust particle counts to the new working resolution
        nparticles = (int)(GET("kc", FloatAttribute) / p_out->shape.x * nparticles);

        hmap::transform(
            {p_out, p_bedrock, p_moisture_map, p_mask, p_erosion_map, p_deposition_map},
            [p_node, nparticles](std::vector<hmap::Array *> p_arrays,
                                 hmap::Vec2<int>            shape,
                                 hmap::Vec4<float>)
            {
              hmap::Array *pa_out = p_arrays[0];
              hmap::Array *pa_bedrock = p_arrays[1];
              hmap::Array *pa_moisture_map = p_arrays[2];
              hmap::Array *pa_mask = p_arrays[3];
              hmap::Array *pa_erosion_map = p_arrays[4];
              hmap::Array *pa_deposition_map = p_arrays[5];

              auto lambda = [p_node,
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
                    coarse_arrays.push_back(
                        std::move(pa->resample_to_shape(shape_coarse)));
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

                hmap::hydraulic_particle(x,
                                         p_coarse_arrays[0], // mask
                                         nparticles,
                                         GET("seed", SeedAttribute),
                                         p_coarse_arrays[1], // bedrock
                                         p_coarse_arrays[2], // moisture
                                         p_coarse_arrays[3], // ero map
                                         p_coarse_arrays[4], // depo map
                                         GET("c_capacity", FloatAttribute),
                                         GET("c_erosion", FloatAttribute),
                                         GET("c_deposition", FloatAttribute),
                                         GET("drag_rate", FloatAttribute),
                                         GET("evap_rate", FloatAttribute),
                                         GET("post-filtering", BoolAttribute));

                // resample output fields to their original shape
                if (pa_erosion_map)
                  *pa_erosion_map = coarse_arrays[3].resample_to_shape_bicubic(shape);
                if (pa_deposition_map)
                  *pa_deposition_map = coarse_arrays[4].resample_to_shape_bicubic(shape);
              };

              hmap::downscale_transform(*pa_out, GET("kc", FloatAttribute), lambda);
            },
            hmap::TransformMode::SINGLE_ARRAY);
      }
    }

    p_out->smooth_overlap_buffers();

    if (p_erosion_map)
    {
      p_erosion_map->smooth_overlap_buffers();
      p_erosion_map->remap();
    }

    if (p_deposition_map)
    {
      p_deposition_map->smooth_overlap_buffers();
      p_deposition_map->remap();
    }
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
