#pragma once

#include <LovyanGFX.hpp>

namespace ui {

void drawSatelliteOverlay(lgfx::LovyanGFX& gfx);

void toggleSatelliteInfoBox();
bool satelliteInfoBoxVisible();

}  // namespace ui