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

void drawCircleBresenham(GrayscaleImage &image, const Point &center, int radius, Byte color = 255)
{
    if (radius <= 0)
    {
        std::cerr << "Radius must be positive." << std::endl;
        return;
    }

    int x = radius;
    int y = 0;
    int d = 3 - 2 * radius;

    while (y <= x)
    {
        setPixel(image, center.x + x, center.y + y, color);
        setPixel(image, center.x - x, center.y + y, color);
        setPixel(image, center.x + x, center.y - y, color);
        setPixel(image, center.x - x, center.y - y, color);
        setPixel(image, center.x + y, center.y + x, color);
        setPixel(image, center.x - y, center.y + x, color);
        setPixel(image, center.x + y, center.y - x, color);
        setPixel(image, center.x - y, center.y - x, color);

        y++;

        if (d <= 0)
        {
            d = d + 4 * y + 6;
        }
        else
        {
            x--;
            d = d + 4 * (y - x) + 10;
        }
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int radius = 100;

    drawCircleBresenham(image, center, radius);

    image.Save("circleBresenham.png");

    return 0;
}