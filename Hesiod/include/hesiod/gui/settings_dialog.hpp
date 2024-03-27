/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStringList>
#include <QWidget>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/NodeDelegateModel>

#include "hesiod/gui/gui_utils.hpp"
#include "hesiod/model/attributes.hpp"

#define HSD_SLIDER_STEPS 128

namespace hesiod
{

class SettingsDialog : public QDialog
{
public:
  SettingsDialog(QWidget *parent = nullptr) : QDialog(parent)
  {
    this->setWindowTitle(tr("Settings dialog"));
  }

  void update_layout(QtNodes::DataFlowGraphModel &model,
                     std::vector<QtNodes::NodeId> selected_node_id)
  {
    // "immediate mode", clear the layout before rebuilding it
    if (this->layout())
    {
      clear_layout(this->layout());
      delete this->layout();
    }

    QGridLayout *layout = new QGridLayout(this);
    int          row = 0; // line number

    for (auto &node_id : selected_node_id)
    {
      hesiod::BaseNode *p_node = model.delegateModel<hesiod::BaseNode>(node_id);

      for (auto &[key, attr] : p_node->attr)
      {
        int     col = 0;
        QLabel *label = new QLabel(QString::fromStdString(key));
        layout->addWidget(label, row, col++);

        switch (attr->get_type())
        {

        case AttributeType::MAP_ENUM:
        {
          MapEnumAttribute *p_attr = attr->get_ref<MapEnumAttribute>();

          QComboBox  *combo_box = new QComboBox();
          QStringList items;
          for (auto &[key, dummy] : p_attr->value)
            combo_box->addItem(key.c_str());

          combo_box->setCurrentText(p_attr->choice.c_str());

          QObject::connect(combo_box,
                           qOverload<int>(&QComboBox::activated),
                           [p_node, p_attr, combo_box](int index)
                           {
                             p_attr->choice = combo_box->itemText(index).toStdString();
                             p_node->compute();
                           });

          layout->addWidget(combo_box, row, col++);
        }
        break;

        case AttributeType::SEED:
        {
          SeedAttribute *p_attr = attr->get_ref<SeedAttribute>();

          //
          QSpinBox *spinbox = new QSpinBox();
          spinbox->setMinimum(0);
          spinbox->setMaximum((int)std::numeric_limits<int>::max());
          spinbox->setValue((int)p_attr->value);
          QObject::connect(spinbox,
                           qOverload<int>(&QSpinBox::valueChanged),
                           [p_node, p_attr, spinbox]()
                           {
                             LOG_DEBUG("%d", (int)p_attr->value);
                             p_attr->value = (uint)spinbox->value();
                             p_node->compute();
                           });
          layout->addWidget(spinbox, row, col++);

          //
          QPushButton *button = new QPushButton("New seed");
          QObject::connect(button,
                           &QPushButton::released,
                           [p_node, p_attr, spinbox]()
                           {
                             p_attr->value = (uint)time(NULL);
                             spinbox->setValue((int)p_attr->value);
                             p_node->compute();
                           });
          layout->addWidget(button, row, col++);
        }
        break;

        case AttributeType::WAVE_NB:
        {
          WaveNbAttribute *p_attr = attr->get_ref<WaveNbAttribute>();

          QCheckBox *checkbox = new QCheckBox("link x and y");
          checkbox->setChecked(p_attr->link_xy);
          layout->addWidget(checkbox, row, col++);

          // next row
          row++;
          col = 1;

          //
          QSlider *slider_x = new QSlider(Qt::Horizontal, this);
          slider_x->setRange(0, HSD_SLIDER_STEPS);
          slider_x->setSingleStep(1);
          slider_x->setValue((int)(HSD_SLIDER_STEPS * (p_attr->value.x - p_attr->vmin) /
                                   (p_attr->vmax - p_attr->vmin)));
          layout->addWidget(slider_x, row, col++);
          LOG_DEBUG("%d", slider_x->value());

          //
          QLabel *label_x = new QLabel(
              QString().asprintf(p_attr->fmt.c_str(), p_attr->value.x));
          layout->addWidget(label_x, row, col++);

          // next row
          row++;
          col = 1;

          //
          QSlider *slider_y = new QSlider(Qt::Horizontal, this);
          slider_y->setRange(0, HSD_SLIDER_STEPS);
          slider_y->setSingleStep(1);
          slider_y->setValue((int)(HSD_SLIDER_STEPS * (p_attr->value.y - p_attr->vmin) /
                                   (p_attr->vmax - p_attr->vmin)));
          layout->addWidget(slider_y, row, col++);

          //
          QLabel *label_y = new QLabel(
              QString().asprintf(p_attr->fmt.c_str(), p_attr->value.y));
          layout->addWidget(label_y, row, col++);

          // connections
          QObject::connect(checkbox,
                           &QCheckBox::clicked,
                           [p_node, p_attr, slider_x, slider_y, label_y]()
                           {
                             p_attr->link_xy = !p_attr->link_xy;
                             // if the wavenumbers are now linked, the node needs to be
                             // updated
                             if (p_attr->link_xy)
                             {
                               p_attr->value.y = p_attr->value.x;
                               slider_y->setValue(slider_x->value());
                               QString vstr = QString().asprintf(p_attr->fmt.c_str(),
                                                                 p_attr->value.y);
                               label_y->setText(vstr);
                               p_node->compute();
                             }
                           });

          QObject::connect(
              slider_x,
              &QSlider::sliderReleased,
              [p_node, p_attr, slider_x, label_x, slider_y, label_y]()
              {
                float val = p_attr->vmin + (float)slider_x->value() *
                                               (p_attr->vmax - p_attr->vmin) /
                                               HSD_SLIDER_STEPS;

                p_attr->value.x = val;
                QString vstr = QString().asprintf(p_attr->fmt.c_str(), p_attr->value.x);
                label_x->setText(vstr);

                // update 'y' slider if directions are linked
                if (p_attr->link_xy)
                {
                  p_attr->value.y = p_attr->value.x;
                  slider_y->setValue(slider_x->value());
                  QString vstr = QString().asprintf(p_attr->fmt.c_str(), p_attr->value.y);
                  label_y->setText(vstr);
                }

                p_node->compute();
              });

          QObject::connect(
              slider_y,
              &QSlider::sliderReleased,
              [p_node, p_attr, slider_x, slider_y, label_y]()
              {
                if (p_attr->link_xy)
                {
                  // if x and y are linked, just ignore the slider new value
                  slider_y->setValue(slider_x->value());
                }
                else
                {
                  p_attr->value.y = p_attr->vmin + (float)slider_y->value() *
                                                       (p_attr->vmax - p_attr->vmin) /
                                                       HSD_SLIDER_STEPS;
                  QString vstr = QString().asprintf(p_attr->fmt.c_str(), p_attr->value.y);
                  label_y->setText(vstr);
                  p_node->compute();
                }
              });
        }
        break;
        }

        // next line
        row++;
      }
    }

    this->setLayout(layout);
  }
};

} // namespace hesiod