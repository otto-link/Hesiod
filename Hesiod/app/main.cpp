/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <QApplication>
#include <QDebug>

#include "hesiod/logger.hpp"
#include "hesiod/gui/main_window.hpp"
#include "hesiod/gui/style.hpp"

#include "hesiod/model/model_registry.hpp" // batch

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
  auto log = hesiod::Logger::get_logger();

  log->info("Welcome to Hesiod v{}.{}.{}!", HESIOD_VERSION_MAJOR, HESIOD_VERSION_MINOR, HESIOD_VERSION_PATCH);

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

  try
  {
    parser.ParseCLI(argc, argv);

    // --- batch mode

    if (batch)
    {
      std::string graph_id = "graph_1"; // only one for now

      std::string filename = args::get(batch);

      log->info("executing Hesiod in batch mode");
      
      hmap::Vec2<int> shape = shape_arg == true ? args::get(shape_arg)
                                                : hmap::Vec2<int>(0, 0);
      hmap::Vec2<int> tiling = tiling_arg == true ? args::get(tiling_arg)
                                                  : hmap::Vec2<int>(0, 0);
      float           overlap = overlap_arg == true ? args::get(overlap_arg) : -1.f;

      log->info("file: {}", filename);
      log->info("cli shape: {{{}, {}}}", shape.x, shape.y);
      log->info("cli tiling: {{{}, {}}}", tiling.x, tiling.y);
      log->info("cli overlap: {}", overlap);

      QFile file(QString::fromStdString(filename));
      if (!file.open(QIODevice::ReadOnly))
      {
        log->critical("not able to open the input graph file: {}", filename);
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

      log->info("computing node graph...");
      model->load(json_doc, model_config);

      return 0;
    }

    // --- snapshot mode

    if (snapshot_generation)
    {
      log->info("executing Hesiod in snapshot generation mode");

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

        log->info("Generating snapshot for node type: {}", node_type);

        hesiod::NodeEditorWidget      *p_ed = new hesiod::NodeEditorWidget("", nullptr, false);
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

  hesiod::MainWindow main_window(&app);
  main_window.show();

  return app.exec();
}
