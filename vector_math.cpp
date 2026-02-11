#include "vector_math.h"
#include <cmath>

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


   // Accesso in lettura/scrittura
    double& operator[](std::size_t i) {
        if (i == 0) return x;
        else if (i == 1) return y;
        else throw std::out_of_range("Vec index must be 0 or 1");
    }

    // Accesso in sola lettura (const)
    double operator[](std::size_t i) const {
        if (i == 0) return x;
        else if (i == 1) return y;
        else throw std::out_of_range("Vec index must be 0 or 1");
    }
};

// Scalar division
Vec Vec::operator/(double s) {
    return Vec(x / s, y / s);
}

// Comparisons (based on vector norm)
bool Vec::operator>(double s) {
    return this->norm() > s;
}

bool Vec::operator>=(double s) {
    return this->norm() >= s;
}

bool Vec::operator<(double s) {
    return this->norm() < s;
}

bool Vec::operator<=(double s) {
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
double Vec::norm() {
    return std::sqrt(x*x + y*y);
}

// Norm squared
double Vec::norm2() {
    return x*x + y*y;
}

// Convert to unit vector
Vec& Vec::unit_vect() {
    double n = this->norm();
    if(n != 0.0) {
        x /= n;
        y /= n;
    }
    return *this;
}

// Convert to polar coordinates (x = r, y = theta)
Vec& Vec::get_polar() {
    double r = this->norm();
    double theta = std::atan2(y, x);
    x = r;
    y = theta;
    return *this;
}
