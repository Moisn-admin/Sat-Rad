#include "satellite_manager.h"

#include <cstring>

namespace satellite {

namespace {

constexpr float kMinimumAutoElevationDeg = 30.0f;
constexpr unsigned long kSelectionIntervalMs = 6000UL;

Satellite g_satellites[MAX_VISIBLE_SATELLITES];
int g_count = 0;

char g_selected_name[32] = {};
int g_selected_index = -1;
unsigned long g_last_selection_ms = 0;

int findSelectedByName() {
  if (g_selected_name[0] == '\0') {
    return -1;
  }

  for (int i = 0; i < g_count; ++i) {
    if (std::strncmp(
            g_satellites[i].name,
            g_selected_name,
            sizeof(g_selected_name)) == 0) {
      return i;
    }
  }

  return -1;
}

int findHighestSatellite() {
  int best_index = -1;
  float best_elevation = -90.0f;

  for (int i = 0; i < g_count; ++i) {
    if (!g_satellites[i].visible) {
      continue;
    }

    if (g_satellites[i].elevation > best_elevation) {
      best_elevation = g_satellites[i].elevation;
      best_index = i;
    }
  }

  return best_index;
}

int findNextAbove30(int current_index) {
  if (g_count <= 0) {
    return -1;
  }

  for (int step = 1; step <= g_count; ++step) {
    const int index = (current_index + step) % g_count;

    if (g_satellites[index].visible &&
        g_satellites[index].elevation >=
            kMinimumAutoElevationDeg) {
      return index;
    }
  }

  return -1;
}

void selectIndex(int index) {
  if (index < 0 || index >= g_count) {
    g_selected_index = -1;
    g_selected_name[0] = '\0';
    return;
  }

  g_selected_index = index;

  std::strncpy(
      g_selected_name,
      g_satellites[index].name,
      sizeof(g_selected_name) - 1);

  g_selected_name[sizeof(g_selected_name) - 1] = '\0';
}

}  // namespace

void begin() {
  g_count = 0;
  g_selected_index = -1;
  g_selected_name[0] = '\0';
  g_last_selection_ms = 0;
}

void clear() {
  g_count = 0;
  g_selected_index = -1;
}

bool add(const Satellite& sat) {
  if (g_count >= MAX_VISIBLE_SATELLITES) {
    return false;
  }

  g_satellites[g_count++] = sat;
  return true;
}

int count() {
  return g_count;
}

const Satellite* get(int index) {
  if (index < 0 || index >= g_count) {
    return nullptr;
  }

  return &g_satellites[index];
}

void updateAutomaticSelection(unsigned long now_ms) {
  if (g_count <= 0) {
    selectIndex(-1);
    return;
  }

  g_selected_index = findSelectedByName();

  // Auswahl verschwunden: zunächst höchsten Satelliten wählen.
  if (g_selected_index < 0) {
    selectIndex(findHighestSatellite());
    g_last_selection_ms = now_ms;
    return;
  }

  if (now_ms - g_last_selection_ms <
      kSelectionIntervalMs) {
    return;
  }

  g_last_selection_ms = now_ms;

  const int next_index =
      findNextAbove30(g_selected_index);

  if (next_index >= 0) {
    selectIndex(next_index);
    return;
  }

  // Kein Satellit über 30°: höchsten sichtbaren behalten.
  selectIndex(findHighestSatellite());
}
void selectNextManual(unsigned long now_ms) {
  if (g_count <= 0) {
    selectIndex(-1);
    return;
  }

  int current_index = findSelectedByName();

  if (current_index < 0) {
    current_index = findHighestSatellite();
  }

  for (int step = 1; step <= g_count; ++step) {
    const int next_index =
        (current_index + step) % g_count;

    if (g_satellites[next_index].visible) {
      selectIndex(next_index);

      // Automatik erst wieder nach 6 Sekunden.
      g_last_selection_ms = now_ms;

      return;
    }
  }
}
const Satellite* selected() {
  if (g_selected_index < 0 ||
      g_selected_index >= g_count) {
    return nullptr;
  }

  return &g_satellites[g_selected_index];
}

int selectedIndex() {
  return g_selected_index;
}

}  // namespace satellite