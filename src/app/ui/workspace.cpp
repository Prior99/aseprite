// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/ui/workspace.h"

#include "app/app.h"
#include "app/ui/main_window.h"
#include "app/ui/skin/skin_theme.h"
#include "app/ui/tabs.h"
#include "app/ui/workspace_view.h"
#include "base/remove_from_container.h"

#include <algorithm>
#include <queue>

namespace app {

using namespace app::skin;
using namespace ui;

Workspace::Workspace()
  : Widget(kGenericWidget)
  , m_activeView(nullptr)
{
  SkinTheme* theme = static_cast<SkinTheme*>(getTheme());
  setBgColor(theme->colors.workspace());
}

Workspace::~Workspace()
{
  // No views at this point.
  ASSERT(m_views.empty());
}

void Workspace::addView(WorkspaceView* view)
{
  m_views.push_back(view);

  App::instance()->getMainWindow()->getTabsBar()->addTab(dynamic_cast<TabView*>(view));
  setActiveView(view);
}

void Workspace::removeView(WorkspaceView* view)
{
  base::remove_from_container(m_views, view);

  Widget* content = view->getContentWidget();
  if (content->getParent())
    content->getParent()->removeChild(content);

  // Remove related tab
  Tabs* tabs = App::instance()->getMainWindow()->getTabsBar();
  tabs->removeTab(dynamic_cast<TabView*>(view));

  TabView* tabView = tabs->getSelectedTab();
  setActiveView(dynamic_cast<WorkspaceView*>(tabView));
}

bool Workspace::closeView(WorkspaceView* view)
{
  return view->onCloseView(this);
}

WorkspaceView* Workspace::activeView()
{
  return m_activeView;
}

void Workspace::setActiveView(WorkspaceView* view)
{
  m_activeView = view;

  removeAllChildren();
  if (view)
    addChild(view->getContentWidget());

  layout();

  ActiveViewChanged();          // Fire ActiveViewChanged event
}

void Workspace::onPaint(PaintEvent& ev)
{
  ev.getGraphics()->fillRect(getBgColor(), getClientBounds());
}

} // namespace app
