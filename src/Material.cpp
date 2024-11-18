// Material.cpp
#include "Material.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

Material::Material()
    : ks(0.0), kd(0.0), specularExponent(0.0),
      isReflective(false), reflectivity(0.0),
      isRefractive(false), refractiveIndex(1.0),
      diffuseColor(0.0, 0.0, 0.0), specularColor(0.0, 0.0, 0.0) {}



void Material::loadTexture() {
    if (!hasTexture || texturePath.empty()) {
        std::cerr << "Error: No texture path specified" << std::endl;
        return;
    }

    std::ifstream file(texturePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open texture file " << texturePath << std::endl;
        hasTexture = false;
        return;
    }

    std::string line;
    // Read PPM header
    std::getline(file, line);
    if (line != "P6") {
        std::cerr << "Error: Unsupported PPM format (must be P6)" << std::endl;
        hasTexture = false;
        return;
    }

    // Skip comments
    while (std::getline(file, line)) {
        if (line[0] != '#') break;
    }

    // Read image dimensions
    std::istringstream iss(line);
    iss >> textureWidth >> textureHeight;

    // Read maximum color value (assumed to be 255)
    int maxColorValue;
    file >> maxColorValue;
    file.get(); // Consume the newline character

    // Read pixel data
    int numPixels = textureWidth * textureHeight;
    textureData = std::make_shared<std::vector<Vector3>>(numPixels);

    for (int i = 0; i < numPixels; ++i) {
        unsigned char r, g, b;
        file.read(reinterpret_cast<char*>(&r), 1);
        file.read(reinterpret_cast<char*>(&g), 1);
        file.read(reinterpret_cast<char*>(&b), 1);

        (*textureData)[i] = Vector3(r / 255.0, g / 255.0, b / 255.0);
    }

    file.close();
}

Vector3 Material::getTextureColor(double u, double v) const {
    if (!hasTexture || textureData->empty()) {
        return diffuseColor;
    }

    // Wrap UV coordinates
    u = u - floor(u);
    v = v - floor(v);

    int x = static_cast<int>(u * textureWidth);
    int y = static_cast<int>((1.0 - v) * textureHeight); // Invert v for image coordinates

    x = std::clamp(x, 0, textureWidth - 1);
    y = std::clamp(y, 0, textureHeight - 1);

    int index = y * textureWidth + x;
    return (*textureData)[index];
}