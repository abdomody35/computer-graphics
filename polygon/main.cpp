#include "../Image.h"
#include "../Line.h"

void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte color = 255)
{
    int n = points.size();

    for (int i = 0; i < n; i++)
    {
        line::drawLine(image, points[i], points[(i + 1) % n], color);
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    std::vector<Point> polygonPoints = {
        {192, 128},
        {128, 192},
        {64, 128},
        {128, 64}};

    drawPolygon(image, polygonPoints);

    image.Save("polygon.png");

    return 0;
}