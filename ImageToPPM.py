import sys
from PIL import Image

def convert_to_ppm(image_path, output_path):
    with Image.open(image_path) as img:
        img = img.convert("RGB")
        width, height = img.size
        pixels = img.tobytes()

    with open(output_path, 'wb') as ppm_file:
        header = f"P6\n{width} {height}\n255\n"
        ppm_file.write(header.encode('ascii'))
        ppm_file.write(pixels)

input_image =  sys.argv[1]
output_ppm = sys.argv[2]
convert_to_ppm(input_image, output_ppm)


print(f"Image successfully converted to {output_ppm} in PPM P6 format.")
