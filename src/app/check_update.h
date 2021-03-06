// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifndef APP_CHECK_UPDATE_H_INCLUDED
#define APP_CHECK_UPDATE_H_INCLUDED
#pragma once

#ifdef ENABLE_UPDATER

#include "base/thread.h"
#include "base/unique_ptr.h"
#include "ui/timer.h"
#include "updater/check_update.h"

namespace app {

  class CheckUpdateDelegate;
  class CheckUpdateBackgroundJob;

  class CheckUpdateThreadLauncher {
  public:
    CheckUpdateThreadLauncher(CheckUpdateDelegate* delegate);
    ~CheckUpdateThreadLauncher();

    void launch();

    bool isReceived() const;

    const updater::CheckUpdateResponse& getResponse() const
    {
      return m_response;
    }

  private:
    void onMonitoringTick();
    void checkForUpdates();

    CheckUpdateDelegate* m_delegate;
    updater::Uuid m_uuid;
    base::UniquePtr<base::thread> m_thread;
    base::UniquePtr<CheckUpdateBackgroundJob> m_bgJob;
    bool m_doCheck;
    bool m_received;

    // Mini-stats
    int m_inits;
    int m_exits;

    // True if this is a developer
    bool m_isDeveloper;

    updater::CheckUpdateResponse m_response;
    ui::Timer m_timer;
  };

} // namespace app

#endif // ENABLE_UPDATER

#endif // APP_CHECK_UPDATE_H_INCLUDED
