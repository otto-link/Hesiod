/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <QApplication>

#include "highmap/opencl/gpu_opencl.hpp"

#include "gnodegui/style.hpp"

#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/config.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"

#if defined(DEBUG_BUILD)
#define HSD_RMODE "Debug"
#elif defined(RELEASE_BUILD)
#define HSD_RMODE "Release"
#else
#define HSD_RMODE "!!! UNDEFINED !!!"
#endif

int main(int argc, char *argv[])
{
  hesiod::Logger::log()->info("Welcome to Hesiod v{}.{}.{}!",
                              HESIOD_VERSION_MAJOR,
                              HESIOD_VERSION_MINOR,
                              HESIOD_VERSION_PATCH);

  hesiod::Logger::log()->info("Release mode: {}", std::string(HSD_RMODE));

  // ----------------------------------- initialization

  // start OpenCL
  hmap::gpu::init_opencl();

  // for colormaps loading
  hesiod::CmapManager::get_instance();

  // start QApplication even if headless (for QObject)
  qputenv("QT_LOGGING_RULES", HESIOD_QPUTENV_QT_LOGGING_RULES);
  QApplication app(argc, argv);

  // style
  const std::string style_sheet =
#include "darkstyle.css"
      ;
  app.setStyleSheet(style_sheet.c_str());

  // graph viewer style
  GN_STYLE->viewer.color_bg = QColor(30, 30, 30, 255);

  // ----------------------------------- batch CLI mode

  args::ArgumentParser parser("Hesiod.");
  int                  ret = hesiod::cli::parse_args(parser, argc, argv);

  if (ret >= 0)
    return ret;

  // ----------------------------------- Main GUI

  app.setWindowIcon(QIcon(HSD_APP_ICON));

  hesiod::MainWindow *main_window = hesiod::MainWindow::instance(&app);
  main_window->show();

  if (HSD_CONFIG->window.open_viewport_at_startup)
    main_window->show_viewport();

  return app.exec();
}
