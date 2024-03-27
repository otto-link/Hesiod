/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QSpinBox>
#include <QWidget>

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

void create_attribute_widget(QGridLayout                *layout,
                             std::unique_ptr<Attribute> &attr,
                             std::string                 label)
{
  switch (attr->get_type())
  {
  case AttributeType::SEED:
  {
    SeedAttribute *p_attr = attr.get()->get_ref<SeedAttribute>();

    QSpinBox *spinbox = new QSpinBox();

    layout->addWidget(spinbox, 0, 0);

    // ImGui::DragInt(label.c_str(), &p_attr->value);
    // has_changed |= ImGui::IsItemDeactivatedAfterEdit();

    // ImGui::SameLine();

    // if (ImGui::Button("Rnd"))
    // {
    //   p_attr->value = (int)time(NULL);
    //   has_changed = true;
    // }
  }
  break;
  }
}

} // namespace hesiod
