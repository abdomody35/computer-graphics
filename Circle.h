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

namespace circle
{
    enum Algorithm
    {
        MIDPOINT = 0,
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
        inline void setCirclePixels(Image &image, const Point &center, int x, int y, bool fill, Color color)
        {
            Point o1 = {center.x + x, center.y + y};
            Point o2 = {center.x - x, center.y + y};
            Point o3 = {center.x + x, center.y - y};
            Point o4 = {center.x - x, center.y - y};
            Point o5 = {center.x + y, center.y + x};
            Point o6 = {center.x - y, center.y + x};
            Point o7 = {center.x + y, center.y - x};
            Point o8 = {center.x - y, center.y - x};

            setPixel(image, o1.x, o1.y, color);
            setPixel(image, o2.x, o2.y, color);
            setPixel(image, o3.x, o3.y, color);
            setPixel(image, o4.x, o4.y, color);
            setPixel(image, o5.x, o5.y, color);
            setPixel(image, o6.x, o6.y, color);
            setPixel(image, o7.x, o7.y, color);
            setPixel(image, o8.x, o8.y, color);

            if (fill)
            {
                for (int i = o2.x; i <= o1.x; i++)
                {
                    setPixel(image, i, o1.y, color);
                }
                for (int i = o4.x; i <= o3.x; i++)
                {
                    setPixel(image, i, o3.y, color);
                }
                for (int i = o6.x; i <= o5.x; i++)
                {
                    setPixel(image, i, o5.y, color);
                }
                for (int i = o8.x; i <= o7.x; i++)
                {
                    setPixel(image, i, o7.y, color);
                }
            }
        }

        template <typename Image, typename Color>
        inline void drawCircleMidPoint(Image &image, const Point &center, int radius, Color color, bool fill = false)
        {
            if (radius <= 0)
            {
                std::cerr << "Radius must be positive." << std::endl;
                return;
            }

            int x = radius;
            int y = 0;

            while (y <= x)
            {
                setCirclePixels(image, center, x, y, fill, color);

                y++;

                int d1 = std::abs(radius * radius - x * x - y * y);
                int d2 = std::abs(radius * radius - (x - 1) * (x - 1) - y * y);

                if (d1 > d2)
                {
                    x--;
                }
            }
        }

        template <typename Image, typename Color>
        inline void drawCircleBresenham(Image &image, const Point &center, int radius, Color color, bool fill = false)
        {
            if (radius <= 0)
            {
                std::cerr << "Radius must be positive." << std::endl;
                return;
            }

            int x = radius;
            int y = 0;
            int d = 3 - 2 * radius;

            while (y <= x)
            {
                setCirclePixels(image, center, x, y, fill, color);

                y++;

                if (d <= 0)
                {
                    d = d + 4 * y + 6;
                }
                else
                {
                    x--;
                    d = d + 4 * (y - x) + 10;
                }
            }
        }
    }

    inline void drawCircle(GrayscaleImage &image, const Point &center, int radius, Byte color = 255, bool fill = false, Algorithm algorithm = BRESENHAM)
    {
        switch (algorithm)
        {
        case MIDPOINT:
            __detail::drawCircleMidPoint(image, center, radius, color, fill);
            break;
        case BRESENHAM:
            __detail::drawCircleBresenham(image, center, radius, color, fill);
        default:
            break;
        }
    }

    inline void drawCircle(ColorImage &image, const Point &center, int radius, RGBA color, bool fill = false, Algorithm algorithm = BRESENHAM)
    {
        switch (algorithm)
        {
        case MIDPOINT:
            __detail::drawCircleMidPoint(image, center, radius, color, fill);
            break;
        case BRESENHAM:
            __detail::drawCircleBresenham(image, center, radius, color, fill);
        default:
            break;
        }
    }
}