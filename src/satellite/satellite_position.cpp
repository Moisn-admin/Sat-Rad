#include "satellite_position.h"

#include <Arduino.h>
#include <Sgp4.h>
#include <cstring>
#include <ctime>

namespace satellite {

bool calculatePositionAtTime(const TleRecord& record,
                             float latitude_deg,
                             float longitude_deg,
                             float altitude_m,
                             time_t unix_time,
                             float& azimuth_deg,
                             float& elevation_deg) {
  if (unix_time < 1700000000) {
    Serial.println("SGP4: UTC time is not ready");
    return false;
  }

  char name[32] = {};
  char line1[130] = {};
  char line2[130] = {};

  strncpy(name, record.name, sizeof(name) - 1);
  strncpy(line1, record.line1, sizeof(line1) - 1);
  strncpy(line2, record.line2, sizeof(line2) - 1);

  Sgp4 sgp4;

  sgp4.site(latitude_deg, longitude_deg, altitude_m);

  if (!sgp4.init(name, line1, line2)) {
    Serial.printf("SGP4: could not initialize %s\n", name);
    return false;
  }

  sgp4.findsat(static_cast<unsigned long>(unix_time));

  azimuth_deg = static_cast<float>(sgp4.satAz);
  elevation_deg = static_cast<float>(sgp4.satEl);

  return true;
}

bool calculatePosition(const TleRecord& record,
                       float latitude_deg,
                       float longitude_deg,
                       float altitude_m,
                       float& azimuth_deg,
                       float& elevation_deg) {
  return calculatePositionAtTime(
      record,
      latitude_deg,
      longitude_deg,
      altitude_m,
      time(nullptr),
      azimuth_deg,
      elevation_deg);
}

}  // namespace satellite