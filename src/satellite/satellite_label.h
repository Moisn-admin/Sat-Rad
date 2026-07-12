#pragma once

#include <cstddef>

namespace satellite {

void makeShortLabel(const char* full_name,
                    char* output,
                    std::size_t output_size);

}  // namespace satellite