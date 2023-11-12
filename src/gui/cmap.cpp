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
  case cmap::afofeale:
    return CMAP_AFOFEALE;
  case cmap::ariresqu:
    return CMAP_ARIRESQU;
  case cmap::arsominc:
    return CMAP_ARSOMINC;
  case cmap::athebyse:
    return CMAP_ATHEBYSE;
  case cmap::aufegoth:
    return CMAP_AUFEGOTH;
  case cmap::aveelill:
    return CMAP_AVEELILL;
  case cmap::betmamys:
    return CMAP_BETMAMYS;
  case cmap::castgear:
    return CMAP_CASTGEAR;
  case cmap::crimobso:
    return CMAP_CRIMOBSO;
  case cmap::dilyanda:
    return CMAP_DILYANDA;
  case cmap::dprripso:
    return CMAP_DPRRIPSO;
  case cmap::fomborec:
    return CMAP_FOMBOREC;
  case cmap::ftintess:
    return CMAP_FTINTESS;
  case cmap::ggasaind:
    return CMAP_GGASAIND;
  case cmap::hprigets:
    return CMAP_HPRIGETS;
  case cmap::hsafofel:
    return CMAP_HSAFOFEL;
  case cmap::hthiglar:
    return CMAP_HTHIGLAR;
  case cmap::ickiocen:
    return CMAP_ICKIOCEN;
  case cmap::lofomube:
    return CMAP_LOFOMUBE;
  case cmap::mieoveft:
    return CMAP_MIEOVEFT;
  case cmap::nedtsheh:
    return CMAP_NEDTSHEH;
  case cmap::ntisouln:
    return CMAP_NTISOULN;
  case cmap::onoppeei:
    return CMAP_ONOPPEEI;
  case cmap::owomiatt:
    return CMAP_OWOMIATT;
  case cmap::perfones:
    return CMAP_PERFONES;
  case cmap::pesanged:
    return CMAP_PESANGED;
  case cmap::petledta:
    return CMAP_PETLEDTA;
  case cmap::plardnsh:
    return CMAP_PLARDNSH;
  case cmap::plllthhe:
    return CMAP_PLLLTHHE;
  case cmap::ppeicive:
    return CMAP_PPEICIVE;
  case cmap::rbepsult:
    return CMAP_RBEPSULT;
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
  case cmap::thejeson:
    return CMAP_THEJESON;
  case cmap::thethind:
    return CMAP_THETHIND;
  case cmap::thterily:
    return CMAP_THTERILY;
  case cmap::tinfeana:
    return CMAP_TINFEANA;
  case cmap::tisathin:
    return CMAP_TISATHIN;
  case cmap::titinkis:
    return CMAP_TITINKIS;
  case cmap::tltrenab:
    return CMAP_TLTRENAB;
  case cmap::tofurofi:
    return CMAP_TOFUROFI;
  case cmap::trollith:
    return CMAP_TROLLITH;
  case cmap::ttaldove:
    return CMAP_TTALDOVE;
  case cmap::tthegana:
    return CMAP_TTHEGANA;
  case cmap::uprongon:
    return CMAP_UPRONGON;
  case cmap::warealor:
    return CMAP_WAREALOR;
  case cmap::wmmrdmar:
    return CMAP_WMMRDMAR;
  }
}

std::map<std::string, int> get_colormap_mapping()
{
  return {
      {"afofeale", hesiod::cmap::afofeale},
      {"ariresqu", hesiod::cmap::ariresqu},
      {"arsominc", hesiod::cmap::arsominc},
      {"athebyse", hesiod::cmap::athebyse},
      {"aufegoth", hesiod::cmap::aufegoth},
      {"aveelill", hesiod::cmap::aveelill},
      {"betmamys", hesiod::cmap::betmamys},
      {"castgear", hesiod::cmap::castgear},
      {"crimobso", hesiod::cmap::crimobso},
      {"dilyanda", hesiod::cmap::dilyanda},
      {"dprripso", hesiod::cmap::dprripso},
      {"fomborec", hesiod::cmap::fomborec},
      {"ftintess", hesiod::cmap::ftintess},
      {"ggasaind", hesiod::cmap::ggasaind},
      {"hprigets", hesiod::cmap::hprigets},
      {"hsafofel", hesiod::cmap::hsafofel},
      {"hthiglar", hesiod::cmap::hthiglar},
      {"ickiocen", hesiod::cmap::ickiocen},
      {"lofomube", hesiod::cmap::lofomube},
      {"mieoveft", hesiod::cmap::mieoveft},
      {"nedtsheh", hesiod::cmap::nedtsheh},
      {"ntisouln", hesiod::cmap::ntisouln},
      {"onoppeei", hesiod::cmap::onoppeei},
      {"owomiatt", hesiod::cmap::owomiatt},
      {"perfones", hesiod::cmap::perfones},
      {"pesanged", hesiod::cmap::pesanged},
      {"petledta", hesiod::cmap::petledta},
      {"plardnsh", hesiod::cmap::plardnsh},
      {"plllthhe", hesiod::cmap::plllthhe},
      {"ppeicive", hesiod::cmap::ppeicive},
      {"rbepsult", hesiod::cmap::rbepsult},
      {"squcrran", hesiod::cmap::squcrran},
      {"stragono", hesiod::cmap::stragono},
      {"tacalanc", hesiod::cmap::tacalanc},
      {"thallung", hesiod::cmap::thallung},
      {"thanltti", hesiod::cmap::thanltti},
      {"thatalea", hesiod::cmap::thatalea},
      {"thejeson", hesiod::cmap::thejeson},
      {"thethind", hesiod::cmap::thethind},
      {"thterily", hesiod::cmap::thterily},
      {"tinfeana", hesiod::cmap::tinfeana},
      {"tisathin", hesiod::cmap::tisathin},
      {"titinkis", hesiod::cmap::titinkis},
      {"tltrenab", hesiod::cmap::tltrenab},
      {"tofurofi", hesiod::cmap::tofurofi},
      {"trollith", hesiod::cmap::trollith},
      {"ttaldove", hesiod::cmap::ttaldove},
      {"tthegana", hesiod::cmap::tthegana},
      {"uprongon", hesiod::cmap::uprongon},
      {"warealor", hesiod::cmap::warealor},
      {"wmmrdmar", hesiod::cmap::wmmrdmar},
  };
}

} // namespace hesiod
