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

void drawEllipseNaive(GrayscaleImage &image, const Point &center, int width, int height, float step = 0.001f, Byte color = 255)
{
    if (width <= 0)
    {
        std::cerr << "Width must be positive." << std::endl;
        return;
    }

    if (height <= 0)
    {
        std::cerr << "Height must be positive." << std::endl;
        return;
    }

    int radius_x = width / 2;
    int radius_y = height / 2;

    float theta = 0;
    float max_theta = 2 * M_PI;

    while (theta <= max_theta)
    {
        int x = std::round(center.x + radius_x * cos(theta));
        int y = std::round(center.y + radius_y * sin(theta));

        setPixel(image, x, y, color);

        theta += step;
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int width = 50;

    int height = 150;

    drawEllipseNaive(image, center, width, height);

    image.Save("ellipseNaive.png");

    return 0;
}
