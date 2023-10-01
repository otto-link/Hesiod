/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <functional>

#include "gnode.hpp"
#include "macrologger.h"

#include "hesiod/view_node.hpp"
#include "hesiod/view_tree.hpp"

namespace hesiod::vnode
{

std::string ViewTree::add_view_node(std::string control_node_type,
                                    std::string node_id)
{
  std::string id;
  if (node_id == "")
    id = control_node_type + "##" + this->get_new_id();
  else
    id = node_id;

  LOG_DEBUG("adding node type: %s", control_node_type.c_str());

  if (control_node_type == "AlterElevation")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewAlterElevation>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "BaseElevation")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewBaseElevation>(id,
                                                           this->shape,
                                                           this->tiling,
                                                           this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "BezierPath")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewBezierPath>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Blend")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewBlend>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Bump")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewBump>(
        id,
        this->shape,
        this->tiling,
        this->overlap);
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
  else if (control_node_type == "Clone")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewClone>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Cloud")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewCloud>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "CloudToArrayInterp")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewCloudToArrayInterp>(id,
                                                                this->shape,
                                                                this->tiling,
                                                                this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ConvolveSVD")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewConvolveSVD>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Debug")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewDebug>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "DigPath")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewDigPath>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Equalize")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewEqualize>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ExpandShrinkDirectional")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewExpandShrinkDirectional>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ExpandShrink")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewExpandShrink>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Export")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewExport>(
        id);
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
  else if (control_node_type == "FbmWorley")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewFbmWorley>(id,
                                                       this->shape,
                                                       this->tiling,
                                                       this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "FractalizePath")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewFractalizePath>(id);
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
  else if (control_node_type == "GaussianPulse")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewGaussianPulse>(id,
                                                           this->shape,
                                                           this->tiling,
                                                           this->overlap);
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
  else if (control_node_type == "HydraulicRidge")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewHydraulicRidge>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "HydraulicStream")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewHydraulicStream>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "HydraulicVpipes")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewHydraulicVpipes>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Import")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewImport>(id, shape, tiling, overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Kernel")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewKernel>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "KmeansClustering2")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewKmeansClustering2>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Laplace")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewLaplace>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Lerp")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewLerp>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "MakeBinary")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewMakeBinary>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "MeanderizePath")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewMeanderizePath>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "MinimumLocal")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewMinimumLocal>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "NormalDisplacement")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewNormalDisplacement>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "OneMinus")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewOneMinus>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Path")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewPath>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "PathFinding")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewPathFinding>(id);
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
  else if (control_node_type == "PerlinBillow")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewPerlinBillow>(id,
                                                          this->shape,
                                                          this->tiling,
                                                          this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Preview")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewPreview>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "RecastCanyon")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewRecastCanyon>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Recurve")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRecurve>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Remap")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRemap>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "RidgedPerlin")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewRidgedPerlin>(id,
                                                          this->shape,
                                                          this->tiling,
                                                          this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Rugosity")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewRugosity>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SedimentDeposition")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSedimentDeposition>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SelectEq")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewSelectEq>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SelectTransitions")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSelectTransitions>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SmoothCpulse")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSmoothCpulse>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SmoothFill")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSmoothFill>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "SteepenConvective")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewSteepenConvective>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Thermal")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewThermal>(
        id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ThermalAutoBedrock")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewThermalAutoBedrock>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "ThermalScree")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewThermalScree>(id);
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
  else if (control_node_type == "WaveSine")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewWaveSine>(
        id,
        this->shape,
        this->tiling,
        this->overlap);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "Warp")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewWarp>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "WarpDownslope")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewWarpDownslope>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "WhiteDensityMap")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewWhiteDensityMap>(id);
    this->add_node(p_view_node);
  }
  else if (control_node_type == "White")
  {
    std::shared_ptr p_view_node = std::make_shared<hesiod::vnode::ViewWhite>(
        id,
        this->shape,
        this->tiling,
        this->overlap);
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
  else if (control_node_type == "ZeroedEdges")
  {
    std::shared_ptr p_view_node =
        std::make_shared<hesiod::vnode::ViewZeroedEdges>(id);
    this->add_node(p_view_node);
  }
  else
  {
    LOG_ERROR("unknown node type: [%s]", control_node_type.c_str());
    throw std::runtime_error("adding an unknown node type");
  }

  return id;
}

} // namespace hesiod::vnode
