/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/model_registry.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> register_data_models(
    hesiod::ModelConfig &config)
{
  auto ret = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

  HSD_REGISTER_NODE(hesiod::Cloud, "Geometry");
  HSD_REGISTER_NODE(hesiod::GammaCorrection, "Filter");
  HSD_REGISTER_NODE(hesiod::HeightMapToMask, "Converter");
  HSD_REGISTER_NODE(hesiod::Noise, "Primitive");
  HSD_REGISTER_NODE(hesiod::NoiseFbm, "Primitive");
  HSD_REGISTER_NODE(hesiod::SmoothCpulse, "Filter");

  return ret;
}

} // namespace hesiod
