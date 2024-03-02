/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "gnode.hpp"
#include "hesiod/control_node.hpp"
#include "macrologger.h"
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <vector>

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"
#include "nlohmann/json_fwd.hpp"

namespace hesiod::vnode
{

// Link

bool Link::serialize_json_v2(std::string fieldName, nlohmann::json& outputData)
{
  outputData[fieldName]["node_id_from"] = node_id_from;
  outputData[fieldName]["port_id_from"] = port_id_from;
  outputData[fieldName]["port_hash_id_from"] = port_hash_id_from;
  outputData[fieldName]["node_id_to"] = node_id_to;
  outputData[fieldName]["port_id_to"] = port_id_to;
  outputData[fieldName]["port_hash_id_to"] = port_hash_id_to;
  return true;
}

bool Link::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData)
{
  if(inputData[fieldName].is_object() == false)
  {
    LOG_ERROR("Encountered invalid link data.");
    return false;
  }

  node_id_from = inputData[fieldName]["node_id_from"].get<std::string>();
  port_id_from = inputData[fieldName]["port_id_from"].get<std::string>();
  port_hash_id_from = inputData[fieldName]["port_hash_id_from"].get<int>();
  node_id_to = inputData[fieldName]["node_id_to"].get<std::string>();
  port_id_to = inputData[fieldName]["port_id_to"].get<std::string>();
  port_hash_id_to = inputData[fieldName]["port_hash_id_to"].get<int>();
  return true;
}


// ViewTree

void ViewTree::load_state(std::string fname)
{
#ifdef USE_CEREAL
  LOG_DEBUG("loading state...");

  // clear the tree first
  this->remove_all_nodes();
  this->links.clear();

  std::ifstream            is(fname);
  cereal::JSONInputArchive iarchive(is);
  iarchive(*this);
#else
  this->remove_all_nodes();
  this->links.clear();

  std::ifstream inputFileStream = std::ifstream(fname);
  nlohmann::json inputSerializedData = nlohmann::json();

  inputFileStream >> inputSerializedData;

  this->deserialize_json_v2("data", inputSerializedData);

  inputFileStream.close();
#endif
}

void ViewTree::save_state(std::string fname)
{
#ifdef USE_CEREAL
  LOG_DEBUG("saving state...");
  std::ofstream             os(fname);
  cereal::JSONOutputArchive oarchive(os);
  oarchive(cereal::make_nvp("tree", *this));
  LOG_DEBUG("ok");
#else
  std::ofstream outputFileStream = std::ofstream(fname, std::ios::trunc);
  nlohmann::json outputSerializedData = nlohmann::json();

  this->serialize_json_v2("data", outputSerializedData);

  outputFileStream << outputSerializedData.dump(1) << std::endl;
  
  outputFileStream.close();
#endif
}

template <class Archive> void ViewTree::load(Archive &archive)
{
#ifdef USE_CEREAL
  archive(cereal::make_nvp("id", this->id),
          cereal::make_nvp("overlap ", this->overlap),
          cereal::make_nvp("shape.x", this->shape.x),
          cereal::make_nvp("shape.y", this->shape.y),
          cereal::make_nvp("tiling.x", this->tiling.x),
          cereal::make_nvp("tiling.y", this->tiling.y),
          cereal::make_nvp("id_counter", this->id_counter));

  // retrive node ids and positions and re-generate all the nodes and
  // position them on the node editor canvas
  {
    std::vector<std::string> node_ids = {};
    std::vector<float>       pos_x = {};
    std::vector<float>       pos_y = {};
    archive(cereal::make_nvp("node_ids", node_ids));
    archive(cereal::make_nvp("pos_x", pos_x));
    archive(cereal::make_nvp("pos_y", pos_y));

    ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());
    for (size_t k = 0; k < node_ids.size(); k++)
    {
      this->add_view_node(node_type_from_id(node_ids[k]), node_ids[k]);
      ax::NodeEditor::SetNodePosition(
          this->get_node_ref_by_id(node_ids[k])->hash_id,
          ImVec2(pos_x[k], pos_y[k]));
    }
    ax::NodeEditor::SetCurrentEditor(nullptr);
  }

  // nodes parameters
  for (auto &[id, node] : this->get_nodes_map())
  {
    // this could be done with a virtual method but it is much more
    // compact to do it this way (since there is only one exception so
    // far)
    if (this->get_node_type(id) != "Clone")
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)->serialize(
          archive);
    else
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->serialize(archive);
  }

  // links
  archive(cereal::make_nvp("links", this->links));

  // links from the ViewTree (GUI links) still needs to be replicated
  // within the GNode framework (data links)
  for (auto &[id, link] : this->links)
    this->link(link.node_id_from,
               link.port_id_from,
               link.node_id_to,
               link.port_id_to);

  this->update();
#endif
}

template <class Archive> void ViewTree::save(Archive &archive) const
{
#ifdef USE_CEREAL
  archive(cereal::make_nvp("id", this->id),
          cereal::make_nvp("overlap ", this->overlap),
          cereal::make_nvp("shape.x", this->shape.x),
          cereal::make_nvp("shape.y", this->shape.y),
          cereal::make_nvp("tiling.x", this->tiling.x),
          cereal::make_nvp("tiling.y", this->tiling.y),
          cereal::make_nvp("id_counter", this->id_counter));

  // node ids and positions
  {
    ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());

    std::vector<std::string> node_ids = {};
    std::vector<float>       pos_x = {};
    std::vector<float>       pos_y = {};
    for (auto &[id, vnode] : this->get_nodes_map())
    {
      ImVec2 pos = ax::NodeEditor::GetNodePosition(vnode.get()->hash_id);
      node_ids.push_back(id);
      pos_x.push_back(pos.x);
      pos_y.push_back(pos.y);
    }

    ax::NodeEditor::SetCurrentEditor(nullptr);

    archive(cereal::make_nvp("node_ids", node_ids));
    archive(cereal::make_nvp("pos_x", pos_x));
    archive(cereal::make_nvp("pos_y", pos_y));
  }

  // nodes parameters
  for (auto &[id, node] : this->get_nodes_map())
  {
    if (this->get_node_type(id) != "Clone")
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)->serialize(
          archive);
    else
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->serialize(archive);
  }

  // links
  archive(cereal::make_nvp("links", this->links));
#endif
}

bool ViewTree::serialize_json_v2(std::string fieldName, nlohmann::json& outputData)
{
  outputData[fieldName]["id"] = id;
  outputData[fieldName]["overlap"] = overlap;
  outputData[fieldName]["shape.x"] = shape.x;
  outputData[fieldName]["shape.y"] = shape.y;
  outputData[fieldName]["tiling.x"] = tiling.x;
  outputData[fieldName]["tiling.y"] = tiling.y;
  outputData[fieldName]["id_counter"] = id_counter;

  // node ids and positions
  {
    ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());

    std::vector<std::string> node_ids = {};
    std::vector<float>       pos_x = {};
    std::vector<float>       pos_y = {};
    for (auto &[id, vnode] : this->get_nodes_map())
    {
      ImVec2 pos = ax::NodeEditor::GetNodePosition(vnode.get()->hash_id);
      node_ids.push_back(id);
      pos_x.push_back(pos.x);
      pos_y.push_back(pos.y);
    }

    ax::NodeEditor::SetCurrentEditor(nullptr);

    outputData[fieldName]["node_ids"] = node_ids;
    outputData[fieldName]["pos_x"] = pos_x;
    outputData[fieldName]["pos_y"] = pos_y;
  }

  std::vector<nlohmann::json> nodesArraySerialized = {};

  // nodes parameters
  for (auto &[id, node] : this->get_nodes_map())
  {
    nlohmann::json currentNodeSerialized = nlohmann::json();

    currentNodeSerialized["id"] = id;
    currentNodeSerialized["type"] = cnode::ControlNodeInstancing::get_name_from_type(this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)->get_type());

    if (this->get_node_type(id) != "Clone")
    {
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)->serialize_json_v2("data", currentNodeSerialized);
    }
    else
    {
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->serialize_json_v2("data", currentNodeSerialized);
    }

    nodesArraySerialized.push_back(currentNodeSerialized);
  }

  std::vector<nlohmann::json> linksArraySerialized = {};

  for(auto& [id, link] : links)
  {
    nlohmann::json currentLinkSerialized = nlohmann::json();

    currentLinkSerialized["key"] = id;
    link.serialize_json_v2("value", currentLinkSerialized);

    linksArraySerialized.push_back(currentLinkSerialized);
  }

  outputData[fieldName]["nodes"] = nodesArraySerialized;
  outputData[fieldName]["links"] = linksArraySerialized;

  return true;
} 

bool ViewTree::deserialize_json_v2(std::string fieldName, nlohmann::json& inputData)
{
  if(inputData[fieldName].is_object() == false)
  {
    return false;
  }

  id = inputData[fieldName]["id"].get<std::string>();
  overlap = inputData[fieldName]["overlap"].get<float>();
  shape.x = inputData[fieldName]["shape.x"].get<int>();
  shape.y = inputData[fieldName]["shape.y"].get<int>();
  tiling.x = inputData[fieldName]["tiling.x"].get<int>();
  tiling.y = inputData[fieldName]["tiling.y"].get<int>();
  id_counter = inputData[fieldName]["id_counter"].get<int>();

  {
    std::vector<std::string> node_ids = inputData[fieldName]["node_ids"].get<std::vector<std::string>>();
    std::vector<float>       pos_x = inputData[fieldName]["pos_x"].get<std::vector<float>>();
    std::vector<float>       pos_y = inputData[fieldName]["pos_y"].get<std::vector<float>>();

    ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());
    for (size_t k = 0; k < node_ids.size(); k++)
    {
      this->add_view_node(node_type_from_id(node_ids[k]), node_ids[k]);
      ax::NodeEditor::SetNodePosition(
          this->get_node_ref_by_id(node_ids[k])->hash_id,
          ImVec2(pos_x[k], pos_y[k]));
    }
    ax::NodeEditor::SetCurrentEditor(nullptr);
  }

  // nodes parameters

  for(nlohmann::json currentNodeSerializedData : inputData[fieldName]["nodes"])
  {
    std::string id = currentNodeSerializedData["data"]["id"].get<std::string>();

    if (this->get_node_type(id) != "Clone")
    {
      this->get_node_ref_by_id<hesiod::cnode::ControlNode>(id)->deserialize_json_v2("data", currentNodeSerializedData);
    }
    else
    {
      this->get_node_ref_by_id<hesiod::cnode::Clone>(id)->serialize_json_v2("data", currentNodeSerializedData);
    }
  }

  // links
  for(nlohmann::json currentLinkObject : inputData[fieldName]["links"])
  {
    Link currentLink = Link();
    int id = currentLinkObject["key"].get<int>();
    currentLink.deserialize_json_v2("value", currentLinkObject);
    links.emplace(id, currentLink);
  }

  // links from the ViewTree (GUI links) still needs to be replicated
  // within the GNode framework (data links)
  for (auto &[id, link] : this->links)
    this->link(link.node_id_from,
               link.port_id_from,
               link.node_id_to,
               link.port_id_to);

  this->update();

  return true;
}

} // namespace hesiod::vnode
