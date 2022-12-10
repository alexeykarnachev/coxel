#pragma once
#include "../components/script.h"


typedef struct CameraMouseControllerArgs {
    float side_sens;
    float straight_sens;
    float rotation_sens;
} CameraMouseControllerArgs;

CameraMouseControllerArgs camera_mouse_controller_create_default_args();
Script* camera_mouse_controller_create_script();
