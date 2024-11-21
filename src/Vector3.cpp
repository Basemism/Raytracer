// Vector3.cpp

#include <cmath>
#include "Vector3.h"

// Default constructor initializes vector to zero
Vector3::Vector3() : x(0), y(0), z(0) {}

// Initialize vector with a single value
Vector3::Vector3(double scalar) : x(scalar), y(scalar), z(scalar) {}

// Initialize vector with given components
Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

// Vector addition
Vector3 Vector3::operator+(const Vector3& v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
}

// Vector subtraction
Vector3 Vector3::operator-(const Vector3& v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

// Scalar multiplication
Vector3 Vector3::operator*(double scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator*(const Vector3& v) const {
    return Vector3(x * v.x, y * v.y, z * v.z);
}

Vector3 Vector3::operator/(const Vector3& v) const {
    return Vector3(x / v.x, y / v.y, z / v.z);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

// Scalar division
Vector3 Vector3::operator/(double scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

// Dot product
double Vector3::dot(const Vector3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

// Scalar addition
Vector3 Vector3::operator+(double scalar) const {
    return Vector3(x + scalar, y + scalar, z + scalar);
}

// Scalar subtraction
Vector3 Vector3::operator-(double scalar) const {
    return Vector3(x - scalar, y - scalar, z - scalar);
}

// Cross product
Vector3 Vector3::cross(const Vector3& v) const {
    return Vector3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}

// Compute the length (magnitude) of the vector
double Vector3::length() const {    
    long double sum = x * x + y * y + z * z;
    return sqrt(sum);
}

// Normalize the vector
Vector3 Vector3::normalize() const {
    double len = length();
    return Vector3(x / len, y / len, z / len);
}

// Output vector to stream
std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return os;
}

// index operator
double& Vector3::operator[](int index) {
    if (index == 0) return x;
    if (index == 1) return y;
    return z;
}

const double& Vector3::operator[](int index) const {
    if (index == 0) return x;
    if (index == 1) return y;
    return z;
}