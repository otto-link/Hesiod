/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

#include "gnode.hpp"
#include "macrologger.h"
#include "nlohmann/json_fwd.hpp"

#include "hesiod/control_node.hpp"
#include "hesiod/control_tree.hpp"
#include "hesiod/serialization.hpp"
#include "hesiod/view_tree.hpp" // for the Link object

namespace hesiod::cnode
{

// ControlTree

void ControlTree::load_state(std::string fname)
{
  this->remove_all_nodes();

  std::ifstream  input_file_stream = std::ifstream(fname);
  nlohmann::json input_serialized_data = nlohmann::json();

  if (this->serialization_type ==
      hesiod::serialization::SerializationType::PLAIN)
  {
    input_file_stream >> input_serialized_data;
  }
  else
  {
    std::vector<char> buffer = std::vector<char>(
        std::istreambuf_iterator<char>(input_file_stream),
        std::istreambuf_iterator<char>());

    switch (this->serialization_type)
    {
    case hesiod::serialization::SerializationType::BJDATA:
      input_serialized_data = nlohmann::json::from_bjdata(buffer);
      break;
    case hesiod::serialization::SerializationType::BSON:
      input_serialized_data = nlohmann::json::from_bson(buffer);
      break;
    case hesiod::serialization::SerializationType::CBOR:
      input_serialized_data = nlohmann::json::from_cbor(buffer);
      break;
    case hesiod::serialization::SerializationType::MESSAGEPACK:
      input_serialized_data = nlohmann::json::from_msgpack(buffer);
      break;
    case hesiod::serialization::SerializationType::UBJSON:
      input_serialized_data = nlohmann::json::from_ubjson(buffer);
      break;
    default:
      LOG_ERROR("Unknown load type");
      break;
    }
  }

  this->deserialize_json_v2("data", input_serialized_data);

  input_file_stream.close();
}

bool ControlTree::deserialize_json_v2(std::string     field_name,
                                      nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false)
  {
    return false;
  }

  // global parameters
  id = input_data[field_name]["id"].get<std::string>();
  overlap = input_data[field_name]["overlap"].get<float>();
  shape.x = input_data[field_name]["shape.x"].get<int>();
  shape.y = input_data[field_name]["shape.y"].get<int>();
  tiling.x = input_data[field_name]["tiling.x"].get<int>();
  tiling.y = input_data[field_name]["tiling.y"].get<int>();
  id_counter = input_data[field_name]["id_counter"].get<int>();

  // create node
  std::vector<std::string> node_ids =
      input_data[field_name]["node_ids"].get<std::vector<std::string>>();

  for (size_t k = 0; k < node_ids.size(); k++)
    this->add_control_node(hesiod::cnode::node_type_from_id(node_ids[k]),
                           node_ids[k]);

  // nodes parameters
  for (nlohmann::json node_data : input_data[field_name]["nodes"])
  {
    std::string id = node_data["data"]["id"].get<std::string>();

    if (this->get_node_type(id) != "Clone")
    {
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)
          ->deserialize_json_v2("data", node_data);
    }
    else
    {
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->deserialize_json_v2(
          "data",
          node_data);
    }
  }

  // links
  for (nlohmann::json link_data : input_data[field_name]["links"])
  {
    // links from the ViewTree (GUI links) are replicated within the
    // GNode framework (data links)
    hesiod::vnode::Link current_link = hesiod::vnode::Link();
    current_link.deserialize_json_v2("value", link_data);

    this->link(current_link.node_id_from,
               current_link.port_id_from,
               current_link.node_id_to,
               current_link.port_id_to);
  }

  this->update();

  return true;
}

} // namespace hesiod::cnode
