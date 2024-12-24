/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <fstream>

#include <QApplication>

#include "highmap/algebra.hpp"
#include "highmap/opencl/gpu_opencl.hpp"

#include "hesiod/gui/main_window.hpp"
#include "hesiod/logger.hpp"

#include "hesiod/model/nodes/node_factory.hpp"

// in this order, required by args.hxx
std::istream &operator>>(std::istream &is, hmap::Vec2<int> &vec2)
{
  is >> vec2.x;
  is.get();
  is >> vec2.y;
  return is;
}
#include <args.hxx>

int main(int argc, char *argv[])
{
  hmap::gpu::init_opencl();
  clwrapper::KernelManager::get_instance().set_block_size(32);

  QApplication app(argc, argv); // enven if headless (for QObject)

  LOG->info("Welcome to Hesiod v{}.{}.{}!",
            HESIOD_VERSION_MAJOR,
            HESIOD_VERSION_MINOR,
            HESIOD_VERSION_PATCH);

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
      std::string graph_id = "graph_1"; // only one for now

      std::string filename = args::get(batch);

      LOG->info("executing Hesiod in batch mode");

      hmap::Vec2<int> shape = shape_arg == true ? args::get(shape_arg)
                                                : hmap::Vec2<int>(0, 0);
      hmap::Vec2<int> tiling = tiling_arg == true ? args::get(tiling_arg)
                                                  : hmap::Vec2<int>(0, 0);
      float           overlap = overlap_arg == true ? args::get(overlap_arg) : -1.f;

      LOG->trace("file: {}", filename);
      LOG->trace("cli shape: {{{}, {}}}", shape.x, shape.y);
      LOG->trace("cli tiling: {{{}, {}}}", tiling.x, tiling.y);
      LOG->trace("cli overlap: {}", overlap);

      // load json
      nlohmann::json json;
      std::ifstream  file(filename);

      if (file.is_open())
      {
        file >> json;
        file.close();
      }
      else
      {
        LOG->critical("Could not open file {} to load JSON", filename);
        return 1;
      }

      // load config from json
      auto config = std::make_shared<hesiod::ModelConfig>();
      config->json_from(json["graph_node"]["model_config"]);

      // adjust config based on command line inputs
      if (shape != hmap::Vec2<int>(0, 0))
        config->shape = shape;
      if (tiling != hmap::Vec2<int>(0, 0))
        config->tiling = tiling;
      if (overlap >= 0.f)
        config->overlap = overlap;

      // instanciate and compute the graph (but replace the config
      // from the file by the input config from the CLI)
      bool                is_headless = true;
      hesiod::GraphEditor ed = hesiod::GraphEditor("graph1", config, is_headless);
      bool                override_config = false;

      ed.load_from_file(filename, override_config);

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

  hesiod::MainWindow *main_window = hesiod::MainWindow::instance(&app);
  main_window->show();

  return app.exec();

  return 0;
}
