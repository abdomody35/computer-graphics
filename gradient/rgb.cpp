#include "../Image.h"
#include <optional>

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

    void setStep(float newStep)
    {
        step = newStep;
    }

    void setValue(float newvalue)
    {
        value = std::clamp(newvalue, 0.0f, 255.0f);
    }
};

class RGBGradient
{
private:
    Gradient r, g, b;

public:
    RGBGradient(float red_start, float red_step, std::optional<float> red_end,
                float green_start, float green_step, std::optional<float> green_end,
                float blue_start, float blue_step, std::optional<float> blue_end)
        : r(red_start, red_step, red_end), g(green_start, green_step, green_end), b(blue_start, blue_step, blue_end) {}

    RGBGradient(RGBA color, float red_step, float green_step, float blue_step)
        : RGBGradient(color.r, red_step, std::nullopt, color.g, green_step, std::nullopt, color.b, blue_step, std::nullopt) {}

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

    void setColor(float red_val, float green_val, float blue_val)
    {
        r.setValue(red_val);
        g.setValue(green_val);
        b.setValue(blue_val);
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

int main()
{
    int width = 256, height = 256;

    ColorImage image(width, height);

    float step = 255.0f / (width * height), color = 0.0f;

    RGBGradient rgbGradient(RGBA(0, 0, 128), step, step, step / 2); // R: 0→255, G: 0→255, B: 128 static

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image(x, y) = rgbGradient();
        }
    }

    image.Save("rgb.png");

    return 0;
}
