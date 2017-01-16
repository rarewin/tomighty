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

#include "StandardPomodoroEngine.h"
#include "../IntervalType.h"
#include "../Preferences.h"
#include "../Timer.h"

using tmty::IntervalType;
using tmty::Preferences;
using tmty::StandardPomodoroEngine;
using tmty::Timer;

StandardPomodoroEngine::StandardPomodoroEngine(Timer &timer, Preferences &preferences, QObject *parent) :
  PomodoroEngine(parent),
  _timer(timer),
  _preferences(preferences)
{}

void StandardPomodoroEngine::start(IntervalType intervalType)
{
  stop();
  int intervalLengthInSeconds = 60 * _preferences.intervalLengthInMinutes(intervalType);
  _timer.set(intervalLengthInSeconds, intervalType);
}

void StandardPomodoroEngine::stop()
{
  if(_timer.isRunning())
  {
    _timer.interrupt();
  }
}

void StandardPomodoroEngine::changeSettingSoundNotificationTimerIsSet(bool shouldPlay)
{
  _preferences.setShouldPlaySoundNotificationWhenTimerIsSet(shouldPlay);
}

void StandardPomodoroEngine::changeSettingSoundNotificationDuringPomodoros(bool shouldPlay)
{
  _preferences.setShouldPlaySoundNotificationDuringPomodoros(shouldPlay);
}

void StandardPomodoroEngine::changeSettingSoundNotificationDuringBreaks(bool shouldPlay)
{
  _preferences.setShouldPlaySoundNotificationDuringBreaks(shouldPlay);
}

void StandardPomodoroEngine::changeSettingSoundNotificationWhenTimerExpires(bool shouldPlay)
{
  _preferences.setShouldPlaySoundNotificationWhenTimerExpires(shouldPlay);
}
