import random
import json

def create_rtiow_scene():
    scene = {
        "nbounces": 8,
        "rendermode": "phong",
        "bvh": False,
        "camera": {
            "type": "pinhole",
            "width": 1200,
            "height": 800,
            "position": [13.0, 0.5, 3.0],
            "lookAt": [0.0, -0.5, 0.0],
            "upVector": [0.0, 1.0, 0.0],
            "fov": 40.0,
            "exposure": 1,
            "focalLength": 10.0,
            "aperture": 0.0
        },
        "scene": {
            "backgroundcolor": [0.53, 0.80, 0.92],
            "lightsources": [
                # area light that mimics the sun
                {
                    "type": "pointlight",
                    "intensity": [1, 1, 1],
                    "position": [0, 100, 0],
                    "u": [900, 0, 0],
                    "v": [0, 0, 900],
                }
            ],
            "shapes": []
        }
    }

    shapes = scene["scene"]["shapes"]

    # Ground plane
    shapes.append({
        "type": "triangle",
        "v0": [-1000, -0.5, -1000],
        "v1": [1000, -0.5, -1000],
        "v2": [-1000, -0.5, 1000],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.25, 0.6, 0.25],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    shapes.append({
        "type": "triangle",
        "v0": [1000, -0.5, -1000],
        "v1": [1000, -0.5, 1000],
        "v2": [-1000, -0.5, 1000],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.25, 0.6, 0.25],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Add small spheres in a grid
    count = 0
    for a in range(-8, 8):
        for b in range(-8, 8):
            count += 1
            choose_mat = random.random()
            center = [a + 0.9 * random.random(), -0.3, b + 0.9 * random.random()]
            if ((center[0] - 4) ** 2 + (center[2] - 0) ** 2) > 0.9 ** 2:
                if choose_mat < 0.9:
                    # Diffuse
                    albedo = [
                        random.random() * random.random(),
                        random.random() * random.random(),
                        random.random() * random.random()
                    ]
                    shapes.append({
                        "type": "sphere",
                        "center": center,
                        "radius": 0.2,
                        "material": {
                            "ks": 0.0,
                            "kd": 1.0,
                            "specularexponent": 0,
                            "diffusecolor": albedo,
                            "specularcolor": [0.0, 0.0, 0.0],
                            "isreflective": False,
                            "reflectivity": 0.0,
                            "isrefractive": False,
                            "refractiveindex": 1.0
                        }
                    })
                elif choose_mat < 0.95:
                    # Metal
                    albedo = [
                        0.5 * (1 + random.random()),
                        0.5 * (1 + random.random()),
                        0.5 * (1 + random.random())
                    ]
                    fuzz = 0.5 * random.random()
                    shapes.append({
                        "type": "sphere",
                        "center": center,
                        "radius": 0.2,
                        "material": {
                            "ks": 1.0 - fuzz,
                            "kd": fuzz,
                            "specularexponent": 100 * (1 - fuzz),
                            "diffusecolor": [0.0, 0.0, 0.0],
                            "specularcolor": albedo,
                            "isreflective": True,
                            "reflectivity": 1.0 - fuzz,
                            "isrefractive": False,
                            "refractiveindex": 1.0
                        }
                    })
                else:
                    # Glass
                    shapes.append({
                        "type": "sphere",
                        "center": center,
                        "radius": 0.2,
                        "material": {
                            "ks": 0.7,
                            "kd": 0.2,
                            "specularexponent": 20,
                            "diffusecolor": [1, 1, 1],
                            "specularcolor": [1.0, 1.0, 1.0],
                            "isreflective": True,
                            "reflectivity": 0.2,
                            "isrefractive": True,
                            "refractiveindex": 1.5
                        }
                    })

    print(f"Added {count} spheres to the scene.")
    return scene

def create_cornell_box_scene():
    scene = {
        "nbounces": 8,
        "rendermode": "pathtracing",
        "useGI": True,
        "usecaustics": True,
        "camera": {
            "type": "pinhole",
            "width": 1200,
            "height": 800,
            "position": [0.0, 1.0, 3.5],  # Camera position remains unchanged
            "lookAt": [0.0, 1.0, 0.0],
            "upVector": [0.0, 1.0, 0.0],
            "fov": 40.0,
            "exposure": 0.1,  # Adjust exposure for brightness if needed
            "focalLength": 10.0,
            "aperture": 0.0
        },
        "scene": {
            "backgroundcolor": [0.0, 0.0, 0.0],
            "lightsources": [],
            "shapes": []
        }
    }

    # Shift amount for the entire scene
    y_shift = 1.0  # Move all objects up by 1.0 unit

    shapes = scene["scene"]["shapes"]
    lights = scene["scene"]["lightsources"]

    # Area Light (Ceiling Light)
    lights.append({
        "type": "arealight",
        "intensity": [1, 1, 1],
        "position": [-0.25, 1 + y_shift - 0.2, -0.25],  # Shifted up
        "u": [0.5, 0, 0],
        "v": [0, 0, 0.5],
        "samples": 8
    })

    # Light square (Ceiling Light) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [-0.25, 1 + y_shift - 0.01, -0.25],
        "v1": [0.25, 1 + y_shift - 0.01, -0.25],
        "v2": [-0.25, 1 + y_shift - 0.01, 0.25],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [1.0, 1.0, 1.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0,
            "emittance": [10, 10, 10]
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [-0.25, 1 + y_shift - 0.01, 0.25],
        "v1": [0.25, 1 + y_shift - 0.01, -0.25],
        "v2": [0.25, 1 + y_shift - 0.01, 0.25],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [1.0, 1.0, 1.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0,
            "emittance": [10, 10, 10]
        }
    })

    # Ceiling (White) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [-1, 1 + y_shift, -1],
        "v1": [1, 1 + y_shift, -1],
        "v2": [-1, 1 + y_shift, 1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [-1, 1 + y_shift, 1],
        "v1": [1, 1 + y_shift, -1],
        "v2": [1, 1 + y_shift, 1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Left Wall (Green) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [-1, -1 + y_shift, 1],
        "v1": [-1, 1 + y_shift, 1],
        "v2": [-1, -1 + y_shift, -1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.0, 0.8, 0.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [-1, 1 + y_shift, 1],
        "v1": [-1, 1 + y_shift, -1],
        "v2": [-1, -1 + y_shift, -1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.0, 0.8, 0.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Right Wall (Red) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [1, -1 + y_shift, -1],
        "v1": [1, 1 + y_shift, -1],
        "v2": [1, -1 + y_shift, 1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.0, 0.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [1, 1 + y_shift, -1],
        "v1": [1, 1 + y_shift, 1],
        "v2": [1, -1 + y_shift, 1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.0, 0.0],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Back Wall (White) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [-1, -1 + y_shift, -1],
        "v1": [1, -1 + y_shift, -1],
        "v2": [-1, 1 + y_shift, -1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [-1, 1 + y_shift, -1],
        "v1": [1, -1 + y_shift, -1],
        "v2": [1, 1 + y_shift, -1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Floor (White) - Two Triangles
    shapes.append({
        "type": "triangle",
        "v0": [-1, -1 + y_shift, -1],
        "v1": [-1, -1 + y_shift, 1],
        "v2": [1, -1 + y_shift, -1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })
    shapes.append({
        "type": "triangle",
        "v0": [1, -1 + y_shift, -1],
        "v1": [-1, -1 + y_shift, 1],
        "v2": [1, -1 + y_shift, 1],
        "material": {
            "ks": 0.0,
            "kd": 1.0,
            "specularexponent": 0,
            "diffusecolor": [0.8, 0.8, 0.8],
            "specularcolor": [0.0, 0.0, 0.0],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # # Front Wall (White) - Two Triangles
    # shapes.append({
    #     "type": "triangle",
    #     "v0": [-1, -1 + y_shift, 1],
    #     "v1": [-1, 1 + y_shift, 1],
    #     "v2": [1, -1 + y_shift, 1],
    #     "material": {
    #         "ks": 0.0,
    #         "kd": 1.0,
    #         "specularexponent": 0,
    #         "diffusecolor": [0.8, 0.8, 0.8],
    #         "specularcolor": [0.0, 0.0, 0.0],
    #         "isreflective": False,
    #         "reflectivity": 0.0,
    #         "isrefractive": False,
    #         "refractiveindex": 1.0
    #     }
    # })
    # shapes.append({
    #     "type": "triangle",
    #     "v0": [-1, 1 + y_shift, 1],
    #     "v1": [1, -1 + y_shift, 1],
    #     "v2": [1, 1 + y_shift, 1],
    #     "material": {
    #         "ks": 0.0,
    #         "kd": 1.0,
    #         "specularexponent": 0,
    #         "diffusecolor": [0.8, 0.8, 0.8],
    #         "specularcolor": [0.0, 0.0, 0.0],
    #         "isreflective": False,
    #         "reflectivity": 0.0,
    #         "isrefractive": False,
    #         "refractiveindex": 1.0
    #     }
    # })

    # Cylinder
    shapes.append({
        "type": "cylinder",
        "center": [-0.3, -1 + y_shift, -0.3],
        "radius": 0.3,
        "height": 1.0,
        "axis": [0, 1, 0],
        "material": {
            "ks": 0.1,
            "kd": 0.9,
            "specularexponent": 30,
            # grey cylinder
            "diffusecolor": [0.7, 0.7, 0.7],
            "specularcolor": [0.1, 0.1, 0.1],
            "isreflective": False,
            "reflectivity": 0.0,
            "isrefractive": False,
            "refractiveindex": 1.0
        }
    })

    # Glass Sphere
    shapes.append({
        "type": "sphere",
        # center y at -0.3 + y_shift for cool caustic effect
        "center": [0.4, -0.3 + y_shift, 0.3],
        "radius": 0.3,
        "material": {
            "ks": 0.1,
            "kd": 0.9,
            "specularexponent": 20,
            "diffusecolor": [0.0, 0.0, 0.0],
            "specularcolor": [1.0, 1.0, 1.0],
            "isreflective": False,
            "reflectivity": 1,
            "isrefractive": True,
            "refractiveindex": 1.5
        }
    })

    return scene



def main():
    scene = create_rtiow_scene()
    with open('./scenes/clutter_250.json', 'w') as f:
        json.dump(scene, f, indent=4)

if __name__ == '__main__':
    main()