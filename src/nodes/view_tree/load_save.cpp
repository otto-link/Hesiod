/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "gnode.hpp"
#include "json.hpp"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

using json = nlohmann::json;

namespace hesiod::vnode
{

void ViewTree::load_state(std::string fname)
{
  LOG_DEBUG("loading state...");

  // clear the tree first
  this->remove_all_nodes();
  this->links.clear();

  std::stringstream buffer;

  std::fstream f;
  f.open(fname, std::ios::in);
  buffer << f.rdbuf();
  f.close();

  json j = json::parse(buffer);

  this->shape.x = j["parameters"]["shape"][0];
  this->shape.y = j["parameters"]["shape"][1];
  this->tiling.x = j["parameters"]["tiling"][0];
  this->tiling.y = j["parameters"]["tiling"][1];
  this->overlap = j["parameters"]["overlap"];

  // nodes
  for (size_t k = 0; k < j["nodes"].size(); k++)
  {
    std::string id = j["nodes"][k]["id"];
    int         cpos = id.find("#");
    std::string node_type = id.substr(0, cpos);
    ImVec2      pos;

    this->add_view_node(node_type, id);

    ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());
    pos.x = j["nodes"][k]["position"][0];
    pos.y = j["nodes"][k]["position"][1];
    ax::NodeEditor::SetNodePosition(this->get_node_ref_by_id(id)->hash_id, pos);
    ax::NodeEditor::SetCurrentEditor(nullptr);

    // TODO load node parameters

    LOG_DEBUG("%s %s", id.c_str(), node_type.c_str());
  }

  // links
  for (size_t k = 0; k < j["links"].size(); k++)
  {
    std::string node_id_from = j["links"][k]["node_id_from"];
    std::string port_id_from = j["links"][k]["port_id_from"];
    std::string node_id_to = j["links"][k]["node_id_to"];
    std::string port_id_to = j["links"][k]["port_id_to"];

    this->new_link(node_id_from, port_id_from, node_id_to, port_id_to);
  }

  this->update();
}

void ViewTree::save_state(std::string fname)
{
  LOG_DEBUG("saving state...");

  json j;

  j["parameters"] = {{"shape", {this->shape.x, this->shape.y}},
                     {"tiling", {this->tiling.x, this->tiling.y}},
                     {"overlap", this->overlap}};

  j["nodes"] = {};
  ax::NodeEditor::SetCurrentEditor(this->get_p_node_editor_context());
  for (auto &[id, vnode] : this->get_nodes_map())
  {
    // TODO move eveything into a node method
    ImVec2 pos = ax::NodeEditor::GetNodePosition(vnode->hash_id);
    j["nodes"].push_back({
        {"id", id.c_str()},
        {"position", {pos.x, pos.y}},
        {"parameters", nullptr} // TODO - node specific
    });
  }
  ax::NodeEditor::SetCurrentEditor(nullptr);

  j["links"] = {};
  for (auto &[id, link] : links)
    j["links"].push_back({{"node_id_from", link.node_id_from.c_str()},
                          {"port_id_from", link.port_id_from.c_str()},
                          {"node_id_to", link.node_id_to.c_str()},
                          {"port_id_to", link.port_id_to.c_str()}});

  std::fstream f;
  f.open(fname, std::ios::out);
  f << j.dump(4).c_str() << std::endl;
  f.close();
}

} // namespace hesiod::vnode
