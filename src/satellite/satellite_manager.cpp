#include "satellite_manager.h"

namespace satellite
{

static Satellite g_satellites[MAX_VISIBLE_SATELLITES];
static int g_count = 0;

void begin()
{
    g_count = 0;
}

void clear()
{
    g_count = 0;
}

bool add(const Satellite& sat)
{
    if (g_count >= MAX_VISIBLE_SATELLITES)
        return false;

    g_satellites[g_count++] = sat;
    return true;
}

int count()
{
    return g_count;
}

const Satellite* get(int index)
{
    if (index < 0 || index >= g_count)
        return nullptr;

    return &g_satellites[index];
}

}