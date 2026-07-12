#pragma once

struct Satellite
{
    char name[32];

    // Aktuelle Position
    float azimuth = 0.0f;
    float elevation = 0.0f;

    // Position in +2 Sekunden
    float nextAzimuth = 0.0f;
    float nextElevation = 0.0f;

    float magnitude = 99.0f;

    bool visible = false;
};