#pragma once

#include <time.h>

namespace services {
namespace time_sync {

bool begin();
bool ready();
time_t now();

}  // namespace time_sync
}  // namespace services