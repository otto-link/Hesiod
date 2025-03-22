/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */

/**
 * @file open_gl_render.hpp
 * @author  Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <filesystem>

#include "highmap/algebra.hpp"

#include "hesiod/gui/widgets/open_gl/open_gl_render.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class OpenGLWidget : public QWidget
{
public:
  OpenGLWidget(QWidget *parent = nullptr);

  void set_data(BaseNode          *new_p_node,
                const std::string &new_port_id_elev,
                const std::string &new_port_id_color,
                const std::string &new_port_id_normal_map);

  void on_node_compute_finished(const std::string &id);

private:
  OpenGLRender *renderer;
};

} // namespace hesiod