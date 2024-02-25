/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "ImGuiFileDialog.h"
#include "highmap.hpp"
#include "macrologger.h"
#include <imgui.h>

#include "hesiod/attribute.hpp"
#include "hesiod/gui.hpp"

namespace hesiod
{

FilenameAttribute::FilenameAttribute(std::string value) : value(value)
{
}

std::string FilenameAttribute::get()
{
  return value;
}

bool FilenameAttribute::render_settings(std::string label)
{
  bool has_changed = false;

  ImGui::TextUnformatted(label.c_str());
  {
    if (ImGui::Button(("File: " + this->value).c_str()))
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
        this->value = ImGuiFileDialog::Instance()->GetFilePathName(
            IGFD_ResultMode_AddIfNoFileExt);
      ImGuiFileDialog::Instance()->Close();
    }
  }
  return has_changed;
}

bool FilenameAttribute::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{ 
  outputData[fieldName] = this->value;
  return true; 
}

bool FilenameAttribute::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) 
{ 
  if(inputData[fieldName].is_string() == false)
  {
    LOG_DEBUG("[%s] Attribute %s is not a string.", __FUNCTION__, fieldName.data());
    return false;
  }

  this->value = inputData[fieldName].get<std::string>();
  return true; 
}

} // namespace hesiod
