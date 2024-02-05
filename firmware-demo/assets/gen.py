#!/usr/bin/env python3
# Load image from file

import sys

from PIL import Image

fname = sys.argv[1]
img = Image.open(fname)

out = fname.rpartition('.')[0] + '.h'
var_name = fname.rpartition('.')[0].rpartition('/')[2]

colors = []

# Iterate through each pixel
for y in range(img.height):
    for x in range(img.width):
        # Get RGB values
        r, g, b, *_ = img.getpixel((x, y))

        # Convert to RGB-565
        r = r >> 3
        g = g >> 2
        b = b >> 3

        colors.append((r << 11) | (g << 5) | b)

        # Print RGB values
        # print(r, g, b)

with open(out, 'w') as f:
    print(f'const uint16_t {var_name}[] = {{', file=f)
    for color in colors:
        print(f'    0x{color:04x},', file=f)
    print('};', file=f)