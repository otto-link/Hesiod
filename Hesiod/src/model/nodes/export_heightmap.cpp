/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include "highmap/export.hpp"
#include "highmap/heightmap.hpp"

#include "attributes.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/enum_mapping.hpp"
#include "hesiod/model/nodes/export.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

ExportHeightmap::ExportHeightmap(std::shared_ptr<ModelConfig> config)
    : BaseNode("ExportHeightmap", config)
{
  LOG->trace("ExportHeightmap::ExportHeightmap");

  // input port(s)
  this->add_port<hmap::HeightMap>(gnode::PortType::IN, "input");

  // output port(s)

  // attribute(s)
  this->attr["fname"] = NEW(FilenameAttribute, "hmap.png", "", "Filename");
  this->attr["format"] = NEW(MapEnumAttribute,
                             heightmap_export_format_map,
                             "Export format");
  this->attr["auto_export"] = NEW(BoolAttribute, true, "Automatic export");

  this->attr_ordered_key = {"fname", "format", "auto_export"};
}

void ExportHeightmap::compute()
{
  Q_EMIT this->compute_started(this->get_id());

  LOG->trace("computing node {}", this->get_label());

  hmap::HeightMap *p_in = this->get_value_ref<hmap::HeightMap>("input");

  if (p_in && GET("auto_export", BoolAttribute))
  {
    std::string fname = GET("fname", FilenameAttribute);

    switch (GET("format", MapEnumAttribute))
    {
    case ExportFormat::PNG8BIT:
      p_in->to_array().to_png_grayscale(fname, CV_8U);
      break;

    case ExportFormat::PNG16BIT:
      p_in->to_array().to_png_grayscale(fname, CV_16U);
      break;

    case ExportFormat::RAW16BIT:
      p_in->to_array().to_raw_16bit(fname);
      break;
    }
  }

  Q_EMIT this->compute_finished(this->get_id());
}

} // namespace hesiod
