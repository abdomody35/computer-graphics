#include "../Image.h"
#include "../Polygon.h"

int main()
{
    std::vector<Point> polygonPoints = {
        {64, 64},
        {64, 192},
        {192, 192},
        {192, 64},
        {96, 128},
        {160, 128}};

    GrayscaleImage oddImage(256, 256);
    polygon::drawPolygon(oddImage, polygonPoints, 255, 128, polygon::WindingRule::ODD);
    oddImage.Save("odd.png");

    GrayscaleImage positiveImage(256, 256);
    polygon::drawPolygon(positiveImage, polygonPoints, 255, 128, polygon::WindingRule::POSITIVE);
    positiveImage.Save("positive.png");

    return 0;
}
