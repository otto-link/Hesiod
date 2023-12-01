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
      ImGuiFileDialog::Instance()->OpenDialog("DialogId",
                                              "Choose File",
                                              ".png,.raw,.bin",
                                              ".");

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

} // namespace hesiod
