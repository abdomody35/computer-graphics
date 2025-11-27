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

void drawCircleNaive(GrayscaleImage &image, const Point &center, int radius, Byte color = 255)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (x * x + y * y <= radius * radius)
            {
                setPixel(image, center.x + x, center.y + y, color);
            }
        }
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int radius = 100;

    drawCircleNaive(image, center, radius);

    image.Save("circleNaive.png");

    return 0;
}