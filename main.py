#!/usr/bin/python3

import sys
import PIL.Image
import PIL.ExifTags
import numpy

from gisutils import *
from heightmap import *

def from_rational(val):
    return float(val[0]) / float(val[1])

def read_coordinate(val):
    d, m, s = val
    d, m, s = from_rational(d), from_rational(m), from_rational(s)
    return d + m / 60 + s / 3600

def load_image(filename):
    img = PIL.Image.open(filename)

    # Index metadata by field name, ignoring the MakerNote (=37500)
    metadata = {}
    for key, value in img._getexif().items():
        if key in PIL.ExifTags.TAGS and key != 37500:
            metadata[PIL.ExifTags.TAGS[key]] = value

    for key, value in metadata["GPSInfo"].items():
        if key in PIL.ExifTags.GPSTAGS:
            metadata[PIL.ExifTags.GPSTAGS[key]] = value
            
    return metadata, img

def write_image(filename, img):
    data = numpy.asarray(img)
    with open(filename, 'wb') as f:
        f.write(data.tobytes())

def write_attributes(filename, image_size):
    with open(filename, 'w') as f:
        f.write("image_size = ({},{})".format(image_size[0], image_size[1]))
        
# Check Arguments
if len(sys.argv) <= 1:
    print("Usage: python3 imagewire /path/to/image.jpg")
    exit()

# TODO: support south/east references
metadata, image = load_image(sys.argv[1])
latitude = read_coordinate(metadata["GPSLatitude"])
longitude = read_coordinate(metadata["GPSLongitude"])
north = int(latitude) - 1
west = int(longitude) - 1 + 1

name = "n{0}_{1}w{2}_{3}".format(north, north+3, west, west+3)
datasets = [[get_dataset(north+x,west+y) for x in range(3)] for y in range(3)]
dataset = combine_datasets(datasets, name)

data, world_size = dataset_to_array(dataset, 512)
write_heightmap("intermediate/heightmap.obj", data, world_size)
write_image("intermediate/image.raw", image)
write_attributes("intermediate/attributes.ini", image.size)
