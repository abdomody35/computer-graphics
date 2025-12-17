#pragma once

#include "Image.h"
#include <optional>

namespace gradient
{
    class Gradient
    {
    private:
        float value;
        float step;
        float startValue;
        std::optional<float> endValue;

    public:
        Gradient(float start, float step, std::optional<float> end = std::nullopt) : value(start), step(step), startValue(start), endValue(end) {}

        void reset() { value = startValue; }

        float getCurrent() const { return value; }

        bool isFinished() const
        {
            return endValue.has_value() ? (step >= 0 ? value >= endValue.value() : value <= endValue.value()) : false;
        }

        int operator()()
        {
            float current = value;
            value += step;

            if (endValue.has_value())
            {
                value = std::clamp(value, std::min(startValue, endValue.value()), std::max(startValue, endValue.value()));
            }
            else
            {
                value = std::clamp(value, 0.0f, 255.0f);
            }

            return static_cast<int>(std::round(current));
        }

        void setStep(float newStep) { step = newStep; }

        void setValue(float newvalue) { value = std::clamp(newvalue, 0.0f, 255.0f); }
    };

    class RGBGradient
    {
    private:
        Gradient r, g, b;

    public:
        RGBGradient(float r_start, float r_step, std::optional<float> r_end,
                    float g_start, float g_step, std::optional<float> g_end,
                    float b_start, float b_step, std::optional<float> b_end)
            : r(r_start, r_step, r_end), g(g_start, g_step, g_end), b(b_start, b_step, b_end) {}

        RGBGradient(RGBA stratColor, RGBA endColor, float r_step, float g_step, float b_step)
            : r(stratColor.r, r_step, endColor.r), g(stratColor.g, g_step, endColor.g), b(stratColor.b, b_step, endColor.b) {}

        RGBGradient(RGBA stratColor, RGBA endColor, float step)
            : r(stratColor.r, step, endColor.r), g(stratColor.g, step, endColor.g), b(stratColor.b, step, endColor.b) {}

        RGBGradient(RGBA color, float r_step, float g_step, float b_step)
            : RGBGradient(color.r, r_step, std::nullopt, color.g, g_step, std::nullopt, color.b, b_step, std::nullopt) {}

        RGBGradient(RGBA color, float step)
            : RGBGradient(color.r, step, std::nullopt, color.g, step, std::nullopt, color.b, step, std::nullopt) {}

        RGBA operator()()
        {
            return RGBA(r(), g(), b());
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
    };
}