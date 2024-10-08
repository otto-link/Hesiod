/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file open_gl_hmm_render.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <filesystem>

#include "hesiod/gui/widgets/open_gl/open_gl_render.hpp"
#include "hesiod/gui/widgets/open_gl/open_gl_shaders.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"

namespace hesiod
{

class OpenGLHmmRender : public OpenGLRender
{
public:
  OpenGLHmmRender(QWidget *parent = nullptr, ShaderType shader_type = ShaderType::NORMAL);

  void set_data(BaseNode          *new_p_node,
                const std::string &new_port_id_elev,
                const std::string &new_port_id_color) override;
};

} // namespace hesiod