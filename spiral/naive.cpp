#include "../Image.h"

struct Point
{
    int x;
    int y;
};

void setPixel(GrayscaleImage &image, int x, int y, Byte color)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        image(x, y) = color;
    }
}

void drawSpiralNaive(GrayscaleImage &image, const Point &center, int intercept, float factor, int loops, Byte color = 255)
{
    if (intercept < 0)
    {
        std::cerr << "Intercept must not be negative." << std::endl;
        return;
    }
    if (factor <= 0.0f)
    {
        std::cerr << "Factor must be posotive." << std::endl;
        return;
    }
    if (loops <= 0)
    {
        std::cerr << "Loops must be positive." << std::endl;
        return;
    }

    float theta = 0;
    float max_theta = 2 * M_PI * loops;

    while (theta <= max_theta)
    {
        float radius = intercept + (factor * theta) / (2.0f * M_PI);

        int x = std::round(center.x + radius * cos(theta));
        int y = std::round(center.y + radius * sin(theta));

        theta += 1 / std::max(radius, 1.0f);

        setPixel(image, x, y, color);
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int intercept = 5;

    float factor = 8.0f;

    int loops = 10;

    drawSpiralNaive(image, center, intercept, factor, loops);

    image.Save("spiralNaive.png");

    return 0;
}
