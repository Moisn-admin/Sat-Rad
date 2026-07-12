#pragma once

#include "tle_reader.h"

namespace satellite {

bool calculatePosition(const TleRecord& record,
                       float latitude_deg,
                       float longitude_deg,
                       float altitude_m,
                       float& azimuth_deg,
                       float& elevation_deg);

}  // namespace satellite