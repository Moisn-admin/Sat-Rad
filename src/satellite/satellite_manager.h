#pragma once

#include "satellite.h"

namespace satellite
{

constexpr int MAX_VISIBLE_SATELLITES = 128;

void begin();

void clear();

bool add(const Satellite& sat);

int count();

const Satellite* get(int index);

}