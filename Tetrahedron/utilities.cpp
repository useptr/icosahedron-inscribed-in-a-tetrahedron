﻿//#define NOMINMAX
#include "stdafx.h"
#include "utilities.h"
#include <limits>
#include <random>
// TODO maybe make template
short getRandomColor() {
  static std::default_random_engine engine;
  static std::uniform_int_distribution<short> dist(
      1, 6); // std::numeric_limits<T>::max()
  return dist(engine);
}
