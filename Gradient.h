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
    class Gradient
    {
    private:
        float value;
        float step;
        float startValue;
        std::optional<float> endValue;
        bool autoStep;

        float angle;
        Point center;
        bool useDirectionalGradient = false;
        bool projectionCalculated = false;
        float minProjection;
        float maxProjection;

    public:
        Gradient(float start, float step, std::optional<float> end = std::nullopt)
            : value(start), step(step), startValue(start), endValue(end), autoStep(false) {}

        Gradient(float start, std::nullopt_t, std::optional<float> end)
            : value(start), step(0.0f), startValue(start), endValue(end), autoStep(true) {}

        Gradient(float start, float step, std::optional<float> end, float angle)
            : value(start), step(step), startValue(start), endValue(end), autoStep(false),
              angle(angle), center{0, 0}, useDirectionalGradient(true) {}

        Gradient(float start, std::nullopt_t, std::optional<float> end, float angle)
            : value(start), step(0.0f), startValue(start), endValue(end), autoStep(true),
              angle(angle), center{0, 0}, useDirectionalGradient(true) {}

        void reset() { value = startValue; }

        float getCurrent() const { return value; }

        float getStartValue() const { return startValue; }

        float getEndValue() const { return endValue.value_or(255.0f); }

        bool isFinished() const
        {
            return endValue.has_value() ? (step >= 0 ? value >= endValue.value() : value <= endValue.value()) : false;
        }

        // For non-directional gradients
        int operator()()
        {
            if (!useDirectionalGradient)
            {
                float current = value;
                value += step;

                if (endValue.has_value())
                {
                    value = std::clamp(value, std::min(startValue, endValue.value()), std::max(startValue, endValue.value()));
                }
                else
                {
                    value = std::clamp(value, startValue, 255.0f);
                }

                return static_cast<int>(std::round(current));
            }
            else
            {
                // This shouldn't be called for directional gradients
                return static_cast<int>(std::round(startValue));
            }
        }

        // For directional gradients
        int operator()(int x, int y)
        {
            if (!useDirectionalGradient)
            {
                return operator()();
            }

            // Calculate projection value for this point
            float angle_rad = angle * M_PI / 180.0f;
            float dir_x = cos(angle_rad);
            float dir_y = sin(angle_rad);

            // Vector from center to current pixel
            float vec_x = x - center.x;
            float vec_y = y - center.y;

            // Project onto direction vector
            float projection = vec_x * dir_x + vec_y * dir_y;

            // Normalize to 0-1 range
            float normalized = (projection - minProjection) / (maxProjection - minProjection);
            normalized = std::clamp(normalized, 0.0f, 1.0f);

            // Map to gradient range
            float gradientValue = startValue + normalized * (endValue.value_or(255.0f) - startValue);

            return static_cast<int>(std::round(gradientValue));
        }

        void setStep(float newStep) { step = newStep; }

        void setValue(float newvalue) { value = std::clamp(newvalue, 0.0f, 255.0f); }

        void calculateStep(int totalPoints)
        {
            if (autoStep && endValue.has_value() && totalPoints > 0)
            {
                step = (endValue.value() - startValue) / static_cast<float>(totalPoints);
            }
        }

        bool isAutoStep() const { return autoStep; }

        void setCenter(const Point &p) { center = p; }

        void calculateProjectionRange(const std::vector<std::pair<Point, Point>> &fillLines)
        {
            if (!useDirectionalGradient)
                return;

            float angle_rad = angle * M_PI / 180.0f;
            float dir_x = cos(angle_rad);
            float dir_y = sin(angle_rad);

            minProjection = std::numeric_limits<float>::max();
            maxProjection = std::numeric_limits<float>::lowest();

            for (const auto &line : fillLines)
            {
                for (int x = line.first.x; x <= line.second.x; x++)
                {
                    int y = line.first.y;

                    float vec_x = x - center.x;
                    float vec_y = y - center.y;

                    float projection = vec_x * dir_x + vec_y * dir_y;

                    minProjection = std::min(minProjection, projection);
                    maxProjection = std::max(maxProjection, projection);
                }
            }

            projectionCalculated = true;
        }

        bool isDirectional() const { return useDirectionalGradient; }
    };

    class RGBGradient
    {
    private:
        Gradient r, g, b;
        bool autoStep;

        float angle;
        Point center;
        bool useDirectionalGradient;
        bool projectionCalculated;
        float minProjection;
        float maxProjection;

    public:
        RGBGradient(float r_start, float r_step, std::optional<float> r_end,
                    float g_start, float g_step, std::optional<float> g_end,
                    float b_start, float b_step, std::optional<float> b_end)
            : r(r_start, r_step, r_end), g(g_start, g_step, g_end), b(b_start, b_step, b_end), autoStep(false),
              angle(0.0f), center{0, 0}, useDirectionalGradient(false), projectionCalculated(false) {}

        RGBGradient(RGBA startColor, RGBA endColor, float r_step, float g_step, float b_step)
            : r(startColor.r, r_step, endColor.r), g(startColor.g, g_step, endColor.g), b(startColor.b, b_step, endColor.b), autoStep(false),
              angle(0.0f), center{0, 0}, useDirectionalGradient(false), projectionCalculated(false) {}

        RGBGradient(RGBA startColor, RGBA endColor, float step)
            : r(startColor.r, step, endColor.r), g(startColor.g, step, endColor.g), b(startColor.b, step, endColor.b), autoStep(false),
              angle(0.0f), center{0, 0}, useDirectionalGradient(false), projectionCalculated(false) {}

        RGBGradient(RGBA color, float r_step, float g_step, float b_step)
            : RGBGradient(color.r, r_step, std::nullopt, color.g, g_step, std::nullopt, color.b, b_step, std::nullopt) {}

        RGBGradient(RGBA color, float step)
            : RGBGradient(color.r, step, std::nullopt, color.g, step, std::nullopt, color.b, step, std::nullopt) {}

        RGBGradient(RGBA startColor, RGBA endColor)
            : r(startColor.r, std::nullopt, endColor.r), g(startColor.g, std::nullopt, endColor.g), b(startColor.b, std::nullopt, endColor.b), autoStep(true),
              angle(0.0f), center{0, 0}, useDirectionalGradient(false), projectionCalculated(false) {}

        RGBGradient(RGBA startColor, float r_step, RGBA endColor, float g_step, float b_step, float angle)
            : r(startColor.r, r_step, endColor.r), g(startColor.g, g_step, endColor.g), b(startColor.b, b_step, endColor.b), autoStep(false),
              angle(angle), center{0, 0}, useDirectionalGradient(true), projectionCalculated(false) {}

        RGBGradient(RGBA startColor, float step, RGBA endColor, float angle)
            : r(startColor.r, step, endColor.r), g(startColor.g, step, endColor.g), b(startColor.b, step, endColor.b), autoStep(false),
              angle(angle), center{0, 0}, useDirectionalGradient(true), projectionCalculated(false) {}

        RGBGradient(RGBA startColor, std::nullopt_t, RGBA endColor, float angle)
            : r(startColor.r, std::nullopt, endColor.r), g(startColor.g, std::nullopt, endColor.g), b(startColor.b, std::nullopt, endColor.b), autoStep(true),
              angle(angle), center{0, 0}, useDirectionalGradient(true), projectionCalculated(false) {}

        // Original operator() for non-directional gradients
        RGBA operator()()
        {
            if (!useDirectionalGradient)
            {
                return RGBA(r(), g(), b());
            }
            else
            {
                // This shouldn't be called for directional gradients
                return RGBA(r.getCurrent(), g.getCurrent(), b.getCurrent());
            }
        }

        // New operator() for directional gradients
        RGBA operator()(int x, int y)
        {
            if (!useDirectionalGradient)
            {
                return operator()();
            }

            // Calculate projection value for this point
            float angle_rad = angle * M_PI / 180.0f;
            float dir_x = cos(angle_rad);
            float dir_y = sin(angle_rad);

            // Vector from center to current pixel
            float vec_x = x - center.x;
            float vec_y = y - center.y;

            // Project onto direction vector
            float projection = vec_x * dir_x + vec_y * dir_y;

            // Normalize to 0-1 range
            float normalized = (projection - minProjection) / (maxProjection - minProjection);
            normalized = std::clamp(normalized, 0.0f, 1.0f);

            // Map to gradient range for each channel
            float r_val = r.getStartValue() + normalized * (r.getEndValue() - r.getStartValue());
            float g_val = g.getStartValue() + normalized * (g.getEndValue() - g.getStartValue());
            float b_val = b.getStartValue() + normalized * (b.getEndValue() - b.getStartValue());

            return RGBA(
                static_cast<int>(std::round(r_val)),
                static_cast<int>(std::round(g_val)),
                static_cast<int>(std::round(b_val)));
        }

        void reset()
        {
            r.reset();
            g.reset();
            b.reset();
        }

        RGBA getCurrent() const
        {
            return RGBA(r.getCurrent(), g.getCurrent(), b.getCurrent());
        }

        bool isFinished() const
        {
            return r.isFinished() || g.isFinished() || b.isFinished();
        }

        bool allFinished() const
        {
            return r.isFinished() && g.isFinished() && b.isFinished();
        }

        void setColor(float r_val, float g_val, float b_val)
        {
            r.setValue(r_val);
            g.setValue(g_val);
            b.setValue(b_val);
        }

        void setRStep(float newStep) { r.setStep(newStep); }
        void setGStep(float newStep) { g.setStep(newStep); }
        void setBStep(float newStep) { b.setStep(newStep); }

        void setRValue(float val) { r.setValue(val); }
        void setGValue(float val) { g.setValue(val); }
        void setBValue(float val) { b.setValue(val); }

        float getR() const { return r.getCurrent(); }
        float getG() const { return g.getCurrent(); }
        float getB() const { return b.getCurrent(); }

        void calculateStep(int totalPoints)
        {
            if (autoStep && totalPoints > 0)
            {
                r.calculateStep(totalPoints);
                g.calculateStep(totalPoints);
                b.calculateStep(totalPoints);
            }
        }

        bool isAutoStep() const { return autoStep; }

        void setCenter(const Point &p) { center = p; }

        void calculateProjectionRange(const std::vector<std::pair<Point, Point>> &fillLines)
        {
            if (!useDirectionalGradient)
                return;

            float angle_rad = angle * M_PI / 180.0f;
            float dir_x = cos(angle_rad);
            float dir_y = sin(angle_rad);

            minProjection = std::numeric_limits<float>::max();
            maxProjection = std::numeric_limits<float>::lowest();

            for (const auto &line : fillLines)
            {
                for (int x = line.first.x; x <= line.second.x; x++)
                {
                    int y = line.first.y;

                    float vec_x = x - center.x;
                    float vec_y = y - center.y;

                    float projection = vec_x * dir_x + vec_y * dir_y;

                    minProjection = std::min(minProjection, projection);
                    maxProjection = std::max(maxProjection, projection);
                }
            }

            projectionCalculated = true;
        }

        bool isDirectional() const { return useDirectionalGradient; }
    };
}
