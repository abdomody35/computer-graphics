#include "../Image.h"
#include "../Line.h"
#include "../Circle.h"
#include "../Polygon.h"
#include "../Curve.h"

enum class SamplingFactor
{
    x2 = 2,
    x4 = 4,
    x8 = 8,
    x16 = 16
};

Point scale(const Point &point, SamplingFactor factor)
{
    return {point.x * (int)factor, point.y * (int)factor};
}

int scale(const int length, SamplingFactor factor)
{
    return length * (int)factor;
}

template <typename Image>
class SuperSampleImage
{
    Image &original;
    int factor;
    Image sampled;

public:
    SuperSampleImage(Image &image, SamplingFactor factor) : original(image), factor((int)factor)
    {
        sampled = Image(image.GetWidth() * this->factor, image.GetHeight() * this->factor);
    }

    void operator()(int x, int y, auto color)
    {
        int scaleFactor = static_cast<int>(factor);

        int sx = x * scaleFactor;
        int sy = y * scaleFactor;

        if (sx < sampled.GetWidth() && sy < sampled.GetHeight())
        {
            sampled(sx, sy) = color;
        }
    }

    Image &getSampledImage()
    {
        return sampled;
    }

    ~SuperSampleImage()
    {
        for (int y = 0; y < original.GetHeight(); y++)
        {
            for (int x = 0; x < original.GetWidth(); x++)
            {
                int total = 0;

                for (int dy = 0; dy < factor; dy++)
                {
                    for (int dx = 0; dx < factor; dx++)
                    {
                        int sx = x * factor + dx;
                        int sy = y * factor + dy;

                        if (sx < sampled.GetWidth() && sy < sampled.GetHeight())
                        {
                            total += sampled(sx, sy);
                        }
                    }
                }

                original(x, y) = std::round(total / (float)factor);
            }
        }
    }
};

template <typename Image, typename DrawFunction>
void applySuperSampling(Image &image, SamplingFactor factor, DrawFunction draw)
{
    SuperSampleImage<Image> ssImage(image, factor);
    draw(ssImage.getSampledImage());
}

int main()
{
    GrayscaleImage image(256, 256);

    Point p1 = {10, 10}, p2 = {245, 70}, center = {128, 144};

    gradient::Gradient gradient = gradient::Gradient::Horizontal(128, 255);

    SamplingFactor factor = SamplingFactor::x8;

    std::vector<Point> polygonPoints = {
        scale({64, 64}, factor),
        scale({64, 192}, factor),
        scale({192, 192}, factor),
        scale({192, 64}, factor),
        scale({128, 128}, factor)
    };

    curve::BezierCurve<5> curve({
        scale({64, 192}, factor),
        scale({96, 255}, factor),
        scale({128, 64}, factor),
        scale({160, 255}, factor),
        scale({192, 192}, factor),
    });

    applySuperSampling(image, factor, [&](GrayscaleImage &image) {
        line::drawLine(image, scale(p1, factor), scale(p2, factor));
        circle::drawCircle(image, scale(center, factor), scale(90, factor));
        polygon::drawPolygon(image, polygonPoints, 255);
        curve::drawCurve(image, curve);
    });

    image.Save("naive.png");

    return 0;
}
