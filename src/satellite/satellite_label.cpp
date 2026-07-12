#include "satellite_label.h"

#include <cctype>
#include <cstdio>
#include <cstring>

namespace satellite {

namespace {

bool startsWith(const char* text, const char* prefix) {
  return std::strncmp(text, prefix, std::strlen(prefix)) == 0;
}

void copyWithoutSpaces(const char* source,
                       char* output,
                       std::size_t output_size) {
  if (output_size == 0) {
    return;
  }

  std::size_t out = 0;

  for (std::size_t i = 0;
       source[i] != '\0' && out + 1 < output_size;
       ++i) {
    const unsigned char c =
        static_cast<unsigned char>(source[i]);

    if (!std::isspace(c) && c != '(' && c != ')') {
      output[out++] = static_cast<char>(c);
    }
  }

  output[out] = '\0';
}

}  // namespace

void makeShortLabel(const char* full_name,
                    char* output,
                    std::size_t output_size) {
  if (output == nullptr || output_size == 0) {
    return;
  }

  output[0] = '\0';

  if (full_name == nullptr || full_name[0] == '\0') {
    return;
  }

  if (startsWith(full_name, "ISS")) {
    std::snprintf(output, output_size, "ISS");
    return;
  }

  if (startsWith(full_name, "HST")) {
    std::snprintf(output, output_size, "HST");
    return;
  }

  if (startsWith(full_name, "CSS")) {
    std::snprintf(output, output_size, "CSS");
    return;
  }

  if (startsWith(full_name, "NOAA")) {
    copyWithoutSpaces(full_name, output, output_size);
    return;
  }

  if (startsWith(full_name, "COSMOS ")) {
    std::snprintf(output,
                  output_size,
                  "COS%s",
                  full_name + 7);
    return;
  }

  if (startsWith(full_name, "STARLINK-")) {
    std::snprintf(output,
                  output_size,
                  "SL%s",
                  full_name + 9);
    return;
  }

  if (startsWith(full_name, "SPACEMOBILE-")) {
    std::snprintf(output,
                  output_size,
                  "SM%s",
                  full_name + 12);
    return;
  }

  copyWithoutSpaces(full_name, output, output_size);

  if (std::strlen(output) > 8) {
    output[8] = '\0';
  }
}

}  // namespace satellite