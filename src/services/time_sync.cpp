#include "time_sync.h"

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

namespace services {
namespace time_sync {

namespace {

constexpr unsigned long kTimeoutMs = 15000;
bool s_ready = false;

}  // namespace

bool begin() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("NTP: WiFi not connected");
    return false;
  }

  Serial.println("NTP: synchronizing UTC time...");

  configTime(0, 0, "pool.ntp.org", "time.cloudflare.com", "time.google.com");

  const unsigned long started = millis();

  while (millis() - started < kTimeoutMs) {
    const time_t current = time(nullptr);

    if (current > 1700000000) {
      s_ready = true;

      struct tm utc_time {};
      gmtime_r(&current, &utc_time);

      char buffer[32];
      strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S UTC", &utc_time);

      Serial.printf("NTP: %s\n", buffer);
      return true;
    }

    delay(250);
  }

  Serial.println("NTP: synchronization failed");
  return false;
}

bool ready() {
  return s_ready;
}

time_t now() {
  return time(nullptr);
}

}  // namespace time_sync
}  // namespace services