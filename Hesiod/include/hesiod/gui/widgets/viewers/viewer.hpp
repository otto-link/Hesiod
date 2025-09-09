/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QPushButton>
#include <QWidget>

#include "nlohmann/json.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

enum ViewerType : int
{
  VIEWER3D,
};

static std::map<ViewerType, std::string> viewer_type_as_string = {
    {ViewerType::VIEWER3D, "Render 3D"}};

// =====================================
// Viewer (abstract)
// =====================================
class Viewer : public QWidget
{
  Q_OBJECT

public:
  Viewer() = delete;
  Viewer(GraphNodeWidget   *p_graph_node_widget_,
         ViewerType         viewer_type_ = ViewerType::VIEWER3D,
         const std::string &label_ = "Viewer",
         QWidget           *parent = nullptr);

  void clear();
  void set_current_node_id(const std::string &new_id);
  void set_viewer_type(const ViewerType &new_viewer_type);
  void setup_connections(bool only_widgets = false);
  void setup_layout();
  void update_widgets();

  // --- Serialization ---
  void           json_from(nlohmann::json const &json);
  nlohmann::json json_to() const;

Q_SIGNALS:
  void clear_view_request();
  void current_node_id_changed(const std::string &new_id);
  void widget_close();

public Q_SLOTS:
  void on_node_deleted(const std::string &new_id);
  void on_node_deselected(const std::string &new_id);
  void on_node_selected(const std::string &new_id);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  // --- Members ---
  GraphNodeWidget *p_graph_node_widget;
  ViewerType       viewer_type;
  std::string      label;
  std::string      current_node_id = "";
  bool             is_node_pinned = false;
  QPushButton     *button_pin_current_node;
};

} // namespace hesiod
