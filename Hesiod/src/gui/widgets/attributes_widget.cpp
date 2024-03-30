/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QGridLayout>

#include "hesiod/gui/widgets.hpp"

namespace hesiod
{

AttributesWidget::AttributesWidget(
    std::map<std::string, std::unique_ptr<Attribute>> *p_attr_map)
    : p_attr_map(p_attr_map)
{
  this->setWindowTitle("Attribute settings");

  QGridLayout *layout = new QGridLayout(this);

  int row = 0;

  for (auto &[key, attr] : *this->p_attr_map)
  {
    Attribute *p_attr = attr.get();

    QLabel *label = new QLabel(QString::fromStdString(key));
    // label->setAlignment(Qt::AlignTop);
    layout->addWidget(label, row, 0);

    switch (p_attr->get_type())
    {

    case (AttributeType::BOOL):
    {
      QWidget *widget = new BoolWidget((BoolAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::FILENAME):
    {
      QWidget *widget = new FilenameWidget((FilenameAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::FLOAT):
    {
      QWidget *widget = new FloatWidget((FloatAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::INT):
    {
      QWidget *widget = new IntWidget((IntAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::MAP_ENUM):
    {
      QWidget *widget = new MapEnumWidget((MapEnumAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::RANGE):
    {
      QWidget *widget = new RangeWidget((RangeAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::SEED):
    {
      QWidget *widget = new SeedWidget((SeedAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    case (AttributeType::WAVE_NB):
    {
      QWidget *widget = new WaveNbWidget((WaveNbAttribute *)p_attr);
      layout->addWidget(widget, row, 1);
    }
    break;

    default:
      LOG_ERROR("attribute settings could not be created, for type [%d]",
                (int)p_attr->get_type());
    }

    row++;
  }

  // this->label_x = new QLabel(
  //     QString().asprintf(this->p_attr->fmt.c_str(), this->p_attr->value.x));
  // layout->addWidget(this->label_x, 0, 1);

  this->setLayout(layout);
}

} // namespace hesiod
