#include "../Image.h"
#include "../Line.h"

void drawRoseCurveNaive(GrayscaleImage &image, const Point &center, int petals, float step = 0.010f, float scale = 0, Byte color = 255)
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

    float radius = cos(petals * theta) * scale;
    int x = center.x + std::round(radius * cos(theta));
    int y = center.y + std::round(radius * sin(theta));
    int previous_x;
    int previous_y;

    while (theta <= max_theta)
    {
        previous_x = x;
        previous_y = y;

        theta += step;

        radius = cos(petals * theta) * scale;

        x = center.x + std::round(radius * cos(theta));
        y = center.y + std::round(radius * sin(theta));

        line::drawLine(image, {previous_x, previous_y}, {x, y});
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    Point center = {128, 128};

    int petals = 4;

    float step = 0.01f;

    drawRoseCurveNaive(image, center, petals, step);

    image.Save("roseCurveLine.png");
}
