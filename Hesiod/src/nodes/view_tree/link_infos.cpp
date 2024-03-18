/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

LinkInfos::LinkInfos(){};

LinkInfos::LinkInfos(std::string node_id_from,
                     std::string port_id_from,
                     int         port_hash_id_from,
                     std::string node_id_to,
                     std::string port_id_to,
                     int         port_hash_id_to)
    : node_id_from(node_id_from), port_id_from(port_id_from),
      port_hash_id_from(port_hash_id_from), node_id_to(node_id_to),
      port_id_to(port_id_to), port_hash_id_to(port_hash_id_to)
{
  LOG_DEBUG("new link [%s]/[%s] to [%s]/[%s]",
            node_id_from.c_str(),
            port_id_from.c_str(),
            node_id_to.c_str(),
            port_id_to.c_str());
}

} // namespace hesiod::vnode
