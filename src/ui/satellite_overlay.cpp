#include "ui/satellite_overlay.h"

#include <algorithm>
#include <cmath>
#include "satellite/satellite_label.h"

#include "satellite/satellite_manager.h"
#include "ui/radar_theme.h"

namespace ui {

namespace {

constexpr float kDegToRad = 0.01745329252f;
constexpr float kDirectionLineLengthPx = 9.0f;
constexpr float kDirectionLineHalfWidth = 1.0f;
constexpr int kSatelliteDotRadiusPx = 3;

void positionToScreen(float azimuth,
                      float elevation,
                      int& x,
                      int& y) {
  const float safe_elevation =
      std::max(0.0f, std::min(90.0f, elevation));

  const float radius =
      static_cast<float>(radar::kGridOuterRadius) *
      (1.0f - safe_elevation / 90.0f);

  const float angle = azimuth * kDegToRad;

  x = radar::kCenterX +
      static_cast<int>(std::lround(std::sin(angle) * radius));

  y = radar::kCenterY -
      static_cast<int>(std::lround(std::cos(angle) * radius));
}

uint16_t colorForElevation(lgfx::LovyanGFX& gfx,
                           float elevation) {
  if (elevation >= 60.0f) {
    return gfx.color565(0, 255, 0);
  }

  if (elevation >= 30.0f) {
    return gfx.color565(255, 220, 0);
  }

  if (elevation >= 10.0f) {
    return gfx.color565(255, 120, 0);
  }

  return gfx.color565(255, 0, 0);
}

}  // namespace

void drawSatelliteLabel(lgfx::LovyanGFX& gfx,
                        const Satellite& sat,
                        int x,
                        int y)
{
    char label[12]{};

    satellite::makeShortLabel(
        sat.name,
        label,
        sizeof(label));

    if (label[0] == '\0')
        return;

    gfx.setTextSize(1);
    gfx.setTextColor(TFT_WHITE);
    gfx.setFont(&fonts::Font0);

    gfx.drawString(label, x + 6, y - 4);
}

void drawSatelliteOverlay(lgfx::LovyanGFX& gfx) {
  const int total = satellite::count();

  if (total <= 0) {
    return;
  }

  const uint16_t direction_color =
      gfx.color565(0, 220, 255);

  for (int i = 0; i < total; ++i) {
    const Satellite* sat = satellite::get(i);

    if (sat == nullptr || !sat->visible) {
      continue;
    }

    int x = 0;
    int y = 0;
    int future_x = 0;
    int future_y = 0;

    positionToScreen(
        sat->azimuth,
        sat->elevation,
        x,
        y);

    positionToScreen(
        sat->nextAzimuth,
        sat->nextElevation,
        future_x,
        future_y);

    const float dx =
        static_cast<float>(future_x - x);

    const float dy =
        static_cast<float>(future_y - y);

    const float length =
        std::sqrt(dx * dx + dy * dy);

    if (length > 0.01f) {
      const float scale =
          kDirectionLineLengthPx / length;

      const int line_end_x =
          x + static_cast<int>(
                  std::lround(dx * scale));

      const int line_end_y =
          y + static_cast<int>(
                  std::lround(dy * scale));

      gfx.drawWideLine(
          x,
          y,
          line_end_x,
          line_end_y,
          kDirectionLineHalfWidth,
          direction_color);
    }

    const uint16_t dot_color =
        colorForElevation(gfx, sat->elevation);

    gfx.fillSmoothCircle(
        x,
        y,
        kSatelliteDotRadiusPx,
        dot_color);

        drawSatelliteLabel(
    gfx,
    *sat,
    x,
    y);
  }
}

}  // namespace ui