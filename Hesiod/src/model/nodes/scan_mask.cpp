/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

ScanMask::ScanMask(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("ScanMask::ScanMask");

  // model
  this->node_caption = "ScanMask";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["contrast"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);
  this->attr["brightness"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "ScanMask adjusts the brightness and contrast of an input mask.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Adjusted heightmap."};

  this->attribute_descriptions["brightness"] = "Brightness adjustment.";
  this->attribute_descriptions["contrast"] = "Contrast adjustment.";
}

std::shared_ptr<QtNodes::NodeData> ScanMask::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void ScanMask::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void ScanMask::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    // copy the input heightmap
    *p_out = *p_in;

    hmap::transform(*p_out,
                    [this](hmap::Array &x) {
                      x = hmap::scan_mask(x,
                                          GET_ATTR_FLOAT("brightness"),
                                          GET_ATTR_FLOAT("contrast"));
                    });

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
