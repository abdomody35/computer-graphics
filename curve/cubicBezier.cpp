#include "../Image.h"
#include "../Line.h"

struct FloatPoint
{
    float x;
    float y;

    operator Point() { return {(int)std::round(x), (int)std::round(y)}; }
};

struct CubicBezierCurve
{
    Point p0, p1, p2, p3;
};

std::pair<CubicBezierCurve, CubicBezierCurve> splitCurve(CubicBezierCurve curve, float t)
{
    auto lerp = [](const Point &p1, const Point &p2, const float t) -> FloatPoint
    { return {p1.x * t + p2.x * (1 - t), p1.y * t + p2.y * (1 - t)}; };

    FloatPoint q0 = lerp(curve.p0, curve.p1, t);
    FloatPoint q1 = lerp(curve.p1, curve.p2, t);
    FloatPoint q2 = lerp(curve.p2, curve.p3, t);

    FloatPoint r0 = lerp(q0, q1, t);
    FloatPoint r1 = lerp(q1, q2, t);

    FloatPoint s0 = lerp(r0, r1, t);

    return {{curve.p0, q0, r0, s0}, {s0, r1, q2, curve.p3}};
}

void drawCurve(GrayscaleImage &image, CubicBezierCurve curve, Byte color = 255, float t = 0.5, float err = 1.05f)
{
    auto calculateDistance = [](const Point &p1, const Point &p2) -> float
    { return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)); };

    auto isStraight = [calculateDistance](const CubicBezierCurve &curve, float err) -> bool
    { return (calculateDistance(curve.p0, curve.p1) + calculateDistance(curve.p1, curve.p2) + calculateDistance(curve.p2, curve.p3)) / calculateDistance(curve.p0, curve.p3) < err; };

    std::vector<CubicBezierCurve> curves;

    curves.push_back(curve);

    for (int i = 0; i < curves.size(); i++)
    {
        if (!isStraight(curves[i], err))
        {
            auto split = splitCurve(curves[i], t);

            curves[i] = split.first;
            curves.insert(curves.begin() + i + 1, split.second);

            i--;
        }
    }

    for (const CubicBezierCurve &curve : curves)
    {
        line::drawLine(image, curve.p0, curve.p1, color);
        line::drawLine(image, curve.p1, curve.p2, color);
        line::drawLine(image, curve.p2, curve.p3, color);
    }
}

int main()
{
    GrayscaleImage image(256, 256);

    CubicBezierCurve curve = {{64, 192}, {128, 64}, {192, 64}, {192, 192}};

    drawCurve(image, curve);

    image.Save("cubicBezierCurve.png");

    return 0;
}
