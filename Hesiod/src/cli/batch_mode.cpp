/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <filesystem>

#include <QTimer>

#include "hesiod/app/hesiod_application.hpp"
#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/gui/project_ui.hpp"
#include "hesiod/gui/widgets/graph_tabs_widget.hpp"
#include "hesiod/gui/widgets/gui_utils.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph/graph_manager.hpp"
#include "hesiod/model/nodes/node_factory.hpp"
#include "hesiod/model/nodes/post_process.hpp"

namespace hesiod::cli
{

int parse_args(args::ArgumentParser &parser, int argc, char *argv[])
{
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

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

    if (batch)
    {
      run_batch_mode(args::get(batch),
                     shape_arg ? args::get(shape_arg) : hmap::Vec2<int>(0, 0),
                     tiling_arg ? args::get(tiling_arg) : hmap::Vec2<int>(0, 0),
                     overlap_arg ? args::get(overlap_arg) : -1.f);
      return 0;
    }
    else if (snapshot_generation)
    {
      run_snapshot_generation();
      return 0;
    }
    else if (node_inventory)
    {
      run_node_inventory();
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

  return -1;
}

void run_batch_mode(const std::string     &filename,
                    const hmap::Vec2<int> &shape,
                    const hmap::Vec2<int> &tiling,
                    float                  overlap,
                    const GraphConfig     *p_input_model_config)
{
  Logger::log()->info("executing Hesiod in batch mode");
  Logger::log()->trace("file: {}", filename);
  Logger::log()->trace("cli shape: {{{}, {}}}", shape.x, shape.y);
  Logger::log()->trace("cli tiling: {{{}, {}}}", tiling.x, tiling.y);
  Logger::log()->trace("cli overlap: {}", overlap);

  // define actual computation configuration based on CLI inputs. If
  // nothing is provided, use the configs from the input file but if
  // an input config is provided, this config is used for all the
  // graph nodes.
  hesiod::GraphConfig config;

  // override some parameters on request
  if (p_input_model_config)
  {
    config.hmap_transform_mode_cpu = p_input_model_config->hmap_transform_mode_cpu;
    config.hmap_transform_mode_gpu = p_input_model_config->hmap_transform_mode_gpu;
  }

  if (shape.x || shape.y || tiling.x || tiling.y || overlap >= 0.f)
  {
    config.shape = (shape.x && shape.y) ? shape : hmap::Vec2<int>(1024, 1024);
    config.tiling = (tiling.x && tiling.y) ? tiling : hmap::Vec2<int>(1, 1);
    config.overlap = overlap >= 0.f
                         ? overlap
                         : ((config.tiling.x == 1 && config.tiling.y == 1) ? 0.f : 0.5f);

    Logger::log()->info("graph configurations will be overriden:");
    Logger::log()->info("compute shape: {{{}, {}}}", config.shape.x, config.shape.y);
    Logger::log()->info("compute tiling: {{{}, {}}}", config.tiling.x, config.tiling.y);
    Logger::log()->info("compute overlap: {}", config.overlap);
  }

  GraphManager graph_manager;
  graph_manager.load_from_file(filename, &config);

  // flatten & export if there is a configuration defined
  if (!graph_manager.get_export_param().export_path.empty())
    graph_manager.export_flatten();
}

void run_node_inventory()
{
  Logger::log()->info("executing Hesiod in node inventory mode");
  hesiod::dump_node_inventory("node_inventory");

  auto config = std::make_shared<hesiod::GraphConfig>();
  hesiod::dump_node_documentation_stub("node_documentation_stub.json", config);

  hesiod::dump_node_settings_screenshots();
}

void run_snapshot_generation()
{
  Logger::log()->info("executing Hesiod in snapshot generation mode");

  std::map<std::string, std::string> inventory = get_node_inventory();

  // TODO hardcoded
  const std::string ex_path = "data/examples/";
  const QSize       size = QSize(512, 512);

  auto *app = static_cast<hesiod::HesiodApplication *>(QCoreApplication::instance());

  // deactivate viewport and node settings pan in graph viewer
  const bool bckp_snsp = app->get_context()
                             .app_settings.node_editor.show_node_settings_pan;
  const bool bckp_sw = app->get_context().app_settings.node_editor.show_viewer;

  app->get_context().app_settings.node_editor.show_node_settings_pan = false;
  app->get_context().app_settings.node_editor.show_viewer = false;

  for (auto &[node_type, _] : inventory)
  {
    const std::string fname = ex_path + node_type + ".hsd";

    if (std::filesystem::exists(fname))
    {
      Logger::log()->trace("- default file exists: {}", fname);

      app->load_project_model_and_ui(fname);

      GraphTabsWidget *p_gtw = app->get_project_ui_ref()->get_graph_tabs_widget_ref();

      if (p_gtw)
      {
        p_gtw->zoom_to_content();

        // TODO refit again, not working...
        auto post_render_callback = [&]() { return; };

        QWidget *widget = dynamic_cast<QWidget *>(p_gtw);

        render_widget_screenshot(widget,
                                 node_type + "_hsd_example.png",
                                 size,
                                 post_render_callback);

        QCoreApplication::processEvents();

        // to avoid Qt panicking...
        QEventLoop loop;
        QTimer::singleShot(50, &loop, &QEventLoop::quit);
        loop.exec();
      }
    }
  }

  app->get_context().app_settings.node_editor.show_node_settings_pan = bckp_snsp;
  app->get_context().app_settings.node_editor.show_viewer = bckp_sw;
}

} // namespace hesiod::cli
