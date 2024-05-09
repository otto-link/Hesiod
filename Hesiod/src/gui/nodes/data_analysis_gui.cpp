/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

QWidget *DataAnalysis::embeddedWidget()
{
  if (!this->label)
  {
    this->label = new QLabel();
    this->label->setMinimumSize(128, 128);
    this->label->setMaximumSize(128, 128);
    this->label->setStyleSheet("background-color: #3B3B3B; color: #DFE1E2");
    this->label->setAlignment(Qt::AlignTop);
    this->label->setWordWrap(true);

    QFont font = this->label->font();
    font.setPointSize(font.pointSize() - 3);
    this->label->setFont(font);

    this->update_widget_content();

    connect(this,
            &BaseNode::computingFinished,
            [this]() { this->update_widget_content(); });
  }
  return (QWidget *)this->label;
}

void DataAnalysis::update_widget_content()
{
  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    std::string text = "";

    text += "shape: {" + std::to_string(p_in->shape.x) + ", " +
            std::to_string(p_in->shape.y) + "}\n";
    text += "tiling: {" + std::to_string(p_in->tiling.x) + ", " +
            std::to_string(p_in->tiling.y) + "}\n";
    text += "overlap: " + std::to_string(p_in->overlap) + "\n";
    text += "min: " + std::to_string(p_in->min()) + "\n";
    text += "max: " + std::to_string(p_in->max()) + "\n";

    this->label->setText(text.c_str());
  }
  else
    this->label->setText("no input");
}

} // namespace hesiod
