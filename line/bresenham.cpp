#include "../Image.h"

struct Point
{
    int x;
    int y;
};

void drawLineBresenham(GrayscaleImage &image, Point p1, Point p2, Byte color = 255)
{
    int delta_x = std::abs(p2.x - p1.x);
    int delta_y = std::abs(p2.y - p1.y);

    int step_x = (p1.x < p2.x) ? 1 : -1;
    int step_y = (p1.y < p2.y) ? 1 : -1;

    int err = 2 * delta_y - delta_x;
    
    if(delta_x >= delta_y)
    {
        for(;;)
        {
            image(p1.x, p1.y) = color;

            if(p1.x == p2.x)
                break;

            if(err >= 0)
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

        for(;;)
        {
            image(p1.x, p1.y) = color;

            if(p1.y == p2.y)
                break;

            if(err >= 0)
            {
                p1.x += step_x;
                err -= 2 * delta_y;
            }

            p1.y += step_y;
            err += 2 * delta_x;
        }
    }
}

int main()
{
    GrayscaleImage image(255, 255);

    Point p1 = {50, 100}, p2 = {20, 20};

    drawLineBresenham(image, p1, p2);

    image.Save("Bresenham.png");

    return 0;
}