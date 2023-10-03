/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "gnode.hpp"
#include "macrologger.h"
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

void ViewTree::load_state(std::string fname)
{
  LOG_DEBUG("loading state...");

  // clear the tree first
  this->remove_all_nodes();
  this->links.clear();

  std::ifstream            is(fname);
  cereal::JSONInputArchive iarchive(is);
  iarchive(*this);
}

template <class Archive> void ViewTree::load(Archive &archive)
{
  archive(cereal::make_nvp("id", this->id),
          cereal::make_nvp("overlap ", this->overlap),
          cereal::make_nvp("shape.x", this->shape.x),
          cereal::make_nvp("shape.y", this->shape.y));

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
    this->get_view_control_node_ref_by_id(id)->serialize_load(archive);

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
}

template <class Archive> void ViewTree::save(Archive &archive) const
{
  archive(cereal::make_nvp("id", this->id),
          cereal::make_nvp("overlap ", this->overlap),
          cereal::make_nvp("shape.x", this->shape.x),
          cereal::make_nvp("shape.y", this->shape.y));

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
    this->get_view_control_node_ref_by_id(id)->serialize_save(archive);

  // links
  archive(cereal::make_nvp("links", this->links));
}

void ViewTree::save_state(std::string fname)
{
  LOG_DEBUG("saving state...");

  std::ofstream             os(fname);
  cereal::JSONOutputArchive oarchive(os);
  oarchive(cereal::make_nvp("tree", *this));
  LOG_DEBUG("ok");
}

} // namespace hesiod::vnode
