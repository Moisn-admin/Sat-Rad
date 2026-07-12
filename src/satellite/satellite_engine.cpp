#include "satellite_engine.h"

#include <LittleFS.h>
#include <cstring>

#include "satellite_manager.h"
#include "satellite_position.h"
#include "tle_reader.h"

#include "services/radar_location.h"

namespace satellite
{

bool update()
{
    clear();

    TleReader reader;
    TleRecord record;

    if (!reader.open(LittleFS, "/visual.tle"))
    {
        return false;
    }

    while (reader.next(record))
    {
        float azimuth = 0.0f;
        float elevation = 0.0f;

        if (!calculatePosition(
                record,
                services::location::lat(),
                services::location::lon(),
                0.0f,
                azimuth,
                elevation))
        {
            continue;
        }

        if (elevation <= 0.0f)
        {
            continue;
        }

        Satellite sat{};

        strncpy(sat.name, record.name, sizeof(sat.name) - 1);

        sat.azimuth = azimuth;
        sat.elevation = elevation;
        sat.visible = true;

        add(sat);
    }

        reader.close();

Serial.printf("Visible satellites stored: %d\n", count());

return true;
}

} // namespace satellite