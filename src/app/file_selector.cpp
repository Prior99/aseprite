// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/file_selector.h"

#include "app/app.h"
#include "app/pref/preferences.h"
#include "app/ui/file_selector.h"
#include "base/split_string.h"
#include "she/display.h"
#include "she/native_dialogs.h"
#include "she/system.h"

namespace app {

std::string show_file_selector(const std::string& title,
  const std::string& initialPath,
  const std::string& showExtensions,
  FileSelectorType type)
{
  if (App::instance()->preferences().experimental.useNativeFileDialog() &&
      she::instance()->nativeDialogs()) {
    she::FileDialog* dlg =
      she::instance()->nativeDialogs()->createFileDialog();

    if (dlg) {
      std::string res;

      dlg->setTitle(title);
      dlg->setFileName(initialPath);

      if (type == FileSelectorType::Save)
        dlg->toSaveFile();
      else
        dlg->toOpenFile();

      std::vector<std::string> tokens;
      base::split_string(showExtensions, tokens, ",");
      std::string known;
      for (const auto& tok : tokens) {
        if (!known.empty())
          known.push_back(';');
        known += "*." + tok;
      }
      dlg->addFilter(known, "Known file types (" + known + ")");
      for (const auto& tok : tokens)
        dlg->addFilter("*." + tok, tok + " files (*." + tok + ")");
      dlg->addFilter("*.*", "All files (*.*)");

      if (dlg->show(she::instance()->defaultDisplay()->nativeHandle())) {
        res = dlg->getFileName();
        dlg->dispose();
      }
      return res;
    }
  }

  FileSelector fileSelector;
  return fileSelector.show(title, initialPath, showExtensions);
}

} // namespace app
