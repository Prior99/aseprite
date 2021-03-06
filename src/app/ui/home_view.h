// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifndef APP_UI_HOME_VIEW_H_INCLUDED
#define APP_UI_HOME_VIEW_H_INCLUDED
#pragma once

#include "app/check_update_delegate.h"
#include "app/ui/tabs.h"
#include "app/ui/workspace_view.h"
#include "ui/box.h"

#include "generated_home_view.h"

namespace ui {
  class View;
}

namespace app {

  class NewsListBox;
  class RecentFilesListBox;
  class RecentFoldersListBox;

  class HomeView : public app::gen::HomeView
                 , public TabView
                 , public WorkspaceView
#ifdef ENABLE_UPDATER
                 , public CheckUpdateDelegate
#endif
  {
  public:
    HomeView();
    ~HomeView();

    // TabView implementation
    std::string getTabText() override;
    TabIcon getTabIcon() override;

    // WorkspaceView implementation
    ui::Widget* getContentWidget() override { return this; }
    WorkspaceView* cloneWorkspaceView() override;
    void onClonedFrom(WorkspaceView* from) override;
    bool onCloseView(Workspace* workspace) override;
    void onTabPopup(Workspace* workspace) override;
    void onWorkspaceViewSelected() override;

  protected:
#ifdef ENABLE_UPDATER
    // CheckUpdateDelegate impl
    void onCheckingUpdates() override;
    void onUpToDate() override;
    void onNewUpdate(const std::string& url, const std::string& version) override;
#endif

  private:
    void onNewFile();
    void onOpenFile();

    RecentFilesListBox* m_files;
    RecentFoldersListBox* m_folders;
    NewsListBox* m_news;
  };

} // namespace app

#endif
