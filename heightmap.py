#!/usr/bin/python3

import numpy as np
import math

def write_heightmap(filename, heights, world_size):
    width = heights.shape[0]
    height = heights.shape[1]

    hmin = 9999999
    hmax = -9999999
    
    f = open(filename, "w")
    for z in range(height):
        for x in range(width):
            pos_x = (x-width/2) / width * world_size[0] * 0.001
            pos_z = (z-height/2) / height *world_size[1] * 0.001
            pos_y = heights[x,z] * 0.001

            hmin = min(heights[x,z], hmin)
            hmax = max(heights[x,z], hmax)
            
            f.write("v {0} {1} {2}\n".format(pos_x, pos_y, pos_z))

    print("Min, max:")
    print(hmin)
    print(hmax)
            
    f.write("\n")
    
    for z in range(height - 1):
        for x in range(width - 1):
            p = z*height + x + 1
            f.write("f {0} {1} {2}\n".format(p, p + 1, p + height))
            f.write("f {0} {1} {2}\n".format(p + 1, p + height + 1, p + height))
                    
    f.close()
