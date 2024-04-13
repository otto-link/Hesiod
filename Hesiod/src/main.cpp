/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <QApplication>
#include <QDebug>

#include "macrologger.h"

#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/style.hpp"

#include "hesiod/model/model_registry.hpp" // batch

// for testing - TO REMOVE
#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/attributes.hpp"
#include "hesiod/model/enum_mapping.hpp"

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
  // --- parse command line arguments

  args::ArgumentParser parser("Hesiod.");
  args::HelpFlag       help(parser, "help", "Display this help menu", {'h', "help"});

  args::ValueFlag<std::string> batch(parser, "batch mode", "Execute ...", {'b', "batch"});

  args::ValueFlag<hmap::Vec2<int>> shape_arg(
      parser,
      "shape",
      "Heightmap shape (in pixels), ex. --shape=512,512",
      {"shape"});

  args::ValueFlag<hmap::Vec2<int>> tiling_arg(parser,
                                              "tiling",
                                              "Heightmap tiling, ex. --tiling=4,4",
                                              {"tiling"});

  args::ValueFlag<float> overlap_arg(
      parser,
      "overlap",
      "Tile overlapping ratio (in [0, 1[), ex. --overlap=0.25",
      {"overlap"});

  try
  {
    parser.ParseCLI(argc, argv);

    // batch mode
    if (batch)
    {
      std::string filename = args::get(batch);

      LOG_INFO("executing Hesiod in batch mode...");
      LOG_DEBUG("file: %s", filename.c_str());

      hmap::Vec2<int> shape = shape_arg == true ? args::get(shape_arg)
                                                : hmap::Vec2<int>(0, 0);
      hmap::Vec2<int> tiling = tiling_arg == true ? args::get(tiling_arg)
                                                  : hmap::Vec2<int>(0, 0);
      float           overlap = overlap_arg == true ? args::get(overlap_arg) : -1.f;

      LOG_DEBUG("cli shape: {%d, %d}", shape.x, shape.y);
      LOG_DEBUG("cli tiling: {%d, %d}", tiling.x, tiling.y);
      LOG_DEBUG("cli overlap: %f", overlap);

      QFile file(QString::fromStdString(filename));
      if (!file.open(QIODevice::ReadOnly))
      {
        LOG_ERROR("error while opening graph file");
        return 1;
      }

      QByteArray const whole_file = file.readAll();
      QJsonObject      json_doc = QJsonDocument::fromJson(whole_file).object();

      // initialize the registry with a small data shape to avoid
      // using excessive memory at this stage
      hesiod::ModelConfig model_config;
      model_config.shape = {4, 4};

      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
          &model_config);

      // load (only) the model configuration and adjust it
      model_config.load(json_doc["model_config"].toObject());
      model_config.log_debug();

      // adjust model config
      if (shape.x > 0)
        model_config.shape = shape;
      if (tiling.x > 0)
        model_config.tiling = tiling;
      if (overlap >= 0.f)
        model_config.overlap = overlap;

      // load the graph and compute
      auto model = std::make_unique<hesiod::HsdDataFlowGraphModel>(registry,
                                                                   &model_config);

      LOG_INFO("computing node graph...");
      model->load(json_doc, model_config);

      return 0;
    }
  }
  catch (args::Help)
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

  QApplication app(argc, argv);

  hesiod::set_style_qtapp(app);
  hesiod::set_style_qtnodes();

  // main window

  hesiod::MainWindow main_window(&app);
  main_window.show();

  // --- WIDGET TESTING
  std::map<std::string, std::unique_ptr<hesiod::Attribute>> attr = {};

  attr["somme choice"] = NEW_ATTR_BOOL(false, "toto");
  attr["seed"] = NEW_ATTR_SEED();
  attr["float"] = NEW_ATTR_FLOAT(1.f, 0.1f, 5.f, "%.3f");
  attr["int"] = NEW_ATTR_INT(32, 1, 64);
  attr["map"] = NEW_ATTR_MAPENUM(hesiod::cmap_map);
  attr["range"] = NEW_ATTR_RANGE(hmap::Vec2<float>(0.5f, 2.f), "%.3f");
  attr["kw"] = NEW_ATTR_WAVENB(hmap::Vec2<float>(16.f, 2.f), 0.1f, 64.f, "%.3f");
  attr["file"] = NEW_ATTR_FILENAME("export.png", "PNG Files(*.png)", "Open toto");
  attr["color"] = NEW_ATTR_COLOR();

  std::vector<int> vi = {4, 5, 6, 7};
  attr["vint"] = NEW_ATTR_VECINT(vi, 0, 64);

  attr["cloud"] = NEW_ATTR_CLOUD();

  hesiod::AttributesWidget *sw = new hesiod::AttributesWidget(&attr);
  // sw->show();

  // ---

  return app.exec();
}
