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

int main()
{
    int width = 256, height = 256;

    GrayscaleImage image(width, height);

    float step = 255.0f / (width * height), color = 0.0f;

    Gradient gradient(0.0f, step);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image(x, y) = gradient();
        }
    }

    image.Save("grayscale.png");

    return 0;
}
