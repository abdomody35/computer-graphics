#pragma once

#include <cstddef>
#include <cmath>

namespace noise
{
    namespace __detail
    {
        const int grad2[8][2] = {
            {1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        // Permutation table - will be modified based on seed
        static unsigned char perm[512];

        // Base permutation table
        const unsigned char perm_base[256] = {
            151, 160, 137, 91, 90, 15,
            131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
            190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
            88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
            77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
            102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
            135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
            5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
            223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
            129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
            251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
            49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
            138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

        static inline int fastfloor(float x)
        {
            int i = (int)x;
            return (x < i) ? (i - 1) : i;
        }

        static inline float dot(const int g[2], float x, float y)
        {
            return g[0] * x + g[1] * y;
        }

        // Simple LCG for shuffling permutation table
        static inline unsigned int lcg(unsigned int &seed)
        {
            seed = seed * 1664525u + 1013904223u;
            return seed;
        }
    }

    class SimplexNoise
    {
    public:
        float noise(float xin, float yin) const
        {
            float n0, n1, n2;

            const float F2 = 0.5f * (std::sqrt(3.0f) - 1.0f);
            float s = (xin + yin) * F2;
            int i = __detail::fastfloor(xin + s);
            int j = __detail::fastfloor(yin + s);

            const float G2 = (3.0f - std::sqrt(3.0f)) / 6.0f;
            float t = (i + j) * G2;
            float X0 = i - t;
            float Y0 = j - t;
            float x0 = xin - X0;
            float y0 = yin - Y0;

            int i1, j1;
            if (x0 > y0)
            {
                i1 = 1;
                j1 = 0;
            }
            else
            {
                i1 = 0;
                j1 = 1;
            }

            float x1 = x0 - i1 + G2;
            float y1 = y0 - j1 + G2;
            float x2 = x0 - 1.0f + 2.0f * G2;
            float y2 = y0 - 1.0f + 2.0f * G2;

            int ii = i & 255;
            int jj = j & 255;
            int gi0 = mPerm[ii + mPerm[jj]] % 8;
            int gi1 = mPerm[ii + i1 + mPerm[jj + j1]] % 8;
            int gi2 = mPerm[ii + 1 + mPerm[jj + 1]] % 8;

            float t0 = 0.5f - x0 * x0 - y0 * y0;
            if (t0 < 0.0f)
            {
                n0 = 0.0f;
            }
            else
            {
                t0 *= t0;
                n0 = t0 * t0 * __detail::dot(__detail::grad2[gi0], x0, y0);
            }

            float t1 = 0.5f - x1 * x1 - y1 * y1;
            if (t1 < 0.0f)
            {
                n1 = 0.0f;
            }
            else
            {
                t1 *= t1;
                n1 = t1 * t1 * __detail::dot(__detail::grad2[gi1], x1, y1);
            }

            float t2 = 0.5f - x2 * x2 - y2 * y2;
            if (t2 < 0.0f)
            {
                n2 = 0.0f;
            }
            else
            {
                t2 *= t2;
                n2 = t2 * t2 * __detail::dot(__detail::grad2[gi2], x2, y2);
            }

            return 70.0f * (n0 + n1 + n2);
        }

        explicit SimplexNoise(unsigned int seed = 0, float frequency = 1.0f, float amplitude = 1.0f, float lacunarity = 2.0f, float persistence = 0.5f)
            : mFrequency(frequency), mAmplitude(amplitude), mLacunarity(lacunarity), mPersistence(persistence)
        {
            // Initialize permutation table with seed
            unsigned char temp[256];
            for (int i = 0; i < 256; i++)
            {
                temp[i] = __detail::perm_base[i];
            }

            // Shuffle using Fisher-Yates with LCG
            unsigned int rng_state = seed;
            for (int i = 255; i > 0; i--)
            {
                int j = __detail::lcg(rng_state) % (i + 1);
                unsigned char swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }

            // Duplicate for easy wrapping
            for (int i = 0; i < 256; i++)
            {
                mPerm[i] = temp[i];
                mPerm[i + 256] = temp[i];
            }
        }

        float fractal(size_t octaves, float x, float y) const
        {
            float output = 0.0f;
            float denom = 0.0f;
            float frequency = mFrequency;
            float amplitude = mAmplitude;

            for (size_t i = 0; i < octaves; i++)
            {
                output += (amplitude * noise(x * frequency, y * frequency));
                denom += amplitude;

                frequency *= mLacunarity;
                amplitude *= mPersistence;
            }

            return (output / denom);
        }

    private:
        float mFrequency;
        float mAmplitude;
        float mLacunarity;
        float mPersistence;
        unsigned char mPerm[512];
    };
}
