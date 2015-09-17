#!/usr/bin/env python

import numpy as np
import math
import cv2

def get_gaussian_kernel(sigma2, v1, v2, normalize=True):
    gauss = [math.exp(-(float(x*x) / sigma2)) for x in range(v1, v2+1)]
    total = sum(gauss)

    if normalize:
        gauss = [x/total for x in gauss]

    return gauss
    

def gaussian_filter(input_array):
    """
    """
    # Step 1: Define the convolution kernel
    sigma = 10000
    r = 256
    kernel = get_gaussian_kernel(4000, -r, r)

    # Step 2: Convolve
    return np.convolve(input_array, kernel, 'same')

def diff(timestamps):
    """
    Returns differences between consecutive elements
    """
    return np.ediff1d(timestamps)

def unit_vector(data, axis=None, out=None):
    if out is None:
        data = np.array(data, dtype=np.float64, copy=True)
        if data.ndim == 1:
            data /= math.sqrt(np.dot(data, data))
            return data
    else:
        if out is not data:
            out[:] = np.array(data, copy=False)
        data = out

    length = np.atleast_1d(np.sum(data*data, axis))
    np.sqrt(length, length)
    if axis is not None:
        length = np.expand_dims(length, axis)
    data /= length
    if out is None:
        return data

def rotation_matrix(angle, direction, point=None):
    """
    Generic method to return a rotation matrix
    - +ve angle = anticlockwise
    """
    sina = math.sin(angle)
    cosa = math.cos(angle)
    direction = unit_vector(direction[:3])

    R  = np.diag([cosa, cosa, cosa])
    R +=  np.outer(direction, direction) * (1.0 - cosa)
    direction *= sina
    R += np.array([ [0.0,           -direction[2],  direction[1]],
                    [direction[2],  0.0,           -direction[0]],
                    [-direction[1], direction[0],  0.0]])
    M = np.identity(4)
    M[:3, :3] = R
    if point is not None:
        point = np.array(point[:3], dtype=np.float64, copy=False)
        M[:3, 3] = point - np.dot(R, point)

    return M

def rotx_matrix(angle):
    return rotation_matrix(angle, [1, 0, 0])

def roty_matrix(angle):
    return rotation_matrix(angle, [0, 1, 0])

def rotz_matrix(angle):
    return rotation_matrix(angle, [0, 0, 1])

def meshwarp(src, distorted_grid):
    """
    src: The source image (Mat)
    distorted_grid: An n*n distorted_grid 
    """
    size = src.shape
    mapsize = (size[0], size[1], 1)
    dst = np.zeros(size, dtype=np.uint8)
    #mapx = np.zeros(mapsize, dtype=np.float32)
    #mapy = np.zeros(mapsize, dtype=np.float32)

    quads_per_row = len(distorted_grid[0]) - 1
    quads_per_col = len(distorted_grid) - 1
    pixels_per_row = size[1] / quads_per_row
    pixels_per_col = size[0] / quads_per_col

    pt_src_all = []
    pt_dst_all = []

    for i in distorted_grid:
        pt_src_all.extend(i)

    for x in range(quads_per_row+1):
        for y in range(quads_per_col+1):
            pt_dst_all.append([x*pixels_per_col, y*pixels_per_row])

    gx, gy = np.mgrid[0:size[1], 0:size[0]]

    import scipy
    from scipy.interpolate import griddata
    g_out = griddata(np.array(pt_dst_all), np.array(pt_src_all), (gx, gy), method='linear')
    mapx = np.append([], [ar[:,0] for ar in g_out]).reshape(mapsize).astype('float32')
    mapy = np.append([], [ar[:,1] for ar in g_out]).reshape(mapsize).astype('float32')

    dst = cv2.remap(src, mapx, mapy, cv2.INTER_LINEAR)
    cv2.imwrite('/tmp/test.png', dst)
    return dst

if __name__ == '__main__':
    #rx = rotx_matrix(math.radians(45))
    #ry = roty_matrix(math.radians(45))
    #rz = rotz_matrix(math.radians(45))

    #print rz * rx * ry
    img = cv2.imread("/work/blueprints/c7/data/mesh2.png")

    grid = [[(0,0), (100, 0), (200, 0), (300, 0)],
            [(0, 100), (150, 50), (200, 50), (300, 100)],
            [(0, 200), (150, 100), (200, 100), (300, 200)],
            [(0, 300), (100, 300), (200, 300), (300, 300)],
    ]

    meshwarp(img, grid)

