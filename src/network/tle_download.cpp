#include "tle_download.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <WiFi.h>

namespace satellite
{

struct TleSource
{
    const char* url;
    const char* file;
};

static const TleSource sources[] =
{
    {
        "https://celestrak.org/NORAD/elements/gp.php?GROUP=visual&FORMAT=tle",
        "/visual.tle"
    },
    {
        "https://celestrak.org/NORAD/elements/gp.php?GROUP=stations&FORMAT=tle",
        "/stations.tle"
    },
    {
        "https://celestrak.org/NORAD/elements/gp.php?GROUP=weather&FORMAT=tle",
        "/weather.tle"
    },
    {
        "https://celestrak.org/NORAD/elements/gp.php?GROUP=amateur&FORMAT=tle",
        "/amateur.tle"
    }
};

static bool downloadOne(const TleSource& src)
{
    Serial.printf("Downloading %s\n", src.file);

    HTTPClient http;
    http.begin(src.url);

    int code = http.GET();

    if (code != HTTP_CODE_OK)
    {
        Serial.printf("HTTP Error %d\n", code);
        http.end();
        return false;
    }

    File file = LittleFS.open(src.file, "w");

    if (!file)
    {
        Serial.println("Cannot create file");
        http.end();
        return false;
    }

    WiFiClient* stream = http.getStreamPtr();

    uint8_t buffer[512];

    while (http.connected())
    {
        size_t available = stream->available();

        if (!available)
        {
            delay(1);
            continue;
        }

        int len = stream->readBytes(
            buffer,
            available > sizeof(buffer) ? sizeof(buffer) : available);

        file.write(buffer, len);
    }

    file.close();
    http.end();

    Serial.println("Done");

    return true;
}

bool downloadTleFiles()
{
    bool ok = true;

    for (const auto& s : sources)
    {
        ok &= downloadOne(s);
    }

    return ok;
}

}