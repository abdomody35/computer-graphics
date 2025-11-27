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

void drawRoseCurveNaive(GrayscaleImage &image, const Point &center, int petals, float step = 0.001f, float scale = 0, Byte color = 255)
{
    if (petals <= 0)
    {
        std::cerr << "Petals must be positive." << std::endl;
        return;
    }

    if (scale <= 0)
    {
        scale = image.GetWidth() / 2 - 10;
    }

    float theta = 0;
    float max_theta = 2 * M_PI;
    while (theta <= max_theta)
    {
        float radius = cos(petals * theta) * scale;

        int x = std::round(center.x + radius * cos(theta));
        int y = std::round(center.y + radius * sin(theta));

        setPixel(image, x, y, color);

        theta += step;
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int petals = 4;

    drawRoseCurveNaive(image, center, petals);

    image.Save("roseCurveNaive.png");

    return 0;
}
