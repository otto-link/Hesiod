/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

QWidget *RadialDisplacementToXy::embeddedWidget()
{
  if (!this->preview_vec2)
  {
    this->preview_vec2 = new PreviewVec2(&this->dx, &this->dy, p_config);
    connect(this,
            &NodeDelegateModel::computingFinished,
            this->preview_vec2,
            &PreviewVec2::update_image);
  }
  return (QWidget *)this->preview_vec2;
}

} // namespace hesiod
