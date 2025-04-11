/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/utils.hpp"

namespace hesiod
{

bool get_ids_from_broadcast_tag(const std::string &tag,
                                std::string       &graph_id,
                                std::string       &node_id,
                                std::string       &port_id)
{
  std::vector<std::string> tag_elements = split_string(tag, '/');

  // not good... or "NO BACKGROUND" tag for example
  if (tag_elements.size() != 3)
    return false;

  graph_id = tag_elements[0];
  node_id = tag_elements[1];
  port_id = tag_elements[2];

  return true;
}

} // namespace hesiod
