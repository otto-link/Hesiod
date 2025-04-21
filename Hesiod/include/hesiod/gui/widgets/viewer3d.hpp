/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file viewer3d.hpp
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

#include "hesiod/gui/widgets/abstract_viewer.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class GraphNodeWidget; // forward

// =====================================
// Viewer3d
// =====================================
class Viewer3d : public AbstractViewer
{
public:
  Viewer3d() = delete;

  Viewer3d(GraphNodeWidget *p_graph_tabs_widget,
           QWidget         *parent = nullptr,
           std::string      label = "");
};

} // namespace hesiod
