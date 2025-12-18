#pragma once

#include "Image.h"
#include "Line.h"
#include <optional>

namespace polygon
{
    enum class WindingRule
    {
        ODD = 0,
        POSITIVE
    };

    namespace __detail
    {
        struct Line
        {
            int y_min, y_max;
            float x, slope_inverse;
            int direction;
        };

        template <typename Image, typename OutlineColor, typename FillColor>
        void drawPolygon(Image &image, const std::vector<Point> &points, OutlineColor outlineColor, std::optional<FillColor> fillColor, bool skipOutline, WindingRule windingRule = WindingRule::ODD)
        {
            int n = points.size();

            if (fillColor.has_value())
            {
                std::vector<__detail::Line> lines;

                int y_min = points[0].y, y_max = points[0].y;
                int x_min = points[0].x, x_max = points[0].x;

                for (int i = 0; i < n; i++)
                {
                    Point p1 = points[i], p2 = points[(i + 1) % n];

                    int direction;

                    if (p1.y > p2.y)
                    {
                        std::swap(p1, p2);
                        direction = -1;
                    }
                    else
                    {
                        direction = 1;
                    }

                    if (p1.y == p2.y || p1.y > image.GetHeight() || p2.y < 0)
                    {
                        continue;
                    }

                    lines.push_back({p1.y, p2.y, float(p1.x), (p2.x - p1.x) / float(p2.y - p1.y), direction});

                    x_min = std::min(x_min, std::min(p1.x, p2.x));
                    x_max = std::max(x_max, std::max(p1.x, p2.x));
                    y_min = std::min(y_min, std::min(p1.y, p2.y));
                    y_max = std::max(y_max, std::max(p1.y, p2.y));
                }

                Point center = {(x_min + x_max) / 2, (y_min + y_max) / 2};

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

                    std::sort(activeLines.begin(), activeLines.end(), [](const auto &a, const auto &b) { return a.x < b.x; });

                    std::vector<std::pair<float, int>> x_coords;

                    for (auto &line : activeLines)
                    {
                        x_coords.push_back({line.x, line.direction});
                        line.x += line.slope_inverse;
                    }

                    int winding = 0;
                    int fill_start = -1;

                    for (const auto &x_coord : x_coords)
                    {
                        winding += x_coord.second;

                        bool should_fill;

                        if (windingRule == WindingRule::ODD)
                        {
                            should_fill = winding % 2;
                        }
                        else // if (windingRule == WindingRule::POSITIVE)
                        {
                            should_fill = winding > 0;
                        }

                        if (should_fill && fill_start < 0)
                        {
                            fill_start = int(std::round(x_coord.first));
                        }
                        else if (!should_fill && fill_start >= 0)
                        {
                            int fill_end = int(std::round(x_coord.first));
                            if (fill_end > fill_start)
                            {
                                fillLines.push_back({{fill_start, y}, {fill_end, y}});
                                pointCount += fill_end - fill_start + 1;
                            }
                            fill_start = -1;
                        }
                    }
                }

                if constexpr (std::is_invocable_v<FillColor>)
                {
                    FillColor gradient = fillColor.value();

                    if constexpr (std::is_same_v<FillColor, gradient::Gradient> || std::is_same_v<FillColor, gradient::RGBGradient>)
                    {
                        if (gradient.isDirectional())
                        {
                            gradient.setCenter(center);
                            gradient.calculateProjectionRange(fillLines);
                        }
                        else
                        {
                            if (gradient.isAutoStep())
                            {
                                gradient.calculateStep(pointCount);
                            }
                        }
                    }

                    for (const auto &line : fillLines)
                    {
                        for (int x = line.first.x; x <= line.second.x; x++)
                        {
                            if (gradient.isDirectional())
                            {
                                image(x, line.first.y) = gradient(x, line.first.y);
                            }
                            else
                            {
                                image(x, line.first.y) = gradient();
                            }
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
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, Byte(255), std::optional<Byte>(), false, windingRule);
    }

    // Custom outline, no fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<Byte>(), false, windingRule);
    }

    // Custom outline, solid fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor, Byte fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        bool skipOutline = (outlineColor == fillColor);
        __detail::drawPolygon(image, points, outlineColor, std::optional<Byte>(fillColor), skipOutline, windingRule);
    }

    // Custom outline, gradient fill
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, Byte outlineColor, gradient::Gradient fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<gradient::Gradient>(fillColor), false, windingRule);
    }

    // Gradient fill only, no outline
    inline void drawPolygon(GrayscaleImage &image, const std::vector<Point> &points, gradient::Gradient fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, 0, std::optional<gradient::Gradient>(fillColor), true, windingRule);
    }

    // ========== ColorImage ==========
    // Default: white outline, no fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, RGBA(255, 255, 255), std::optional<RGBA>(), false, windingRule);
    }

    // Custom outline, no fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<RGBA>(), false, windingRule);
    }

    // Custom outline, solid fill
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor, RGBA fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        bool skipOutline = (outlineColor.r == fillColor.r && outlineColor.g == fillColor.g && outlineColor.b == fillColor.b);
        __detail::drawPolygon(image, points, outlineColor, std::optional<RGBA>(fillColor), skipOutline, windingRule);
    }

    // Custom outline, gradient fill (never skip outline)
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, RGBA outlineColor, gradient::RGBGradient fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, outlineColor, std::optional<gradient::RGBGradient>(fillColor), false, windingRule);
    }

    // Gradient fill only, no outline
    inline void drawPolygon(ColorImage &image, const std::vector<Point> &points, gradient::RGBGradient fillColor, WindingRule windingRule = WindingRule::ODD)
    {
        __detail::drawPolygon(image, points, RGBA(0, 0, 0), std::optional<gradient::RGBGradient>(fillColor), true, windingRule);
    }
}
