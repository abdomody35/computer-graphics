#include "../Image.h"
#include "../Line.h"
#include "../Gradient.h"
#include <functional>

struct Line
{
    int y_min, y_max;
    float x, slope_inverse;
};

void drawColorGradientPolygon(ColorImage &image, const std::vector<Point> &points, std::optional<RGBA> outlineColor = std::nullopt, std::optional<std::function<RGBA()>> fillColor = std::nullopt)
{
    int n = points.size();

    std::vector<Line> lines;

    int y_min, y_max, pointCount = 0;

    for (int i = 0; i < n; i++)
    {
        Point p1 = points[i], p2 = points[(i + 1) % n];

        if (p1.y > p2.y)
        {
            std::swap(p1, p2);
        }

        if (p1.y == p2.y || p1.y > image.GetHeight() || p2.y < 0)
        {
            continue;
        }

        lines.push_back({p1.y, p2.y, float(p1.x), (p2.x - p1.x) / float(p2.y - p1.y)});

        if (i == 0)
        {
            y_min = p1.y;
            y_max = p2.y;
        }
        else
        {
            if (p1.y < y_min)
            {
                y_min = p1.y;
            }
            if (p2.y > y_max)
            {
                y_max = p2.y;
            }
        }
    }

    std::vector<Line> activeLines;

    std::vector<std::pair<Point, Point>> fillLines;

    for (int y = y_min; y <= y_max; y++)
    {
        for (int i = 0; i < activeLines.size(); i++)
        {
            if (activeLines[i].y_max == y)
            {
                activeLines.erase(activeLines.begin() + i);
                i--;
            }
        }

        for (int i = 0; i < lines.size(); i++)
        {
            if (lines[i].y_min == y)
            {
                activeLines.push_back(lines[i]);
                lines.erase(lines.begin() + i);
                i--;
            }
        }

        std::vector<int> x_coords;

        for (auto &line : activeLines)
        {
            x_coords.push_back(line.x);
            line.x += line.slope_inverse;
        }

        std::sort(x_coords.begin(), x_coords.end());

        for (int i = 0; i < x_coords.size(); i += 2)
        {
            int x1 = int(std::round(x_coords[i])), x2 = int(std::round(x_coords[i + 1]));

            fillLines.push_back({{x1, y}, {x2, y}});

            pointCount += x2 - x1;
        }
    }

    gradient::Gradient gradient(0, 255.0f / pointCount);

    if (!fillColor.has_value())
    {
        fillColor = gradient;
    }

    for (const auto &line : fillLines)
    {
        for (int x = line.first.x; x <= line.second.x; x++)
        {
            image(x, line.first.y) = fillColor.value()();
        }
    }

    if (outlineColor.has_value())
    {
        for (int i = 0; i < n; i++)
        {
            line::drawLine(image, points[i], points[(i + 1) % n], outlineColor.value());
        }
    }
}

int main()
{
    ColorImage image(256, 256);

    std::vector<Point> polygonPoints = {
        {64, 64},
        {64, 192},
        {192, 192},
        {192, 64},
        {128, 128}};

    gradient::RGBGradient rgbGradient(RGBA(0, 0, 0), 0.02f, 0, 0);

    drawColorGradientPolygon(image, polygonPoints, RGBA(255, 255, 0), rgbGradient);

    image.Save("colorGradient.png");

    return 0;
}
