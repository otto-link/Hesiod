/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "gnodegui/style.hpp"

#include "qsx/config.hpp"

#include "hesiod/config.hpp"
#include "hesiod/gui/style.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod
{

void apply_global_style(QApplication &app)
{
  Logger::log()->trace("apply_global_style");

  std::string style_sheet =
#include "hesiod/gui/darkstyle.css"
      ;

  std::map<std::string, QColor> place_holders = {
      {"COLOR_BG_PRIMARY", HSD_CONFIG->colors.bg_primary},
      {"COLOR_BG_SECONDARY", HSD_CONFIG->colors.bg_secondary},
      {"COLOR_TEXT_PRIMARY", HSD_CONFIG->colors.text_primary},
      {"COLOR_TEXT_DISABLED", HSD_CONFIG->colors.text_disabled},
      {"COLOR_ACCENT", HSD_CONFIG->colors.accent},
      {"COLOR_BORDER", HSD_CONFIG->colors.border},
      {"COLOR_HOVER", HSD_CONFIG->colors.hover},
      {"COLOR_PRESSED", HSD_CONFIG->colors.pressed},
      {"COLOR_SEPARATOR", HSD_CONFIG->colors.separator}};

  for (auto &[p, color] : place_holders)
    hesiod::replace_all(style_sheet, p, color.name().toStdString());

  app.setStyleSheet(style_sheet.c_str());

  // graph viewer style
  {
    gngui::Style *p_style = gngui::Style::get_style().get();

    p_style->viewer.color_bg = HSD_CONFIG->colors.bg_primary;
  }

  // QSliderX style
  {
    qsx::Config *p_cfg = qsx::Config::get_config().get();

    p_cfg->global.color_text = HSD_CONFIG->colors.text_primary;
    p_cfg->global.color_border = HSD_CONFIG->colors.border;
    p_cfg->global.color_hovered = HSD_CONFIG->colors.hover;
    p_cfg->global.color_selected = HSD_CONFIG->colors.accent;
    p_cfg->global.color_faded = HSD_CONFIG->colors.text_disabled;
    p_cfg->global.color_bg = HSD_CONFIG->colors.bg_secondary;
  }
}

} // namespace hesiod
