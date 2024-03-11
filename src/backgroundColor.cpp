#include "backgroundColor/backgroundColor.h"
#include <stdexcept>

float color_factor = 0.0005f;

bool bool_check(float x, bool dimming)
{
    //DIMS UNTIL COLOR HITS 0.01F
    if (dimming && x > 0.1f)
    {
        return true;
    }
    //BRIGHTENS UNTIL COLOR HITS 0.99F
    if (!dimming && x < 0.99f)
    {
        return false;
    }
    //STOPS DIMMING COLOR AT 0.01F
    if (dimming && x < 0.1f)
    {
        return false;
    }
    //STOPS BRIGHTENING COLOR AT 0.99F
    if (!dimming && x > 0.99)
    {
        return true;
    }
    else
    {
        throw std::invalid_argument("No dimming parameters met.");
    }
}

float red_change(float x, bool a)
{
    bool color_dim = bool_check(x, a);

    if (color_dim)
    {
        x -= color_factor * 0.15;
    }
    if (!color_dim)
    {
        x += color_factor * 0.15;
    }
    return x;
}

float green_change(float x, bool a)
{
    bool green_dim = bool_check(x, a);

    if (green_dim)
    {
        x -= color_factor * 0.07;
    }
    if (!green_dim)
    {
        x += color_factor * 0.07;
    }
    return x;
}

float blue_change(float x, bool a)
{
    bool blue_dim = bool_check(x, a);

    if (blue_dim)
    {
        x -= color_factor * 0.12;
    }
    if (!blue_dim)
    {
        x += color_factor * 0.12;
    }
    return x;
}