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
  std::string id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());
  LOG_DEBUG("uid: %s", uid.c_str());

  if (control_node_type == "BaseElevation")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewBaseElevation>(id,
                                                           this->shape,
                                                           this->tiling,
                                                           this->overlap);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "Blend")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewBlend>(
        id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "Debug")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewDebug>(
        id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "FbmPerlin")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewFbmPerlin>(id,
                                                       this->shape,
                                                       this->tiling,
                                                       this->overlap);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "GammaCorrection")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGammaCorrection>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "Perlin")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewPerlin>(
        id,
        this->shape,
        this->tiling,
        this->overlap);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "SmoothCpulse")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSmoothCpulse>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "GradientNorm")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGradientNorm>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "GradientTalus")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGradientTalus>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "HydraulicParticle")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewHydraulicParticle>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "Remap")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRemap>(
        id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "ValueNoiseDelaunay")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewValueNoiseDelaunay>(id,
                                                                this->shape,
                                                                this->tiling,
                                                                this->overlap);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "WhiteDensityMap")
  {
    id = control_node_type + "##" + uid;
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(id);

    this->add_node(p_view_node);
  }
  else if (control_node_type == "Worley")
  {
    id = control_node_type + "##" + uid;
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
