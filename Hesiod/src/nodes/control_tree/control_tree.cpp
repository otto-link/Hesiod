/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/control_tree.hpp"
#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

ControlTree::ControlTree(std::string id)
    : gnode::Tree(id),
      serialization_type(hesiod::serialization::SerializationType::MESSAGEPACK)
{
  this->shape = hmap::Vec2<int>(0, 0);
  this->tiling = hmap::Vec2<int>(0, 0);
  this->overlap = 0.f;
}

ControlTree::ControlTree(std::string     id,
                         hmap::Vec2<int> shape,
                         hmap::Vec2<int> tiling,
                         float           overlap)
    : gnode::Tree(id), shape(shape), tiling(tiling), overlap(overlap),
      serialization_type(hesiod::serialization::SerializationType::MESSAGEPACK)
{
}

std::string ControlTree::get_new_id()
{
  return std::to_string(this->id_counter++);
}

std::string ControlTree::get_node_type(std::string node_id) const
{
  return this->get_node_ref_by_id(node_id)->get_node_type();
}

std::string ControlTree::add_control_node(std::string control_node_type,
                                          std::string node_id)
{
  std::string id;
  if (node_id == "")
    id = control_node_type + "##" + this->get_new_id();
  else
    id = node_id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());

  this->add_node(create_control_node_from_type(control_node_type,
                                               id,
                                               this->shape,
                                               this->tiling,
                                               this->overlap));
  return id;
}

void ControlTree::clear()
{
  this->json_filename = "";
  this->remove_all_nodes();
}

} // namespace hesiod::cnode
