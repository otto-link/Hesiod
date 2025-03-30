/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/receive_node.hpp"

namespace hesiod
{

void ReceiveNode::update_tag_list(const std::vector<std::string> &new_tags)
{
  auto p_attr = this->get_attr_ref()->at("tags")->get_ref<attr::ChoiceAttribute>();

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
