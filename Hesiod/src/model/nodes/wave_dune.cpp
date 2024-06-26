/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

WaveDune::WaveDune(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("WaveDune::WaveDune");

  // model
  this->node_caption = "WaveDune";

  // inputs
  this->input_captions = {"dr"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["kw"] = NEW_ATTR_FLOAT(2.f, 0.01f, 32.f);
  this->attr["angle"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);
  this->attr["xtop"] = NEW_ATTR_FLOAT(0.7f, 0.f, 1.f);
  this->attr["xbottom"] = NEW_ATTR_FLOAT(0.9f, 0.f, 1.f);
  this->attr["phase_shift"] = NEW_ATTR_FLOAT(0.f, -180.f, 180.f);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"kw",
                            "angle",
                            "xtop",
                            "xbottom",
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
  this->description = "WaveDune mimics using a periodic function the formation and "
                      "spatial distribution of transverse sand dunes on a terrain.";

  this->input_descriptions = {
      "Displacement with respect to the domain size (normal direction)."};
  this->output_descriptions = {"WaveDune heightmap."};

  this->attribute_descriptions["kw"] = "Noise wavenumbers (kx, ky) for each directions.";
  this->attribute_descriptions["angle"] = "Angle in the horizontal plane.";
  this->attribute_descriptions["xtop"] = "Relative position of the dune top, in [0, 1].";
  this->attribute_descriptions
      ["xbottom"] = "Relative position of the dune bottom, in [0, 1].";
  this->attribute_descriptions["phase_shift"] = "Phase shift.";
}

std::shared_ptr<QtNodes::NodeData> WaveDune::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void WaveDune::setInData(std::shared_ptr<QtNodes::NodeData> data,
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

void WaveDune::compute()
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
               return hmap::wave_dune(shape,
                                      GET_ATTR_FLOAT("kw"),
                                      GET_ATTR_FLOAT("angle"),
                                      GET_ATTR_FLOAT("xtop"),
                                      GET_ATTR_FLOAT("xbottom"),
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
