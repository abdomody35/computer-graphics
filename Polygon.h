#pragma once

#include "Image.h"
#include "Line.h"
#include <optional>

namespace polygon
{
    namespace __detail
    {
        struct Line
        {
            int y_min, y_max;
            float x, slope_inverse;
        };

        template <typename Image, typename OutlineColor, typename FillColor>
        void drawPolygon(Image &image, const std::vector<Point> &points, OutlineColor outlineColor, std::optional<FillColor> fillColor, bool skipOutline)
        {
            int n = points.size();

            if (fillColor.has_value())
            {
                std::vector<__detail::Line> lines;
                int y_min, y_max;

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

                std::vector<__detail::Line> activeLines;
                std::vector<std::pair<Point, Point>> fillLines;
                int pointCount = 0;

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
                        pointCount += x2 - x1 + 1;
                    }
                }

                if constexpr (std::is_invocable_v<FillColor>)
                {
                    FillColor gradient = fillColor.value();

                    if constexpr (std::is_same_v<FillColor, gradient::Gradient>)
                    {
                        if (gradient.isAutoStep())
                        {
                            gradient.calculateStep(pointCount);
                        }
                    }
                    else if constexpr (std::is_same_v<FillColor, gradient::RGBGradient>)
                    {
                        if (gradient.isAutoStep())
                        {
                            gradient.calculateSteps(pointCount);
                        }
                    }

                    for (const auto &line : fillLines)
                    {
                        for (int x = line.first.x; x <= line.second.x; x++)
                        {
                            image(x, line.first.y) = gradient();
                        }
                    }
                }
                else
                {
                    for (const auto &line : fillLines)
                    {
                        line::drawLine(image, line.first, line.second, fillColor.value());
                    }
                }
            }

            if (!skipOutline)
            {
                for (int i = 0; i < n; i++)
                {
                    line::drawLine(image, points[i], points[(i + 1) % n], outlineColor);
                }
            }
        }
    }

    // ========== GrayscaleImage ==========
    // Default: white outline, no fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points)
    {
        __detail::drawPolygon(image, points, Byte(255), std::optional<Byte>(), false);
    }

    // Custom outline, no fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<Byte>(), false);
    }

    // Custom outline, solid fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor, Byte fillColor)
    {
        bool skipOutline = (outlineColor == fillColor);
        __detail::drawPolygon(image, points, outlineColor, std::optional<Byte>(fillColor), skipOutline);
    }

    // Custom outline, gradient fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor, gradient::Gradient fillColor)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<gradient::Gradient>(fillColor), false);
    }

    // Gradient fill only, no outline
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, gradient::Gradient fillColor)
    {
        __detail::drawPolygon(image, points, 0, std::optional<gradient::Gradient>(fillColor), true);
    }

    // ========== ColorImage ==========
    // Default: white outline, no fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points)
    {
        __detail::drawPolygon(image, points, RGBA(255, 255, 255), std::optional<RGBA>(), false);
    }

    // Custom outline, no fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<RGBA>(), false);
    }

    // Custom outline, solid fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor, RGBA fillColor)
    {
        bool skipOutline = (outlineColor.r == fillColor.r && outlineColor.g == fillColor.g && outlineColor.b == fillColor.b);
        __detail::drawPolygon(image, points, outlineColor, std::optional<RGBA>(fillColor), skipOutline);
    }

    // Custom outline, gradient fill (never skip outline)
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor, gradient::RGBGradient fillColor)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<gradient::RGBGradient>(fillColor), false);
    }

    // Gradient fill only, no outline
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, gradient::RGBGradient fillColor)
    {
        __detail::drawPolygon(image, points, RGBA(0, 0, 0), std::optional<gradient::RGBGradient>(fillColor), true);
    }
}
