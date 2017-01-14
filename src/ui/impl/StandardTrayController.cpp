/*
 * Copyright (c) 2010-2014 Célio Cidral Junior.
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QIcon>

#include "TimerSignalEmitter.h"
#include "Interval.h"
#include "IntervalType.h"
#include "PomodoroEngine.h"
#include "Preferences.h"
#include "StandardTrayController.h"
#include "Tray.h"
#include "TrayIconFiles.h"

using tmty::Interval;
using tmty::IntervalType;
using tmty::PomodoroEngine;
using tmty::Preferences;
using tmty::TimerSignalEmitter;
using tmty::ui::StandardTrayController;
using tmty::ui::Tray;
using tmty::ui::TrayController;
using tmty::ui::TrayIconFiles;

StandardTrayController::StandardTrayController(Tray &tray, const TrayIconFiles &trayIconFiles, PomodoroEngine &pomodoroEngine, TimerSignalEmitter &timerSignalEmitter, Preferences &preferences, QObject *parent) :
  TrayController(parent),
  _tray(tray),
  _trayIconFiles(trayIconFiles),
  _pomodoroEngine(pomodoroEngine),
  _timerSignalEmitter(timerSignalEmitter),
  _preferences(preferences)
{
  _tray.enablePomodoroCountResetAction(false);
  switchToIdleState();

  connect(&_tray, SIGNAL(pomodoroClicked()), this, SLOT(startPomodoro()));
  connect(&_tray, SIGNAL(shortBreakClicked()), this, SLOT(startShortBreak()));
  connect(&_tray, SIGNAL(longBreakClicked()), this, SLOT(startLongBreak()));
  connect(&_tray, SIGNAL(stopTimerClicked()), this, SLOT(stopTimer()));

  connect(&_timerSignalEmitter, SIGNAL(timerSet(const Interval &)),
          this, SLOT(timerSet(const Interval &)));

  connect(&_timerSignalEmitter, SIGNAL(timerStopped(int, const Interval &)),
          this, SLOT(timerStopped()));

  connect(&_timerSignalEmitter, SIGNAL(secondElapsed(int, const Interval &)),
          this, SLOT(secondElapsed(int, const Interval &)));

  if(QApplication::instance())
  {
    connect(&tray, SIGNAL(quitClicked()),
            QApplication::instance(), SLOT(quit()));
  }
}

void StandardTrayController::switchToIdleState()
{
  Interval interval;

  updateRemainingTime(0, interval);
  _tray.setIcon(QIcon(_trayIconFiles.idle()));
  _tray.enableStopTimerAction(false);
  _tray.enablePomodoroAction(true);
  _tray.enableShortBreakAction(true);
  _tray.enableLongBreakAction(true);
}

void StandardTrayController::showTrayIcon()
{
  _tray.show();
}

void StandardTrayController::startPomodoro()
{
  _pomodoroEngine.start(IntervalType::POMODORO);
}

void StandardTrayController::startShortBreak()
{
  _pomodoroEngine.start(IntervalType::SHORT_BREAK);
}

void StandardTrayController::startLongBreak()
{
  _pomodoroEngine.start(IntervalType::LONG_BREAK);
}

void StandardTrayController::stopTimer()
{
  _pomodoroEngine.stop();
}

void StandardTrayController::timerSet(const Interval &interval)
{
  updateRemainingTime(interval.seconds(), interval);
  _tray.setIcon(QIcon(_trayIconFiles.forIntervalType(interval.type())));
  _tray.enableStopTimerAction(true);
  _tray.enablePomodoroAction(interval.type() != IntervalType::POMODORO);
  _tray.enableShortBreakAction(interval.type() != IntervalType::SHORT_BREAK);
  _tray.enableLongBreakAction(interval.type() != IntervalType::LONG_BREAK);
}

void StandardTrayController::timerStopped()
{
  switchToIdleState();
}

void StandardTrayController::secondElapsed(int secondsLeft, const Interval &interval)
{
  updateRemainingTime(secondsLeft, interval);
}

void StandardTrayController::updateRemainingTime(int secondsLeft, const Interval &interval)
{
  int minutes = secondsLeft / 60;
  int seconds = secondsLeft - minutes * 60;
  QString text;

  text.sprintf("%02d:%02d", minutes, seconds);

  _tray.setRemainingTimeText(text);

  // put remaining time on the icon
  QString figImgFile;
  figImgFile.sprintf(":images/icons/tray/%02d.png", minutes);

  QPixmap pixmap(16, 16);
  QPixmap pic(_trayIconFiles.forIntervalType(interval.type()));
  QPixmap _01pic(figImgFile);
  QRectF icon_size(0, 0, 16, 16);

  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.drawPixmap(icon_size, pic, icon_size);
  painter.drawPixmap(icon_size, _01pic, icon_size);

  _tray.setIcon(QIcon(pixmap));
}
