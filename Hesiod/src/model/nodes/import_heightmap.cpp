/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <fstream>

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ImportHeightmap::ImportHeightmap(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ImportHeightmap::ImportHeightmap");

  // model
  this->node_caption = "ImportHeightmap";

  // inputs
  this->input_captions = {};
  this->input_types = {};

  // outputs
  this->output_captions = {"texture"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["fname"] = NEW_ATTR_FILENAME("", "TIFF (*.png)");
  this->attr["remap"] = NEW_ATTR_BOOL(true);

  this->attr_ordered_key = {"fname", "remap"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ImportHeightmap imports an heighmap from a grayscale PNG 8bit "
                      "file.";

  this->input_descriptions = {};
  this->output_descriptions = {"Heightmap."};

  this->attribute_descriptions["fname"] = "Import file name.";
  this->attribute_descriptions["remap"] = "Remap imported heightmap elevation to [0, 1].";
}

std::shared_ptr<QtNodes::NodeData> ImportHeightmap::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ImportHeightmap::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                                QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void ImportHeightmap::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_out = this->out->get_ref();

  std::ifstream f(GET_ATTR_FILENAME("fname").c_str());
  if (f.good())
  {
    hmap::Array z = hmap::Array(GET_ATTR_FILENAME("fname"));
    p_out->from_array_interp(z);

    if (GET_ATTR_BOOL("remap"))
      p_out->remap();
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
