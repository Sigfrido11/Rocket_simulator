#include "vector_math.h"
#include <cmath>
#include <stdexcept> 

// Constructors
Vec::Vec() : x(0.0), y(0.0) {}
Vec::Vec(double x_, double y_) : x(x_), y(y_) {}

/* ------------------------- Algebraic operations ------------------------- */

// Vector addition
Vec Vec::operator+(Vec const& other) {
    return Vec(x + other.x, y + other.y);
}

// Vector subtraction
Vec Vec::operator-(Vec const& other) {
    return Vec(x - other.x, y - other.y);
}

// Scalar multiplication
Vec Vec::operator*(double s) {
    return Vec(x * s, y * s);
}

Vec Vec::operator*(int s) {
    return Vec(x * s, y * s);
}


   // Read7write access
    double& Vec::operator[](std::size_t i) {
        if (i == 0) return x;
        else if (i == 1) return y;
        else throw std::out_of_range("Vec index must be 0 or 1");
    }

    // only read access
    double Vec::operator[](std::size_t i) const {
        if (i == 0) return x;
        else if (i == 1) return y;
        else throw std::runtime_error("Vec index must be 0 or 1");
    };

bool Vec::operator!=( Vec const& other) const {
    return (x != other.x || y != other.y);
}

bool Vec::operator==( Vec const& other) const {
    return (x == other.x && y == other.y);
}


// Scalar division
Vec Vec::operator/(double s) {
    return Vec(x / s, y / s);
}

// Comparisons (based on vector norm)
bool Vec::operator>(double s) const {
    return this->norm() > s;
}

bool Vec::operator>=(double s) const {
    return this->norm() >= s;
}

bool Vec::operator<(double s) const {
    return this->norm() < s;
}

bool Vec::operator<=(double s) const {
    return this->norm() <= s;
}

// Compound assignment
Vec& Vec::operator+=(Vec const& other) {
    x += other.x;
    y += other.y;
    return *this;
}

/* ------------------------- Useful relations ------------------------- */

// Norm (magnitude)
double Vec::norm()const {
    return std::sqrt(x*x + y*y);
}

// Norm squared
double Vec::norm2()const {
    return x*x + y*y;
}


