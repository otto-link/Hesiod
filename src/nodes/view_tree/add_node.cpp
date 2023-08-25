/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "imnodes.h"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

std::string ViewTree::add_view_node(std::string control_node_type)
{
  std::string uid = this->get_new_id();
  std::string id = control_node_type + "##" + uid;
  ;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());
  LOG_DEBUG("uid: %s", uid.c_str());

  if (control_node_type == "BaseElevation")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewBaseElevation>(id,
                                                           this->shape,
                                                           this->tiling,
                                                           this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Blend")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewBlend>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Checkerboard")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewCheckerboard>(id,
                                                          this->shape,
                                                          this->tiling,
                                                          this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Clamp")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewClamp>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Debug")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewDebug>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ExpandShrink")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewExpandShrink>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "FbmPerlin")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewFbmPerlin>(id,
                                                       this->shape,
                                                       this->tiling,
                                                       this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Gain")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewGain>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "GammaCorrection")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGammaCorrection>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "GammaCorrectionLocal")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGammaCorrectionLocal>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Gradient")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewGradient>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "GradientNorm")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGradientNorm>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "GradientTalus")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGradientTalus>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "HydraulicParticle")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewHydraulicParticle>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "KmeansClustering2")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewKmeansClustering2>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Lerp")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewLerp>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Perlin")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewPerlin>(
        id,
        this->shape,
        this->tiling,
        this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Remap")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRemap>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Rugosity")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRugosity>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SmoothCpulse")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSmoothCpulse>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SteepenConvective")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSteepenConvective>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ValleyWidth")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewValleyWidth>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ValueNoiseDelaunay")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewValueNoiseDelaunay>(id,
                                                                this->shape,
                                                                this->tiling,
                                                                this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "WhiteDensityMap")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Worley")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewWorley>(
        id,
        this->shape,
        this->tiling,
        this->overlap);
    this->add_node(p_view_node);
  }
  else
  {
    LOG_ERROR("unknown node type: [%s]", control_node_type.c_str());
    throw std::runtime_error("unknown node type");
  }

  return id;
}

} // namespace hesiod::vnode
