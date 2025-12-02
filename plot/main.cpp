#include "../Image.h"
#include "../Line.h"
#include "../Circle.h"

void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte color = 255)
{
    int n = points.size();

    for (int i = 0; i < n; i++)
    {
        line::drawLine(image, points[i], points[(i + 1) % n], color);
    }
}

void plotValues(GrayscaleImage &image, const int radius, const std::vector<float> &values, Byte color = 255)
{
    const float margin = 0.05;

    const int width = image.GetWidth();
    const int height = image.GetHeight();

    const Point center = {width / 2, height / 2};

    int size = values.size();

    float theta = 2 * M_PI / size;

    std::vector<Point> points;

    for (int i = 0; i < size; i++)
    {
        int x = radius * values[i] * cos(theta * i) + center.x;
        int y = radius * values[i] * sin(theta * i) + center.y;

        points.push_back({x, y});
    }

    line::drawLine(image, {int(std::round(width * margin)), center.y}, {int(std::round(width * (1 - margin))), center.y}, color);

    line::drawLine(image, {center.x, int(std::round(height * margin))}, {center.x, int(std::round(height * (1 - margin)))}, color);

    circle::drawCircle(image, center, std::round(radius * 1 / 3));

    circle::drawCircle(image, center, std::round(radius * 2 / 3));

    circle::drawCircle(image, center, radius);

    drawPolygon(image, points, color);
}

int main()
{
    GrayscaleImage image(256, 256);

    int radius = 64;

    std::vector<float> values = {1, 1, 1, 1, 1};

    plotValues(image, radius, values);

    image.Save("plot.png");

    return 0;
}
