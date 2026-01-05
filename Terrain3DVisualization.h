#pragma once

#include "Image.h"
#include <cmath>

namespace terrain
{
    // Generate a shaded relief map (hillshade)
    ColorImage generateHillshade(const GrayscaleImage &elevation, const ColorImage &biomes,
                                 float azimuth = 315.0f, float altitude = 45.0f, float zFactor = 2.0f)
    {
        int width = elevation.GetWidth();
        int height = elevation.GetHeight();
        ColorImage result(width, height);

        // Convert angles to radians
        float azimuthRad = azimuth * M_PI / 180.0f;
        float altitudeRad = altitude * M_PI / 180.0f;

        for (int y = 0; y < height - 1; y++)
        {
            result(0, y) = biomes(0, y);
            result(width - 1, y) = biomes(width - 1, y);
        }

        for (int x = 0; x < width - 1; x++)
        {
            result(x, 0) = biomes(x, 0);
            result(x, height - 1) = biomes(x, height - 1);
        }

        for (int y = 1; y < height - 1; y++)
        {
            for (int x = 1; x < width - 1; x++)
            {
                // Calculate slope using Sobel operator
                float dzdx = ((elevation.Get(x + 1, y - 1) + 2.0f * elevation.Get(x + 1, y) + elevation.Get(x + 1, y + 1)) -
                              (elevation.Get(x - 1, y - 1) + 2.0f * elevation.Get(x - 1, y) + elevation.Get(x - 1, y + 1))) /
                             8.0f;

                float dzdy = ((elevation.Get(x - 1, y + 1) + 2.0f * elevation.Get(x, y + 1) + elevation.Get(x + 1, y + 1)) -
                              (elevation.Get(x - 1, y - 1) + 2.0f * elevation.Get(x, y - 1) + elevation.Get(x + 1, y - 1))) /
                             8.0f;

                // Apply z-factor
                dzdx *= zFactor;
                dzdy *= zFactor;

                // Calculate slope and aspect
                float slope = std::atan(std::sqrt(dzdx * dzdx + dzdy * dzdy));
                float aspect = std::atan2(dzdy, -dzdx);

                // Calculate hillshade value
                float hillshade = std::cos(altitudeRad) * std::cos(slope) +
                                  std::sin(altitudeRad) * std::sin(slope) * std::cos(azimuthRad - aspect);

                hillshade = std::max(0.0f, hillshade);

                // Blend hillshade with biome color
                RGBA biomeColor = biomes(x, y);
                float intensity = 0.5f + 0.5f * hillshade; // Map [0,1] to [0.5, 1.0] for better visibility

                result(x, y) = RGBA(
                    (Byte)(biomeColor.r * intensity),
                    (Byte)(biomeColor.g * intensity),
                    (Byte)(biomeColor.b * intensity),
                    255);
            }
        }

        return result;
    }
}