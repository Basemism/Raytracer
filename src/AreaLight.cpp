#include "AreaLight.h"

Vector3 AreaLight::sample(const Vector3& point, Vector3& lightDir, double& distance, double& pdf) const {
    // Generate random point on the area light
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(-0.5, 0.5);
    double u = dist(rng) * width;
    double v = dist(rng) * height;
    Vector3 samplePoint = position + uVec * u + vVec * v;

    lightDir = (samplePoint - point);
    distance = lightDir.length();
    lightDir = lightDir / distance;

    // Compute the PDF (area sampling)
    double area = width * height;
    double cosine = std::max(0.0, normal.dot(-lightDir));
    pdf = (distance * distance) / (area * cosine);

    return intensity;
}