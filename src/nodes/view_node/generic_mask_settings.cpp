/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "imgui.h"
#include "macrologger.h"

#include "hesiod/control_node.hpp"
#include "hesiod/gui.hpp"
#include "hesiod/view_node.hpp"

namespace hesiod::vnode
{

bool render_settings_mask(hesiod::cnode::Mask *p_node)
{
  bool ret = false;

  if (ImGui::Checkbox("inverse", &p_node->inverse))
    ret = true;

  if (ImGui::Checkbox("smoothing", &p_node->smoothing))
    ret = true;

  if (p_node->smoothing)
  {
    ImGui::Indent();
    ImGui::SliderInt("ir_smoothing", &p_node->ir_smoothing, 1, 256);
    if (ImGui::IsItemDeactivatedAfterEdit())
      ret = true;
    ImGui::Unindent();
  }

  if (ImGui::Checkbox("saturate", &p_node->saturate))
    ret = true;

  if (p_node->saturate)
  {
    ImGui::Indent();
    if (hesiod::gui::slider_vmin_vmax(p_node->smin, p_node->smax))
      ret = true;

    ImGui::SliderFloat("k", &p_node->k, 0.01f, 1.f, "%.2f");
    if (ImGui::IsItemDeactivatedAfterEdit())
      ret = true;
    ImGui::Unindent();
  }

  if (ImGui::Checkbox("remap", &p_node->remap))
    ret = true;

  if (p_node->remap)
  {
    ImGui::Indent();
    if (hesiod::gui::slider_vmin_vmax(p_node->vmin, p_node->vmax))
      ret = true;
    ImGui::Unindent();
  }

  if (ret)
    p_node->force_update();

  return ret;
}

void serialize_load_settings_mask(hesiod::cnode::Mask      *p_node,
                                  cereal::JSONInputArchive &ar)
{
  ar(cereal::make_nvp("inverse", p_node->inverse));
  ar(cereal::make_nvp("smoothing", p_node->smoothing));
  ar(cereal::make_nvp("ir_smoothing", p_node->ir_smoothing));
  ar(cereal::make_nvp("saturate", p_node->saturate));
  ar(cereal::make_nvp("smin", p_node->smin));
  ar(cereal::make_nvp("smax", p_node->smax));
  ar(cereal::make_nvp("k", p_node->k));
  ar(cereal::make_nvp("remap", p_node->remap));
  ar(cereal::make_nvp("vmin", p_node->vmin));
  ar(cereal::make_nvp("vmax", p_node->vmax));
}

void serialize_save_settings_mask(hesiod::cnode::Mask       *p_node,
                                  cereal::JSONOutputArchive &ar)
{
  ar(cereal::make_nvp("inverse", p_node->inverse));
  ar(cereal::make_nvp("smoothing", p_node->smoothing));
  ar(cereal::make_nvp("ir_smoothing", p_node->ir_smoothing));
  ar(cereal::make_nvp("saturate", p_node->saturate));
  ar(cereal::make_nvp("smin", p_node->smin));
  ar(cereal::make_nvp("smax", p_node->smax));
  ar(cereal::make_nvp("k", p_node->k));
  ar(cereal::make_nvp("remap", p_node->remap));
  ar(cereal::make_nvp("vmin", p_node->vmin));
  ar(cereal::make_nvp("vmax", p_node->vmax));
}

} // namespace hesiod::vnode
