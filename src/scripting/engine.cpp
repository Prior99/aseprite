// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#include "scripting/engine.h"

#ifdef HAVE_V8_LIBRARY
#include "scripting/engine_v8.h"
#else
#include "scripting/engine_none.h"
#endif

namespace scripting {

  Engine::Engine() : m_impl(new EngineImpl) {
  }

  Engine::~Engine() {
    delete m_impl;
  }

  bool Engine::supportEval() const {
    return m_impl->supportEval();
  }

  void Engine::eval(const std::string& script) {
    m_impl->eval(script);
  }

} // namespace scripting
