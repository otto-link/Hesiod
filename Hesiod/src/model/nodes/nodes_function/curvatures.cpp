/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/curvature.hpp"
#include "highmap/opencl/gpu_opencl.hpp"
#include "highmap/range.hpp"

#include "attributes.hpp"

#include "hesiod/app/enum_mappings.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{
// -----------------------------------------------------------------------------
// Ports & Attributes
// -----------------------------------------------------------------------------

constexpr const char *P_IN = "input";
constexpr const char *P_OUT = "mask";

constexpr const char *A_RADIUS = "radius";
constexpr const char *A_CTYPE = "ctype";
constexpr const char *A_CLAMPING = "clamping";
constexpr const char *A_SATMAX = "satmax";

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup_curvatures_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_IN);
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, P_OUT, CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>(A_RADIUS, "Radius", 0.f, 0.f, 0.5f);
  node.add_attr<EnumAttribute>(A_CTYPE,
                               "Curvature Type",
                               enum_mappings.curvature_type_map,
                               "Mean");
  std::vector<std::string> choices = {"Positive", "Negative", "Both"};
  node.add_attr<ChoiceAttribute>(A_CLAMPING, "Values Kept", choices);
  node.add_attr<FloatAttribute>(A_SATMAX, "Saturation Ratio", 2.f, 0.f, 20.f, "{:.0f}%");

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Metric Choice",
                             A_RADIUS,
                             A_CTYPE,
                             A_CLAMPING,
                             A_SATMAX,
                             "_GROUPBOX_END_"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = true});
}

// -----------------------------------------------------------------------------
// Compute
// -----------------------------------------------------------------------------

void compute_curvatures_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  auto *p_in = node.get_value_ref<hmap::VirtualArray>(P_IN);
  auto *p_out = node.get_value_ref<hmap::VirtualArray>(P_OUT);

  if (!p_in)
    return;

  // --- Params lambda

  const auto params = [&node, p_out]()
  {
    struct P
    {
      int                 ir;
      hmap::CurvatureType ctype;
      std::string         choice;
      bool                keep_both;
      float               satmin;
      float               satmax;
    };

    int ir = std::max(1, (int)(node.get_attr<FloatAttribute>(A_RADIUS) * p_out->shape.x));
    auto ctype = hmap::CurvatureType(node.get_attr<EnumAttribute>(A_CTYPE));

    std::string choice = node.get_attr<ChoiceAttribute>(A_CLAMPING);
    bool        keep_both = (choice == "Both");

    float sat_perc = 0.01f * node.get_attr<FloatAttribute>(A_SATMAX);

    // clang-format off
    return P{
      .ir = ir,
      .ctype = ctype,
      .choice = choice,
      .keep_both = keep_both,
      .satmin = sat_perc,
      .satmax = 1.f -  sat_perc
    };
    // clang-format on
  }();

  // --- Compute

  hmap::for_each_tile(
      {p_out, p_in},
      [&params](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
      {
        auto [pa_out, pa_in] = unpack<2>(p_arrays);
        *pa_out = hmap::gpu::curvature_quadric(*pa_in, params.ir, params.ctype);

        // if only one curvature sign is kept
        if (!params.keep_both)
        {
          if (params.choice == "Negative")
            *pa_out *= -1.f;

          hmap::clamp_min(*pa_out, 0.f);
        }
      },
      node.cfg().cm_gpu);

  // add percentile-based saturation
  if (params.satmin > 0.f)
  {
    glm::vec2 range_sat = p_out->range_percentile(params.satmin,
                                                  params.satmax,
                                                  node.cfg().cm_cpu);
    glm::vec2 range = p_out->range(node.cfg().cm_cpu);

    hmap::for_each_tile(
        {p_out},
        [range, range_sat](std::vector<hmap::Array *> p_arrays, const hmap::TileRegion &)
        {
          auto [pa_out] = unpack<1>(p_arrays);

          float k_smoothing = 0.1f * (range_sat.y - range_sat.x);

          hmap::saturate(*pa_out,
                         range_sat.x,
                         range_sat.y,
                         range.x,
                         range.y,
                         k_smoothing);
        },
        node.cfg().cm_cpu);
  }

  // post-process
  p_out->smooth_overlap_buffers();
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
