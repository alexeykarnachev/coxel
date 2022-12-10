#pragma once
#include "includes.h"
#include "la.h"

#define DEFAULT_TRANSFORMATION_SCALE vec3_ones
#define DEFAULT_TRANSFORMATION_ROTATION vec3_zeros
#define DEFAULT_TRANSFORMATION_TRANSLATION vec3_zeros

#define DEFAULT_CAMERA_FOV deg2rad(45.0)
#define DEFAULT_CAMERA_NEAR 0.1
#define DEFAULT_CAMERA_FAR 1000.0
#define DEFAULT_CAMERA_ASPECT 16.0 / 9.0

#define DEFAULT_MATERIAL_DIFFUSE_COLOR vec3(0.8, 0.8, 0.8)
#define DEFAULT_MATERIAL_SPECULAR 1.0

#define DEFAULT_POINT_LIGHT_COLOR vec3(100.0, 100.0, 100.0)
#define DEFAULT_POINT_LIGHT_LINEAR 0.7
#define DEFAULT_POINT_LIGHT_QUADRATIC 1.8
