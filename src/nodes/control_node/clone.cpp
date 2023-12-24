/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

Clone::Clone(std::string id) : ControlNode(id)
{
  LOG_DEBUG("Unary::Unary()");
  this->node_type = "Clone";
  this->category = category_mapping.at(this->node_type);
  this->add_port(gnode::Port("input", gnode::direction::in, dtype::dHeightMap));
  this->add_port(gnode::Port("thru##" + std::to_string(this->id_count++),
                             gnode::direction::out,
                             dtype::dHeightMap));
  this->update_inner_bindings();
}

void Clone::remove_unused_outputs()
{
  // --- make sure there is always only one available output ready for
  // connection
  this->n_outputs = this->get_nports_by_direction(gnode::direction::out);
  int n_connected_outputs = this->get_nports_by_direction(
      gnode::direction::out,
      false, // do not skip optional
      true); // skip unconnected

  if (this->n_outputs > n_connected_outputs)
  {
    // unconnected outputs are removed until there remains only one
    // unconnected output
    int                      count = 0;
    std::vector<std::string> port_id_to_remove = {};
    for (auto &[port_id, port] : this->get_ports())
    {
      if (count > this->n_outputs - n_connected_outputs - 2)
        break;

      if (port.direction == gnode::direction::out && !port.is_connected)
      {
        port_id_to_remove.push_back(port_id);
        count++;
      }
    }

    for (auto &port_id : port_id_to_remove)
      this->remove_port(port_id);
  }

  this->n_outputs = this->get_nports_by_direction(gnode::direction::out);
}

void Clone::update_inner_bindings()
{
  LOG_DEBUG("inner bindings [%s]", this->id.c_str());

  this->n_outputs = this->get_nports_by_direction(gnode::direction::out);
  int n_connected_outputs = this->get_nports_by_direction(
      gnode::direction::out,
      false, // do not skip optional
      true); // skip unconnected

  if (this->n_outputs == n_connected_outputs)
    this->add_port(gnode::Port("thru##" + std::to_string(this->id_count++),
                               gnode::direction::out,
                               dtype::dHeightMap));

  // input is passed as a reference to the output(s)
  for (auto &[port_id, port] : this->get_ports())
    if (port.direction == gnode::direction::out)
      this->set_p_data(port_id, (void *)&(this->value_out));

  this->n_outputs = this->get_nports_by_direction(gnode::direction::out);
}

void Clone::compute()
{
  LOG_DEBUG("computing node [%s]", this->id.c_str());
  // just make a copy of the input
  hmap::HeightMap *p_input_hmap = CAST_PORT_REF(hmap::HeightMap, "input");

  this->value_out.set_sto(p_input_hmap->shape,
                          p_input_hmap->tiling,
                          p_input_hmap->overlap);

  this->value_out = *p_input_hmap;
}

} // namespace hesiod::cnode
