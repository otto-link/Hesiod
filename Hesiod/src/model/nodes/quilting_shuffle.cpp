/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/quilting_shuffle.hpp"


namespace hesiod
{

QuiltingShuffle::QuiltingShuffle(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("QuiltingShuffle::QuiltingShuffle");

  // model
  this->node_caption = "QuiltingShuffle";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["patch_width"] = NEW_ATTR_FLOAT(0.3f, 0.1f, 1.f);
  this->attr["overlap"] = NEW_ATTR_FLOAT(0.9f, 0.05f, 0.95f);
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["patch_flip"] = NEW_ATTR_BOOL(true);
  this->attr["patch_rotate"] = NEW_ATTR_BOOL(true);
  this->attr["patch_transpose"] = NEW_ATTR_BOOL(true);
  this->attr["filter_width_ratio"] = NEW_ATTR_FLOAT(0.5f, 0.f, 1.f);

  this->attr_ordered_key = {"patch_width",
                            "overlap",
                            "seed",
                            "patch_flip",
                            "patch_rotate",
                            "patch_transpose",
                            "filter_width_ratio"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "QuiltingShuffle performs a quilting procedure and generates a new "
                      "image by seamlessly stitching together patches from an existing "
                      "image.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Synthetized heightmap."};

  this->attribute_descriptions
      ["patch_width"] = "Decide on the size (with respect to the domain size) of the "
                        "patches that will be used to create the new heightmap. This "
                        "determines the level of detail and texture in the final result.";
  this->attribute_descriptions["overlap"] = "Overlap ratio between patches.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["patch_flip"] = "Allow patch flipping.";
  this->attribute_descriptions["patch_rotate"] = "Allow patch 90 degree rotation.";
  this->attribute_descriptions["patch_transpose"] = "Allow patch tranposition.";
  this->attribute_descriptions
      ["filter_width_ratio"] = "Smooth filter width with respect the overlap length.";
}

std::shared_ptr<QtNodes::NodeData> QuiltingShuffle::outData(
    QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void QuiltingShuffle::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void QuiltingShuffle::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    int ir = std::max(1, (int)(GET_ATTR_FLOAT("patch_width") * p_out->shape.x));
    hmap::Vec2<int> patch_base_shape = hmap::Vec2<int>(ir, ir);

    // --- work on a single array (i.e. not-tiled algo)

    hmap::Array in_array = p_in->to_array();
    hmap::Array out_array = hmap::Array(p_out->shape);

    out_array = hmap::quilting_shuffle(in_array,
                                       patch_base_shape,
                                       GET_ATTR_FLOAT("overlap"),
                                       GET_ATTR_SEED("seed"),
                                       GET_ATTR_BOOL("patch_flip"),
                                       GET_ATTR_BOOL("patch_rotate"),
                                       GET_ATTR_BOOL("patch_transpose"),
                                       GET_ATTR_FLOAT("filter_width_ratio"));

    p_out->from_array_interp_nearest(out_array);
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
