#include "../Image.h"
#include "../Polygon.h"

int main()
{
    std::vector<Point> polygonPoints = {
        {64, 64},
        {64, 192},
        {192, 192},
        {192, 64},
        {128, 128}};

    GrayscaleImage grayscaleImage(256, 256);

    gradient::Gradient grayGradient(0, std::nullopt, 255, 120.0f);

    polygon::drawPolygon(grayscaleImage, polygonPoints, grayGradient);

    grayscaleImage.Save("directional-grayscale.png");

    ColorImage colorImage(256, 256);

    gradient::RGBGradient rgbGradient(RGBA(255, 0, 0), std::nullopt, RGBA(0, 0, 255), 30.0f);

    polygon::drawPolygon(colorImage, polygonPoints, rgbGradient);

    colorImage.Save("directional-rgb.png");

    return 0;
}
