/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>
#include <string>
#include <vector>

#include "hesiod/model/cmap.hpp"

namespace hesiod
{

std::vector<std::vector<float>> get_colormap_data(int cmap)
{
  switch (cmap)
  {
  case hesiod::cmap::adrceist:
    return CMAP_ADRCEIST;
  case hesiod::cmap::aneprosi:
    return CMAP_ANEPROSI;
  case hesiod::cmap::angghore:
    return CMAP_ANGGHORE;
  case hesiod::cmap::anilyftp:
    return CMAP_ANILYFTP;
  case hesiod::cmap::anthatly:
    return CMAP_ANTHATLY;
  case hesiod::cmap::ascaired:
    return CMAP_ASCAIRED;
  case hesiod::cmap::aselllfa:
    return CMAP_ASELLLFA;
  case hesiod::cmap::aveflili:
    return CMAP_AVEFLILI;
  case hesiod::cmap::blesiblh:
    return CMAP_BLESIBLH;
  case hesiod::cmap::blywixps:
    return CMAP_BLYWIXPS;
  case hesiod::cmap::boustous:
    return CMAP_BOUSTOUS;
  case hesiod::cmap::byapsahi:
    return CMAP_BYAPSAHI;
  case hesiod::cmap::bychpmem:
    return CMAP_BYCHPMEM;
  case hesiod::cmap::cecelldf:
    return CMAP_CECELLDF;
  case hesiod::cmap::ceitwope:
    return CMAP_CEITWOPE;
  case hesiod::cmap::cesintho:
    return CMAP_CESINTHO;
  case hesiod::cmap::coffrrea:
    return CMAP_COFFRREA;
  case hesiod::cmap::ctthaton:
    return CMAP_CTTHATON;
  case hesiod::cmap::dffygutu:
    return CMAP_DFFYGUTU;
  case hesiod::cmap::dfvieryo:
    return CMAP_DFVIERYO;
  case hesiod::cmap::drctheet:
    return CMAP_DRCTHEET;
  case hesiod::cmap::drfftiee:
    return CMAP_DRFFTIEE;
  case hesiod::cmap::dwinthed:
    return CMAP_DWINTHED;
  case hesiod::cmap::eafbissf:
    return CMAP_EAFBISSF;
  case hesiod::cmap::eporepop:
    return CMAP_EPOREPOP;
  case hesiod::cmap::ewntherb:
    return CMAP_EWNTHERB;
  case hesiod::cmap::feredwnt:
    return CMAP_FEREDWNT;
  case hesiod::cmap::fffadeng:
    return CMAP_FFFADENG;
  case hesiod::cmap::ffwatsan:
    return CMAP_FFWATSAN;
  case hesiod::cmap::fleoreha:
    return CMAP_FLEOREHA;
  case hesiod::cmap::fromsesh:
    return CMAP_FROMSESH;
  case hesiod::cmap::ftapewon:
    return CMAP_FTAPEWON;
  case hesiod::cmap::gsercrts:
    return CMAP_GSERCRTS;
  case hesiod::cmap::haviarof:
    return CMAP_HAVIAROF;
  case hesiod::cmap::hendesth:
    return CMAP_HENDESTH;
  case hesiod::cmap::imongsir:
    return CMAP_IMONGSIR;
  case hesiod::cmap::indthsad:
    return CMAP_INDTHSAD;
  case hesiod::cmap::inosulul:
    return CMAP_INOSULUL;
  case hesiod::cmap::intoshee:
    return CMAP_INTOSHEE;
  case hesiod::cmap::iresetyl:
    return CMAP_IRESETYL;
  case hesiod::cmap::isungror:
    return CMAP_ISUNGROR;
  case hesiod::cmap::itteloud:
    return CMAP_ITTELOUD;
  case hesiod::cmap::ivericed:
    return CMAP_IVERICED;
  case hesiod::cmap::lereitth:
    return CMAP_LEREITTH;
  case hesiod::cmap::majucent:
    return CMAP_MAJUCENT;
  case hesiod::cmap::mburmats:
    return CMAP_MBURMATS;
  case hesiod::cmap::mearesit:
    return CMAP_MEARESIT;
  case hesiod::cmap::meppremb:
    return CMAP_MEPPREMB;
  case hesiod::cmap::mofhewon:
    return CMAP_MOFHEWON;
  case hesiod::cmap::nalymeve:
    return CMAP_NALYMEVE;
  case hesiod::cmap::noasbian:
    return CMAP_NOASBIAN;
  case hesiod::cmap::ofasnfoi:
    return CMAP_OFASNFOI;
  case hesiod::cmap::ofoffrmp:
    return CMAP_OFOFFRMP;
  case hesiod::cmap::oleonowh:
    return CMAP_OLEONOWH;
  case hesiod::cmap::onaaloni:
    return CMAP_ONAALONI;
  case hesiod::cmap::ontomayw:
    return CMAP_ONTOMAYW;
  case hesiod::cmap::outtsthe:
    return CMAP_OUTTSTHE;
  case hesiod::cmap::owwheplu:
    return CMAP_OWWHEPLU;
  case hesiod::cmap::pechrede:
    return CMAP_PECHREDE;
  case hesiod::cmap::pellaine:
    return CMAP_PELLAINE;
  case hesiod::cmap::piventon:
    return CMAP_PIVENTON;
  case hesiod::cmap::plfgsthi:
    return CMAP_PLFGSTHI;
  case hesiod::cmap::rbonblnc:
    return CMAP_RBONBLNC;
  case hesiod::cmap::rredtthe:
    return CMAP_RREDTTHE;
  case hesiod::cmap::sarermat:
    return CMAP_SARERMAT;
  case hesiod::cmap::seclytme:
    return CMAP_SECLYTME;
  case hesiod::cmap::sfreante:
    return CMAP_SFREANTE;
  case hesiod::cmap::sheshesa:
    return CMAP_SHESHESA;
  case hesiod::cmap::sinabuse:
    return CMAP_SINABUSE;
  case hesiod::cmap::sindwirt:
    return CMAP_SINDWIRT;
  case hesiod::cmap::snotheca:
    return CMAP_SNOTHECA;
  case hesiod::cmap::ssemeool:
    return CMAP_SSEMEOOL;
  case hesiod::cmap::suttlseb:
    return CMAP_SUTTLSEB;
  case hesiod::cmap::taldidov:
    return CMAP_TALDIDOV;
  case hesiod::cmap::teasitis:
    return CMAP_TEASITIS;
  case hesiod::cmap::teieebit:
    return CMAP_TEIEEBIT;
  case hesiod::cmap::tharetwe:
    return CMAP_THARETWE;
  case hesiod::cmap::thengici:
    return CMAP_THENGICI;
  case hesiod::cmap::therinon:
    return CMAP_THERINON;
  case hesiod::cmap::thesihas:
    return CMAP_THESIHAS;
  case hesiod::cmap::thisathe:
    return CMAP_THISATHE;
  case hesiod::cmap::thoeftsw:
    return CMAP_THOEFTSW;
  case hesiod::cmap::tiamatsp:
    return CMAP_TIAMATSP;
  case hesiod::cmap::tinusprr:
    return CMAP_TINUSPRR;
  case hesiod::cmap::tiomithe:
    return CMAP_TIOMITHE;
  case hesiod::cmap::tnymeewh:
    return CMAP_TNYMEEWH;
  case hesiod::cmap::tonndind:
    return CMAP_TONNDIND;
  case hesiod::cmap::trererat:
    return CMAP_TRERERAT;
  case hesiod::cmap::ttyaveft:
    return CMAP_TTYAVEFT;
  case hesiod::cmap::tyfotinm:
    return CMAP_TYFOTINM;
  case hesiod::cmap::ucerecen:
    return CMAP_UCERECEN;
  case hesiod::cmap::ucererds:
    return CMAP_UCERERDS;
  case hesiod::cmap::upeesith:
    return CMAP_UPEESITH;
  case hesiod::cmap::usoerlea:
    return CMAP_USOERLEA;
  case hesiod::cmap::veranghh:
    return CMAP_VERANGHH;
  case hesiod::cmap::verdeisu:
    return CMAP_VERDEISU;
  case hesiod::cmap::wadsuthe:
    return CMAP_WADSUTHE;
  case hesiod::cmap::wbegweic:
    return CMAP_WBEGWEIC;
  case hesiod::cmap::whetitat:
    return CMAP_WHETITAT;
  case hesiod::cmap::whhorefr:
    return CMAP_WHHOREFR;
  case hesiod::cmap::wminamin:
    return CMAP_WMINAMIN;
  case hesiod::cmap::wsatavin:
    return CMAP_WSATAVIN;
  case hesiod::cmap::yierahon:
  default:
    return CMAP_YIERAHON;
  }
}

std::map<std::string, int> get_colormap_mapping()
{
  return {
      {"adrceist", hesiod::cmap::adrceist}, {"aneprosi", hesiod::cmap::aneprosi},
      {"angghore", hesiod::cmap::angghore}, {"anilyftp", hesiod::cmap::anilyftp},
      {"anthatly", hesiod::cmap::anthatly}, {"ascaired", hesiod::cmap::ascaired},
      {"aselllfa", hesiod::cmap::aselllfa}, {"aveflili", hesiod::cmap::aveflili},
      {"blesiblh", hesiod::cmap::blesiblh}, {"blywixps", hesiod::cmap::blywixps},
      {"boustous", hesiod::cmap::boustous}, {"byapsahi", hesiod::cmap::byapsahi},
      {"bychpmem", hesiod::cmap::bychpmem}, {"cecelldf", hesiod::cmap::cecelldf},
      {"ceitwope", hesiod::cmap::ceitwope}, {"cesintho", hesiod::cmap::cesintho},
      {"coffrrea", hesiod::cmap::coffrrea}, {"ctthaton", hesiod::cmap::ctthaton},
      {"dffygutu", hesiod::cmap::dffygutu}, {"dfvieryo", hesiod::cmap::dfvieryo},
      {"drctheet", hesiod::cmap::drctheet}, {"drfftiee", hesiod::cmap::drfftiee},
      {"dwinthed", hesiod::cmap::dwinthed}, {"eafbissf", hesiod::cmap::eafbissf},
      {"eporepop", hesiod::cmap::eporepop}, {"ewntherb", hesiod::cmap::ewntherb},
      {"feredwnt", hesiod::cmap::feredwnt}, {"fffadeng", hesiod::cmap::fffadeng},
      {"ffwatsan", hesiod::cmap::ffwatsan}, {"fleoreha", hesiod::cmap::fleoreha},
      {"fromsesh", hesiod::cmap::fromsesh}, {"ftapewon", hesiod::cmap::ftapewon},
      {"gsercrts", hesiod::cmap::gsercrts}, {"haviarof", hesiod::cmap::haviarof},
      {"hendesth", hesiod::cmap::hendesth}, {"imongsir", hesiod::cmap::imongsir},
      {"indthsad", hesiod::cmap::indthsad}, {"inosulul", hesiod::cmap::inosulul},
      {"intoshee", hesiod::cmap::intoshee}, {"iresetyl", hesiod::cmap::iresetyl},
      {"isungror", hesiod::cmap::isungror}, {"itteloud", hesiod::cmap::itteloud},
      {"ivericed", hesiod::cmap::ivericed}, {"lereitth", hesiod::cmap::lereitth},
      {"majucent", hesiod::cmap::majucent}, {"mburmats", hesiod::cmap::mburmats},
      {"mearesit", hesiod::cmap::mearesit}, {"meppremb", hesiod::cmap::meppremb},
      {"mofhewon", hesiod::cmap::mofhewon}, {"nalymeve", hesiod::cmap::nalymeve},
      {"noasbian", hesiod::cmap::noasbian}, {"ofasnfoi", hesiod::cmap::ofasnfoi},
      {"ofoffrmp", hesiod::cmap::ofoffrmp}, {"oleonowh", hesiod::cmap::oleonowh},
      {"onaaloni", hesiod::cmap::onaaloni}, {"ontomayw", hesiod::cmap::ontomayw},
      {"outtsthe", hesiod::cmap::outtsthe}, {"owwheplu", hesiod::cmap::owwheplu},
      {"pechrede", hesiod::cmap::pechrede}, {"pellaine", hesiod::cmap::pellaine},
      {"piventon", hesiod::cmap::piventon}, {"plfgsthi", hesiod::cmap::plfgsthi},
      {"rbonblnc", hesiod::cmap::rbonblnc}, {"rredtthe", hesiod::cmap::rredtthe},
      {"sarermat", hesiod::cmap::sarermat}, {"seclytme", hesiod::cmap::seclytme},
      {"sfreante", hesiod::cmap::sfreante}, {"sheshesa", hesiod::cmap::sheshesa},
      {"sinabuse", hesiod::cmap::sinabuse}, {"sindwirt", hesiod::cmap::sindwirt},
      {"snotheca", hesiod::cmap::snotheca}, {"ssemeool", hesiod::cmap::ssemeool},
      {"suttlseb", hesiod::cmap::suttlseb}, {"taldidov", hesiod::cmap::taldidov},
      {"teasitis", hesiod::cmap::teasitis}, {"teieebit", hesiod::cmap::teieebit},
      {"tharetwe", hesiod::cmap::tharetwe}, {"thengici", hesiod::cmap::thengici},
      {"therinon", hesiod::cmap::therinon}, {"thesihas", hesiod::cmap::thesihas},
      {"thisathe", hesiod::cmap::thisathe}, {"thoeftsw", hesiod::cmap::thoeftsw},
      {"tiamatsp", hesiod::cmap::tiamatsp}, {"tinusprr", hesiod::cmap::tinusprr},
      {"tiomithe", hesiod::cmap::tiomithe}, {"tnymeewh", hesiod::cmap::tnymeewh},
      {"tonndind", hesiod::cmap::tonndind}, {"trererat", hesiod::cmap::trererat},
      {"ttyaveft", hesiod::cmap::ttyaveft}, {"tyfotinm", hesiod::cmap::tyfotinm},
      {"ucerecen", hesiod::cmap::ucerecen}, {"ucererds", hesiod::cmap::ucererds},
      {"upeesith", hesiod::cmap::upeesith}, {"usoerlea", hesiod::cmap::usoerlea},
      {"veranghh", hesiod::cmap::veranghh}, {"verdeisu", hesiod::cmap::verdeisu},
      {"wadsuthe", hesiod::cmap::wadsuthe}, {"wbegweic", hesiod::cmap::wbegweic},
      {"whetitat", hesiod::cmap::whetitat}, {"whhorefr", hesiod::cmap::whhorefr},
      {"wminamin", hesiod::cmap::wminamin}, {"wsatavin", hesiod::cmap::wsatavin},
      {"yierahon", hesiod::cmap::yierahon},
  };
}

} // namespace hesiod
