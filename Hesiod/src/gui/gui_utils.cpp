/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QWidget>

#include "hesiod/gui/gui_utils.hpp"

namespace hesiod
{

DocumentationPopup::DocumentationPopup(const std::string &title,
                                       const std::string &html_source,
                                       QWidget           *parent)
    : QWidget(parent)
{
  this->setWindowTitle(title.c_str());
  this->setMinimumSize(512, 768);

  QVBoxLayout *layout = new QVBoxLayout(this);

  // Create a QTextBrowser
  QTextBrowser *text_browser = new QTextBrowser(this);
  text_browser->setHtml(html_source.c_str());

  QPushButton *close_button = new QPushButton("Close", this);

  layout->addWidget(text_browser);
  layout->addWidget(close_button);
  this->setLayout(layout);

  connect(close_button, &QPushButton::clicked, this, &QWidget::close);
}

// https://stackoverflow.com/questions/4857188

void clear_layout(QLayout *layout)
{
  if (layout == nullptr)
    return;

  QLayoutItem *item;

  while ((item = layout->takeAt(0)))
  {
    if (item->layout())
    {
      clear_layout(item->layout());
      delete item->layout();
    }
    if (item->widget())
    {
      delete item->widget();
    }
    delete item;
  }
}

void resize_font(QWidget *widget, int relative_size_modification)
{
  QFont font = widget->font();
  font.setPointSize(font.pointSize() + relative_size_modification);
  widget->setFont(font);
}

} // namespace hesiod
