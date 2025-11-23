/* Copyright (c) 2025 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <QApplication>
#include <QPropertyAnimation>

#include "hesiod/gui/widgets/splash_screen.hpp"

namespace hesiod
{

SplashScreen::SplashScreen()
{
  QPixmap pixmap("data/splash.png");
  this->setPixmap(pixmap);
  this->show();
  QApplication::processEvents();
}

void SplashScreen::close()
{
  // Fade out animation
  QPropertyAnimation *anim = new QPropertyAnimation(this, "windowOpacity");

  anim->setDuration(1000);
  anim->setStartValue(1.f);
  anim->setEndValue(0.f);
  anim->setEasingCurve(QEasingCurve::Linear);

  QObject::connect(anim,
                   &QPropertyAnimation::finished,
                   this,
                   &QSplashScreen::deleteLater);

  anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SplashScreen::show_message(const QString &message)
{
  this->showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::white);
  QApplication::processEvents();
}
} // namespace hesiod
