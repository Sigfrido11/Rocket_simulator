#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include <array>
#include <cmath>
#include <utility> // for std::pair

/**
 * Class representing a 2D vector.
 *
 * This class provides basic vector operations such as
 * addition, subtraction, scalar multiplication/division,
 * norm computation, normalization, and conversion to polar coordinates.
 */
// 2D vector alias

class Vec{
    public:
    double x; ///< x-component of the vector
    double y; ///< y-component of the vector
    
    //constructor
    Vec();
    Vec(double x, double y);

    /* -------------------------------------------------------------------------- */
/*                         algebraic operation                         */
/* -------------------------------------------------------------------------- */
// Vector addition
Vec operator+(Vec const& other);

// Vector subtraction
Vec operator-(Vec const& other);

// Scalar multiplication
Vec operator*(double s);

Vec operator*(int s);

// Scalar division
Vec operator/(double s);

double& operator[](std::size_t i);

double operator[](std::size_t i) const;

//Bigger than comparison
bool operator>(double s);

bool operator>=( double s);

//Less than comparison 
bool operator<(double s);

bool operator<=(double s);

bool operator!=(Vec const& other);

// Compound assignment
Vec& operator+=(Vec const& other);



    /* -------------------------------------------------------------------------- */
/*                         Usefull relation                         */
/* -------------------------------------------------------------------------- */

double norm();

double norm2();

Vec& unit_vect();

Vec& get_polar();

};



#endif