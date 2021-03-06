// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/flip_masked_cel.h"

#include "app/document.h"
#include "doc/algorithm/flip_image.h"
#include "doc/cel.h"
#include "doc/image.h"
#include "doc/layer.h"
#include "doc/mask.h"

namespace app {
namespace cmd {

FlipMaskedCel::FlipMaskedCel(Cel* cel, doc::algorithm::FlipType flipType)
  : WithCel(cel)
  , m_flipType(flipType)
  , m_bgcolor(static_cast<app::Document*>(cel->document())->bgColor(cel->layer()))
{
}

void FlipMaskedCel::onExecute()
{
  swap();
}

void FlipMaskedCel::onUndo()
{
  swap();
}

void FlipMaskedCel::swap()
{
  Cel* cel = this->cel();
  Image* image = cel->image();
  Mask* mask = static_cast<app::Document*>(cel->document())->mask();
  ASSERT(mask->bitmap());
  if (!mask->bitmap())
    return;

  int x = cel->x();
  int y = cel->y();

  mask->offsetOrigin(-x, -y);
  doc::algorithm::flip_image_with_mask(image, mask, m_flipType, m_bgcolor);
  mask->offsetOrigin(x, y);
}

} // namespace cmd
} // namespace app
