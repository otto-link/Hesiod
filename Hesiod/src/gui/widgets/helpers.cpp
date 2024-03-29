/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>
#include <QPushButton>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

int float_to_slider_pos(float v, float min, float max, int slider_steps)
{
  return (int)((v - min) / (max - min) * (float)slider_steps);
}

float slider_pos_to_float(int pos, float min, float max, int slider_steps)
{
  return min + (float)pos / (float)slider_steps * (max - min);
}

} // namespace hesiod
