/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/attribute.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"
#include <map>
#include <memory>
#include <vector>

#include "hesiod/control_node.hpp"

#define CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(ClassName) bool ClassName ::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) { return ControlNode::serialize_json_v2(fieldName, outputData); } \
                                                                bool ClassName ::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData) { return ControlNode::deserialize_json_v2(fieldName, inputData); }

namespace hesiod::cnode
{

bool ControlNode::serialize_json_v2(std::string fieldName, nlohmann::json& outputData) 
{
  std::vector<nlohmann::json> attributesListJsonData = {};

  for(std::map<std::string, std::unique_ptr<hesiod::Attribute>>::iterator currentAttributeIterator = attr.begin(); currentAttributeIterator != attr.end(); currentAttributeIterator++)
  {
    nlohmann::json currentAttributeIteratorJsonData = nlohmann::json();

    currentAttributeIteratorJsonData["type"] = AttributeInstancing::get_name_from_type(currentAttributeIterator->second->get_type());
    currentAttributeIteratorJsonData["key"] = currentAttributeIterator->first;
    currentAttributeIterator->second->serialize_json_v2("value", currentAttributeIteratorJsonData);
  
    attributesListJsonData.push_back(currentAttributeIteratorJsonData);
  }
  
  outputData[fieldName]["id"] = this->id;
  outputData[fieldName]["attributes"] = attributesListJsonData;
  return true;
}

bool ControlNode::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData)
{
  if(
    inputData[fieldName]["id"].is_string() == false ||
    inputData[fieldName]["attributes"].is_array() == false
  )
  {
    LOG_ERROR("Encountered invalid control node!");
    return false;
  }

  attr.clear();

  this->id = inputData[fieldName]["id"].get<std::string>();

  for(nlohmann::json currentAttributeIteratorJsonData : inputData[fieldName]["attributes"])
  {
    AttributeType currentAttributeIteratorType = AttributeInstancing::get_type_from_name(currentAttributeIteratorJsonData["type"].get<std::string>());

    if(currentAttributeIteratorType == AttributeType::INVALID)
    {
      LOG_ERROR("Encountered invalid control node attribute type!");
      continue;
    }

    std::unique_ptr<Attribute> currentAttribute = AttributeInstancing::create_attribute_from_type(currentAttributeIteratorType);
    std::string currentAttributeKey = currentAttributeIteratorJsonData["key"].get<std::string>();

    currentAttribute->deserialize_json_v2("value", currentAttributeIteratorJsonData);

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

CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Unary);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Binary);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Debug);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Erosion);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Filter);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Mask);
CONTROL_NODE_IMPLEMENT_FORWARD_FUNCTION_BASE(Primitive);

} // namespace hesiod::cnode
