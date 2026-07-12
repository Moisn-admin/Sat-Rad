#pragma once

enum class SatelliteVisibility
{
    Unknown,
    Daylight,
    Eclipsed,
    Lighted
};

struct Satellite
{
    char name[32];

    float azimuth = 0.0f;
    float elevation = 0.0f;

    float nextAzimuth = 0.0f;
    float nextElevation = 0.0f;

    float magnitude = 99.0f;

    bool visible = false;

    SatelliteVisibility visibility =
        SatelliteVisibility::Unknown;
};