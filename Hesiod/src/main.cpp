/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <QApplication>
#include <QDebug>

#include <QtNodes/ConnectionStyle>

#include "macrologger.h"

#include "hesiod/gui/main_window.hpp"

// for testing - TO REMOVE
#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/attributes.hpp"
#include "hesiod/model/enum_mapping.hpp"

static void set_style()
{
  QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
  {
  "GraphicsViewStyle": {
    "BackgroundColor": [255, 0, 0],
    "FineGridColor": [68, 71, 90],
    "CoarseGridColor": [88, 91, 45]
    },
  "ConnectionStyle": {
    "ConstructionColor": "gray",
    "NormalColor": "darkcyan",
    "SelectedColor": [100, 100, 100],
    "SelectedHaloColor": "orange",
    "HoveredColor": "lightcyan",

    "LineWidth": 1.0,
    "ConstructionLineWidth": 3.0,
    "PointDiameter": 10.0,

    "UseDataDefinedColors": true
  }

}
  )");
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QFont font("Roboto");
  font.setPointSize(10);
  app.setFont(font);

  app.setStyleSheet(R"(
* {
    font-family: Roboto, sans-serif;
}

QSlider::groove:horizontal { 
	background: #D8DEE9;
	height: 24px; 
	border-radius: 4px;
}

QSlider::handle:horizontal { 
	background-color: #81A1C1; 
	width: 24px; 
	height: 24px; 
	border-radius: 8px;
}

QSlider::groove:vertical { 
	background: #D8DEE9;
	width: 24px; 
	border-radius: 4px;
}

QSlider::handle:vertical { 
	background-color: #81A1C1; 
	width: 24px; 
	height: 24px; 
	border-radius: 8px;
}

)");

  set_style();

  // --- main window

  hesiod::MainWindow main_window(&app);
  main_window.show();

  // QObject::connect(editor,
  //                  &hesiod::NodeEditorWidget::computingFinished,
  //                  [](QtNodes::NodeId const node_id)
  //                  { LOG_DEBUG("node updated: %d", (int)node_id); });

  // hesiod::Viewer2dWidget viewer2d = hesiod::Viewer2dWidget(&model_config,
  //                                                          editor->get_scene_ref());
  // viewer2d.show();

  // //
  // auto editor2 = new hesiod::NodeEditorWidget(model_config);
  // editor2->show();

  // hesiod::Viewer2dWidget *viewer2d2 = new hesiod::Viewer2dWidget(
  //     &model_config,
  //     editor2->get_scene_ref());
  // viewer2d2->show();

  // --- WIDGET TESTING
  std::map<std::string, std::unique_ptr<hesiod::Attribute>> attr = {};

  attr["somme choice"] = NEW_ATTR_BOOL(false, "toto");
  attr["seed"] = NEW_ATTR_SEED();
  attr["float"] = NEW_ATTR_FLOAT(1.f, 0.1f, 5.f, "%.3f");
  attr["int"] = NEW_ATTR_INT(32, 1, 64);
  attr["map"] = NEW_ATTR_MAPENUM(hesiod::cmap_map);
  attr["range"] = NEW_ATTR_RANGE(hmap::Vec2<float>(0.5f, 2.f), false, "%.3f");
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
