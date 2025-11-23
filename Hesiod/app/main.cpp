/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/logger.hpp"

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

  // start QApplication even if headless (for QObject)
  qputenv("QT_LOGGING_RULES", HESIOD_QPUTENV_QT_LOGGING_RULES);
  hesiod::HesiodApplication app(argc, argv);

  // ----------------------------------- batch CLI mode

  args::ArgumentParser parser("Hesiod.");
  int                  ret = hesiod::cli::parse_args(parser, argc, argv);

  if (ret >= 0)
    return ret;

  app.show();

  return app.exec();
}
