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

void setup_export_points_to_ply_node(BaseNode *p_node)
{
  Logger::log()->trace("setup node {}", p_node->get_label());

  // port(s)
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "x");
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "y");
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "z");
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "point_data1");
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "point_data2");
  p_node->add_port<std::vector<float>>(gnode::PortType::IN, "point_data3");

  // attribute(s)
  ADD_ATTR(FilenameAttribute,
           "fname",
           std::filesystem::path("points.ply"),
           "Stanford PLY (*.ply)",
           true);
  ADD_ATTR(BoolAttribute, "auto_export", false);

  p_node->add_attr<StringAttribute>("label1", "", "data1");
  p_node->add_attr<StringAttribute>("label2", "", "data2");
  p_node->add_attr<StringAttribute>("label3", "", "data3");

  ADD_ATTR(FloatAttribute, "xmin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "xmax", 1.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "ymin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "ymax", 1.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "zmin", 0.f, -FLT_MAX, FLT_MAX);
  ADD_ATTR(FloatAttribute, "zmax", 1.f, -FLT_MAX, FLT_MAX);

  // attribute(s) order
  p_node->set_attr_ordered_key({"_GROUPBOX_BEGIN_Main parameters",
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
  add_export_button(p_node);
}

void compute_export_points_to_ply_node(BaseNode *p_node)
{
  Q_EMIT p_node->compute_started(p_node->get_id());

  Logger::log()->trace("computing node [{}]/[{}]", p_node->get_label(), p_node->get_id());

  auto p_x = p_node->get_value_ref<std::vector<float>>("x");
  auto p_y = p_node->get_value_ref<std::vector<float>>("y");
  auto p_z = p_node->get_value_ref<std::vector<float>>("z");

  if (p_x && p_y && p_z && GET("auto_export", BoolAttribute))
  {
    std::filesystem::path fname = GET("fname", FilenameAttribute);
    fname = ensure_extension(fname, ".ply");

    // --- create custom fields

    std::map<std::string, std::vector<float>> custom_fields = {};

    std::vector<std::string> labels = {"label1", "label2", "label3"};

    std::vector<std::vector<float> *> data_ptrs = {
        p_node->get_value_ref<std::vector<float>>("point_data1"),
        p_node->get_value_ref<std::vector<float>>("point_data2"),
        p_node->get_value_ref<std::vector<float>>("point_data3")};

    for (size_t k = 0; k < labels.size(); ++k)
    {
      if (data_ptrs[k])
        custom_fields[GET(labels[k], StringAttribute)] = *data_ptrs[k];
    }

    // --- export

    auto xr = hmap::rescaled_vector(*p_x,
                                    GET("xmin", FloatAttribute),
                                    GET("xmax", FloatAttribute));
    auto yr = hmap::rescaled_vector(*p_y,
                                    GET("ymin", FloatAttribute),
                                    GET("ymax", FloatAttribute));
    auto zr = hmap::rescaled_vector(*p_z,
                                    GET("zmin", FloatAttribute),
                                    GET("zmax", FloatAttribute));

    hmap::export_points_to_ply(fname.string(), xr, yr, zr, custom_fields);
  }

  Q_EMIT p_node->compute_finished(p_node->get_id());
}

} // namespace hesiod
