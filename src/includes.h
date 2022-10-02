#define PI 3.14159265358979323846264338327950288
#define deg2rad(d) ((d) * PI / 180.0)
#define rad2deg(r) ((r) * 180.0 / PI)

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

#include "utils.h"
#include "gl_utils.h"
#include "la.h"
#include "camera.h"
#include "meshes.h"
#include "hdr.h"
#include "model.h"
#include "program.h"
