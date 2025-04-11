/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QLabel>

#include "highmap/colorize.hpp"

#include "hesiod/gui/widgets/graph_manager_widget.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/broadcast_param.hpp"
#include "hesiod/model/graph_node.hpp"

namespace hesiod
{

GraphQListWidget::GraphQListWidget(GraphNode *p_graph_node, QWidget *parent)
    : QWidget(parent), p_graph_node(p_graph_node)
{
  LOG->trace("GraphQListWidget::GraphQListWidget");

  QGridLayout *layout = new QGridLayout(this);
  this->setLayout(layout);

  QLabel *label = new QLabel(p_graph_node->get_id().c_str());
  layout->addWidget(label, 0, 0);

  this->combobox = new QComboBox(this);
  this->current_bg_tag = "NO BACKGROUND";
  this->update_combobox();
  layout->addWidget(this->combobox, 0, 1);

  this->connect(this->combobox,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                this,
                &GraphQListWidget::on_combobox_changed);
}

void GraphQListWidget::json_from(nlohmann::json const &json)
{
  this->current_bg_tag = json["current_bg_tag"].get<std::string>();
  this->combobox->setCurrentText(this->current_bg_tag.c_str());
}

nlohmann::json GraphQListWidget::json_to() const
{
  nlohmann::json json;
  json["current_bg_tag"] = this->current_bg_tag;
  return json;
}

void GraphQListWidget::on_combobox_changed()
{
  LOG->trace("GraphQListWidget::on_combobox_changed: current tag: {}",
             this->current_bg_tag);

  this->current_bg_tag = this->combobox->currentText().toStdString();

  // retrieve node ID from tag
  std::string graph_id, node_id, port_id;
  bool ret = get_ids_from_broadcast_tag(this->current_bg_tag, graph_id, node_id, port_id);

  if (!ret)
  {
    Q_EMIT this->bg_image_updated(this->p_graph_node->get_id(), QImage());
    return;
  }

  LOG->trace("GraphQListWidget::on_combobox_changed: node_id={}, port_id={}",
             node_id,
             port_id);

  // TODO allow other data types (see data_preview)

  // generate background image
  QImage image;

  hmap::Heightmap *p_h = this->p_graph_node->get_node_ref_by_id(node_id)
                             ->get_value_ref<hmap::Heightmap>(port_id);

  if (p_h)
  {
    hmap::Array          array = p_h->to_array();
    std::vector<uint8_t> img = hmap::colorize(array,
                                              array.min(),
                                              array.max(),
                                              hmap::Cmap::TURBO,
                                              true)
                                   .to_img_8bit();
    image = QImage(img.data(), p_h->shape.x, p_h->shape.y, QImage::Format_RGB888);
  }
  else
  {
    // null image
    image = QImage();
  }

  Q_EMIT this->bg_image_updated(this->p_graph_node->get_id(), image);
}

void GraphQListWidget::update_combobox()
{
  // backup current tag selection (modified by itemChanged signal of
  // combobox)
  const std::string tag_bckp = this->current_bg_tag;

  this->combobox->clear();
  this->combobox->addItem("NO BACKGROUND");

  LOG->trace("GraphQListWidget::update_combobox, tag: {}", tag_bckp);

  // list of possible data available to show on the canvas as Pixmap
  bool is_current_tag_in_combo = false;

  for (auto &[id, node] : this->p_graph_node->get_nodes())
    for (auto &port : node->get_ports())
    {
      LOG->trace("GraphQListWidget::update_combobox: {}/{}",
                 node->get_label(),
                 port->get_label());

      // TODO allow other data types

      // only allow heighmap (for now)
      if (port->get_data_type() == typeid(hmap::Heightmap).name())
      {
        const std::string tag = node->get_label() + "/" + node->get_id() + "/" +
                                port->get_label();
        this->combobox->addItem(tag.c_str());

        if (tag_bckp == tag)
          is_current_tag_in_combo = true;
      }
    }

  // set selection only if it is still available
  if (is_current_tag_in_combo)
    this->combobox->setCurrentText(tag_bckp.c_str());

  // for image update
  this->on_combobox_changed();
}

} // namespace hesiod
