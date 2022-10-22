#define PI 3.14159265358979323846264338327950288

#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)
#define clear_struct(s) (memset(s, 0, sizeof(*s)))
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "constants.h"
#include "utils.h"
#include "la.h"
#include "renderer/program.h"
#include "renderer/camera.h"
#include "renderer/primitives.h"
#include "renderer/buffer.h"
#include "renderer/depth_cubemap.h"
#include "renderer/material.h"
#include "renderer/transformation.h"
#include "renderer/mesh.h"
#include "renderer/point_light.h"
#include "renderer/renderer.h"
#include "renderer/window.h"
