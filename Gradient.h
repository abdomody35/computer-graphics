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
        bool autoStep;

    public:
        Gradient(float start, float step, std::optional<float> end = std::nullopt)
            : value(start), step(step), startValue(start), endValue(end), autoStep(false) {}

        Gradient(float start, std::nullopt_t, std::optional<float> end)
            : value(start), step(0.0f), startValue(start), endValue(end), autoStep(true) {}

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
                value = std::clamp(value, startValue, 255.0f);
            }

            return static_cast<int>(std::round(current));
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
    };

    class RGBGradient
    {
    private:
        Gradient r, g, b;
        bool autoStep;

    public:
        RGBGradient(float r_start, float r_step, std::optional<float> r_end,
                    float g_start, float g_step, std::optional<float> g_end,
                    float b_start, float b_step, std::optional<float> b_end)
            : r(r_start, r_step, r_end), g(g_start, g_step, g_end), b(b_start, b_step, b_end), autoStep(false) {}

        RGBGradient(RGBA startColor, RGBA endColor, float r_step, float g_step, float b_step)
            : r(startColor.r, r_step, endColor.r), g(startColor.g, g_step, endColor.g), b(startColor.b, b_step, endColor.b), autoStep(false) {}

        RGBGradient(RGBA startColor, RGBA endColor, float step)
            : r(startColor.r, step, endColor.r), g(startColor.g, step, endColor.g), b(startColor.b, step, endColor.b), autoStep(false) {}

        RGBGradient(RGBA color, float r_step, float g_step, float b_step)
            : RGBGradient(color.r, r_step, std::nullopt, color.g, g_step, std::nullopt, color.b, b_step, std::nullopt) {}

        RGBGradient(RGBA color, float step)
            : RGBGradient(color.r, step, std::nullopt, color.g, step, std::nullopt, color.b, step, std::nullopt) {}

        RGBGradient(RGBA startColor, RGBA endColor)
            : r(startColor.r, std::nullopt, endColor.r), g(startColor.g, std::nullopt, endColor.g), b(startColor.b, std::nullopt, endColor.b), autoStep(true) {}

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

        void calculateSteps(int totalPoints)
        {
            if (autoStep && totalPoints > 0)
            {
                r.calculateStep(totalPoints);
                g.calculateStep(totalPoints);
                b.calculateStep(totalPoints);
            }
        }

        bool isAutoStep() const { return autoStep; }
    };
}
