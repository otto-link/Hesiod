/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/colorize.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "hesiod/model/nodes/post_process.hpp"

using namespace attr;

namespace hesiod
{

void setup_brush_node(BaseNode &node)
{
  Logger::log()->trace("setup node {}", node.get_label());

  // port(s)
  node.add_port<hmap::VirtualArray>(gnode::PortType::IN, "background");
  node.add_port<hmap::VirtualArray>(gnode::PortType::OUT, "out", CONFIG(node));

  // attribute(s)
  node.add_attr<ArrayAttribute>("hmap", "Heightmap", glm::ivec2(512, 512));

  // attribute(s) order
  node.set_attr_ordered_key({"hmap"});

  setup_post_process_heightmap_attributes(node,
                                          {.add_mix = true, .remap_active_state = true});

  // set background image callback
  std::string port_id = "background";

  auto lambda = [&node, port_id]()
  {
    hmap::VirtualArray *p_in = node.get_value_ref<hmap::VirtualArray>(port_id);

    if (!p_in)
      return QImage();

    // generate a preview of the heightmap
    glm::ivec2  shape_preview = glm::ivec2(256, 256);
    hmap::Array array = p_in->to_array(shape_preview, node.cfg().cm_cpu);

    std::vector<uint8_t> img(shape_preview.x * shape_preview.y);

    img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
              .to_img_8bit();

    QImage tmp_image = QImage(img.data(),
                              shape_preview.x,
                              shape_preview.y,
                              QImage::Format_RGB888);
    return tmp_image.copy().mirrored(false, true);
  };

  // assign function to attr
  node.get_attr_ref<ArrayAttribute>("hmap")->set_background_image_fct(lambda);
}

void compute_brush_node(BaseNode &node)
{
  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::VirtualArray *p_out = node.get_value_ref<hmap::VirtualArray>("out");

  // retrieve raw data and convert them to an hmap::Array
  std::vector<float> data = node.get_attr_ref<ArrayAttribute>("hmap")->get_value();
  glm::vec2          shape = node.get_attr_ref<ArrayAttribute>("hmap")->get_shape();

  hmap::Array array(shape);
  array.vector = std::move(data);
  array = array.resample_to_shape_bilinear(node.get_config_ref()->shape);

  // Array -> VirtualArray
  p_out->from_array(array, node.cfg().cm_cpu);

  // post-process
  post_process_heightmap(node, *p_out);
}

} // namespace hesiod
