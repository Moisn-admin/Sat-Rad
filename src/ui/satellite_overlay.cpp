#include "ui/satellite_overlay.h"

#include <algorithm>
#include <cmath>
#include <lgfx/v1/lgfx_fonts.hpp>
#include <cstdio>
#include <cstring>

#include "satellite/satellite_label.h"
#include "satellite/satellite_manager.h"
#include "ui/radar_theme.h"

namespace ui {

namespace {

constexpr float kDegToRad = 0.01745329252f;
constexpr float kDirectionLineLengthPx = 9.0f;
constexpr float kDirectionLineHalfWidth = 1.0f;
constexpr int kLabelGapPx = 7;
bool s_info_box_visible = true;

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

void drawRotatedSatellite(lgfx::LovyanGFX& gfx,
                          int x,
                          int y,
                          float direction_x,
                          float direction_y,
                          uint16_t color) {
  float length =
      std::sqrt(direction_x * direction_x +
                direction_y * direction_y);

  if (length < 0.01f) {
    direction_x = 0.0f;
    direction_y = -1.0f;
    length = 1.0f;
  }

  const float ux = direction_x / length;
  const float uy = direction_y / length;

  // Senkrechte Richtung für die Solarpanels.
  const float px = -uy;
  const float py = ux;

  const int panel_left_x =
      x - static_cast<int>(std::lround(px * 6.0f));
  const int panel_left_y =
      y - static_cast<int>(std::lround(py * 6.0f));

  const int panel_right_x =
      x + static_cast<int>(std::lround(px * 6.0f));
  const int panel_right_y =
      y + static_cast<int>(std::lround(py * 6.0f));

  // Verbindung und Solarpanels.
  gfx.drawWideLine(
      panel_left_x,
      panel_left_y,
      panel_right_x,
      panel_right_y,
      1.0f,
      color);

  gfx.fillRect(
      panel_left_x - 1,
      panel_left_y - 1,
      3,
      3,
      color);

  gfx.fillRect(
      panel_right_x - 1,
      panel_right_y - 1,
      3,
      3,
      color);

  // Satellitenkörper.
  gfx.fillRect(x - 2, y - 2, 5, 5, color);

  // Kleine Antenne in Flugrichtung.
  const int antenna_start_x =
      x + static_cast<int>(std::lround(ux * 3.0f));
  const int antenna_start_y =
      y + static_cast<int>(std::lround(uy * 3.0f));

  const int antenna_end_x =
      x + static_cast<int>(std::lround(ux * 6.0f));
  const int antenna_end_y =
      y + static_cast<int>(std::lround(uy * 6.0f));

  gfx.drawLine(
      antenna_start_x,
      antenna_start_y,
      antenna_end_x,
      antenna_end_y,
      color);
}

void drawSatelliteLabel(lgfx::LovyanGFX& gfx,
                        const Satellite& sat,
                        int x,
                        int y) {
  char label[12]{};

  satellite::makeShortLabel(
      sat.name,
      label,
      sizeof(label));

  if (label[0] == '\0') {
    return;
  }

  gfx.setFont(&fonts::Font0);
  gfx.setTextSize(1);
  gfx.setTextDatum(textdatum_t::top_left);

  // Nur Vordergrundfarbe: Hintergrund bleibt transparent.
  gfx.setTextColor(TFT_WHITE);

  const int text_width = gfx.textWidth(label);
  const int text_height = gfx.fontHeight();

  constexpr int kGap = 4;

  int label_x;
  int label_y = y - text_height / 2;

  // Links im Radar: Text rechts vom Satelliten.
  // Rechts im Radar: Text links vom Satelliten.
  if (x < radar::kCenterX) {
    label_x = x + kGap;
  } else {
    label_x = x - kGap - text_width;
  }

  label_x = std::max(
      0,
      std::min(label_x, radar::kSize - text_width));

  label_y = std::max(
      0,
      std::min(label_y, radar::kSize - text_height));

  gfx.drawString(label, label_x, label_y);
}

}  // namespace
void drawSelectedSatelliteInfo(lgfx::LovyanGFX& gfx) {
  const Satellite* sat = satellite::selected();

  if (sat == nullptr) {
    return;
  }

  char name[32]{};
  std::strncpy(name, sat->name, sizeof(name) - 1);

  // Leerzeichen am Ende des TLE-Namens entfernen.
  for (int i = static_cast<int>(std::strlen(name)) - 1;
       i >= 0 && name[i] == ' ';
       --i) {
    name[i] = '\0';
  }

  const char* visibility_text = "Unbekannt";

  switch (sat->visibility) {
    case SatelliteVisibility::Lighted:
      visibility_text = "Beleuchtet";
      break;

    case SatelliteVisibility::Eclipsed:
      visibility_text = "Erdschatten";
      break;

    case SatelliteVisibility::Daylight:
      visibility_text = "Tageshimmel";
      break;

    default:
      break;
  }

  char position_text[32]{};

  std::snprintf(
      position_text,
      sizeof(position_text),
      "Az %.0f  El %.0f",
      sat->azimuth,
      sat->elevation);

  constexpr int box_x = 52;
constexpr int box_y = 199;
constexpr int box_w = 136;
constexpr int box_h = 24;

 // Schwarzer Balken
gfx.fillRect(
    0,
    195,
    radar::kSize,
    52,
    TFT_BLACK);

// Dünne rote Linie oben
gfx.drawFastHLine(
    0,
    195,
    radar::kSize,
    gfx.color565(0, 0, 255));

  gfx.setFont(&fonts::Font0);
  gfx.setTextSize(1);
  gfx.setTextColor(TFT_WHITE);
  gfx.setTextDatum(textdatum_t::top_center);

char detail_text[40]{};

std::snprintf(
    detail_text,
    sizeof(detail_text),
    "Az %.0f  El %.0f  %s",
    sat->azimuth,
    sat->elevation,
    visibility_text);

gfx.drawString(name, radar::kCenterX, box_y + 3);
gfx.drawString(position_text, radar::kCenterX, box_y + 14);
gfx.drawString(visibility_text, radar::kCenterX, box_y + 25);
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

    const uint16_t satellite_color =
        colorForElevation(gfx, sat->elevation);

    drawRotatedSatellite(
        gfx,
        x,
        y,
        dx,
        dy,
        satellite_color);

    if (i == satellite::selectedIndex()) {
    const uint16_t selection_color =
        gfx.color565(255, 255, 255);

    gfx.drawCircle(x, y, 9, selection_color);
    gfx.drawCircle(x, y, 10, selection_color);
}
    

    drawSatelliteLabel(
        gfx,
        *sat,
        x,
        y);
  }
  if (s_info_box_visible) {
    drawSelectedSatelliteInfo(gfx);
}
}
void toggleSatelliteInfoBox() {
  s_info_box_visible = !s_info_box_visible;
}

bool satelliteInfoBoxVisible() {
  return s_info_box_visible;
}
}  // namespace ui