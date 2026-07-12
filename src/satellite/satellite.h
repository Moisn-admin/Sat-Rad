#pragma once

struct Satellite
{
    char name[32];

    float azimuth = 0.0f;
    float elevation = 0.0f;

    float magnitude = 99.0f;

    bool visible = false;
};