/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SelectAngle::SelectAngle(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("SelectAngle::SelectAngle");

  // model
  this->node_caption = "SelectAngle";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, 0.f, 360.f);
  this->attr["sigma"] = NEW_ATTR_FLOAT(90.f, 0.f, 180.f);
  this->attr["radius"] = NEW_ATTR_FLOAT(0.f, 0.f, 0.2f, "%.3f");

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key = {"angle",
                            "sigma",
                            "radius",
                            "_SEPARATOR_",
                            "inverse",
                            "smoothing",
                            "smoothing_radius"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "SelectAngle is a thresholding operator. It selects angle values "
                      "within a specified range defined by the shape of a Gaussian "
                      "pulse.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Mask heightmap (in [0, 1])."};

  this->attribute_descriptions["angle"] = "Selection center value.";
  this->attribute_descriptions["sigma"] = "Selection half-width.";
  this->attribute_descriptions["radius"] = "Pre-filter radius.";
}

std::shared_ptr<QtNodes::NodeData> SelectAngle::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SelectAngle::setInData(std::shared_ptr<QtNodes::NodeData> data,
                            QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void SelectAngle::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    int ir = (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x);

    hmap::transform(*p_out,
                    *p_in,
                    [this, &ir](hmap::Array &array) {
                      return select_angle(array,
                                          GET_ATTR_FLOAT("angle"),
                                          GET_ATTR_FLOAT("sigma"),
                                          ir);
                    });

    p_out->smooth_overlap_buffers();

    // post-process
    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           GET_ATTR_FLOAT("smoothing_radius"),
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           false, // remap
                           {0.f, 0.f});

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
