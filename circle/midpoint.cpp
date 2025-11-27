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

void drawCircleMidPoint(GrayscaleImage &image, const Point &center, int radius, bool fill = false, Byte color = 255)
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
        Point o1 = {center.x + x, center.y + y};
        Point o2 = {center.x - x, center.y + y};
        Point o3 = {center.x + x, center.y - y};
        Point o4 = {center.x - x, center.y - y};
        Point o5 = {center.x + y, center.y + x};
        Point o6 = {center.x - y, center.y + x};
        Point o7 = {center.x + y, center.y - x};
        Point o8 = {center.x - y, center.y - x};

        setPixel(image, o1.x, o1.y, color);
        setPixel(image, o2.x, o2.y, color);
        setPixel(image, o3.x, o3.y, color);
        setPixel(image, o4.x, o4.y, color);
        setPixel(image, o5.x, o5.y, color);
        setPixel(image, o6.x, o6.y, color);
        setPixel(image, o7.x, o7.y, color);
        setPixel(image, o8.x, o8.y, color);

        if (fill)
        {
            for (int i = o2.x; i <= o1.x; i++)
            {
                setPixel(image, i, o1.y, color);
            }
            for (int i = o4.x; i <= o3.x; i++)
            {
                setPixel(image, i, o3.y, color);
            }
            for (int i = o6.x; i <= o5.x; i++)
            {
                setPixel(image, i, o5.y, color);
            }
            for (int i = o8.x; i <= o7.x; i++)
            {
                setPixel(image, i, o7.y, color);
            }
        }

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

    drawCircleMidPoint(image, center, radius, true, 128);
    drawCircleMidPoint(image, center, radius);

    image.Save("circleMidPoint.png");

    return 0;
}