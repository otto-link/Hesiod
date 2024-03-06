/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>
#include <memory>
#include <vector>

#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"

#include "hesiod/attribute.hpp"
#include "hesiod/control_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::cnode
{

bool ControlNode::serialize_json_v2(std::string     field_name,
                                    nlohmann::json &output_data)
{
  std::vector<nlohmann::json> attributesListJsonData = {};

  for (std::map<std::string, std::unique_ptr<hesiod::Attribute>>::iterator
           currentAttributeIterator = attr.begin();
       currentAttributeIterator != attr.end();
       currentAttributeIterator++)
  {
    nlohmann::json currentAttributeIteratorJsonData = nlohmann::json();

    currentAttributeIteratorJsonData["type"] = get_attribute_name_from_type(
        currentAttributeIterator->second->get_type());

    currentAttributeIteratorJsonData["key"] = currentAttributeIterator->first;
    currentAttributeIterator->second->serialize_json_v2(
        "value",
        currentAttributeIteratorJsonData);

    attributesListJsonData.push_back(currentAttributeIteratorJsonData);
  }

  output_data[field_name]["id"] = this->id;
  output_data[field_name]["attributes"] = attributesListJsonData;
  return true;
}

bool ControlNode::deserialize_json_v2(std::string     field_name,
                                      nlohmann::json &input_data)
{
  if (input_data[field_name]["id"].is_string() == false ||
      input_data[field_name]["attributes"].is_array() == false)
  {
    LOG_ERROR("Encountered invalid control node!");
    return false;
  }

  attr.clear();

  this->id = input_data[field_name]["id"].get<std::string>();

  for (nlohmann::json currentAttributeIteratorJsonData :
       input_data[field_name]["attributes"])
  {
    AttributeType currentAttributeIteratorType = get_attribute_type_from_name(
        currentAttributeIteratorJsonData["type"].get<std::string>());

    if (currentAttributeIteratorType == AttributeType::INVALID)
    {
      LOG_ERROR("Encountered invalid control node attribute type!");
      continue;
    }

    std::unique_ptr<Attribute> currentAttribute = create_attribute_from_type(
        currentAttributeIteratorType);
    std::string currentAttributeKey =
        currentAttributeIteratorJsonData["key"].get<std::string>();

    currentAttribute->deserialize_json_v2("value",
                                          currentAttributeIteratorJsonData);

    attr.emplace(currentAttributeKey, std::move(currentAttribute));
  }

  return true;
}

void ControlNode::post_process_heightmap(hmap::HeightMap &h)
{
  if (this->attr.contains("inverse"))
    if (GET_ATTR_BOOL("inverse"))
      h.inverse();

  if (this->attr.contains("smoothing"))
    if (GET_ATTR_BOOL("smoothing"))
    {
      int ir_smoothing = GET_ATTR_INT("ir_smoothing");

      hmap::transform(h,
                      [&ir_smoothing](hmap::Array &array)
                      { return hmap::smooth_cpulse(array, ir_smoothing); });
      h.smooth_overlap_buffers();
    }

  if (this->attr.contains("saturate"))
    if (GET_ATTR_REF_RANGE("saturate")->is_activated())
    {
      hmap::Vec2<float> srange = GET_ATTR_RANGE("saturate");
      float             k = GET_ATTR_FLOAT("k_saturate");

      float hmin = h.min();
      float hmax = h.max();

      // node parameters are assumed normalized and thus in [0, 1],
      // they need to be rescaled
      float smin_n = hmin + srange.x * (hmax - hmin);
      float smax_n = hmax - (1.f - srange.y) * (hmax - hmin);
      float k_n = k * (hmax - hmin);

      hmap::transform(h,
                      [&smin_n, &smax_n, &k_n](hmap::Array &array)
                      { hmap::clamp_smooth(array, smin_n, smax_n, k_n); });

      // keep original amplitude
      h.remap(hmin, hmax);
    }

  if (this->attr.contains("remap"))
    if (GET_ATTR_REF_RANGE("remap")->is_activated())
    {
      hmap::Vec2<float> vrange = GET_ATTR_RANGE("remap");
      h.remap(vrange.x, vrange.y);
    }
}

} // namespace hesiod::cnode
