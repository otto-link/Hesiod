/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/model/attributes.hpp"

namespace hesiod
{

FilenameAttribute::FilenameAttribute(std::string value) : value(value) {}

std::string FilenameAttribute::get() { return value; }

} // namespace hesiod
