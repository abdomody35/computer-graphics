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

void drawCircleMidPoint(GrayscaleImage &image, const Point &center, int radius, Byte color = 255)
{
    if (radius <= 0)
    {
        std::cerr << "Radius must be positive." << std::endl;
        return;
    }

    int x = radius;
    int y = 0;

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

        int d1 = std::abs(radius * radius - x * x - y * y);
        int d2 = std::abs(radius * radius - (x - 1) * (x - 1) - y * y);

        if (d1 > d2)
        {
            x--;
        }
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int radius = 100;

    drawCircleMidPoint(image, center, radius);

    image.Save("circleMidPoint.png");

    return 0;
}