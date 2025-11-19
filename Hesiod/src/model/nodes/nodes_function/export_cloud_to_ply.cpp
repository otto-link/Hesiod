/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/export.hpp"
#include "highmap/geometry/cloud.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/base_node_gui.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"
#include "hesiod/model/utils.hpp"

using namespace attr;

namespace hesiod
{

void setup_export_cloud_to_ply_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::Cloud>(gnode::PortType::IN, "cloud");
  node.add_port<std::vector<float>>(gnode::PortType::IN, "point_data1");
  node.add_port<std::vector<float>>(gnode::PortType::IN, "point_data2");
  node.add_port<std::vector<float>>(gnode::PortType::IN, "point_data3");

  // attribute(s)
  ADD_ATTR(node,
           FilenameAttribute,
           "fname",
           std::filesystem::path("points.ply"),
           "Stanford PLY (*.ply)",
           true);
  ADD_ATTR(node, BoolAttribute, "auto_export", false);
  node.add_attr<StringAttribute>("label1", "", "data1");
  node.add_attr<StringAttribute>("label2", "", "data2");
  node.add_attr<StringAttribute>("label3", "", "data3");

  ADD_ATTR(node, FloatAttribute, "xmin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "xmax", 1.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "ymin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "ymax", 1.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "zmin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(node, FloatAttribute, "zmax", 1.f, -FLT_MAX, FLT_MAX);

  // attribute(s) order
  node.set_attr_ordered_key({"_GROUPBOX_BEGIN_Main Parameters",
                             "fname",
                             "auto_export",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Custom data labels",
                             "label1",
                             "label2",
                             "label3",
                             "_GROUPBOX_END_",
                             //
                             "_GROUPBOX_BEGIN_Rescaling",
                             "xmin",
                             "xmax",
                             "ymin",
                             "ymax",
                             "zmin",
                             "zmax",
                             "_GROUPBOX_END_"});

  // specialized GUI
  add_export_button(node);
}

void compute_export_cloud_to_ply_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  hmap::Cloud *p_in = node.get_value_ref<hmap::Cloud>("cloud");

  if (p_in && GET(node, "auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET(node, "fname", FilenameAttribute);
    fname = ensure_extension(fname, ".ply");

    // --- create custom fields

    std::map<std::string, std::vector<float>> custom_fields = {};

    std::vector<std::string> labels = {"label1", "label2", "label3"};

    std::vector<std::vector<float> *> data_ptrs = {
        node.get_value_ref<std::vector<float>>("point_data1"),
        node.get_value_ref<std::vector<float>>("point_data2"),
        node.get_value_ref<std::vector<float>>("point_data3")};

    for (size_t k = 0; k < labels.size(); ++k)
    {
      if (data_ptrs[k])
        custom_fields[GET(node, labels[k], StringAttribute)] = *data_ptrs[k];
    }

    // --- export

    auto xr = hmap::rescaled_vector(p_in->get_x(),
                                    GET(node, "xmin", FloatAttribute),
                                    GET(node, "xmax", FloatAttribute));
    auto yr = hmap::rescaled_vector(p_in->get_y(),
                                    GET(node, "ymin", FloatAttribute),
                                    GET(node, "ymax", FloatAttribute));
    auto zr = hmap::rescaled_vector(p_in->get_values(),
                                    GET(node, "zmin", FloatAttribute),
                                    GET(node, "zmax", FloatAttribute));

    hmap::export_points_to_ply(fname.string(), xr, yr, zr, custom_fields);
  }

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
