/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once
#include <QDialog>
#include <QWidget>

#include "gnodegui/graphics_node.hpp"

#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

// =====================================
// NodeInfoDialog
// =====================================
class NodeInfoDialog : public QDialog
{
  Q_OBJECT

public:
  NodeInfoDialog() = default;
  NodeInfoDialog(BaseNode            *p_node,
                 gngui::GraphicsNode *p_gfx_node,
                 QWidget             *parent = nullptr);

private:
};

} // namespace hesiod