#pragma once

#include <ctime>

#include "tle_reader.h"

namespace satellite {

bool calculatePositionAtTime(const TleRecord& record,
                             float latitude_deg,
                             float longitude_deg,
                             float altitude_m,
                             time_t unix_time,
                             float& azimuth_deg,
                             float& elevation_deg);

bool calculatePosition(const TleRecord& record,
                       float latitude_deg,
                       float longitude_deg,
                       float altitude_m,
                       float& azimuth_deg,
                       float& elevation_deg);

bool calculateMotion(const TleRecord& record,
                     float latitude_deg,
                     float longitude_deg,
                     float altitude_m,
                     time_t unix_time,
                     time_t preview_seconds,
                     float& azimuth_deg,
                     float& elevation_deg,
                     float& next_azimuth_deg,
                     float& next_elevation_deg);

}  // namespace satellite