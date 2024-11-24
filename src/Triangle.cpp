// Triangle.cpp
#include "Triangle.h"

// Initialize triangle with vertices and material
Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Material& material)
    : v0(v0), v1(v1), v2(v2), material(material) {
            normal = (v1 - v0).cross(v2 - v0).normalize();

            // Ensure that the normal points towards the camera
            if (normal.dot(v0) > 0) {
                normal = -normal;
            }
}

void Triangle::getUV(const Vector3& point, double& u, double& v) const {
    // Compute vectors
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 pVec = point - v0;

    // Compute dot products
    double d00 = edge1.dot(edge1);
    double d01 = edge1.dot(edge2);
    double d11 = edge2.dot(edge2);
    double d20 = pVec.dot(edge1);
    double d21 = pVec.dot(edge2);

    // Compute denominators
    double denom = d00 * d11 - d01 * d01;

    // Compute barycentric coordinates
    double v_coord = (d11 * d20 - d01 * d21) / denom;
    double w_coord = (d00 * d21 - d01 * d20) / denom;
    double u_coord = 1.0 - v_coord - w_coord;

    // Adjust UV coordinates to map to the diagonal of the image
    u = (u_coord + v_coord) / 2.0;
    v = (v_coord + w_coord) / 2.0;
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
        hitRecord.normal = normal;
        hitRecord.material = material;
        hitRecord.getUV = [this](const Vector3& point, double& u, double& v) {
            getUV(point, u, v);
        };
        
        return true;
    } else {
        return false;
    }
}

BoundingBox Triangle::getBoundingBox() const {
    Vector3 minVec(
        std::min({v0.x, v1.x, v2.x}),
        std::min({v0.y, v1.y, v2.y}),
        std::min({v0.z, v1.z, v2.z})
    );
    Vector3 maxVec(
        std::max({v0.x, v1.x, v2.x}),
        std::max({v0.y, v1.y, v2.y}),
        std::max({v0.z, v1.z, v2.z})
    );
    return BoundingBox(minVec, maxVec);
}
