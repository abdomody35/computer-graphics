#pragma once

#include "Image.h"
#include "Line.h"
#include <array>

#ifndef POINT
#define POINT

struct Point
{
    int x;
    int y;
};

#endif

namespace curve
{
    template <int Degree>
    struct BezierCurve
    {
        std::array<Point, Degree> points;

        BezierCurve() = default;

        BezierCurve(std::initializer_list<Point> curvePoints)
        {
            if (curvePoints.size() != Degree)
            {
                throw std::invalid_argument("Number of control points (" + std::to_string(curvePoints.size()) + ") does not match expected degree (" + std::to_string(Degree) + ")");
            }

            std::copy(curvePoints.begin(), curvePoints.end(), points.begin());
        }

        template <std::size_t N>
        BezierCurve(const std::array<Point, N> &curvePoints)
        {
            if (N != Degree)
            {
                throw std::invalid_argument("Array size (" + std::to_string(N) + ") does not match expected degree (" + std::to_string(Degree) + ")");
            }

            points = curvePoints;
        }

        BezierCurve(const std::vector<Point> &curvePoints)
        {
            if (curvePoints.size() != Degree)
            {
                throw std::invalid_argument("Vector size (" + std::to_string(curvePoints.size()) + ") does not match expected degree (" + std::to_string(Degree) + ")");
            }

            std::copy(curvePoints.begin(), curvePoints.end(), points.begin());
        }
    };

    struct FloatPoint
    {
        float x;
        float y;

        operator Point() { return {(int)std::round(x), (int)std::round(y)}; }
    };

    namespace __detail
    {
        inline FloatPoint lerp(const Point &p1, const Point &p2, const float t)
        {
            return {p1.x * t + p2.x * (1 - t), p1.y * t + p2.y * (1 - t)};
        }

        inline float calculateDistance(const Point &p1, const Point &p2)
        {
            return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
        }

        template <int Degree>
        inline bool isStraight(const BezierCurve<Degree> &curve, float err)
        {
            float total = 0;

            int size = curve.points.size() - 1;

            for (int i = 0; i < size; i++)
            {
                total += calculateDistance(curve.points[i], curve.points[i + 1]);
            }

            return total / calculateDistance(curve.points[0], curve.points[size]) < err;
        }

        template <int Degree>
        inline std::pair<std::array<Point, Degree>, std::array<Point, Degree>> splitCurve(const BezierCurve<Degree> &curve, float t)
        {
            int size = curve.points.size() - 1;

            std::vector<FloatPoint> oldPoints, newPoints;

            for (const Point &point : curve.points)
            {
                oldPoints.push_back({(float)point.x, (float)point.y});
            }

            std::array<Point, Degree> first, second;

            int firstIndex = 0, secondIndex = size;

            first[firstIndex++] = curve.points[0];

            second[secondIndex--] = curve.points[size];

            while (size > 0)
            {
                for (int i = 0; i < size; i++)
                {
                    newPoints.push_back(lerp(oldPoints[i], oldPoints[i + 1], t));
                }

                first[firstIndex++] = newPoints[0];

                second[secondIndex--] = newPoints[newPoints.size() - 1];

                oldPoints = newPoints;

                newPoints.clear();

                size--;
            }

            return {first, second};
        }

        template <typename Image, typename Color, int Degree>
        inline void drawCurve(Image &image, const BezierCurve<Degree> &curve, Color color, float t, float err)
        {
            std::vector<BezierCurve<Degree>> curves;

            curves.push_back(curve);

            for (int i = 0; i < curves.size(); i++)
            {
                if (!isStraight(curves[i], err))
                {
                    auto split = splitCurve(curves[i], t);

                    BezierCurve<Degree> firstCurve, secondCurve;
                    firstCurve.points = split.first;
                    secondCurve.points = split.second;

                    curves[i] = firstCurve;
                    curves.insert(curves.begin() + i + 1, secondCurve);

                    i--;
                }
            }

            for (const auto &curve : curves)
            {
                for (int i = 0; i < curve.points.size() - 1; i++)
                {
                    line::drawLine(image, curve.points[i], curve.points[i + 1], color);
                }
            }
        }
    }

    template <int Degree>
    inline void drawCurve(GrayscaleImage &image, const BezierCurve<Degree> &curve, Byte color = 255, float t = 0.5f, float err = 1.05f)
    {
        __detail::drawCurve(image, curve, color, t, err);
    }

    template <int Degree>
    inline void drawCurve(ColorImage &image, const BezierCurve<Degree> &curve, RGBA color = RGBA(255, 255, 255), float t = 0.5f, float err = 1.05f)
    {
        __detail::drawCurve(image, curve, color, t, err);
    }
}
