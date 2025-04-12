/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/cli/batch_mode.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
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
                    float                  overlap)
{
  LOG->info("executing Hesiod in batch mode");
  LOG->trace("file: {}", filename);
  LOG->trace("cli shape: {{{}, {}}}", shape.x, shape.y);
  LOG->trace("cli tiling: {{{}, {}}}", tiling.x, tiling.y);
  LOG->trace("cli overlap: {}", overlap);

  // define actual computation configuration based on CLI inputs. If
  // nothing is provided, use the configs from the input file but if
  // an input config is provided, this config is used for all the
  // graph nodes.
  hesiod::ModelConfig *p_input_config = nullptr;

  auto config = std::make_shared<hesiod::ModelConfig>();

  if (shape.x || shape.y || tiling.x || tiling.y || overlap >= 0.f)
  {
    config->shape = (shape.x && shape.y) ? shape : hmap::Vec2<int>(1024, 1024);
    config->tiling = (tiling.x && tiling.y) ? tiling : hmap::Vec2<int>(1, 1);
    config->overlap = overlap >= 0.f
                          ? overlap
                          : ((config->tiling.x == 1 && config->tiling.y == 1) ? 0.f
                                                                              : 0.5f);

    LOG->info("graph configurations will be overriden:");
    LOG->info("compute shape: {{{}, {}}}", config->shape.x, config->shape.y);
    LOG->info("compute tiling: {{{}, {}}}", config->tiling.x, config->tiling.y);
    LOG->info("compute overlap: {}", config->overlap);

    p_input_config = config.get();
  }

  GraphManager graph_manager;
  graph_manager.load_from_file(filename, p_input_config);
}

void run_node_inventory()
{
  LOG->info("executing Hesiod in node inventory mode");
  hesiod::dump_node_inventory("node_inventory");

  auto config = std::make_shared<hesiod::ModelConfig>();
  hesiod::dump_node_documentation_stub("node_documentation_stub.json", config);
}

void run_snapshot_generation()
{
  LOG->info("executing Hesiod in snapshot generation mode");
  LOG->critical("TODO");
}

} // namespace hesiod::cli
