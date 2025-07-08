/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "hesiod/gui/widgets/select_string_dialog.hpp"

namespace hesiod
{

SelectStringDialog::SelectStringDialog(const std::vector<std::string> &options,
                                       const std::string              &message,
                                       QWidget                        *parent)
    : QDialog(parent)
{
  this->setWindowTitle("Select a Value");
  QVBoxLayout *main_layout = new QVBoxLayout(this);

  QLabel *label = new QLabel(message.c_str());
  main_layout->addWidget(label);

  QStringList string_list;
  for (auto &s : options)
  {
    string_list.append(QString::fromStdString(s));
  }

  this->combo_box = new QComboBox;
  this->combo_box->addItems(string_list);
  main_layout->addWidget(this->combo_box);

  QHBoxLayout *button_layout = new QHBoxLayout;
  QPushButton *ok_button = new QPushButton("OK");
  QPushButton *cancel_button = new QPushButton("Cancel");

  button_layout->addStretch();
  button_layout->addWidget(ok_button);
  button_layout->addWidget(cancel_button);

  main_layout->addLayout(button_layout);

  connect(ok_button, &QPushButton::clicked, this, &QDialog::accept);
  connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
}

std::string SelectStringDialog::selected_value() const
{
  return this->combo_box->currentText().toStdString();
}

} // namespace hesiod
