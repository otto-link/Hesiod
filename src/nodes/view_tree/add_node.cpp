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

void ViewTree::add_node(std::string control_node_type)
{
  std::string uid = this->get_new_id();

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());
  LOG_DEBUG("uid: %s", uid.c_str());

  if (control_node_type == "GammaCorrection")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::GammaCorrection>(
        id);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::GammaCorrection,
                                   hesiod::vnode::ViewGammaCorrection>(
            p_node.get());
  }
  else if (control_node_type == "GradientNorm")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::GradientNorm>(id);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::GradientNorm,
                                   hesiod::vnode::ViewGradientNorm>(
            p_node.get());
  }
  else if (control_node_type == "GradientTalus")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::GradientTalus>(id);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::GradientTalus,
                                   hesiod::vnode::ViewGradientTalus>(
            p_node.get());
  }
  else if (control_node_type == "Perlin")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::Perlin>(
        id,
        this->shape,
        this->tiling,
        this->overlap);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::Perlin,
                                   hesiod::vnode::ViewPerlin>(p_node.get());
  }
  else if (control_node_type == "Remap")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::Remap>(id);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::Remap,
                                   hesiod::vnode::ViewRemap>(p_node.get());
  }
  else if (control_node_type == "WhiteDensityMap")
  {
    std::string     id = control_node_type + "##" + uid;
    std::shared_ptr p_node = std::make_shared<hesiod::cnode::WhiteDensityMap>(
        id);

    this->p_control_tree->add_node(p_node);
    this->view_nodes_mapping[id] =
        generate_view_from_control<hesiod::cnode::WhiteDensityMap,
                                   hesiod::vnode::ViewWhiteDensityMap>(
            p_node.get());
  }
  else
  {
    LOG_ERROR("unknown node type: [%s]", control_node_type.c_str());
    throw std::runtime_error("unknown node type");
  }
}

void ViewTree::generate_view_node_from_control_node(std::string control_node_id)
{
  gnode::Node *p_cnode = this->p_control_tree->get_node_ref_by_id(
      control_node_id);

  LOG_DEBUG("control node [%s], type: %s",
            p_cnode->id.c_str(),
            p_cnode->get_node_type().c_str());

  if (p_cnode->get_node_type() == "GammaCorrection")
  {
    this->view_nodes_mapping[p_cnode->id] =
        generate_view_from_control<hesiod::cnode::GammaCorrection,
                                   hesiod::vnode::ViewGammaCorrection>(p_cnode);
  }
  else if (p_cnode->get_node_type() == "Perlin")
  {
    this->view_nodes_mapping[p_cnode->id] =
        generate_view_from_control<hesiod::cnode::Perlin,
                                   hesiod::vnode::ViewPerlin>(p_cnode);
  }
  else if (p_cnode->get_node_type() == "WhiteDensityMap")
  {
    this->view_nodes_mapping[p_cnode->id] =
        generate_view_from_control<hesiod::cnode::WhiteDensityMap,
                                   hesiod::vnode::ViewWhiteDensityMap>(p_cnode);
  }
  else
  {
    LOG_ERROR("unknown node type: [%s]", p_cnode->get_node_type().c_str());
    throw std::runtime_error("unknown node type");
  }
}

// HELPERS

template <class TControl, class TView>
std::shared_ptr<TView> generate_view_from_control(gnode::Node *p_gnode_node)
{
  TControl *p_control_node = (TControl *)p_gnode_node;
  return std::make_shared<TView>(p_control_node);
}

} // namespace hesiod::vnode
