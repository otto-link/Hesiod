/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/config.hpp"
#include "hesiod/logger.hpp"
#include "hesiod/model/graph_manager.hpp"
#include "hesiod/model/utils.hpp"

namespace hesiod::cli
{

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

} // namespace hesiod::cli
