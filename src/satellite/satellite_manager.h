#pragma once

#include "satellite.h"

namespace satellite {

constexpr int MAX_VISIBLE_SATELLITES = 128;

void begin();
void clear();
bool add(const Satellite& sat);

int count();
const Satellite* get(int index);

// Automatische Auswahl
void updateAutomaticSelection(unsigned long now_ms);
const Satellite* selected();
int selectedIndex();

void selectNextManual(unsigned long now_ms);
}  // namespace satellite