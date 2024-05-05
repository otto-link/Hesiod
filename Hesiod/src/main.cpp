/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "hesiod/model/cmap.hpp"
#include <filesystem>
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

  args::Group group(parser,
                    "This group is all exclusive:",
                    args::Group::Validators::DontCare);

  args::Flag snapshot_generation(group, "", "Node snapshot generation", {"snapshot"});

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

  if(hesiod::load_colormap_data_from_file(std::filesystem::current_path() / "resources/colormap.json") == false)
  {
    LOG_ERROR("Failed to colormap data from file!");
    return -1;
  }

  try
  {
    parser.ParseCLI(argc, argv);

    // --- batch mode

    if (batch)
    {
      std::string graph_id = "graph_1"; // only one for now

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

      hesiod::ModelConfig model_config;
      model_config.compute_nodes_at_instanciation = false;

      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
          &model_config);

      // load (only) the model configuration and adjust it
      model_config.load(json_doc[graph_id.c_str()].toObject()["model_config"].toObject());
      model_config.compute_nodes_at_instanciation = true;

      // adjust model config
      if (shape.x > 0)
        model_config.shape = shape;
      if (tiling.x > 0)
        model_config.tiling = tiling;
      if (overlap >= 0.f)
        model_config.overlap = overlap;

      model_config.log_debug();

      // load the graph and compute
      auto model = std::make_unique<hesiod::HsdDataFlowGraphModel>(registry,
                                                                   &model_config,
                                                                   graph_id);

      LOG_INFO("computing node graph...");
      model->load(json_doc, model_config);

      return 0;
    }

    // --- snapshot mode

    if (snapshot_generation)
    {
      LOG_INFO("executing Hesiod in snapshot generation mode...");

      // ignite Qt application
      QApplication app(argc, argv);
      hesiod::set_style_qtapp(app);
      hesiod::set_style_qtnodes();

      //
      hesiod::ModelConfig model_config = hesiod::ModelConfig();
      std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry = register_data_models(
          &model_config);
      auto node_type_category_map = registry->registeredModelsCategoryAssociation();

      QJsonObject json;

      for (auto &[node_type_qstring, category] : node_type_category_map)
      {
        std::string node_type = node_type_qstring.toStdString();

        LOG_DEBUG("Generating snapshot for node type: %s", node_type.c_str());

        hesiod::NodeEditorWidget      *p_ed = new hesiod::NodeEditorWidget("");
        hesiod::HsdDataFlowGraphModel *p_model = p_ed->get_model_ref();

        p_ed->get_model_config_ref()->shape = hmap::Vec2<int>(128, 128);
        p_ed->get_model_config_ref()->tiling = hmap::Vec2<int>(1, 1);
        p_ed->get_model_config_ref()->overlap = 0.f;
        p_ed->get_model_config_ref()->preview_type = hesiod::PreviewType::MAGMA;

        p_ed->show();

        QtNodes::NodeId node_id = hesiod::add_graph_example(p_model,
                                                            node_type,
                                                            category.toStdString());
        std::string     snapshot_fname = node_type + ".png";
        p_ed->save_screenshot(snapshot_fname);

        hesiod::BaseNode *p_node = p_model->delegateModel<hesiod::BaseNode>(node_id);
        QJsonObject       json_node = p_node->full_description_to_json();
        json_node["category"] = category;
        json_node["snapshot"] = snapshot_fname.c_str();
        json[node_type_qstring] = json_node;

        delete p_ed;
      }

      QFile file("nodes_description.json");
      if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument(json).toJson());

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

  // attr["cloud"] = NEW_ATTR_CLOUD();

  attr["text"] = NEW_ATTR_STRING("test");

  std::vector<std::vector<float>> gradient = {{0.f, 1.f, 0.f, 0.f, 1.f},
                                              {0.25f, 0.f, 1.f, 0.f, 1.f},
                                              {1.f, 0.f, 0.f, 1.f, 1.f}};

  attr["color_vec"] = NEW_ATTR_COLORGRADIENT(gradient);

  hesiod::AttributesWidget *sw = new hesiod::AttributesWidget(&attr);
  // sw->show();

  // ---

  return app.exec();
}
