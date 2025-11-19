
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General Public
   License. The full license is in the file LICENSE, distributed with this software. */
#pragma once

namespace hesiod
{

class BaseNode; // forward

// specialized GUI to add on the node face in the node editor
void add_export_button(BaseNode &node);
void add_deprecated_warning_label(BaseNode &node, const std::string &msg = "");
void add_wip_warning_label(BaseNode &node);

void setup_background_image_for_cloud_attribute(BaseNode          &node,
                                                const std::string &attribute_key,
                                                const std::string &port_id);

void setup_histogram_for_range_attribute(BaseNode          &node,
                                         const std::string &attribute_key,
                                         const std::string &port_id);

} // namespace hesiod
