/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QGridLayout>
#include <QtNodes/NodeData>

#include "hesiod/model/attributes.hpp"
#include "hesiod/model/model_config.hpp"

namespace hesiod
{

void add_attribute_widget(QGridLayout                *layout,
                          std::unique_ptr<Attribute> &attr,
                          std::string                 label);

} // namespace hesiod