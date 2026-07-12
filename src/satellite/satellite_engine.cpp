#include "satellite_engine.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <cstring>
#include <ctime>

#include "satellite_manager.h"
#include "satellite_position.h"
#include "tle_reader.h"

#include "services/radar_location.h"

namespace satellite {

namespace {

constexpr time_t kDirectionPreviewSeconds = 2;

}  // namespace

bool update() {
  clear();

  TleReader reader;
  TleRecord record;

  if (!reader.open(LittleFS, "/visual.tle")) {
    Serial.println("Satellite engine: could not open /visual.tle");
    return false;
  }

  const time_t current_time = time(nullptr);

  while (reader.next(record)) {
    float azimuth = 0.0f;
    float elevation = 0.0f;

    if (!calculatePositionAtTime(
            record,
            services::location::lat(),
            services::location::lon(),
            0.0f,
            current_time,
            azimuth,
            elevation)) {
      continue;
    }

    if (elevation <= 0.0f) {
      continue;
    }

    float next_azimuth = azimuth;
    float next_elevation = elevation;

    calculatePositionAtTime(
        record,
        services::location::lat(),
        services::location::lon(),
        0.0f,
        current_time + kDirectionPreviewSeconds,
        next_azimuth,
        next_elevation);

    Satellite sat{};

    strncpy(sat.name, record.name, sizeof(sat.name) - 1);
    sat.name[sizeof(sat.name) - 1] = '\0';

    sat.azimuth = azimuth;
    sat.elevation = elevation;
    sat.nextAzimuth = next_azimuth;
    sat.nextElevation = next_elevation;
    sat.visible = true;

    add(sat);
  }

  reader.close();

  Serial.printf("Visible satellites stored: %d\n", count());

  return true;
}

}  // namespace satellite