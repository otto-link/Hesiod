/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

WaveTriangular::WaveTriangular(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("WaveTriangular::WaveTriangular");

  // model
  this->node_caption = "WaveTriangular";

  // inputs
  this->input_captions = {"dr"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["kw"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, 0.f, 180.f);
  this->attr["slant_ratio"] = NEW_ATTR_FLOAT(0.2f, 0.f, 1.f);
  this->attr["phase_shift"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"kw",
                            "angle",
                            "slant_ratio",
                            "phase_shift",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(this->p_config);
    this->compute();
  }

  // documentation
  this->description = "WaveTriangular generates triangular waves.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (normal direction)."};
  this->output_descriptions = {"WaveTriangular heightmap."};

  this->attribute_descriptions["kw"] = "Noise wavenumbers (kx, ky) for each directions.";
  this->attribute_descriptions["angle"] = "Angle in the horizontal plane.";
  this->attribute_descriptions
      ["slant_ratio"] = "Decides on wave asymmetry, expected in [0, 1].";
  this->attribute_descriptions["phase_shift"] = "Phase shift.";
}

std::shared_ptr<QtNodes::NodeData> WaveTriangular::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void WaveTriangular::setInData(std::shared_ptr<QtNodes::NodeData> data,
                               QtNodes::PortIndex /* port_index */)
{
  if (!data)
  {
    QtNodes::PortIndex const out_port_index = 0;
    Q_EMIT this->dataInvalidated(out_port_index);
  }

  this->dr = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void WaveTriangular::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  // base noise function
  hmap::HeightMap *p_dr = HSD_GET_POINTER(this->dr);
  hmap::HeightMap *p_out = this->out->get_ref();

  hmap::fill(*p_out,
             p_dr,
             [this](hmap::Vec2<int> shape, hmap::Vec4<float> bbox, hmap::Array *p_noise)
             {
               return hmap::wave_triangular(shape,
                                            GET_ATTR_FLOAT("kw"),
                                            GET_ATTR_FLOAT("angle"),
                                            GET_ATTR_FLOAT("slant_ratio"),
                                            GET_ATTR_FLOAT("phase_shift"),
                                            p_noise,
                                            nullptr,
                                            nullptr,
                                            bbox);
             });

  // post-process
  post_process_heightmap(*p_out,
                         GET_ATTR_BOOL("inverse"),
                         false, // smooth
                         0,
                         false, // saturate
                         {0.f, 0.f},
                         0.f,
                         GET_ATTR_BOOL("remap"),
                         GET_ATTR_RANGE("remap_range"));

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
