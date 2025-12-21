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

inline float fpart(float x) { return x - std::floor(x); }

inline float rfpart(float x) { return 1.0f - fpart(x); }

void drawLineWu(GrayscaleImage &image, Point p1, Point p2, Byte color = 255)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    dx = std::abs(dx);
    dy = std::abs(dy);

    float mainAxis1, mainAxis2, otherAxis1, otherAxis2;

    if (dx > dy)
    {
        mainAxis1 = p1.x;
        mainAxis2 = p2.x;
        otherAxis1 = p1.y;
        otherAxis2 = p2.y;
    }
    else
    {
        mainAxis1 = p1.y;
        mainAxis2 = p2.y;
        otherAxis1 = p1.x;
        otherAxis2 = p2.x;
    }

    if (mainAxis1 > mainAxis2)
    {
        std::swap(mainAxis1, mainAxis2);
        std::swap(otherAxis1, otherAxis2);
    }

    float m = mainAxis1 == mainAxis2 ? 0 : (otherAxis2 - otherAxis1) / (mainAxis2 - mainAxis1);

    float mainAxisStart = mainAxis1;
    float mainAxisEnd = mainAxis2;

    float otherAxisStart = otherAxis1 + m * (mainAxisStart - mainAxis1);
    float otherAxisEnd = otherAxis2 + m * (mainAxisEnd - mainAxis2);

    float w1 = rfpart(mainAxis1 + 0.5);
    float w2 = fpart(mainAxis2 + 0.5);

    if (dx > dy)
    {
        setPixel(image, mainAxisStart, otherAxisStart, rfpart(otherAxisStart) * w1 * color);
        setPixel(image, mainAxisStart, otherAxisStart + 1, fpart(otherAxisStart) * w1 * color);

        setPixel(image, mainAxisEnd, otherAxisEnd, rfpart(otherAxisEnd) * w2 * color);
        setPixel(image, mainAxisEnd, otherAxisEnd + 1, fpart(otherAxisEnd) * w2 * color);
    }
    else
    {
        setPixel(image, otherAxisStart, mainAxisStart, rfpart(otherAxisStart) * w1 * color);
        setPixel(image, otherAxisStart, mainAxisStart + 1, fpart(otherAxisStart) * w1 * color);

        setPixel(image, otherAxisEnd, mainAxisEnd, rfpart(otherAxisEnd) * w2 * color);
        setPixel(image, otherAxisEnd, mainAxisEnd + 1, fpart(otherAxisEnd) * w2 * color);
    }

    float otherAxis = otherAxisStart + m;

    for (int mainAxis = mainAxisStart + 1; mainAxis < mainAxisEnd; mainAxis++)
    {
        if (dx > dy)
        {
            setPixel(image, mainAxis, std::floor(otherAxis), rfpart(otherAxis) * color);
            setPixel(image, mainAxis, std::floor(otherAxis + 1), fpart(otherAxis) * color);
        }
        else
        {
            setPixel(image, std::floor(otherAxis), mainAxis, rfpart(otherAxis) * color);
            setPixel(image, std::floor(otherAxis + 1), mainAxis, fpart(otherAxis) * color);
        }

        otherAxis += m;
    }
}

int main()
{
    GrayscaleImage image(255, 255);

    Point p1 = {50, 100}, p2 = {20, 20};

    drawLineWu(image, p2, p1);

    image.Save("wu-line.png");

    return 0;
}
