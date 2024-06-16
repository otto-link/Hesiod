/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/logger.hpp"
#include "hesiod/model/nodes.hpp"

namespace hesiod
{

Stamping::Stamping(const ModelConfig *p_config) : BaseNode(p_config)
{
  LOG->trace("Stamping::Stamping");

  // model
  this->node_caption = "Stamping";

  // inputs
  this->input_captions = {"cloud", "kernel"};
  this->input_types = {CloudData().type(), KernelData().type()};

  // outputs
  this->output_captions = {"output"};
  this->output_types = {HeightMapData().type()};

  // attributes
  this->attr["kernel_radius"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 0.5f, "%.2f");
  this->attr["kernel_scale_radius"] = NEW_ATTR_BOOL(false);
  this->attr["kernel_scale_amplitude"] = NEW_ATTR_BOOL(true);
  this->attr["blend_method"] = NEW_ATTR_MAPENUM(stamping_blend_method_map, "maximum");
  this->attr["seed"] = NEW_ATTR_SEED();
  this->attr["k_smoothing"] = NEW_ATTR_FLOAT(0.1f, 0.01f, 1.f);
  this->attr["kernel_flip"] = NEW_ATTR_BOOL(false);
  this->attr["kernel_rotate"] = NEW_ATTR_BOOL(false);

  this->attr["inverse"] = NEW_ATTR_BOOL(false);
  this->attr["remap"] = NEW_ATTR_BOOL(true);
  this->attr["remap_range"] = NEW_ATTR_RANGE();

  this->attr_ordered_key = {"kernel_radius",
                            "kernel_scale_radius",
                            "kernel_scale_amplitude",
                            "blend_method",
                            "seed",
                            "k_smoothing",
                            "kernel_flip",
                            "kernel_rotate",
                            "_SEPARATOR_",
                            "inverse",
                            "remap",
                            "remap_range"};

  // update
  if (this->p_config->compute_nodes_at_instanciation)
  {
    this->out = std::make_shared<HeightMapData>(p_config);
    this->compute();
  }

  // documentation
  this->description = "Stamping .";

  this->input_descriptions = {"Stamping locations and intensities (as a Cloud).",
                              "Kernel to be stamped."};
  this->output_descriptions = {"Output heightmap."};

  this->attribute_descriptions
      ["kernel_radius"] = "Kernel base radius, with respect a unit square domain.";
  this->attribute_descriptions["kernel_scale_radius"] =
      "Determine whether the kernel radius is scaled with the point values.";
  this->attribute_descriptions["kernel_scale_amplitude"] =
      "Determine whether the kernel amplitude is scaled with the point values.";
  this->attribute_descriptions["blend_method"] = "Blending method.";
  this->attribute_descriptions["seed"] = "Random seed number.";
  this->attribute_descriptions["k_smoothing"] = "Smoothing parameter (if any).";
  this->attribute_descriptions["kernel_flip"] = "Randomly flip, or not, the kernel "
                                                "before stamping (includes tranposing).";
  this->attribute_descriptions
      ["kernel_rotate"] = "Randomly rotate, or not, the kernel before stamping (can be "
                          "any rotation angle, can also be ressource consuming).";
}

std::shared_ptr<QtNodes::NodeData> Stamping::outData(QtNodes::PortIndex /* port_index */)
{
  return std::static_pointer_cast<QtNodes::NodeData>(this->out);
}

void Stamping::setInData(std::shared_ptr<QtNodes::NodeData> data,
                         QtNodes::PortIndex                 port_index)
{
  if (!data)
    Q_EMIT this->dataInvalidated(0);

  switch (port_index)
  {
  case 0:
    this->cloud = std::dynamic_pointer_cast<CloudData>(data);
    break;
  case 1:
    this->kernel = std::dynamic_pointer_cast<KernelData>(data);
  }

  this->compute();
}

// --- computing

void Stamping::compute()
{
  LOG->trace("computing node {}", this->name().toStdString());

  hmap::Cloud *p_cloud = HSD_GET_POINTER(this->cloud);
  hmap::Array *p_kernel = HSD_GET_POINTER(this->kernel);

  if (p_cloud && p_kernel)
  {
    Q_EMIT this->computingStarted();

    hmap::HeightMap *p_out = this->out->get_ref();

    std::vector<float> xp = p_cloud->get_x();
    std::vector<float> yp = p_cloud->get_y();
    std::vector<float> zp = p_cloud->get_values();

    int  ir = std::max(1, (int)(GET_ATTR_FLOAT("kernel_radius") * p_out->shape.x));
    uint seed = GET_ATTR_SEED("seed");

    hmap::fill(*p_out,
               [this, &xp, &yp, &zp, p_kernel, ir, &seed](hmap::Vec2<int>   shape,
                                                          hmap::Vec4<float> bbox)
               {
                 return stamping(
                     shape,
                     xp,
                     yp,
                     zp,
                     *p_kernel,
                     ir,
                     GET_ATTR_BOOL("kernel_scale_radius"),
                     GET_ATTR_BOOL("kernel_scale_amplitude"),
                     (hmap::StampingBlendMethod)GET_ATTR_MAPENUM("blend_method"),
                     seed++,
                     GET_ATTR_FLOAT("k_smoothing"),
                     GET_ATTR_BOOL("kernel_flip"),
                     GET_ATTR_BOOL("kernel_rotate"),
                     bbox);
               });

    p_out->smooth_overlap_buffers();

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
}

} // namespace hesiod
