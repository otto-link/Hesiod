/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <QApplication>

#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"

int main(int argc, char *argv[])
{
  LOG->info("Welcome to Hesiod v{}.{}.{}!",
            HESIOD_VERSION_MAJOR,
            HESIOD_VERSION_MINOR,
            HESIOD_VERSION_PATCH);

  QApplication app(argc, argv);

  const std::string style_sheet =
#include "darkstyle.css"
      ;

  app.setStyleSheet(style_sheet.c_str());

  hesiod::MainWindow main_window(&app);
  main_window.show();

  return app.exec();

  return 0;
}
