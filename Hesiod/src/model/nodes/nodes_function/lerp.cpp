/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/math.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{
// -----------------------------------------------------------------------------
// Ports & Attributes
// -----------------------------------------------------------------------------

constexpr const char *P_A = "a";
constexpr const char *P_B = "b";
constexpr const char *P_T = "t";
constexpr const char *P_OUT = "output";

constexpr const char *A_T = "t";
constexpr const char *A_SWAP_INPUTS = "swap_inputs";

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void setup_lerp_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_A);
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_B);
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, P_T);
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, P_OUT, CONFIG(node));

  // attribute(s)
  node.add_attr<FloatAttribute>(A_T, "Blend Factor 't' (a -> b)", 0.5f, 0.f, 1.f);
  node.add_attr<BoolAttribute>(A_SWAP_INPUTS, "Swap Inputs", false);

  // attribute(s) order
  node.set_attr_ordered_key({A_T, A_SWAP_INPUTS});
}

// -----------------------------------------------------------------------------
// Compute
// -----------------------------------------------------------------------------

void compute_lerp_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  auto *p_a = node.get_value_ref<hmap::VirtualArray>(P_A);
  auto *p_b = node.get_value_ref<hmap::VirtualArray>(P_B);
  auto *p_t = node.get_value_ref<hmap::VirtualArray>(P_T);
  auto *p_out = node.get_value_ref<hmap::VirtualArray>(P_OUT);

  if (!p_a || !p_b)
    return;

  // adjust inputs
  if (node.get_attr<BoolAttribute>(A_SWAP_INPUTS))
    std::swap(p_a, p_b);

  // compute output
  hmap::for_each_tile(
      {p_a, p_b, p_t},
      {p_out},
      [&node](std::vector<const hmap::Array *> p_arrays_in,
              std::vector<hmap::Array *>       p_arrays_out,
              const hmap::TileRegion &)
      {
        auto [pa_a, pa_b, pa_t] = unpack<3>(p_arrays_in);
        auto [pa_out] = unpack<1>(p_arrays_out);

        if (pa_t)
          *pa_out = hmap::lerp(*pa_a, *pa_b, *pa_t);
        else
          *pa_out = hmap::lerp(*pa_a, *pa_b, node.get_attr<FloatAttribute>(P_T));
      },
      node.cfg().cm_cpu);
}

} // namespace hesiod
