#pragma once

#include "Image.h"
#include "Noise.h"

namespace terrain
{
    namespace __detail
    {
        enum class Biome
        {
            OCEAN = 0,
            BEACH,
            FOREST,
            JUNGLE,
            SAVANNAH,
            DESERT,
            SNOW,
            MOUNTAIN
        };

        inline Biome getBiome(float elevation, float moisture, float waterLevel)
        {
            if (elevation < waterLevel)
            {
                return Biome::OCEAN;
            }

            if (elevation < waterLevel + 0.1f)
            {
                return Biome::BEACH;
            }

            if (elevation > 0.8f)
            {
                return Biome::SNOW;
            }

            if (elevation > 0.6f)
            {
                return Biome::MOUNTAIN;
            }

            if (moisture < 0.2f)
            {
                return Biome::DESERT;
            }

            if (moisture < 0.5f)
            {
                return Biome::SAVANNAH;
            }

            if (elevation < 0.4f)
            {
                return Biome::JUNGLE;
            }

            return Biome::FOREST;
        }

        inline RGBA getBiomeColor(Biome biome)
        {
            switch (biome)
            {
            case Biome::OCEAN:
                return RGBA(54, 54, 97);
            case Biome::BEACH:
                return RGBA(255, 255, 153);
            case Biome::FOREST:
                return RGBA(34, 139, 34);
            case Biome::JUNGLE:
                return RGBA(0, 100, 0);
            case Biome::SAVANNAH:
                return RGBA(210, 180, 140);
            case Biome::DESERT:
                return RGBA(238, 203, 173);
            case Biome::SNOW:
                return RGBA(255, 255, 255);
            case Biome::MOUNTAIN:
                return RGBA(139, 137, 137);
            default:
                return RGBA(0, 0, 0);
            }
        }
    }

    struct TerrainConfig
    {
        float elevationOctaves[6] = {1.0f, 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f};
        float moistureOctaves[6] = {1.0f, 0.75f, 0.33f, 0.33f, 0.33f, 0.5f};
        float exponent = 1.5f;    // Reduced from 5.0 to avoid crushing values
        float waterLevel = 0.25f; // Adjusted to create reasonable ocean coverage
        float scale = 3.0f;       // Increased for better terrain features
    };

    class TerrainGenerator
    {
    public:
        TerrainGenerator(unsigned int seed = 12345) : elevationNoise(seed), moistureNoise(seed + 1000) {}

        void generateTerrain(GrayscaleImage &elevation, ColorImage &biomes, int width, int height)
        {
            TerrainConfig defaultConfig;
            generateTerrain(elevation, biomes, width, height, defaultConfig);
        }

        void generateTerrain(GrayscaleImage &elevation, ColorImage &biomes, int width, int height, const TerrainConfig &config)
        {
            if (elevation.GetWidth() != width || elevation.GetHeight() != height)
            {
                elevation = GrayscaleImage(width, height);
            }

            if (biomes.GetWidth() != width || biomes.GetHeight() != height)
            {
                biomes = ColorImage(width, height);
            }

            float elevationSum = 0.0f;
            for (int i = 0; i < 6; i++)
            {
                elevationSum += config.elevationOctaves[i];
            }

            float moistureSum = 0.0f;
            for (int i = 0; i < 6; i++)
            {
                moistureSum += config.moistureOctaves[i];
            }

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // Scale coordinates to appropriate range
                    // Typical scale is 1.0-5.0 depending on desired feature size
                    float nx = (x / (float)width) * config.scale;
                    float ny = (y / (float)height) * config.scale;

                    // Generate elevation using multiple octaves
                    float e = (config.elevationOctaves[0] * elevationNoise.fractal(1, nx, ny) +
                               config.elevationOctaves[1] * elevationNoise.fractal(2, nx, ny) +
                               config.elevationOctaves[2] * elevationNoise.fractal(4, nx, ny) +
                               config.elevationOctaves[3] * elevationNoise.fractal(8, nx, ny) +
                               config.elevationOctaves[4] * elevationNoise.fractal(16, nx, ny) +
                               config.elevationOctaves[5] * elevationNoise.fractal(32, nx, ny));

                    e = e / elevationSum;

                    // Map from approximately [-1, 1] to [0, 1]
                    e = (e + 1.0f) * 0.5f;
                    e = std::clamp(e, 0.0f, 1.0f);

                    // Apply redistribution curve
                    // Option 1: Simple power curve (gentler)
                    // e = std::pow(e, config.exponent);

                    // Option 2: Custom redistribution for island-like shapes
                    // Uncomment this and comment out the line above for island generation
                    float distance = std::sqrt(std::pow(nx - config.scale/2, 2) + std::pow(ny - config.scale/2, 2));
                    float maxDistance = config.scale * 0.7f;
                    e = e + (1.0f - std::min(1.0f, distance / maxDistance)) - 0.5f;
                    e = std::max(0.0f, std::min(1.0f, e));

                    // Generate moisture using multiple octaves
                    float m = (config.moistureOctaves[0] * moistureNoise.fractal(1, nx, ny) +
                               config.moistureOctaves[1] * moistureNoise.fractal(2, nx, ny) +
                               config.moistureOctaves[2] * moistureNoise.fractal(4, nx, ny) +
                               config.moistureOctaves[3] * moistureNoise.fractal(8, nx, ny) +
                               config.moistureOctaves[4] * moistureNoise.fractal(16, nx, ny) +
                               config.moistureOctaves[5] * moistureNoise.fractal(32, nx, ny));

                    m = m / moistureSum;

                    // Map from approximately [-1, 1] to [0, 1]
                    m = (m + 1.0f) * 0.5f;
                    m = std::clamp(m, 0.0f, 1.0f);

                    elevation(x, y) = (Byte)std::round(e * 255.0f);

                    __detail::Biome biome = __detail::getBiome(e, m, config.waterLevel);
                    biomes(x, y) = __detail::getBiomeColor(biome);
                }
            }
        }

    private:
        noise::SimplexNoise elevationNoise;
        noise::SimplexNoise moistureNoise;
    };
}