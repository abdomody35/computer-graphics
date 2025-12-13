#pragma once

#include "Image.h"

struct HS
{
    float hue, saturation;
};

struct HSL : public HS
{
    float lightness;
};

struct HSV : public HS
{
    float value;
};

struct HSI : public HS
{
    float intensity;
};

namespace color
{
    namespace __detail
    {
        float computeHue(const float r, const float g, const float b, const float max, const float delta)
        {
            float hue;
            if (delta == 0)
            {
                hue = 0;
            }
            else if (max == r)
            {
                hue = 60 * std::fmod(((g - b) / delta), 6);
                if (hue < 0)
                    hue += 360;
            }
            else if (max == g)
            {
                hue = 60 * ((b - r) / delta + 2);
            }
            else
            {
                hue = 60 * ((r - g) / delta + 4);
            }
            return hue;
        }

        RGBA calculateRGB(float hue, float x, float m)
        {
            float y = x * (1 - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));

            float r, g, b;

            if (hue < 60)
            {
                r = x;
                g = y;
                b = 0;
            }
            else if (hue < 120)
            {
                r = y;
                g = x;
                b = 0;
            }
            else if (hue < 180)
            {
                r = 0;
                g = x;
                b = y;
            }
            else if (hue < 240)
            {
                r = 0;
                g = y;
                b = x;
            }
            else if (hue < 300)
            {
                r = y;
                g = 0;
                b = x;
            }
            else
            {
                r = x;
                g = 0;
                b = y;
            }

            return RGBA(car((r + m) * 255.0, 255), car((g + m) * 255.0, 255), car((b + m) * 255.0, 255));
        }
    }

    HSL toHSL(const RGBA &pixel)
    {
        float r = pixel.r / 255.0;
        float g = pixel.g / 255.0;
        float b = pixel.b / 255.0;
        float max = std::max(r, std::max(g, b));
        float min = std::min(r, std::min(g, b));
        float delta = max - min;

        HSL hsl;

        hsl.hue = __detail::computeHue(r, g, b, max, delta);

        hsl.lightness = (max + min) / 2;

        if (delta == 0)
        {
            hsl.saturation = 0;
        }
        else
        {
            hsl.saturation = delta / (1 - std::abs(2 * hsl.lightness - 1));
        }

        return hsl;
    }

    HSV toHSV(const RGBA &pixel)
    {
        float r = pixel.r / 255.0;
        float g = pixel.g / 255.0;
        float b = pixel.b / 255.0;
        float max = std::max(r, std::max(g, b));
        float min = std::min(r, std::min(g, b));
        float delta = max - min;

        HSV hsv;

        hsv.hue = __detail::computeHue(r, g, b, max, delta);

        hsv.value = max;

        if (max == 0)
        {
            hsv.saturation = 0;
        }
        else
        {
            hsv.saturation = delta / max;
        }

        return hsv;
    }

    HSI toHSI(const RGBA &pixel)
    {
        float r = pixel.r / 255.0;
        float g = pixel.g / 255.0;
        float b = pixel.b / 255.0;
        float max = std::max(r, std::max(g, b));
        float min = std::min(r, std::min(g, b));
        float delta = max - min;

        HSI hsi;

        hsi.hue = __detail::computeHue(r, g, b, max, delta);

        hsi.intensity = (r + g + b) / 3;

        if (hsi.intensity == 0)
        {
            hsi.saturation = 0;
        }
        else
        {
            hsi.saturation = 1 - (min / hsi.intensity);
        }

        return hsi;
    }

    RGBA toRGBA(const HSL &pixel)
    {
        float x = 1 - std::abs(2 * pixel.lightness - 1) * pixel.saturation;
        float m = pixel.lightness - x / 2;
        return __detail::calculateRGB(pixel.hue, x, m);
    }

    RGBA toRGBA(const HSV &pixel)
    {
        float x = pixel.saturation * pixel.value;
        float m = pixel.value - x;
        return __detail::calculateRGB(pixel.hue, x, m);
    }

    RGBA toRGBA(const HSI &pixel)
    {
        float z = 1 - std::abs(std::fmod(pixel.hue / 60.0f, 2.0f) - 1.0f);
        float x = 3 * pixel.intensity * pixel.saturation / (1 + z);
        float m = pixel.intensity * (1 - pixel.saturation);
        return __detail::calculateRGB(pixel.hue, x, m);
    }
}