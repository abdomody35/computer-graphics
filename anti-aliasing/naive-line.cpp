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

float computeDecisionParameter(int x, int y, float a, float b, float c)
{
    return std::fabs(a * x + b * y + c);
}

void drawLineMidPoint(GrayscaleImage &image, Point p1, Point p2, Byte color = 255)
{
    float m = (float)(p2.y - p1.y) / (float)(p2.x - p1.x);
    float a = p1.y - p2.y;
    float b = p2.x - p1.x;
    float c = p1.x * p2.y - p2.x * p1.y;

    if (m >= 0 && m <= 1)
    {
        if (p1.x > p2.x)
        {
            std::swap(p1, p2);
        }

        float y = p1.y;

        for (int x = p1.x; x <= p2.x; x++)
        {
            float d1 = computeDecisionParameter(x + 1, y, a, b, c);
            float d2 = computeDecisionParameter(x + 1, y + 1, a, b, c);

            setPixel(image, x + 1, y, color * (d2 / (d1 + d2)));
            setPixel(image, x + 1, y + 1, color * (d1 / (d1 + d2)));

            if (d1 > d2)
            {
                y += 1;
            }
        }
    }
    else if (m > 1)
    {
        if (p1.y > p2.y)
        {
            std::swap(p1, p2);
        }

        float x = p1.x;

        for (int y = p1.y; y <= p2.y; y++)
        {
            float d1 = computeDecisionParameter(x, y + 1, a, b, c);
            float d2 = computeDecisionParameter(x + 1, y + 1, a, b, c);

            setPixel(image, x, y + 1, color * (d2 / (d1 + d2)));
            setPixel(image, x + 1, y + 1, color * (d1 / (d1 + d2)));

            if (d1 > d2)
            {
                x += 1;
            }
        }
    }
    else if (m >= -1 && m < 0)
    {
        if (p1.x > p2.x)
        {
            std::swap(p1, p2);
        }

        float y = p1.y;

        for (int x = p1.x; x <= p2.x; x++)
        {
            float d1 = computeDecisionParameter(x + 1, y, a, b, c);
            float d2 = computeDecisionParameter(x + 1, y - 1, a, b, c);

            setPixel(image, x + 1, y, color * (d2 / (d1 + d2)));
            setPixel(image, x + 1, y - 1, color * (d1 / (d1 + d2)));

            if (d1 > d2)
            {
                y -= 1;
            }
        }
    }
    else // m < -1
    {
        if (p1.y > p2.y)
        {
            std::swap(p1, p2);
        }

        float x = p1.x;

        for (int y = p1.y; y <= p2.y; y++)
        {
            float d1 = computeDecisionParameter(x, y - 1, a, b, c);
            float d2 = computeDecisionParameter(x - 1, y - 1, a, b, c);

            setPixel(image, x, y - 1, color * (d2 / (d1 + d2)));
            setPixel(image, x - 1, y - 1, color * (d1 / (d1 + d2)));

            if (d1 > d2)
            {
                x -= 1;
            }
        }
    }
}

int main()
{
    GrayscaleImage image(255, 255);

    Point p1 = {50, 100}, p2 = {20, 20};

    drawLineMidPoint(image, p2, p1);

    image.Save("naive-line.png");

    return 0;
}