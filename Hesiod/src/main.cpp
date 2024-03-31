/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
typedef unsigned int uint;
#include <QApplication>

#include "highmap/vector.hpp"
#include "macrologger.h"

#include "hesiod/gui/node_editor_widget.hpp"
#include "hesiod/gui/viewer2d_widget.hpp"

// for testing - TO REMOVE
#include "hesiod/gui/widgets.hpp"
#include "hesiod/model/attributes.hpp"
#include "hesiod/model/enum_mapping.hpp"

static void set_style()
{
  QtNodes::ConnectionStyle::setConnectionStyle(
      R"(
  {
    "ConnectionStyle": {
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
  QApplication::setFont(font);

  app.setStyleSheet(R"(
QSlider::groove:horizontal { 
	background: #C9CDD0;
	height: 16px; 
	border-radius: 4px;
}

QSlider::handle:horizontal { 
	background-color: #51A0D5; 
	width: 16px; 
	height: 16px; 
	border-radius: 8px;
}
)");
  set_style();

  hesiod::ModelConfig model_config;
  model_config.set_shape({1024, 1024});
  model_config.tiling = {4, 4};

  //
  auto editor = new hesiod::NodeEditorWidget(model_config);
  editor->show();

  hesiod::Viewer2dWidget *viewer2d = new hesiod::Viewer2dWidget(&model_config,
                                                                editor->get_scene_ref());
  viewer2d->show();

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

  hesiod::AttributesWidget *sw = new hesiod::AttributesWidget(&attr);
  // sw->show();

  // ---

  return app.exec();
}
