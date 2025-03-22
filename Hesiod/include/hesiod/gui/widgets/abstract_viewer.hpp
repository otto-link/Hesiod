/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file abstract_viewer.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "nlohmann/json.hpp"

#include "hesiod/graph_editor.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

#define DEFAULT_VIEWER_WIDTH 512

namespace hesiod
{

class AbstractViewer : public QWidget
{
  Q_OBJECT

public:
  AbstractViewer() = delete;

  AbstractViewer(GraphEditor *p_graph_editor,
                 QWidget     *parent = nullptr,
                 std::string  label = "");

  void clear();

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

Q_SIGNALS:
  void widget_close();

  void view_param_changed(BaseNode          *p_node,
                          const std::string &port_id_elev,
                          const std::string &port_id_color,
                          const std::string &port_id_normal_map);

public Q_SLOTS:
  void on_node_deleted(const std::string &id);

  void on_node_deselected(const std::string &id);

  void on_node_selected(const std::string &id);

protected:
  void closeEvent(QCloseEvent *event) override;

protected:
  GraphEditor *p_graph_editor;
  std::string  label;

  struct NodeViewParam
  {
    NodeViewParam() = default;

    std::string port_id_elev = "";
    std::string port_id_color = "";
    std::string port_id_normal_map = "";
  };

  std::string current_node_id;

  NodeViewParam current_view_param;

  std::map<std::string, NodeViewParam> node_view_param_map;

  QWidget *render_widget = nullptr;

  QPushButton *button_pin_current_node;
  QLabel      *label_node_id;
  QComboBox   *combo_elev;
  QComboBox   *combo_color;
  QComboBox   *combo_normal_map;

  // used to avoid interfering with the combo and the data when programmatically
  // modifying
  // the combo
  bool freeze_combo_change_event = false;

  // wrapper to ease code maintainability and readability
  void emit_view_param_changed();

  void update_view_param_widgets();
};

} // namespace hesiod
