#include "../Image.h"

struct Point {
    int x;
    int y;
};

void drawLineDDA(GrayscaleImage &image, Point p1, Point p2, Byte color = 255)
{
    int width = image.GetWidth();
    int height = image.GetHeight();

    float m = std::fabs((float)(p2.y - p1.y) / (float)(p2.x - p1.x));

    if (m < 1)
    {
        if (p1.x > p2.x)
        {
            std::swap(p1, p2);
        }

        float y = p1.y;

        for (int x = p1.x; x <= p2.x; x++)
        {
            if (x > 0 && x < width && std::round(y) > 0 && std::round(y) < height)
            {
                image(x, std::round(y)) = color;
            }

            if (p1.y < p2.y)
            {
                y += m;
            }
            else
            {
                y -= m;
            }
        }
    }
    else
    {
        if (p1.y > p2.y)
        {
            std::swap(p1, p2);
        }

        float m_inv = 1 / m;

        float x = p1.x;

        for (int y = p1.y; y <= p2.y; y++)
        {
            if (std::round(x) > 0 && std::round(x) < width && y > 0 && y < height)
            {
                image(std::round(x), y) = color;
            }

            if (p1.x < p2.x)
            {
                x += m_inv;
            }
            else
            {
                x -= m_inv;
            }
        }
    }
}

int main()
{
    GrayscaleImage image(255, 255);

    Point p1 = { 50, 20 }, p2 = { 80, 100 };

    drawLineDDA(image, p2, p1);

    image.Save("DDA.png");

    return 0;
}