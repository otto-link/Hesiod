/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/nodes/clamp.hpp"


namespace hesiod
{

Clamp::Clamp(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG_DEBUG("Clamp::Clamp");

  // model
  this->node_caption = "Clamp";

  // inputs
  this->input_captions = {"input"};
  this->input_types = {HeightMapData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["clamp"] = NEW_ATTR_RANGE();
  this->attr["smooth_min"] = NEW_ATTR_BOOL(false);
  this->attr["k_min"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 1.f);
  this->attr["smooth_max"] = NEW_ATTR_BOOL(false);
  this->attr["k_max"] = NEW_ATTR_FLOAT(0.05f, 0.01f, 1.f);

  this->attr_ordered_key = {"clamp", "smooth_min", "k_min", "smooth_max", "k_max"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Clamp restrict a value within a specified range. Essentially, it "
                      "ensures that a value does not exceed a defined upper limit or "
                      "fall below a defined lower limit.";

  this->input_descriptions = {"Input heightmap."};
  this->output_descriptions = {"Clamped heightmap."};

  this->attribute_descriptions["clamp"] = "Clamping range.";
  this->attribute_descriptions
      ["smooth_min"] = "Activate smooth clamping for the lower bound.";
  this->attribute_descriptions["k_min"] = "Lower bound smoothing intensity.";
  this->attribute_descriptions
      ["smooth_max"] = "Activate smooth clamping for the upper bound.";
  this->attribute_descriptions["k_max"] = "Upper bound smoothing intensity.";
}

std::shared_ptr<QtNodes::NodeData> Clamp::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Clamp::setInData(std::shared_ptr<QtNodes::NodeData> data,
                      QtNodes::PortIndex /* port_index */)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  this->in = std::dynamic_pointer_cast<HeightMapData>(data);

  this->compute();
}

// --- computing

void Clamp::compute()
{
  Q_EMIT this->computingStarted();

  LOG_DEBUG("computing node [%s]", this->name().toStdString().c_str());

  hmap::HeightMap *p_in = HSD_GET_POINTER(this->in);
  hmap::HeightMap *p_out = this->out->get_ref();

  if (p_in)
  {
    // copy the input heightmap
    *p_out = *p_in;

    // retrieve parameters
    hmap::Vec2<float> crange = GET_ATTR_RANGE("clamp");
    bool              smooth_min = GET_ATTR_BOOL("smooth_min");
    bool              smooth_max = GET_ATTR_BOOL("smooth_max");
    float             k_min = GET_ATTR_FLOAT("k_min");
    float             k_max = GET_ATTR_FLOAT("k_max");

    // compute
    if (!smooth_min && !smooth_max)
    {
      hmap::transform(*p_out,
                      [&crange](hmap::Array &x) { hmap::clamp(x, crange.x, crange.y); });
    }
    else
    {
      if (smooth_min)
        hmap::transform(*p_out,
                        [&crange, &k_min](hmap::Array &x)
                        { hmap::clamp_min_smooth(x, crange.x, k_min); });
      else
        hmap::transform(*p_out,
                        [&crange](hmap::Array &x) { hmap::clamp_min(x, crange.x); });

      if (smooth_max)
        hmap::transform(*p_out,
                        [&crange, &k_max](hmap::Array &x)
                        { hmap::clamp_max_smooth(x, crange.y, k_max); });
      else
        hmap::transform(*p_out,
                        [&crange](hmap::Array &x) { hmap::clamp_max(x, crange.y); });
    }
  }

  // propagate
  Q_EMIT this->computingFinished();
  this->trigger_outputs_updated();
}

} // namespace hesiod
