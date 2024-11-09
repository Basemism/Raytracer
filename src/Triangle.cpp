// Triangle.cpp
#include "Triangle.h"

// Initialize triangle with vertices and material
Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Material& material)
    : Intersectable(material), v0(v0), v1(v1), v2(v2) {
            // std::cout << "Initialized Triangle with Material:\n";
            // std::cout << "ks: " << material.ks << "\n";
            // std::cout << "kd: " << material.kd << "\n";
            // std::cout << "specularExponent: " << material.specularExponent << "\n";
            // std::cout << "diffuseColor: " << material.diffuseColor << "\n";
            // std::cout << "specularColor: " << material.specularColor << "\n";
            // std::cout << std::boolalpha;
            // std::cout << "isReflective: " << material.isReflective << "\n";
            // std::cout << "reflectivity: " << material.reflectivity << "\n";
            // std::cout << "isRefractive: " << material.isRefractive << "\n";
            // std::cout << "refractiveIndex: " << material.refractiveIndex << "\n";
            }

// Ray-triangle intersection using Möller–Trumbore algorithm
bool Triangle::intersect(const Ray& ray, HitRecord& hitRecord) const {
    const double EPSILON = 1e-8;
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 h = ray.direction.cross(edge2);
    double a = edge1.dot(h);

    if (std::abs(a) < EPSILON)
        return false;

    double f = 1.0 / a;
    Vector3 s = ray.origin - v0;
    double u = f * s.dot(h);

    if (u < 0.0 || u > 1.0)
        return false;

    Vector3 q = s.cross(edge1);
    double v = f * ray.direction.dot(q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = f * edge2.dot(q);

    if (t > EPSILON) {
        hitRecord.t = t;
        hitRecord.point = ray.at(t);
        hitRecord.normal = edge1.cross(edge2).normalize();
        hitRecord.material = material;
        
        return true;
    } else {
        return false;
    }
}

