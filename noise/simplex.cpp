#include "../Image.h"
#include "../Noise.h"

int main()
{
    GrayscaleImage image(256, 256);

    noise::SimplexNoise simplexNoise(12345);

    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            float noiseValue = simplexNoise.noise(x * 0.05f, y * 0.05f);
            Byte pixelValue = (Byte)std::round((noiseValue + 1.0f) * 127.5f);
            image(x, y) = pixelValue;
        }
    }

    image.Save("simplex.png");

    return 0;
}
