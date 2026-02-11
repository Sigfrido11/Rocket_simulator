#include "vector_math.h"

Vec operator+(Vec const& a, Vec const& b)
{
    return {a[0] + b[0], a[1] + b[1]};
}

Vec operator-(Vec const& a, Vec const& b)
{
    return {a[0] - b[0], a[1] - b[1]};
}

Vec operator*(Vec const& v, double s)
{
    return {v[0] * s, v[1] * s};
}

Vec operator*(double s, Vec const& v)
{
    return v * s;
}

Vec operator/(Vec const& v, double s)
{
    return {v[0] / s, v[1] / s};
}

Vec& operator+=(Vec& a, Vec const& b)
{
    a[0] += b[0];
    a[1] += b[1];
    return a;
}