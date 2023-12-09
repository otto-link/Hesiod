/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

SmoothFillSmearPeaks::SmoothFillSmearPeaks(std::string id)
    : ControlNode(id), Filter(id)
{
  LOG_DEBUG("SmoothFillSmearPeaks::SmoothFillSmearPeaks()");
  this->node_type = "SmoothFillSmearPeaks";
  this->category = category_mapping.at(this->node_type);
  this->attr["ir"] = NEW_ATTR_INT(8, 1, 128);
}

void SmoothFillSmearPeaks::compute_filter(hmap::HeightMap &h,
                                          hmap::HeightMap *p_mask)
{
  LOG_DEBUG("computing filter node [%s]", this->id.c_str());

  hmap::transform(
      h,
      p_mask,
      [this](hmap::Array &x, hmap::Array *p_mask)
      { hmap::smooth_fill_smear_peaks(x, GET_ATTR_INT("ir"), p_mask); });

  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
