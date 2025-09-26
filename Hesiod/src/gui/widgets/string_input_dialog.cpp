/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QDialogButtonBox>

#include "hesiod/gui/widgets/string_input_dialog.hpp"
#include "hesiod/logger.hpp"

namespace hesiod
{

StringInputDialog::StringInputDialog(const std::string              &window_title,
                                     const std::vector<std::string> &invalid_strings,
                                     QWidget                        *parent)
    : QDialog(parent)
{
  setWindowTitle(window_title.c_str());

  QVBoxLayout *layout = new QVBoxLayout(this);

  this->input_line = new QLineEdit("graph", this);
  this->input_line->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  this->error_label = new QLabel(this);
  this->error_label->setStyleSheet("color: red;"); // errors in red
  this->error_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  this->error_label->hide();

  QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                          QDialogButtonBox::Cancel,
                                                      this);

  layout->addWidget(this->input_line);
  layout->addWidget(this->error_label);
  layout->addWidget(button_box);

  // automatically hide error label when the user types
  this->connect(this->input_line,
                &QLineEdit::textChanged,
                [this](const QString &)
                {
                  if (this->error_label && this->error_label->isVisible())
                    this->error_label->hide();
                });

  this->connect(button_box,
                &QDialogButtonBox::accepted,
                this,
                &StringInputDialog::validate_input);
  this->connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

  for (const auto &s : invalid_strings)
    invalid_strings_qset.insert(QString::fromStdString(s));

  // set a reasonable fixed width for compact appearance
  this->setMinimumWidth(300);
  this->setMaximumWidth(600);
}

void StringInputDialog::validate_input()
{
  if (!this->input_line || !this->error_label)
  {
    Logger::log()->error(
        "StringInputDialog::validate_input: internal widgets are nullptr");
    return;
  }

  QString text = this->input_line->text().trimmed();

  if (invalid_strings_qset.contains(text))
  {
    this->error_label->setText("This choice is not allowed. Choose a different one.");
    this->error_label->show();
  }
  else
  {
    this->accept(); // input is valid
  }
}

} // namespace hesiod
