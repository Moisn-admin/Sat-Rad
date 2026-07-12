#include "ui/satellite_overlay.h"

#include <cmath>

#include "satellite/satellite_manager.h"
#include "ui/radar_theme.h"

namespace ui {

void drawSatelliteOverlay(lgfx::LovyanGFX& gfx) {
  if (satellite::count() <= 0) {
    return;
  }

  const Satellite* sat = satellite::get(0);

  if (sat == nullptr || !sat->visible) {
    return;
  }

  const float elevation =
      std::max(0.0f, std::min(90.0f, sat->elevation));

  const float radius =
      static_cast<float>(radar::kGridOuterRadius) *
      (1.0f - elevation / 90.0f);

  constexpr float kDegToRad = 0.01745329252f;
  const float angle = sat->azimuth * kDegToRad;

  const int x =
      radar::kCenterX +
      static_cast<int>(std::lround(std::sin(angle) * radius));

  const int y =
      radar::kCenterY -
      static_cast<int>(std::lround(std::cos(angle) * radius));

  const uint16_t color = gfx.color565(255, 255, 255);

  gfx.fillSmoothCircle(x, y, 4, color);
}

}  // namespace ui