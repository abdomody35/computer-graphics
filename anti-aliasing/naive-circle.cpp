#include "../Image.h"

struct Point
{
    int x;
    int y;
};

template <typename Image, typename Color>
void setPixel(Image &image, int x, int y, Color color)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        image(x, y) = color;
    }
}

template <typename Image, typename Color>
inline void setCirclePixels(Image &image, const Point &center, int x, int y, bool fill, Color color)
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
}

template <typename Image, typename Color>
inline void drawCircleMidPoint(Image &image, const Point &center, int radius, Color color, bool fill = false)
{
    if (radius <= 0)
    {
        std::cerr << "Radius must be positive." << std::endl;
        return;
    }

    int x = radius;
    int y = 0;

    int d1 = 0, d2 = 0;

    while (y <= x)
    {
        float dist = std::sqrt(x * x + y * y);

        float w1 = 1.0f - std::abs(dist - radius);
        float w2 = 1.0f - w1;

        w1 = std::clamp(w1, 0.0f, 1.0f);
        w2 = std::clamp(w2, 0.0f, 1.0f);

        if (d1 == 0 && d2 == 0)
        {
            setCirclePixels(image, center, x, y, fill, color);
        }
        else if (d1 > d2)
        {
            setCirclePixels(image, center, x + 1, y, false, color * w1);
            setCirclePixels(image, center, x, y, fill, color);
        }
        else
        {
            setCirclePixels(image, center, x - 1, y, false, color * w1);
            setCirclePixels(image, center, x, y, fill, color);
        }

        y++;

        d1 = std::abs(radius * radius - x * x - y * y);
        d2 = std::abs(radius * radius - (x - 1) * (x - 1) - y * y);

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

    drawCircleMidPoint(image, center, radius, 128, true);
    drawCircleMidPoint(image, center, radius, 255);

    image.Save("naive-circle.png");

    return 0;
}
