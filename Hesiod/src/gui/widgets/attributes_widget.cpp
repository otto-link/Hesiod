/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

AttributesWidget::AttributesWidget(
    std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map,
    std::vector<std::string>                          *p_attr_ordered_key)
    : p_attr_map(p_attr_map), p_attr_ordered_key(p_attr_ordered_key)
{
  this->setWindowTitle("Attribute settings");

  // define the attribute order either on the key map order or on a
  // given order provided as a key list (optional)
  std::vector<std::string> attr_key_queue = {};
  bool                     check_count = false;

  for (auto &[k, v] : *p_attr_map)
    attr_key_queue.push_back(k);

  if (p_attr_ordered_key)
    if (p_attr_ordered_key->size() > 0)
    {
      attr_key_queue = *p_attr_ordered_key;
      check_count = true;
    }

  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  int row = 0;
  // to check the number of widgets corresponds to the number of keys
  // in "p_attr_ordered_key"
  int count = 0;

  for (auto &key : attr_key_queue)
  {
    if (key == "_SEPARATOR_")
    {
      QFrame *line = new QFrame;
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);
      layout->addWidget(line, row, 0, 1, 2);
    }
    else
    {
      if (!this->p_attr_map->contains(key))
        LOG_ERROR("attribute key not found: [%s]", key.c_str());

      Attribute *p_attr = this->p_attr_map->at(key).get();

      QLabel *label = new QLabel(QString::fromStdString(key));
      // label->setAlignment(Qt::AlignTop);
      layout->addWidget(label, row, 0);

      switch (p_attr->get_type())
      {

      case (AttributeType::BOOL):
      {
        BoolWidget *widget = new BoolWidget((BoolAttribute *)p_attr);
        connect(widget,
                &BoolWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::CLOUD):
      {
        CloudWidget *widget = new CloudWidget((CloudAttribute *)p_attr);
        connect(widget,
                &CloudWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::COLOR):
      {
        ColorWidget *widget = new ColorWidget((ColorAttribute *)p_attr);
        connect(widget,
                &ColorWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::FILENAME):
      {
        FilenameWidget *widget = new FilenameWidget((FilenameAttribute *)p_attr);
        connect(widget,
                &FilenameWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::FLOAT):
      {
        FloatWidget *widget = new FloatWidget((FloatAttribute *)p_attr);
        connect(widget,
                &FloatWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::INT):
      {
        IntWidget *widget = new IntWidget((IntAttribute *)p_attr);
        connect(widget,
                &IntWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::MAP_ENUM):
      {
        MapEnumWidget *widget = new MapEnumWidget((MapEnumAttribute *)p_attr);
        connect(widget,
                &MapEnumWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::RANGE):
      {
        RangeWidget *widget = new RangeWidget((RangeAttribute *)p_attr);
        connect(widget,
                &RangeWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::SEED):
      {
        SeedWidget *widget = new SeedWidget((SeedAttribute *)p_attr);
        connect(widget,
                &SeedWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::STRING):
      {
        StringWidget *widget = new StringWidget((StringAttribute *)p_attr);
        connect(widget,
                &StringWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::VEC_FLOAT):
      {
        VecFloatWidget *widget = new VecFloatWidget((VecFloatAttribute *)p_attr);
        connect(widget,
                &VecFloatWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::VEC_INT):
      {
        VecIntWidget *widget = new VecIntWidget((VecIntAttribute *)p_attr);
        connect(widget,
                &VecIntWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      case (AttributeType::WAVE_NB):
      {
        WaveNbWidget *widget = new WaveNbWidget((WaveNbAttribute *)p_attr);
        connect(widget,
                &WaveNbWidget::value_changed,
                [this]() { Q_EMIT this->value_changed(); });
        layout->addWidget(widget, row, 1);
      }
      break;

      default:
        LOG_ERROR("attribute settings could not be created, for type [%d]",
                  (int)p_attr->get_type());
      }
      count++;
    }

    row++;
  }

  if (check_count && count != (int)this->p_attr_map->size())
  {
    LOG_ERROR("missing attributes in AttributesWidget (check attr_ordered_key)");
    throw std::runtime_error(
        "missing attributes in AttributesWidget (check attr_ordered_key)");
  }

  // as a last resort, for empty "settings"
  if (p_attr_map->size() == 0)
  {
    QLabel *widget = new QLabel("no settings");
    QFont   f = widget->font();
    f.setItalic(true);
    widget->setFont(f);
    layout->addWidget(widget, 0, 0);
  }

  this->setLayout(layout);
}

} // namespace hesiod
