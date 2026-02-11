#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <array>

// 2D vector alias
using Vec = std::array<double, 2>;

// Vector addition
Vec operator+(Vec const& a, Vec const& b);

// Vector subtraction
Vec operator-(Vec const& a, Vec const& b);

// Scalar multiplication
Vec operator*(Vec const& v, double s);
Vec operator*(double s, Vec const& v);

// Scalar division
Vec operator/(Vec const& v, double s);

// Compound assignment
Vec& operator+=(Vec& a, Vec const& b);

#endif