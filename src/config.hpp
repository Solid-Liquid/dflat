#pragma once

#include "type.hpp"

namespace dflat::config {

inline bool traceParse = false;
inline bool traceTypeCheck = false;
inline unsigned traceIndent = 2;

// Where to stick things when there's no class (e.g. during tests).
inline ValueType const globalClass("");

inline String thisName("this");

} // namespace dflat::config
