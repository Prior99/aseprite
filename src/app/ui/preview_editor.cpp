// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/ui/preview_editor.h"

#include "app/app.h"
#include "app/document.h"
#include "app/handle_anidir.h"
#include "app/ini_file.h"
#include "app/loop_tag.h"
#include "app/modules/editors.h"
#include "app/modules/gui.h"
#include "app/pref/preferences.h"
#include "app/ui/editor/editor.h"
#include "app/ui/editor/editor_view.h"
#include "app/ui/editor/navigate_state.h"
#include "app/ui/skin/skin_button.h"
#include "app/ui/skin/skin_theme.h"
#include "app/ui/status_bar.h"
#include "app/ui/toolbar.h"
#include "app/ui_context.h"
#include "base/bind.h"
#include "doc/sprite.h"
#include "gfx/rect.h"
#include "ui/base.h"
#include "ui/button.h"
#include "ui/close_event.h"
#include "ui/message.h"
#include "ui/system.h"

#include "doc/frame_tag.h"

namespace app {

using namespace app::skin;
using namespace ui;

class MiniCenterButton : public SkinButton<CheckBox> {
public:
  MiniCenterButton()
    : SkinButton<CheckBox>(
      PART_WINDOW_CENTER_BUTTON_NORMAL,
      PART_WINDOW_CENTER_BUTTON_HOT,
      PART_WINDOW_CENTER_BUTTON_SELECTED)
  {
    setup_bevels(this, 0, 0, 0, 0);
    setDecorative(true);
    setSelected(true);
  }

protected:
  void onSetDecorativeWidgetBounds() override
  {
    SkinTheme* theme = static_cast<SkinTheme*>(getTheme());
    Widget* window = getParent();
    gfx::Rect rect(0, 0, 0, 0);
    gfx::Size iconSize = theme->get_part_size(PART_WINDOW_PLAY_BUTTON_NORMAL);
    gfx::Size closeSize = theme->get_part_size(PART_WINDOW_CLOSE_BUTTON_NORMAL);

    rect.w = iconSize.w;
    rect.h = iconSize.h;

    rect.offset(window->getBounds().x2() - 3*guiscale()
      - iconSize.w - 1*guiscale()
      - iconSize.w - 1*guiscale() - closeSize.w,
      window->getBounds().y + 3*guiscale());

    setBounds(rect);
  }

  bool onProcessMessage(Message* msg) override
  {
    switch (msg->type()) {

      case kSetCursorMessage:
        ui::set_mouse_cursor(kArrowCursor);
        return true;
    }

    return SkinButton<CheckBox>::onProcessMessage(msg);
  }
};

class MiniPlayButton : public SkinButton<Button> {
public:
  MiniPlayButton()
    : SkinButton<Button>(PART_WINDOW_PLAY_BUTTON_NORMAL,
                         PART_WINDOW_PLAY_BUTTON_HOT,
                         PART_WINDOW_PLAY_BUTTON_SELECTED)
    , m_isPlaying(false)
  {
    setup_bevels(this, 0, 0, 0, 0);
    setDecorative(true);
  }

  bool isPlaying() { return m_isPlaying; }

protected:
  void onClick(Event& ev) override
  {
    m_isPlaying = !m_isPlaying;
    if (m_isPlaying)
      setParts(PART_WINDOW_STOP_BUTTON_NORMAL,
               PART_WINDOW_STOP_BUTTON_HOT,
               PART_WINDOW_STOP_BUTTON_SELECTED);
    else
      setParts(PART_WINDOW_PLAY_BUTTON_NORMAL,
               PART_WINDOW_PLAY_BUTTON_HOT,
               PART_WINDOW_PLAY_BUTTON_SELECTED);

    SkinButton<Button>::onClick(ev);
  }

  void onSetDecorativeWidgetBounds() override
  {
    SkinTheme* theme = static_cast<SkinTheme*>(getTheme());
    Widget* window = getParent();
    gfx::Rect rect(0, 0, 0, 0);
    gfx::Size playSize = theme->get_part_size(PART_WINDOW_PLAY_BUTTON_NORMAL);
    gfx::Size closeSize = theme->get_part_size(PART_WINDOW_CLOSE_BUTTON_NORMAL);

    rect.w = playSize.w;
    rect.h = playSize.h;

    rect.offset(window->getBounds().x2() - 3*guiscale()
      - playSize.w - 1*guiscale() - closeSize.w,
      window->getBounds().y + 3*guiscale());

    setBounds(rect);
  }

  bool onProcessMessage(Message* msg) override
  {
    switch (msg->type()) {

      case kSetCursorMessage:
        ui::set_mouse_cursor(kArrowCursor);
        return true;
    }

    return SkinButton<Button>::onProcessMessage(msg);
  }

private:
  bool m_isPlaying;
};

PreviewEditorWindow::PreviewEditorWindow()
  : Window(WithTitleBar, "Preview")
  , m_docView(NULL)
  , m_centerButton(new MiniCenterButton())
  , m_playButton(new MiniPlayButton())
  , m_playTimer(10)
  , m_pingPongForward(true)
  , m_refFrame(0)
{
  child_spacing = 0;
  setAutoRemap(false);
  setWantFocus(false);

  m_isEnabled = get_config_bool("MiniEditor", "Enabled", true);

  m_centerButton->Click.connect(Bind<void>(&PreviewEditorWindow::onCenterClicked, this));
  m_playButton->Click.connect(Bind<void>(&PreviewEditorWindow::onPlayClicked, this));

  addChild(m_centerButton);
  addChild(m_playButton);

  m_playTimer.Tick.connect(&PreviewEditorWindow::onPlaybackTick, this);
}

PreviewEditorWindow::~PreviewEditorWindow()
{
  set_config_bool("MiniEditor", "Enabled", m_isEnabled);
}

void PreviewEditorWindow::setPreviewEnabled(bool state)
{
  m_isEnabled = state;

  updateUsingEditor(current_editor);
}

bool PreviewEditorWindow::onProcessMessage(ui::Message* msg)
{
  switch (msg->type()) {

    case kOpenMessage:
      {
        // Default bounds
        int width = ui::display_w()/4;
        int height = ui::display_h()/4;
        int extra = 2*kEditorViewScrollbarWidth*guiscale();
        setBounds(
          gfx::Rect(
            ui::display_w() - width - ToolBar::instance()->getBounds().w - extra,
            ui::display_h() - height - StatusBar::instance()->getBounds().h - extra,
            width, height));

        load_window_pos(this, "MiniEditor");
        invalidate();
      }
      break;

    case kCloseMessage:
      save_window_pos(this, "MiniEditor");
      break;

  }

  return Window::onProcessMessage(msg);
}

void PreviewEditorWindow::onClose(ui::CloseEvent& ev)
{
  Button* closeButton = dynamic_cast<Button*>(ev.getSource());
  if (closeButton != NULL &&
      closeButton->getId() == SkinTheme::kThemeCloseButtonId) {
    // Here we don't use "setMiniEditorEnabled" to change the state of
    // "m_isEnabled" because we're coming from a close event of the
    // window.
    m_isEnabled = false;

    // Redraw the tool bar because it shows the mini editor enabled
    // state. TODO abstract this event
    ToolBar::instance()->invalidate();

    delete m_docView;
    m_docView = NULL;
  }
}

void PreviewEditorWindow::onWindowResize()
{
  Window::onWindowResize();

  DocumentView* view = UIContext::instance()->activeView();
  if (view)
    updateUsingEditor(view->getEditor());
}

void PreviewEditorWindow::onCenterClicked()
{
  if (m_centerButton->isSelected()) {
    DocumentView* view = UIContext::instance()->activeView();
    if (view)
      updateUsingEditor(view->getEditor());
  }
}

void PreviewEditorWindow::onPlayClicked()
{
  Editor* miniEditor = (m_docView ? m_docView->getEditor(): nullptr);

  if (m_playButton->isPlaying()) {
    if (miniEditor && miniEditor->document())
      m_nextFrameTime = miniEditor->sprite()->frameDuration(miniEditor->frame());
    else
      m_nextFrameTime = -1;

    m_curFrameTick = ui::clock();
    m_pingPongForward = true;

    m_playTimer.start();
  }
  else {
    m_playTimer.stop();

    if (miniEditor)
      miniEditor->setFrame(m_refFrame);
  }
}

void PreviewEditorWindow::updateUsingEditor(Editor* editor)
{
  if (!m_isEnabled || !editor) {
    hideWindow();
    return;
  }

  Document* document = editor->document();
  Editor* miniEditor = (m_docView ? m_docView->getEditor(): NULL);

  if (!isVisible())
    openWindow();

  gfx::Rect visibleBounds = editor->getVisibleSpriteBounds();
  gfx::Point centerPoint = visibleBounds.getCenter();
  bool center = (m_centerButton->isSelected());

  // Set the same location as in the given editor.
  if (!miniEditor || miniEditor->document() != document) {
    delete m_docView;
    m_docView = new DocumentView(document, DocumentView::Preview);
    addChild(m_docView);

    miniEditor = m_docView->getEditor();
    miniEditor->setZoom(render::Zoom(1, 1));
    miniEditor->setState(EditorStatePtr(new NavigateState));
    layout();
    center = true;
  }

  if (center)
    miniEditor->centerInSpritePoint(centerPoint);

  miniEditor->setLayer(editor->layer());
  miniEditor->setFrame(m_refFrame = editor->frame());
}

void PreviewEditorWindow::uncheckCenterButton()
{
  if (m_centerButton->isSelected())
    m_centerButton->setSelected(false);
}

void PreviewEditorWindow::hideWindow()
{
  delete m_docView;
  m_docView = NULL;

  if (isVisible())
    closeWindow(NULL);
}

void PreviewEditorWindow::onPlaybackTick()
{
  Editor* miniEditor = (m_docView ? m_docView->getEditor(): NULL);
  if (!miniEditor)
    return;

  doc::Document* document = miniEditor->document();
  doc::Sprite* sprite = miniEditor->sprite();
  if (!document || !sprite)
    return;

  if (m_nextFrameTime >= 0) {
    m_nextFrameTime -= (ui::clock() - m_curFrameTick);

    // TODO get the frame tag in updateUsingEditor()
    doc::FrameTag* tag = get_shortest_tag(sprite, m_refFrame);
    if (!tag)
      tag = get_loop_tag(sprite);

    while (m_nextFrameTime <= 0) {
      doc::frame_t frame = calculate_next_frame(
        sprite,
        miniEditor->frame(),
        tag,
        m_pingPongForward);

      miniEditor->setFrame(frame);

      m_nextFrameTime += miniEditor->sprite()->frameDuration(frame);
    }

    m_curFrameTick = ui::clock();
  }

  invalidate();
}

} // namespace app
