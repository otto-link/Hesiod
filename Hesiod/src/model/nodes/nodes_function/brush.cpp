/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/colorize.hpp"
#include "highmap/heightmap.hpp"

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
  node.add_port<hmap::Heightmap>(gnode::PortType::IN, "background");
  node.add_port<hmap::Heightmap>(gnode::PortType::OUT, "out", CONFIG);

  // attribute(s)
  node.add_attr<ArrayAttribute>("hmap", "Heightmap", hmap::Vec2<int>(512, 512));

  // attribute(s) order
  node.set_attr_ordered_key({"hmap"});

  setup_post_process_heightmap_attributes(node);

  // set background image callback
  std::string port_id = "background";

  auto lambda = [&node, port_id]()
  {
    hmap::Heightmap *p_in = node.get_value_ref<hmap::Heightmap>(port_id);

    if (!p_in)
      return QImage();

    // generate a preview of the heightmap
    hmap::Vec2<int> shape_preview = hmap::Vec2<int>(256, 256);
    hmap::Array     array = p_in->to_array(shape_preview);

    std::vector<uint8_t> img(shape_preview.x * shape_preview.y);

    img = hmap::colorize(array, array.min(), array.max(), hmap::Cmap::MAGMA, false)
              .to_img_8bit();

    QImage tmp_image = QImage(img.data(),
                              shape_preview.x,
                              shape_preview.y,
                              QImage::Format_RGB888);
    return tmp_image.copy();
  };

  // assign function to attr
  GET_REF(node, "hmap", ArrayAttribute)->set_background_image_fct(lambda);
}

void compute_brush_node(BaseNode &node)
{
  Q_EMIT node.compute_started(node.get_id());

  Logger::log()->trace("computing node [{}]/[{}]", node.get_label(), node.get_id());

  // base noise function
  hmap::Heightmap *p_out = node.get_value_ref<hmap::Heightmap>("out");

  hmap::Array array = GET(node, "hmap", ArrayAttribute);
  p_out->from_array_interp(array);

  // post-process
  post_process_heightmap(node, *p_out);

  Q_EMIT node.compute_finished(node.get_id());
}

} // namespace hesiod
