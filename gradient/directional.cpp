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

    auto grayGradient = gradient::Gradient::Angle(0, 255, 120.0f);

    polygon::drawPolygon(grayscaleImage, polygonPoints, 255, grayGradient);

    grayscaleImage.Save("directional-grayscale.png");

    ColorImage colorImage(256, 256);

    auto rgbGradient = gradient::RGBGradient::Angle(RGBA(255, 0, 0), RGBA(0, 0, 255), 30.0f);

    polygon::drawPolygon(colorImage, polygonPoints, RGBA(255, 255, 0), rgbGradient);

    colorImage.Save("directional-rgb.png");

    return 0;
}
