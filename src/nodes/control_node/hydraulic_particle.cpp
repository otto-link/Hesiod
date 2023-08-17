/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "hesiod/control_node.hpp"

namespace hesiod::cnode
{

HydraulicParticle::HydraulicParticle(std::string id) : Erosion(id)
{
  this->node_type = "HydraulicParticle";
  this->category = category_mapping.at(this->node_type);
}

uint HydraulicParticle::get_seed()
{
  return this->seed;
}

int HydraulicParticle::get_nparticles()
{
  return this->nparticles;
}

int HydraulicParticle::get_c_radius()
{
  return this->c_radius;
}

float HydraulicParticle::get_c_capacity()
{
  return this->c_capacity;
}

float HydraulicParticle::get_c_erosion()
{
  return this->c_erosion;
}

float HydraulicParticle::get_c_deposition()
{
  return this->c_deposition;
}

float HydraulicParticle::get_drag_rate()
{
  return this->drag_rate;
}

float HydraulicParticle::get_evap_rate()
{
  return this->evap_rate;
}

void HydraulicParticle::set_seed(uint new_seed)
{
  if (new_seed != this->seed)
  {
    this->seed = new_seed;
    this->force_update();
  }
}

void HydraulicParticle::set_nparticles(int new_nparticles)
{
  if (new_nparticles != this->nparticles)
  {
    this->nparticles = new_nparticles;
    this->force_update();
  }
}

void HydraulicParticle::set_c_radius(int new_c_radius)
{
  if (new_c_radius != this->c_radius)
  {
    this->c_radius = new_c_radius;
    this->force_update();
  }
}

void HydraulicParticle::set_c_capacity(float new_c_capacity)
{
  if (new_c_capacity != this->c_capacity)
  {
    this->c_capacity = new_c_capacity;
    this->force_update();
  }
}

void HydraulicParticle::set_c_erosion(float new_c_erosion)
{
  if (new_c_erosion != this->c_erosion)
  {
    this->c_erosion = new_c_erosion;
    this->force_update();
  }
}

void HydraulicParticle::set_c_deposition(float new_c_deposition)
{
  if (new_c_deposition != this->c_deposition)
  {
    this->c_deposition = new_c_deposition;
    this->force_update();
  }
}

void HydraulicParticle::set_drag_rate(float new_drag_rate)
{
  if (new_drag_rate != this->drag_rate)
  {
    this->drag_rate = new_drag_rate;
    this->force_update();
  }
}

void HydraulicParticle::set_evap_rate(float new_evap_rate)
{
  if (new_evap_rate != this->evap_rate)
  {
    this->evap_rate = new_evap_rate;
    this->force_update();
  }
}

void HydraulicParticle::compute_erosion(hmap::HeightMap &h,
                                        hmap::HeightMap *p_bedrock,
                                        hmap::HeightMap *p_moisture_map,
                                        hmap::HeightMap *p_mask,
                                        hmap::HeightMap *p_erosion_map,
                                        hmap::HeightMap *p_deposition_map)
{
  LOG_DEBUG("computing erosion node [%s]", this->id.c_str());

  int nparticles_tile = (int) (this->nparticles / (float)h.get_ntiles());

  hmap::transform(h,
                  p_bedrock,
                  p_moisture_map,
                  p_mask,
                  p_erosion_map,
                  p_deposition_map,
                  [this, &nparticles_tile](hmap::Array &h_out,
                                           hmap::Array *p_bedrock_array,
                                           hmap::Array *p_moisture_map_array,
                                           hmap::Array *p_mask_array,
                                           hmap::Array *p_erosion_map_array,
                                           hmap::Array *p_deposition_map_array)
                  {
                    hmap::hydraulic_particle(h_out,
                                             p_mask_array,
                                             nparticles_tile,
                                             this->seed,
                                             p_bedrock_array,
                                             p_moisture_map_array,
                                             p_erosion_map_array,
                                             p_deposition_map_array,
                                             this->c_radius,
                                             this->c_capacity,
                                             this->c_erosion,
                                             this->c_deposition,
                                             this->drag_rate,
                                             this->evap_rate);
                  });

  h.smooth_overlap_buffers();
}

} // namespace hesiod::cnode
