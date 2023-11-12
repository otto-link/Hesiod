/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>
#include <string>
#include <vector>

#include "hesiod/cmap.hpp"

namespace hesiod
{

std::vector<std::vector<float>> get_colormap_data(int cmap)
{
  switch (cmap)
  {
  case cmap::ariresqu:
    return CMAP_ARIRESQU;
  case cmap::athebyse:
    return CMAP_ATHEBYSE;
  case cmap::aufegoth:
    return CMAP_AUFEGOTH;
  case cmap::betmamys:
    return CMAP_BETMAMYS;
  case cmap::castgear:
    return CMAP_CASTGEAR;
  case cmap::crimobso:
    return CMAP_CRIMOBSO;
  case cmap::fomborec:
    return CMAP_FOMBOREC;
  case cmap::ggasaind:
    return CMAP_GGASAIND;
  case cmap::hprigets:
    return CMAP_HPRIGETS;
  case cmap::ickiocen:
    return CMAP_ICKIOCEN;
  case cmap::mieoveft:
    return CMAP_MIEOVEFT;
  case cmap::nedtsheh:
    return CMAP_NEDTSHEH;
  case cmap::onoppeei:
    return CMAP_ONOPPEEI;
  case cmap::owomiatt:
    return CMAP_OWOMIATT;
  case cmap::perfones:
    return CMAP_PERFONES;
  case cmap::plllthhe:
    return CMAP_PLLLTHHE;
  case cmap::squcrran:
    return CMAP_SQUCRRAN;
  case cmap::stragono:
    return CMAP_STRAGONO;
  case cmap::tacalanc:
    return CMAP_TACALANC;
  case cmap::thallung:
    return CMAP_THALLUNG;
  case cmap::thanltti:
    return CMAP_THANLTTI;
  case cmap::thatalea:
    return CMAP_THATALEA;
  case cmap::thethind:
    return CMAP_THETHIND;
  case cmap::tisathin:
    return CMAP_TISATHIN;
  case cmap::titinkis:
    return CMAP_TITINKIS;
  case cmap::tltrenab:
    return CMAP_TLTRENAB;
  case cmap::tofurofi:
    return CMAP_TOFUROFI;
  case cmap::ttaldove:
    return CMAP_TTALDOVE;
  }
}

std::map<std::string, int> get_colormap_mapping()
{
  return {
      {"ariresqu", hesiod::cmap::ariresqu},
      {"athebyse", hesiod::cmap::athebyse},
      {"aufegoth", hesiod::cmap::aufegoth},
      {"betmamys", hesiod::cmap::betmamys},
      {"castgear", hesiod::cmap::castgear},
      {"crimobso", hesiod::cmap::crimobso},
      {"fomborec", hesiod::cmap::fomborec},
      {"ggasaind", hesiod::cmap::ggasaind},
      {"hprigets", hesiod::cmap::hprigets},
      {"ickiocen", hesiod::cmap::ickiocen},
      {"mieoveft", hesiod::cmap::mieoveft},
      {"nedtsheh", hesiod::cmap::nedtsheh},
      {"onoppeei", hesiod::cmap::onoppeei},
      {"owomiatt", hesiod::cmap::owomiatt},
      {"perfones", hesiod::cmap::perfones},
      {"plllthhe", hesiod::cmap::plllthhe},
      {"squcrran", hesiod::cmap::squcrran},
      {"stragono", hesiod::cmap::stragono},
      {"tacalanc", hesiod::cmap::tacalanc},
      {"thallung", hesiod::cmap::thallung},
      {"thanltti", hesiod::cmap::thanltti},
      {"thatalea", hesiod::cmap::thatalea},
      {"thethind", hesiod::cmap::thethind},
      {"tisathin", hesiod::cmap::tisathin},
      {"titinkis", hesiod::cmap::titinkis},
      {"tltrenab", hesiod::cmap::tltrenab},
      {"tofurofi", hesiod::cmap::tofurofi},
      {"ttaldove", hesiod::cmap::ttaldove},
  };
}

} // namespace hesiod
