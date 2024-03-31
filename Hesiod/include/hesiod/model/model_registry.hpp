/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once
#include <QtNodes/NodeDelegateModelRegistry>

#include "hesiod/model/model_config.hpp"

// see model_registry.cpp
#define HSD_REGISTER_NODE(TYPE, CATEGORY)                                                \
  ret->registerModel<TYPE>([&config]() { return std::make_unique<TYPE>(config); },       \
                           CATEGORY);

namespace hesiod
{

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> register_data_models(
    hesiod::ModelConfig &config);

}