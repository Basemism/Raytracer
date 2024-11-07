// Vector3.h
#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>

/**
 * @brief A class representing a 3D vector or point.
 */
class Vector3 {
public:
    double x, y, z;

    // Constructors
    Vector3();
    Vector3(double x, double y, double z);

    // Vector operations
    Vector3 operator+(const Vector3& v) const;
    Vector3 operator-(const Vector3& v) const;
    Vector3 operator*(const Vector3& v) const;
    Vector3 operator/(const Vector3& v) const;
    Vector3 operator+(double scalar) const;
    Vector3 operator-(double scalar) const;
    Vector3 operator*(double scalar) const;
    Vector3 operator/(double scalar) const;


    Vector3 operator-() const;
    Vector3& operator+=(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    };
    Vector3& operator-=(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    };
    Vector3& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    };
    Vector3& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    };
    Vector3& operator*=(const Vector3& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    };
    Vector3& operator/=(const Vector3& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    };

    // Comparison
    bool operator==(const Vector3& v) const;
    bool operator!=(const Vector3& v) const;



    // Dot and cross products
    double dot(const Vector3& v) const;
    Vector3 cross(const Vector3& v) const;

    // Magnitude and normalization
    double length() const;
    Vector3 normalize() const;

    // Utility
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
};

#endif // VECTOR3_H
