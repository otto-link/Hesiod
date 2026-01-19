/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/colorize.hpp"
#include "highmap/operator.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes/base_node.hpp"
#include "highmap/virtual_array/virtual_array.hpp"

using namespace attr;

namespace hesiod
{

void setup_background_image_for_cloud_attribute(BaseNode          &node,
                                                const std::string &attribute_key,
                                                const std::string &port_id)
{
  Logger::log()->trace("setup_background_image_for_cloud_attribute: node {}",
                       node.get_label());

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
    return tmp_image.copy();
  };

  // assign function to attr
  node.get_attr_ref<CloudAttribute>(attribute_key)->set_background_image_fct(lambda);
}

} // namespace hesiod
