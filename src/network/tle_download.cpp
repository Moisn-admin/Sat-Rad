#include "tle_download.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

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

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(20);

    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(20000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    if (!http.begin(client, src.url))
    {
        Serial.println("HTTP setup failed");
        return false;
    }

    const int code = http.GET();

    if (code != HTTP_CODE_OK)
    {
        Serial.printf("HTTP error: %d\n", code);
        http.end();
        return false;
    }

    char tempFile[40];
    snprintf(tempFile, sizeof(tempFile), "%s.tmp", src.file);

    LittleFS.remove(tempFile);

    File file = LittleFS.open(tempFile, "w");

    if (!file)
    {
        Serial.printf("Cannot create %s\n", tempFile);
        http.end();
        return false;
    }

    const int written = http.writeToStream(&file);

    file.close();
    http.end();

    if (written <= 0)
    {
        Serial.printf("Download failed: %d\n", written);
        LittleFS.remove(tempFile);
        return false;
    }

    if (written < 100)
    {
        Serial.printf("Downloaded file is too small: %d bytes\n", written);
        LittleFS.remove(tempFile);
        return false;
    }

    LittleFS.remove(src.file);

    if (!LittleFS.rename(tempFile, src.file))
    {
        Serial.println("Could not save downloaded file");
        LittleFS.remove(tempFile);
        return false;
    }

    Serial.printf("Saved %s (%d bytes)\n", src.file, written);
    return true;
}

bool downloadTleFiles()
{
    bool allOk = true;

    for (const auto& source : sources)
    {
        if (!downloadOne(source))
        {
            allOk = false;
        }

        delay(500);
    }

    return allOk;
}

}