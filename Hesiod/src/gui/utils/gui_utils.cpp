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

int float_to_slider_pos(float v, float min, float max, int slider_steps)
{
  return (int)((v - min) / (max - min) * (float)slider_steps);
}

int get_column_count(QGridLayout *layout)
{
  int max_col = -1;

  for (int i = 0; i < layout->count(); ++i)
  {
    int row, col, row_span, col_span;
    layout->getItemPosition(i, &row, &col, &row_span, &col_span);

    int last_col = col + col_span - 1;
    if (last_col > max_col)
      max_col = last_col;
  }

  return max_col + 1; // +1 because columns are 0-based
}

int get_row_count(QGridLayout *layout)
{
  int max_row = -1;

  for (int i = 0; i < layout->count(); ++i)
  {
    int row, col, row_span, col_span;
    layout->getItemPosition(i, &row, &col, &row_span, &col_span);

    int last_row = row + row_span - 1;
    if (last_row > max_row)
      max_row = last_row;
  }

  return max_row + 1; // +1 because rows are 0-based
}

void resize_font(QWidget *widget, int relative_size_modification)
{
  QFont font = widget->font();
  font.setPointSize(font.pointSize() + relative_size_modification);
  widget->setFont(font);
}

float slider_pos_to_float(int pos, float min, float max, int slider_steps)
{
  return min + (float)pos / (float)slider_steps * (max - min);
}

} // namespace hesiod
