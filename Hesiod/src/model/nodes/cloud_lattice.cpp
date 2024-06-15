/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

CloudLattice::CloudLattice(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("CloudLattice::CloudLattice");

  // model
  this->node_caption = "CloudLattice";

  // inputs
  this->input_types = {};
  this->input_captions = {};

  // outputs
  this->output_captions = {"cloud"};
  this->output_types = {CloudData().type()};

  // attributes
  this->attr["delta"] = NEW_ATTR_WAVENB(hmap::Vec2<float>(0.1f, 0.1f),
                                        1e-2f,
                                        0.2f,
                                        "%.2f");
  this->attr["stagger_ratio"] = NEW_ATTR_WAVENB(hmap::Vec2<float>(0.f, 0.f),
                                                0.f,
                                                1.f,
                                                "%.3f");
  this->attr["jitter_ratio"] = NEW_ATTR_WAVENB(hmap::Vec2<float>(0.1f, 0.1f),
                                               0.f,
                                               1.f,
                                               "%.3f");
  this->attr["seed"] = NEW_ATTR_SEED();

  this->attr["remap"] = NEW_ATTR_BOOL(false);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"delta",
                            "stagger_ratio",
                            "jitter_ratio",
                            "seed",
                            "_SEPARATOR_",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<CloudData>();
    this->compute();
  }

  // documentation
  this->description = "Generate a grid lattice set of points.";

  this->input_descriptions = {};
  this->output_descriptions = {"Set of points (x, y) and elevations z."};

  this->attribute_descriptions["delta"] = "Point spacing in x and y directions.";
  this->attribute_descriptions["stagger_ratio"] = "Point offset in x and y directions "
                                                  "for every two lines or columns.";
  this->attribute_descriptions
      ["jitter_ratio"] = "Point jittering (noise) in x and y directions.";
  this->attribute_descriptions["seed"] = "Random seed number";
}

std::shared_ptr<QtNodes::NodeData> CloudLattice::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void CloudLattice::setInData(std::shared_ptr<QtNodes::NodeData> /* data */,
                             QtNodes::PortIndex /* port_index */)
{
}

// --- computing

void CloudLattice::compute()
{
  Q_EMIT this->computingStarted();

  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Cloud *p_out = this->out->get_ref();

  std::vector<float> x, y, v;
  hmap::Vec4<float>  bbox = {0.f, 1.f, 0.f, 1.f};

  hmap::random_grid(x,
                    y,
                    v,
                    GET_ATTR_SEED("seed"),
                    GET_ATTR_WAVENB("delta"),
                    GET_ATTR_WAVENB("stagger_ratio"),
                    GET_ATTR_WAVENB("jitter_ratio"),
                    bbox);

  *p_out = hmap::Cloud(x, y, v);

  if (GET_ATTR_BOOL("remap"))
    p_out->remap_values(GET_ATTR_RANGE("remap_range").x, GET_ATTR_RANGE("remap_range").y);

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
