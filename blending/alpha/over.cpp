#include "../../Image.h"
#include <optional>

std::optional<ColorImage> blendImages(const ColorImage &original, const ColorImage &secondary)
{
    int height = original.GetHeight();
    int width = original.GetWidth();

    if (height != secondary.GetHeight() || width != secondary.GetWidth())
    {
        std::cerr << "Both images must have the same width and height" << std::endl;
        return std::nullopt;
    }

    ColorImage image(width, height);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float af = secondary(x, y).a / 255.0f;
            float ab = original(x, y).a / 255.0f;

            float a = af + ab * (1 - af);

            if (a > 0.0f)
            {
                float r = (original(x, y).r * ab * (1 - af) + secondary(x, y).r * af) / a;
                float g = (original(x, y).g * ab * (1 - af) + secondary(x, y).g * af) / a;
                float b = (original(x, y).b * ab * (1 - af) + secondary(x, y).b * af) / a;

                image(x, y) = {(Byte)car(r, 255), (Byte)car(g, 255), (Byte)car(b, 255), (Byte)car(a * 255, 255)};
            }
            else
            {
                image(x, y) = {0, 0, 0, 0};
            }
        }
    }

    return std::optional<ColorImage>(std::move(image));
}

int main()
{
    ColorImage original, secondary;

    original.Load("original.png");

    secondary.Load("secondary.png");

    auto blended_result = blendImages(original, secondary);

    if (!blended_result.has_value())
    {
        std::cerr << "Failed to blend images" << std::endl;
        return 1;
    }

    ColorImage blended = std::move(blended_result.value());

    blended.Save("blended.png");

    return 0;
}