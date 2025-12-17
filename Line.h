#pragma once

#include "Image.h"

#ifndef POINT
#define POINT

struct Point
{
    int x;
    int y;
};

#endif

namespace line
{
    enum Algorithm
    {
        DDA = 0,
        MIDPOINT,
        BRESENHAM,
    };

    namespace __detail
    {
        template <typename Image, typename Color>
        void setPixel(Image &image, int x, int y, Color color)
        {
            int width = image.GetWidth();
            int height = image.GetHeight();

            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                image(x, y) = color;
            }
        }

        template <typename Image, typename Color>
        inline void drawLineDDA(Image &image, Point p1, Point p2, Color color)
        {
            float m = std::fabs((float)(p2.y - p1.y) / (float)(p2.x - p1.x));

            if (m < 1)
            {
                if (p1.x > p2.x)
                {
                    std::swap(p1, p2);
                }

                float y = p1.y;

                for (int x = p1.x; x <= p2.x; x++)
                {
                    setPixel(image, x, std::round(y), color);

                    if (p1.y < p2.y)
                    {
                        y += m;
                    }
                    else
                    {
                        y -= m;
                    }
                }
            }
            else
            {
                if (p1.y > p2.y)
                {
                    std::swap(p1, p2);
                }

                float m_inv = 1 / m;

                float x = p1.x;

                for (int y = p1.y; y <= p2.y; y++)
                {
                    setPixel(image, std::round(x), y, color);

                    if (p1.x < p2.x)
                    {
                        x += m_inv;
                    }
                    else
                    {
                        x -= m_inv;
                    }
                }
            }
        }

        inline float computeDecisionParameter(int x, int y, float a, float b, float c)
        {
            return std::fabs(a * x + b * y + c);
        }

        template <typename Image, typename Color>
        inline void drawLineMidPoint(Image &image, Point p1, Point p2, Color color)
        {
            float m = (float)(p2.y - p1.y) / (float)(p2.x - p1.x);
            float a = p1.y - p2.y;
            float b = p2.x - p1.x;
            float c = p1.x * p2.y - p2.x * p1.y;

            if (m >= 0 && m <= 1)
            {
                if (p1.x > p2.x)
                {
                    std::swap(p1, p2);
                }

                float y = p1.y;

                for (int x = p1.x; x <= p2.x; x++)
                {
                    setPixel(image, x, std::round(y), color);

                    float d1 = computeDecisionParameter(x + 1, y, a, b, c);
                    float d2 = computeDecisionParameter(x + 1, y + 1, a, b, c);

                    if (d1 > d2)
                    {
                        y += 1;
                    }
                }
            }
            else if (m > 1)
            {
                if (p1.y > p2.y)
                {
                    std::swap(p1, p2);
                }

                float x = p1.x;

                for (int y = p1.y; y <= p2.y; y++)
                {
                    setPixel(image, std::round(x), y, color);

                    float d1 = computeDecisionParameter(x, y + 1, a, b, c);
                    float d2 = computeDecisionParameter(x + 1, y + 1, a, b, c);

                    if (d1 > d2)
                    {
                        x += 1;
                    }
                }
            }
            else if (m >= -1 && m < 0)
            {
                if (p1.x > p2.x)
                {
                    std::swap(p1, p2);
                }

                float y = p1.y;

                for (int x = p1.x; x <= p2.x; x++)
                {
                    setPixel(image, x, std::round(y), color);

                    float d1 = computeDecisionParameter(x + 1, y, a, b, c);
                    float d2 = computeDecisionParameter(x + 1, y - 1, a, b, c);

                    if (d1 > d2)
                    {
                        y -= 1;
                    }
                }
            }
            else // m < -1
            {
                if (p1.y > p2.y)
                {
                    std::swap(p1, p2);
                }

                float x = p1.x;

                for (int y = p1.y; y <= p2.y; y++)
                {
                    setPixel(image, std::round(x), y, color);

                    float d1 = computeDecisionParameter(x, y - 1, a, b, c);
                    float d2 = computeDecisionParameter(x - 1, y - 1, a, b, c);

                    if (d1 > d2)
                    {
                        x -= 1;
                    }
                }
            }
        }

        template <typename Image, typename Color>
        inline void drawLineBresenham(Image &image, Point p1, Point p2, Color color)
        {
            int delta_x = std::abs(p2.x - p1.x);
            int delta_y = std::abs(p2.y - p1.y);

            int step_x = (p1.x < p2.x) ? 1 : -1;
            int step_y = (p1.y < p2.y) ? 1 : -1;

            int err = 2 * delta_y - delta_x;

            if (delta_x >= delta_y)
            {
                for (;;)
                {
                    setPixel(image, p1.x, p1.y, color);

                    if (p1.x == p2.x)
                        break;

                    if (err >= 0)
                    {
                        p1.y += step_y;
                        err -= 2 * delta_x;
                    }

                    p1.x += step_x;
                    err += 2 * delta_y;
                }
            }
            else
            {
                err = 2 * delta_x - delta_y;

                for (;;)
                {
                    setPixel(image, p1.x, p1.y, color);

                    if (p1.y == p2.y)
                        break;

                    if (err >= 0)
                    {
                        p1.x += step_x;
                        err -= 2 * delta_y;
                    }

                    p1.y += step_y;
                    err += 2 * delta_x;
                }
            }
        }
    }

    inline void drawLine(GrayscaleImage &image, Point p1, Point p2, Byte color = 255, Algorithm algorithm = BRESENHAM)
    {
        switch (algorithm)
        {
        case DDA:
            __detail::drawLineDDA(image, p1, p2, color);
            break;
        case MIDPOINT:
            __detail::drawLineMidPoint(image, p1, p2, color);
            break;
        case BRESENHAM:
            __detail::drawLineBresenham(image, p1, p2, color);
        default:
            break;
        }
    }

    inline void drawLine(ColorImage &image, Point p1, Point p2, RGBA color = RGBA(255, 255, 255), Algorithm algorithm = BRESENHAM)
    {
        switch (algorithm)
        {
        case DDA:
            __detail::drawLineDDA(image, p1, p2, color);
            break;
        case MIDPOINT:
            __detail::drawLineMidPoint(image, p1, p2, color);
            break;
        case BRESENHAM:
            __detail::drawLineBresenham(image, p1, p2, color);
        default:
            break;
        }
    }

}