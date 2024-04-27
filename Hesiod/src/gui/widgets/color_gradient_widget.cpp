/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <format>

#include <QColorDialog>
#include <QGridLayout>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

ColorGradientWidget::ColorGradientWidget(ColorGradientAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("ColorGradient attribute");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;
  int col = 0;

  this->colorbar = new ColorbarWidget(this);
  this->colorbar->update_colors(p_attr->value);
  layout->addWidget(this->colorbar, row, 0, 1, 2);
  row++;

  QPushButton *add_button = new QPushButton("Add color", this);
  QPushButton *remove_button = new QPushButton("Remove selected", this);

  connect(add_button, &QPushButton::clicked, this, &ColorGradientWidget::add_color);
  connect(remove_button, &QPushButton::clicked, this, &ColorGradientWidget::remove_color);

  layout->addWidget(add_button, row, 0);
  layout->addWidget(remove_button, row, 1);
  row++;

  this->color_list = new QListWidget(this);
  this->update_color_list();

  connect(this->color_list,
          &QListWidget::itemDoubleClicked,
          this,
          &ColorGradientWidget::on_item_double_click);

  layout->addWidget(this->color_list, row, 0, 1, 2);

  this->setLayout(layout);
}

void ColorGradientWidget::add_color()
{
  QColorDialog color_dialog;
  color_dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  QColor qcolor = color_dialog.getColor();

  std::vector<float> color = {1.f,
                              (float)qcolor.redF(),
                              (float)qcolor.greenF(),
                              (float)qcolor.blueF(),
                              (float)qcolor.alphaF()};

  // insert new color next to the currently selected color (if any),
  // else put it at the end
  QListWidgetItem *selectedItem = this->color_list->currentItem();

  int ncolors = this->p_attr->value.size();
  int selected_row = ncolors - 1;

  if (selectedItem)
    selected_row = this->color_list->row(selectedItem);

  if (ncolors == 0)
  {
    color[0] = 0.f;
    this->p_attr->value.push_back(color);
  }
  else
  {
    if (selected_row == ncolors - 1 || ncolors == 1)
    {
      // rescale positions according to the new number of colors
      // and append at the end
      for (auto &data : this->p_attr->value)
        data[0] *= (float)(1.f / (1.f / ncolors + 1.f));
      color[0] = 1.f;
      this->p_attr->value.push_back(color);
    }
    else
    {
      // insert in-between two existing colors
      float pos = 0.5f * (this->p_attr->value[selected_row + 1][0] +
                          this->p_attr->value[selected_row][0]);
      color[0] = pos;

      this->p_attr->value.insert(this->p_attr->value.begin() + selected_row + 1, color);
    }
  }

  this->colorbar->update_colors(this->p_attr->value);
  this->update_color_list();
}

void ColorGradientWidget::on_item_double_click(QListWidgetItem *item)
{
  QColorDialog color_dialog;
  color_dialog.setOption(QColorDialog::ShowAlphaChannel, true);
  QColor qcolor = color_dialog.getColor();

  // TODO detect "Cancel" button

  std::vector<float> color = {0.f,
                              (float)qcolor.redF(),
                              (float)qcolor.greenF(),
                              (float)qcolor.blueF(),
                              (float)qcolor.alphaF()};

  // retrieve corresponding widget
  ColorbarWidget *colorbar_item = dynamic_cast<ColorbarWidget *>(
      this->color_list->itemWidget(item));

  float pos = colorbar_item->get_colors().front()[0];

  colorbar_item->update_colors({{pos, color[1], color[2], color[3], color[4]}});
  this->update();
}

void ColorGradientWidget::remove_color()
{
  QListWidgetItem *selected_item = this->color_list->currentItem();
  delete selected_item;
  this->update();
}

void ColorGradientWidget::update()
{
  this->update_attribute();
  this->colorbar->update_colors(this->p_attr->value);
  this->update_color_list();
}

void ColorGradientWidget::update_attribute()
{
  // update attribute value
  this->p_attr->value.clear();

  for (int i = 0; i < this->color_list->count(); ++i)
  {
    QListWidgetItem *item = this->color_list->item(i);
    if (item)
    {
      // retrieve corresponding widget
      ColorbarWidget *colorbar_item = dynamic_cast<ColorbarWidget *>(
          this->color_list->itemWidget(item));

      std::vector<float> color = colorbar_item->get_colors().front();
      this->p_attr->value.push_back(color);
    }
  }

  Q_EMIT this->value_changed();
}

void ColorGradientWidget::update_color_list()
{
  this->color_list->clear();

  for (auto &c : this->p_attr->value)
  {
    QListWidgetItem *item = new QListWidgetItem(this->color_list);
    ColorbarWidget  *cbar = new ColorbarWidget({c});
    this->color_list->setItemWidget(item, cbar);
  }
}

} // namespace hesiod
