#include "filesystem.h"

#include <Arduino.h>
#include <LittleFS.h>

namespace storage
{

bool begin()
{
    Serial.println("Mounting LittleFS...");

    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed");
        return false;
    }

    Serial.println("LittleFS mounted");

    return true;
}

}