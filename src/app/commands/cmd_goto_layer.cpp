// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/app.h"
#include "app/commands/command.h"
#include "app/context_access.h"
#include "app/modules/editors.h"
#include "app/modules/gui.h"
#include "app/ui/editor/editor.h"
#include "app/ui/status_bar.h"
#include "doc/layer.h"
#include "doc/sprite.h"

namespace app {

class GotoCommand : public Command {
public:
  GotoCommand(const char* short_name, const char* friendly_name, CommandFlags flags)
    : Command(short_name, friendly_name, flags) {
  }

protected:
  void updateStatusBar(DocumentLocation& location) {
    if (location.layer() != NULL)
      StatusBar::instance()
        ->setStatusText(1000, "Layer `%s' selected",
          location.layer()->name().c_str());
  }
};

class GotoPreviousLayerCommand : public GotoCommand {
public:
  GotoPreviousLayerCommand();
  Command* clone() const override { return new GotoPreviousLayerCommand(*this); }

protected:
  bool onEnabled(Context* context);
  void onExecute(Context* context);
};

GotoPreviousLayerCommand::GotoPreviousLayerCommand()
 : GotoCommand("GotoPreviousLayer",
               "Go to Previous Layer",
               CmdUIOnlyFlag)
{
}

bool GotoPreviousLayerCommand::onEnabled(Context* context)
{
  return context->checkFlags(ContextFlags::ActiveDocumentIsWritable |
                             ContextFlags::HasActiveSprite);
}

void GotoPreviousLayerCommand::onExecute(Context* context)
{
  const ContextReader reader(context);
  const Sprite* sprite = reader.sprite();
  DocumentLocation location = *reader.location();

  if (location.layerIndex() > 0)
    location.layerIndex(location.layerIndex().previous());
  else
    location.layerIndex(LayerIndex(sprite->countLayers()-1));

  // Flash the current layer
  ASSERT(current_editor != NULL && "Current editor cannot be null when we have a current sprite");
  current_editor->setLayer(location.layer());
  current_editor->flashCurrentLayer();

  updateStatusBar(location);
}

class GotoNextLayerCommand : public GotoCommand {
public:
  GotoNextLayerCommand();
  Command* clone() const override { return new GotoNextLayerCommand(*this); }

protected:
  bool onEnabled(Context* context);
  void onExecute(Context* context);
};

GotoNextLayerCommand::GotoNextLayerCommand()
  : GotoCommand("GotoNextLayer",
                "Go to Next Layer",
                CmdUIOnlyFlag)
{
}

bool GotoNextLayerCommand::onEnabled(Context* context)
{
  return context->checkFlags(ContextFlags::ActiveDocumentIsWritable |
                             ContextFlags::HasActiveSprite);
}

void GotoNextLayerCommand::onExecute(Context* context)
{
  const ContextReader reader(context);
  const Sprite* sprite = reader.sprite();
  DocumentLocation location = *reader.location();

  if (location.layerIndex() < sprite->countLayers()-1)
    location.layerIndex(location.layerIndex().next());
  else
    location.layerIndex(LayerIndex(0));

  // Flash the current layer
  ASSERT(current_editor != NULL && "Current editor cannot be null when we have a current sprite");
  current_editor->setLayer(location.layer());
  current_editor->flashCurrentLayer();

  updateStatusBar(location);
}

Command* CommandFactory::createGotoPreviousLayerCommand()
{
  return new GotoPreviousLayerCommand;
}

Command* CommandFactory::createGotoNextLayerCommand()
{
  return new GotoNextLayerCommand;
}

} // namespace app
