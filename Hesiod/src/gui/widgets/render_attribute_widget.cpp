/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/widgets.hpp"

namespace hesiod::gui
{

bool render_attribute_widget(std::unique_ptr<Attribute> &attr,
                             std::string                 label)
{
  bool has_changed = false;

  switch (attr->get_type())
  {

  case AttributeType::BOOL_ATTRIBUTE:
  {
    BoolAttribute *p_attr = attr.get()->get_ref<BoolAttribute>();
    has_changed |= ImGui::Checkbox(label.c_str(), &p_attr->value);
  }
  break;

  case AttributeType::CLOUD_ATTRIBUTE:
  {
    CloudAttribute *p_attr = attr.get()->get_ref<CloudAttribute>();
    ImGui::TextUnformatted(label.c_str());
    has_changed |= hesiod::gui::hmap_cloud_editor(p_attr->value);
  }
  break;

  case AttributeType::COLOR_ATTRIBUTE:
  {
    ColorAttribute *p_attr = attr.get()->get_ref<ColorAttribute>();
    ImGui::ColorEdit3(label.c_str(), p_attr->value.data());
    has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  }
  break;

  case AttributeType::FILENAME_ATTRIBUTE:
  {
    FilenameAttribute *p_attr = attr.get()->get_ref<FilenameAttribute>();

    ImGui::TextUnformatted(label.c_str());
    {
      if (ImGui::Button(("File: " + p_attr->value).c_str()))
      {
        IGFD::FileDialogConfig config;
        config.path = ".";

        ImGuiFileDialog::Instance()->OpenDialog("DialogId",
                                                "Choose File",
                                                ".png,.raw,.bin",
                                                config);
      }

      if (ImGuiFileDialog::Instance()->Display("DialogId"))
      {
        if (ImGuiFileDialog::Instance()->IsOk())
          p_attr->value = ImGuiFileDialog::Instance()->GetFilePathName(
              IGFD_ResultMode_AddIfNoFileExt);
        ImGuiFileDialog::Instance()->Close();
      }
    }
  }
  break;

  case AttributeType::FLOAT_ATTRIBUTE:
  {
    FloatAttribute *p_attr = attr.get()->get_ref<FloatAttribute>();
    ImGui::SliderFloat(label.c_str(),
                       &p_attr->value,
                       p_attr->vmin,
                       p_attr->vmax,
                       p_attr->fmt.c_str());
    has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  }
  break;
  //
  case AttributeType::INT_ATTRIBUTE:
  {
    IntAttribute *p_attr = attr.get()->get_ref<IntAttribute>();
    ImGui::SliderInt(label.c_str(), &p_attr->value, p_attr->vmin, p_attr->vmax);
    has_changed |= ImGui::IsItemDeactivatedAfterEdit();
  }
  break;

  case AttributeType::MAP_ENUM_ATTRIBUTE:
  {
    MapEnumAttribute *p_attr = attr.get()->get_ref<MapEnumAttribute>();

    int choice_index = p_attr->value.at(p_attr->choice);

    has_changed |= hesiod::gui::listbox_map_enum(p_attr->value,
                                                 choice_index,
                                                 128.f);

    // reverse map to get the key (string) based on the value chosen (int)
    for (auto &[k, v] : p_attr->value)
      if (v == choice_index)
      {
        p_attr->choice = k;
        break;
      }
  }
  break;

  case AttributeType::MATRIX_ATTRIBUTE:
  {
    MatrixAttribute *p_attr = attr.get()->get_ref<MatrixAttribute>();
    ImGui::TextUnformatted(label.c_str());
    has_changed |= hesiod::gui::drag_float_matrix(p_attr->value);
  }
  break;

  case AttributeType::PATH_ATTRIBUTE:
  {
    PathAttribute *p_attr = attr.get()->get_ref<PathAttribute>();
    ImGui::TextUnformatted(label.c_str());

    ImGui::Checkbox("closed", &p_attr->value.closed);
    has_changed |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SameLine();

    if (ImGui::Button("reverse"))
    {
      p_attr->value.reverse();
      has_changed = true;
    }

    has_changed |= hesiod::gui::hmap_path_editor(p_attr->value);
  }
  break;

  case AttributeType::RANGE_ATTRIBUTE:
  {
    RangeAttribute *p_attr = attr.get()->get_ref<RangeAttribute>();
    if (ImGui::Checkbox(label.c_str(), &p_attr->activate))
      has_changed = true;

    // if (p_attr->activate)
    {
      ImGui::Indent();
      if (hesiod::gui::slider_vmin_vmax(p_attr->value.x, p_attr->value.y))
        has_changed = true;
      ImGui::Unindent();
    }
  }
  break;

  case AttributeType::SEED_ATTRIBUTE:
  {
    SeedAttribute *p_attr = attr.get()->get_ref<SeedAttribute>();
    ImGui::DragInt(label.c_str(), &p_attr->value);
    has_changed |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SameLine();

    if (ImGui::Button("Rnd"))
    {
      p_attr->value = (int)time(NULL);
      has_changed = true;
    }
  }
  break;

  case AttributeType::SHAPE_ATTRIBUTE:
  {
    ShapeAttribute *p_attr = attr.get()->get_ref<ShapeAttribute>();
    has_changed |= hesiod::gui::select_shape(label.c_str(),
                                             p_attr->value,
                                             p_attr->value_max);
  }
  break;

  case AttributeType::VEC_FLOAT_ATTRIBUTE:
  {
    VecFloatAttribute *p_attr = attr.get()->get_ref<VecFloatAttribute>();
    ImGui::TextUnformatted(label.c_str());
    ImGui::Indent();
    has_changed |=
        hesiod::gui::drag_float_vector(p_attr->value, true, true, 0.f, 1.f);
    ImGui::Unindent();
  }
  break;

  case AttributeType::VEC_INT_ATTRIBUTE:
  {
    // VecIntAttribute *p_attr = attr.get()->get_ref<VecIntAttribute>();
    ImGui::TextUnformatted(label.c_str());
    // ImGui::Indent();
    // // has_changed |=
    // //     hesiod::gui::drag_int_vector(p_attr->value, true, true, 0.f, 1.f);
    // ImGui::Unindent();
  }
  break;

  case AttributeType::WAVE_NB_ATTRIBUTE:
  {
    WaveNbAttribute *p_attr = attr.get()->get_ref<WaveNbAttribute>();
    has_changed |= hesiod::gui::drag_float_vec2(p_attr->value, p_attr->link_xy);
  }
  break;

  case AttributeType::INVALID:
    LOG_ERROR("Encountered unknown type! (%i)",
              static_cast<int>(attr->get_type()));
    return false;
  }

  return has_changed;
}

} // namespace hesiod::gui
