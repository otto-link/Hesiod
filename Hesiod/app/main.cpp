/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include "highmap/opencl/gpu_opencl.hpp"

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/cmap.hpp"
#include "hesiod/model/color_gradient.hpp"

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
  hesiod::ColorGradientManager::get_instance();

  // start QApplication even if headless (for QObject)
  qputenv("QT_LOGGING_RULES", HESIOD_QPUTENV_QT_LOGGING_RULES);
  hesiod::HesiodApplication app(argc, argv);

  // --- styles

  hesiod::apply_global_style(app.get_qapp());

  // ----------------------------------- batch CLI mode

  args::ArgumentParser parser("Hesiod.");
  int                  ret = hesiod::cli::parse_args(parser, argc, argv);

  if (ret >= 0)
    return ret;

  // ----------------------------------- Main GUI

  app.setWindowIcon(QIcon(HSD_APP_ICON));

  hesiod::MainWindow *main_window = hesiod::MainWindow::instance(&app);
  main_window->show();

  hesiod::AppContext &ctx = HSD_CONTEXT;

  if (ctx.app_settings.window.open_viewport_at_startup)
    main_window->show_viewport();

  return app.exec();
}
