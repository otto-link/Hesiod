/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <format>

#include <QColorDialog>
#include <QGridLayout>

#include "highmap/op.hpp"

#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/cmap.hpp"

namespace hesiod
{

// helper
void set_focus(QListWidget *list, QListWidgetItem *item, int border_width)
{
  ColorbarWidget *colorbar = dynamic_cast<ColorbarWidget *>(list->itemWidget(item));
  colorbar->set_border(border_width, Qt::white);
}

// class method

ColormapEnumWidget::ColormapEnumWidget(MapEnumAttribute *p_attr) : p_attr(p_attr)
{
  this->setWindowTitle("ColormapEnum attribute");

  QGridLayout *layout = new QGridLayout(this);

  this->cmap_list = new QListWidget(this);
  int count = 0;

  for (auto &[key, cmap_idx] : this->p_attr->value)
  {
    // backup enum choice corresponding to each list item
    this->item_choice.push_back(key);

    // create and add colorbar widget as item widget
    std::vector<std::vector<float>> colormap_colors = hesiod::get_colormap_data(cmap_idx);
    std::vector<float> t = hmap::linspace(0.f, 1.f, (int)colormap_colors.size());

    std::vector<std::vector<float>> gradient = {};

    for (size_t k = 0; k < colormap_colors.size(); k++)
      gradient.push_back({t[k],
                          colormap_colors[k][0],
                          colormap_colors[k][1],
                          colormap_colors[k][2],
                          1.f});

    QListWidgetItem *item = new QListWidgetItem(this->cmap_list);
    ColorbarWidget  *cbar = new ColorbarWidget(gradient, QString::fromStdString(key));
    this->cmap_list->setItemWidget(item, cbar);

    // identify current choice
    if (key == this->p_attr->choice)
    {
      this->current_choice_item_idx = count;
      set_focus(this->cmap_list, this->cmap_list->item(count), 6);
    }
    count++;
  }

  connect(this->cmap_list,
          &QListWidget::itemClicked,
          this,
          &ColormapEnumWidget::on_item_click);

  layout->addWidget(this->cmap_list, 0, 0);

  this->setLayout(layout);
}

void ColormapEnumWidget::on_item_click(QListWidgetItem *item)
{

  // revert previous item borders to normal
  set_focus(this->cmap_list, this->cmap_list->item(this->current_choice_item_idx), 0);

  // emphasized current item
  int item_number = this->cmap_list->row(item);
  p_attr->choice = this->item_choice[item_number];

  set_focus(this->cmap_list, item, 6);

  // backup item row
  this->current_choice_item_idx = item_number;

  Q_EMIT this->value_changed();
}

} // namespace hesiod
