/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value, std::string fmt)
    : value(value), fmt(fmt)
{
}

WaveNbAttribute::WaveNbAttribute(hmap::Vec2<float> value,
                                 float             vmin,
                                 float             vmax,
                                 std::string       fmt)
    : value(value), vmin(vmin), vmax(vmax), fmt(fmt)
{
}

hmap::Vec2<float> WaveNbAttribute::get() { return value; }

} // namespace hesiod
