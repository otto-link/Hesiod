/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

namespace hesiod
{

ReceiveNode::ReceiveNode(const std::string &label, std::shared_ptr<ModelConfig> config)
    : BaseNode(label, config)
{
}

std::string ReceiveNode::get_current_tag() const
{
  return this->get_attr<attr::ChoiceAttribute>("tag");
}

BroadcastMap *ReceiveNode::get_p_broadcast_params()
{
  if (!this->p_broadcast_params)
  {
    LOG->warn("ReceiveNode::get_p_broadcast_params: p_broadcast_param is nullptr");
  }

  return this->p_broadcast_params;
}

hmap::Terrain *ReceiveNode::get_p_target_terrain() { return this->p_target_terrain; }

void ReceiveNode::set_p_broadcast_params(BroadcastMap *new_p_broadcast_params)
{
  this->p_broadcast_params = new_p_broadcast_params;
}

void ReceiveNode::set_p_target_terrain(hmap::Terrain *new_p_target_terrain)
{
  this->p_target_terrain = new_p_target_terrain;
}

void ReceiveNode::update_tag_list(const std::vector<std::string> &new_tags)
{
  auto p_attr = this->get_attr_ref()->at("tag")->get_ref<attr::ChoiceAttribute>();

  // add option to select nothing
  std::vector<std::string> new_tags_mod = new_tags;
  new_tags_mod.emplace(new_tags_mod.begin(), "NO TAG");

  // --- update attribute

  p_attr->set_choice_list(new_tags_mod);

  // if the currently selected tag is not available anymore, set a dummy value
  std::string current_tag = p_attr->get_value();

  LOG->trace("ReceiveNode::update_tag_list: current tag value: {}", current_tag);

  if (std::find(new_tags_mod.begin(), new_tags_mod.end(), current_tag) ==
      new_tags_mod.end())
    p_attr->set_value(new_tags_mod.front());
}

} // namespace hesiod
