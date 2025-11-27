#include "../Image.h"

struct Point {
    int x;
    int y;
};

void DrawCircleNaive(GrayscaleImage &image, const Point &center, int radius, Byte color = 255)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    for (int y = -radius; y <= radius; y++)
    {
        for (int x = -radius; x <= radius; x++)
        {
            if (center.x + x < 0 || center.x + x >= width ||
                center.y + y < 0 || center.y + y >= height)
            {
                continue;
            }

            if (x * x + y * y <= radius * radius)
            {
                image(center.x + x, center.y + y) = color;
            }
        }
    }
}

int main() {
    GrayscaleImage image(256, 256);

    Point center = { 128, 128 };
    
    int radius = 100;

    DrawCircleNaive(image, center, radius);

    image.Save("circleNaive.png");

    return 0;
}