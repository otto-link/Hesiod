/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/geometry/path.hpp"

#include "attributes.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_path_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<hmap::Path>(gnode::PortType::IN, "input");

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path("path.csv"),
           "CSV (*.csv)",
           true);
  ADD_ATTR(BoolAttribute, "auto_export", false);

  // attribute(s) order
  p_node->set_attr_ordered_key({"fname", "auto_export"});

  // specialized GUI
  add_export_button(p_node);
}

void compute_export_path_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  hmap::Path *p_in = p_node->get_value_ref<hmap::Path>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET("fname", FilenameAttribute);
    fname = ensure_extension(fname, ".csv");

    p_in->to_csv(fname.string());
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
