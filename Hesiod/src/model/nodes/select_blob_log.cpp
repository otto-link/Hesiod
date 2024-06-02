/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/selector.hpp"

#include "hesiod/model/nodes.hpp"

namespace hesiod
{

SelectBlobLog::SelectBlobLog(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("SelectBlobLog::SelectBlobLog");

  // model
  this->node_caption = "SelectBlobLog";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["radius"] = NEW_ATTR_FLOAT(0.05f, 0.001f, 0.2f, "%.3f");
  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing"] = NEW_ATTR_BOOL(false);
  this->attr["smoothing_radius"] = NEW_ATTR_FLOAT(0.05f, 0.f, 0.2f, "%.2f");

  this->attr_ordered_key = {"radius",
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
  this->description = "SelectBlobLog performs 'blob' detection using oa Laplacian of "
                      "Gaussian (log) method. Blobs are areas in an image that are "
                      "either brighter or darker than the surrounding areas.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Mask heightmap (in [0, 1])."};

  this->attribute_descriptions
      ["radius"] = "Detection radius with respect to the domain size.";
}

std::shared_ptr<QtNodes::NodeData> SelectBlobLog::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void SelectBlobLog::setInData(std::shared_ptr<QtNodes::NodeData> data,
                              QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void SelectBlobLog::compute()
{
  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);

  if (p_in)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    int ir = std::max(1, (int)(GET_ATTR_FLOAT("radius") * p_out->shape.x));

    hmap::transform(*p_out,
                    *p_in,
                    [this, &ir](hmap::Array &array)
                    { return hmap::select_blob_log(array, ir); });

    p_out->smooth_overlap_buffers();

    // post-process
    ir = std::max(1, (int)(GET_ATTR_FLOAT("smoothing_radius") * p_out->shape.x));

    post_process_heightmap(*p_out,
                           GET_ATTR_BOOL("inverse"),
                           GET_ATTR_BOOL("smoothing"),
                           ir,
                           false, // saturate
                           {0.f, 0.f},
                           0.f,
                           true, // force remap
                           {0.f, 1.f});

    // propagate
    Q_EMIT this->computingFinished();
    this->trigger_outputs_updated();
  }
}

} // namespace hesiod
