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

float normalizeAngle(float theta)
{
    float TWO_PI = 2 * M_PI;

    while (theta < 0)
    {
        theta += TWO_PI;
    }

    while (theta >= TWO_PI)
    {
        theta -= TWO_PI;
    }

    return theta;
}

void drawPointInArc(GrayscaleImage &image, int x, int y, const Point &center, float start, float end, Byte color)
{
    float angle = normalizeAngle(std::atan2((double)(y - center.y), (double)(x - center.x)));

    start = normalizeAngle(start);
    end = normalizeAngle(end);

    if (start <= end)
    {
        if (angle >= start && angle <= end)
        {
            setPixel(image, x, y, color);
        }
    }
    else // wrapped interval
    {
        if (angle >= start || angle <= end)
        {
            setPixel(image, x, y, color);
        }
    }
}

void drawArcMidPoint(GrayscaleImage &image, const Point &center, int radius, int previous_theta, int theta, Byte color = 255)
{
    if (radius <= 0)
    {
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

        drawPointInArc(image, o1.x, o1.y, center, previous_theta, theta, color);
        drawPointInArc(image, o2.x, o2.y, center, previous_theta, theta, color);
        drawPointInArc(image, o3.x, o3.y, center, previous_theta, theta, color);
        drawPointInArc(image, o4.x, o4.y, center, previous_theta, theta, color);
        drawPointInArc(image, o5.x, o5.y, center, previous_theta, theta, color);
        drawPointInArc(image, o6.x, o6.y, center, previous_theta, theta, color);
        drawPointInArc(image, o7.x, o7.y, center, previous_theta, theta, color);
        drawPointInArc(image, o8.x, o8.y, center, previous_theta, theta, color);

        y++;

        int d1 = std::abs(radius * radius - x * x - y * y);
        int d2 = std::abs(radius * radius - (x - 1) * (x - 1) - y * y);

        if (d1 > d2)
        {
            x--;
        }
    }
}

void drawSpiralMidPoint(GrayscaleImage &image, const Point &center, int intercept, float factor, int loops, Byte color = 255)
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
    float previous_theta = theta;
    float max_theta = 2 * M_PI * loops;

    while (theta <= max_theta)
    {
        float radius = intercept + (factor * theta) / (2.0f * M_PI);

        previous_theta = theta;

        theta += 1 / std::max(radius, 1.0f);

        drawArcMidPoint(image, center, std::round(radius), previous_theta, theta, color);
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int intercept = 5;

    float factor = 8.0f;

    int loops = 10;

    drawSpiralMidPoint(image, center, intercept, factor, loops);

    image.Save("spiralMidPoint.png");

    return 0;
}
