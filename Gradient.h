#pragma once

#include "Image.h"
#include <optional>

#ifndef POINT
#define POINT

struct Point
{
    int x;
    int y;
};

#endif

namespace gradient
{
    namespace __detail
    {
        struct Direction
        {
            enum class Type
            {
                Horizontal,
                Vertical,
                Custom
            };

            Type type;
            float angleDegrees;

            static Direction Horizontal() { return {Type::Horizontal, 0.0f}; }
            static Direction Vertical() { return {Type::Vertical, 90.0f}; }
            static Direction Angle(float degrees) { return {Type::Custom, degrees}; }

            float getAngle() const { return angleDegrees; }
        };

        // Base gradient class using CRTP for common functionality
        template <typename Derived, typename ColorType>
        class GradientBase
        {
        protected:
            ColorType startColor;
            ColorType endColor;
            std::optional<Direction> direction;

            Point center{0, 0};
            float minProjection = 0.0f;
            float maxProjection = 1.0f;
            bool projectionReady = false;

        public:
            GradientBase(ColorType start, ColorType end, std::optional<Direction> dir = std::nullopt)
                : startColor(start), endColor(end), direction(dir) {}

            ColorType at(int x, int y) const
            {
                if (!direction.has_value())
                {
                    // Sequential gradient shouldn't use this method
                    return startColor;
                }

                float t = calculateNormalizedPosition(x, y);
                return static_cast<const Derived *>(this)->interpolate(startColor, endColor, t);
            }

            ColorType next(int totalSteps, int currentStep) const
            {
                if (direction.has_value())
                {
                    // Directional gradient shouldn't use this method
                    return startColor;
                }

                float t = totalSteps > 1 ? static_cast<float>(currentStep) / (totalSteps - 1) : 0.0f;
                return static_cast<const Derived *>(this)->interpolate(startColor, endColor, t);
            }

            bool isDirectional() const { return direction.has_value(); }

            void prepareDirectional(const Point &polygonCenter,
                                    const std::vector<std::pair<Point, Point>> &fillLines)
            {
                if (!direction.has_value())
                    return;

                center = polygonCenter;

                float angle_rad = direction->getAngle() * M_PI / 180.0f;
                float dir_x = cos(angle_rad);
                float dir_y = sin(angle_rad);

                minProjection = std::numeric_limits<float>::max();
                maxProjection = std::numeric_limits<float>::lowest();

                for (const auto &[start, end] : fillLines)
                {
                    for (int x = start.x; x <= end.x; x++)
                    {
                        float vec_x = x - center.x;
                        float vec_y = start.y - center.y;
                        float projection = vec_x * dir_x + vec_y * dir_y;

                        minProjection = std::min(minProjection, projection);
                        maxProjection = std::max(maxProjection, projection);
                    }
                }

                projectionReady = true;
            }

        private:
            float calculateNormalizedPosition(int x, int y) const
            {
                float angle_rad = direction->getAngle() * M_PI / 180.0f;
                float dir_x = cos(angle_rad);
                float dir_y = sin(angle_rad);

                float vec_x = x - center.x;
                float vec_y = y - center.y;
                float projection = vec_x * dir_x + vec_y * dir_y;

                if (maxProjection == minProjection)
                    return 0.5f;

                float t = (projection - minProjection) / (maxProjection - minProjection);
                return std::clamp(t, 0.0f, 1.0f);
            }
        };
    }

    struct Gradient : public __detail::GradientBase<Gradient, float>
    {
        using GradientBase::GradientBase;

        static Gradient Sequential(float start, float end)
        {
            return Gradient(start, end, std::nullopt);
        }

        static Gradient Horizontal(float start, float end)
        {
            return Gradient(start, end, __detail::Direction::Horizontal());
        }

        static Gradient Vertical(float start, float end)
        {
            return Gradient(start, end, __detail::Direction::Vertical());
        }

        static Gradient Angle(float start, float end, float degrees)
        {
            return Gradient(start, end, __detail::Direction::Angle(degrees));
        }

        float interpolate(float start, float end, float t) const
        {
            float value = start + t * (end - start);
            return std::clamp(value, 0.0f, 255.0f);
        }

        Byte next(int totalSteps, int currentStep) const
        {
            return static_cast<Byte>(std::round(GradientBase::next(totalSteps, currentStep)));
        }

        Byte at(int x, int y) const
        {
            return static_cast<Byte>(std::round(GradientBase::at(x, y)));
        }
    };

    struct RGBGradient : public __detail::GradientBase<RGBGradient, RGBA>
    {
        using GradientBase::GradientBase;

        static RGBGradient Sequential(RGBA start, RGBA end)
        {
            return RGBGradient(start, end, std::nullopt);
        }

        static RGBGradient Horizontal(RGBA start, RGBA end)
        {
            return RGBGradient(start, end, __detail::Direction::Horizontal());
        }

        static RGBGradient Vertical(RGBA start, RGBA end)
        {
            return RGBGradient(start, end, __detail::Direction::Vertical());
        }

        static RGBGradient Angle(RGBA start, RGBA end, float degrees)
        {
            return RGBGradient(start, end, __detail::Direction::Angle(degrees));
        }

        RGBA interpolate(RGBA start, RGBA end, float t) const
        {
            return RGBA(
                static_cast<int>(std::round(start.r + t * (end.r - start.r))),
                static_cast<int>(std::round(start.g + t * (end.g - start.g))),
                static_cast<int>(std::round(start.b + t * (end.b - start.b))));
        }

        RGBA next(int totalSteps, int currentStep) const
        {
            return GradientBase::next(totalSteps, currentStep);
        }

        RGBA at(int x, int y) const
        {
            return GradientBase::at(x, y);
        }
    };
}
