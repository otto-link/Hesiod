/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file viewer3d.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
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

#define DEFAULT_VIEWER3D_WIDTH 512

namespace hesiod
{

class Viewer3d : public QWidget
{
  Q_OBJECT

public:
  Viewer3d() = delete;

  Viewer3d(GraphEditor *p_graph_editor,
           QWidget     *parent = nullptr,
           std::string  label = "");

  void json_from(nlohmann::json const &json);

  nlohmann::json json_to() const;

Q_SIGNALS:
  void widget_close();

  void view_param_changed(BaseNode          *p_node,
                          const std::string &port_id_elev,
                          const std::string &port_id_color);

public Q_SLOTS:
  void on_node_deleted(const std::string &id);

  void on_node_deselected(const std::string &id);

  void on_node_selected(const std::string &id);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  GraphEditor *p_graph_editor;
  std::string  label;

  struct NodeViewParam
  {
    NodeViewParam() = default;

    std::string port_id_elev = "";
    std::string port_id_color = "";
  };

  std::string current_node_id;

  NodeViewParam current_view_param;

  std::map<std::string, NodeViewParam> node_view_param_map;

  QWidget *render_widget = nullptr;

  QPushButton *button_pin_current_node;
  QLabel      *label_node_id;
  QComboBox   *combo_elev;
  QComboBox   *combo_color;

  // used to avoid interfering with the combo and the data when programmatically modifying
  // the combo
  bool freeze_combo_change_event = false;

  void clear();

  // wrapper to ease code maintainability and readability
  void emit_view_param_changed();

  void update_view_param_widgets();
};

} // namespace hesiod
