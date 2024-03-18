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
#include "hesiod/serialization.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

// LinkInfos

bool LinkInfos::serialize_json_v2(std::string     field_name,
                                  nlohmann::json &output_data)
{
  output_data[field_name]["node_id_from"] = node_id_from;
  output_data[field_name]["port_id_from"] = port_id_from;
  output_data[field_name]["port_hash_id_from"] = port_hash_id_from;
  output_data[field_name]["node_id_to"] = node_id_to;
  output_data[field_name]["port_id_to"] = port_id_to;
  output_data[field_name]["port_hash_id_to"] = port_hash_id_to;
  return true;
}

bool LinkInfos::deserialize_json_v2(std::string     field_name,
                                    nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false)
  {
    LOG_ERROR("Encountered invalid link data.");
    return false;
  }

  node_id_from = input_data[field_name]["node_id_from"].get<std::string>();
  port_id_from = input_data[field_name]["port_id_from"].get<std::string>();
  port_hash_id_from = input_data[field_name]["port_hash_id_from"].get<int>();
  node_id_to = input_data[field_name]["node_id_to"].get<std::string>();
  port_id_to = input_data[field_name]["port_id_to"].get<std::string>();
  port_hash_id_to = input_data[field_name]["port_hash_id_to"].get<int>();
  return true;
}

// ViewTree

void ViewTree::load_state(std::string fname)
{
  this->remove_all_nodes();
  this->links_infos.clear();
  this->selected_node_hid.clear();
  this->node_positions.clear();

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

  this->update();
}

void ViewTree::save_state(std::string fname)
{
  std::ofstream  output_file_stream = std::ofstream(fname, std::ios::trunc);
  nlohmann::json output_serialized_data = nlohmann::json();

  this->serialize_json_v2("data", output_serialized_data);

  if (this->serialization_type ==
      hesiod::serialization::SerializationType::PLAIN)
  {
    output_file_stream << output_serialized_data.dump(1) << std::endl;
  }
  else
  {
    std::vector<uint8_t> bytes = {};

    switch (this->serialization_type)
    {
    case hesiod::serialization::SerializationType::BJDATA:
      bytes = nlohmann::json::to_bjdata(output_serialized_data);
      break;
    case hesiod::serialization::SerializationType::BSON:
      bytes = nlohmann::json::to_bson(output_serialized_data);
      break;
    case hesiod::serialization::SerializationType::CBOR:
      bytes = nlohmann::json::to_cbor(output_serialized_data);
      break;
    case hesiod::serialization::SerializationType::MESSAGEPACK:
      bytes = nlohmann::json::to_msgpack(output_serialized_data);
      break;
    case hesiod::serialization::SerializationType::UBJSON:
      bytes = nlohmann::json::to_ubjson(output_serialized_data);
      break;
    default:
      LOG_ERROR("Unknown load type");
      break;
    }

    output_file_stream.write(reinterpret_cast<char *>(bytes.data()),
                             bytes.size());
    output_file_stream.flush();
  }

  output_file_stream.close();
}

bool ViewTree::serialize_json_v2(std::string     field_name,
                                 nlohmann::json &output_data)
{
  output_data[field_name]["id"] = id;
  output_data[field_name]["overlap"] = overlap;
  output_data[field_name]["shape.x"] = shape.x;
  output_data[field_name]["shape.y"] = shape.y;
  output_data[field_name]["tiling.x"] = tiling.x;
  output_data[field_name]["tiling.y"] = tiling.y;
  output_data[field_name]["id_counter"] = id_counter;
  output_data[field_name]["show_comments"] = show_comments;

  // node ids and positions
  {
    std::vector<std::string> node_ids = {};
    std::vector<float>       pos_x = {};
    std::vector<float>       pos_y = {};
    for (auto &[id, pos] : this->get_node_positions())
    {
      node_ids.push_back(id);
      pos_x.push_back(pos.x);
      pos_y.push_back(pos.y);
    }

    output_data[field_name]["node_ids"] = node_ids;
    output_data[field_name]["pos_x"] = pos_x;
    output_data[field_name]["pos_y"] = pos_y;
  }

  std::vector<nlohmann::json> nodesArraySerialized = {};

  // nodes parameters
  for (auto &[id, node] : this->get_nodes_map())
  {
    nlohmann::json currentNodeSerialized = nlohmann::json();

    currentNodeSerialized["id"] = id;
    currentNodeSerialized["type"] = this->get_node_type(id);

    if (this->get_node_type(id) != "Clone")
    {
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)
          ->serialize_json_v2("data", currentNodeSerialized);
    }
    else
    {
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->serialize_json_v2(
          "data",
          currentNodeSerialized);
    }

    nodesArraySerialized.push_back(currentNodeSerialized);
  }

  std::vector<nlohmann::json> linksArraySerialized = {};

  for (auto &[id, link] : links_infos)
  {
    nlohmann::json currentLinkSerialized = nlohmann::json();

    currentLinkSerialized["key"] = id;
    link.serialize_json_v2("value", currentLinkSerialized);

    linksArraySerialized.push_back(currentLinkSerialized);
  }

  output_data[field_name]["nodes"] = nodesArraySerialized;
  output_data[field_name]["links_infos"] = linksArraySerialized;

  return true;
}

bool ViewTree::deserialize_json_v2(std::string     field_name,
                                   nlohmann::json &input_data)
{
  if (input_data[field_name].is_object() == false)
  {
    return false;
  }

  id = input_data[field_name]["id"].get<std::string>();
  overlap = input_data[field_name]["overlap"].get<float>();
  shape.x = input_data[field_name]["shape.x"].get<int>();
  shape.y = input_data[field_name]["shape.y"].get<int>();
  tiling.x = input_data[field_name]["tiling.x"].get<int>();
  tiling.y = input_data[field_name]["tiling.y"].get<int>();
  id_counter = input_data[field_name]["id_counter"].get<int>();
  show_comments = input_data[field_name]["show_comments"].get<bool>();

  // node positions
  std::vector<std::string> node_ids =
      input_data[field_name]["node_ids"].get<std::vector<std::string>>();
  std::vector<float> pos_x =
      input_data[field_name]["pos_x"].get<std::vector<float>>();
  std::vector<float> pos_y =
      input_data[field_name]["pos_y"].get<std::vector<float>>();

  for (size_t k = 0; k < node_ids.size(); k++)
  {
    this->add_view_node(hesiod::cnode::node_type_from_id(node_ids[k]),
                        node_ids[k]);
    this->node_positions[node_ids[k]] = ImVec2(pos_x[k], pos_y[k]);
  }

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

  // links_infos
  for (nlohmann::json link_data : input_data[field_name]["links_infos"])
  {
    LinkInfos current_link = LinkInfos();
    int       id = link_data["key"].get<int>();
    current_link.deserialize_json_v2("value", link_data);
    links_infos.emplace(id, current_link);
  }

  // links_infos from the ViewTree (GUI links_infos) still needs to be
  // replicated within the GNode framework (data links_infos)
  for (auto &[id, link] : this->links_infos)
    this->link(link.node_id_from,
               link.port_id_from,
               link.node_id_to,
               link.port_id_to);

  return true;
}

} // namespace hesiod::vnode
