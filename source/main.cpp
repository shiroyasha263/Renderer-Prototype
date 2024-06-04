#include <iostream>

#include "common/common.h"

#include "surfaces/surface.h"
#include "surfaces/sphere.h"
#include "surfaces/surface_list.h"
#include "camera/camera.h"
#include "materials/material.h"
#include "scene/scene.h"

int main(int argc, char* argv[]) {
    std::ifstream scene_file;
    scene_file.open("scene.json");
    json j = json::parse(scene_file);
    scene s(j);
    s.render();
    return 0;
}