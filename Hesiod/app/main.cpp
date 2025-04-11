/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <fstream>

#include <QApplication>

#include "highmap/algebra.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "hesiod/config.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/utils.hpp"

#include "hesiod/cli/batch_mode.hpp"

// in this order, required by args.hxx
std::istream &operator>>(std::istream &is, hmap::Vec2<int> &vec2)
{
  is >> vec2.x;
  is.get();
  is >> vec2.y;
  return is;
}
#include <args.hxx>

#include "hesiod/model/cmap.hpp"

int main(int argc, char *argv[])
{
  LOG->info("Welcome to Hesiod v{}.{}.{}!",
            HESIOD_VERSION_MAJOR,
            HESIOD_VERSION_MINOR,
            HESIOD_VERSION_PATCH);

  // start OpenCL
  hmap::gpu::init_opencl();

  // load colormaps
  hesiod::CmapManager::get_instance();

  QApplication app(argc, argv); // even if headless (for QObject)

  // --- parse command line arguments

  args::ArgumentParser parser("Hesiod.");
  args::HelpFlag       help(parser, "help", "Display this help menu", {'h', "help"});

  args::Group group(parser,
                    "This group is all exclusive:",
                    args::Group::Validators::DontCare);

  args::Flag snapshot_generation(group, "", "Node snapshot generation", {"snapshot"});

  args::Flag node_inventory(group, "", "Node inventory output", {"inventory"});

  args::ValueFlag<std::string> batch(group,
                                     "hsd file",
                                     "Execute Hesiod in batch mode",
                                     {'b', "batch"});

  args::Group batch_args(group,
                         "batch mode arguments",
                         args::Group::Validators::DontCare);

  args::ValueFlag<hmap::Vec2<int>> shape_arg(
      batch_args,
      "shape",
      "Heightmap shape (in pixels), ex. --shape=512,512",
      {"shape"});

  args::ValueFlag<hmap::Vec2<int>> tiling_arg(batch_args,
                                              "tiling",
                                              "Heightmap tiling, ex. --tiling=4,4",
                                              {"tiling"});

  args::ValueFlag<float> overlap_arg(
      batch_args,
      "overlap",
      "Tile overlapping ratio (in [0, 1[), ex. --overlap=0.25",
      {"overlap"});

  try
  {
    parser.ParseCLI(argc, argv);

    // --- batch mode

    if (batch)
    {
      hesiod::cli::run_batch_mode(
          args::get(batch),
          shape_arg ? args::get(shape_arg) : hmap::Vec2<int>(0, 0),
          tiling_arg ? args::get(tiling_arg) : hmap::Vec2<int>(0, 0),
          overlap_arg ? args::get(overlap_arg) : -1.f);
      return 0;
    }

    // --- snapshot mode

    if (snapshot_generation)
    {
      LOG->info("executing Hesiod in snapshot generation mode");
      LOG->critical("TODO");
      return 0;
    }

    // --- inventory mode

    if (node_inventory)
    {
      LOG->info("executing Hesiod in node inventory mode");
      hesiod::dump_node_inventory("node_inventory");

      auto config = std::make_shared<hesiod::ModelConfig>();
      hesiod::dump_node_documentation_stub("node_documentation_stub.json", config);
      return 0;
    }
  }
  catch (const args::Help &help)
  {
    std::cout << parser;
    return 0;
  }

  catch (args::Error &e)
  {
    std::cerr << e.what() << std::endl << parser;
    return 1;
  }

  // ----------------------------------- Main GUI

  const std::string style_sheet =
#include "darkstyle.css"
      ;

  app.setStyleSheet(style_sheet.c_str());
  app.setWindowIcon(QIcon(HSD_APP_ICON));

  hesiod::MainWindow *main_window = hesiod::MainWindow::instance(&app);
  main_window->show();

  return app.exec();

  return 0;
}
