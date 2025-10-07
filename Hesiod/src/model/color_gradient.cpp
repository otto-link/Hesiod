/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/color_gradient.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

ColorGradientManager::ColorGradientManager()
{
  Logger::log()->info("ColorGradientManager::ColorGradientManager: initializing...");
  this->update_data();
}

std::vector<attr::Preset> ColorGradientManager::get_as_attr_presets() const
{
  std::vector<attr::Preset> presets;

  for (auto &data : this->gradient_data)
  {
    attr::Preset preset;
    preset.name = data.name;
    preset.stops = {};

    for (size_t k = 0; k < data.positions.size(); ++k)
    {
      attr::Stop stop;
      stop.position = data.positions[k];
      stop.color = data.colors[k];
      preset.stops.push_back(stop);
    }

    presets.push_back(preset);
  }

  return presets;
}

void ColorGradientManager::update_data(bool append)
{
  Logger::log()->trace("ColorGradientManager::update_data");

  if (!append)
    this->gradient_data.clear();

  nlohmann::json json = json_from_file(HSD_COLOR_GRADIENT_FILE);

  if (json.empty())
  {
    Logger::log()->error(
        "ColorGradientManager::update_data: json parsing error, empty json");
    return;
  }

  if (!json.contains("gradients"))
  {
    Logger::log()->error(
        "ColorGradientManager::update_data: json parsing error, no 'gradients' key");
    return;
  }

  for (const auto &grad_json : json["gradients"])
  {
    if (!grad_json.contains("name") || !grad_json.contains("stops"))
    {
      Logger::log()->error(
          "ColorGradientManager::update_data: missing fields in gradient");
      continue;
    }

    ColorGradientData grad;
    grad.name = grad_json["name"].get<std::string>();
    Logger::log()->trace("- {}", grad.name);

    for (const auto &stop_json : grad_json["stops"])
    {
      if (!stop_json.contains("position") || !stop_json.contains("color"))
      {
        Logger::log()->error("ColorGradientManager::update_data: invalid stop in {}",
                             grad.name);
        continue;
      }

      grad.positions.push_back(stop_json["position"].get<float>());

      const auto &color_array = stop_json["color"];
      if (!color_array.is_array() || color_array.size() < 4)
      {
        Logger::log()->error("ColorGradientManager::update_data: invalid color in {}",
                             grad.name);
        continue;
      }

      std::array<float, 4> cf;
      for (size_t k = 0; k < 4; ++k)
        cf[k] = color_array[k].get<float>() / 255.f;

      grad.colors.push_back(std::move(cf));
    }

    this->gradient_data.push_back(std::move(grad));
  }
}

} // namespace hesiod
