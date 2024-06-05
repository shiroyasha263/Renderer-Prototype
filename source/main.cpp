#include <iostream>

#include "common/common.h"

#include "surfaces/surface.h"
#include "surfaces/sphere.h"
#include "surfaces/surface_list.h"
#include "camera/camera.h"
#include "materials/material.h"
#include "scene/scene.h"
#include "bvh/bvh.h"
#include "materials/texture.h"
#include "surfaces/quad.h"
#include "surfaces/triangle.h"

int main(int argc, char* argv[]) {

    /*std::ifstream scene_file;
    scene_file.open("scene.json");
    json j = json::parse(scene_file);
    scene s(j);
    s.render();*/
    
    /*surface_list world;

    auto checker = make_shared<checker_texture>(0.32f, Color3f(0.2f, 0.3f, 0.1f), Color3f(0.9f));
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto pertext = make_shared<noise_texture>();

    auto material_ground = make_shared<lambertian>(pertext);
    auto material_center = make_shared<lambertian>(earth_texture);
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(Color3f(0.8, 0.6, 0.2), 0.2f);

    world.add(make_shared<sphere>(Vec3f(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(Vec3f(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(Vec3f(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(Vec3f(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(Vec3f(1.0, 0.0, -1.0), 0.5, material_right));

    world = surface_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = Vec3f(-2, 2, 1);
    cam.lookat = Vec3f(0, 0, -1);
    cam.vup = Vec3f(0, 1, 0);

    cam.defocus_angle = 0.f;
    cam.focus_dist = 3.4;

    cam.render(world);*/

    surface_list world;

    // Materials
    auto left_red = make_shared<lambertian>(Color3f(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(Color3f(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(Color3f(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(Color3f(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(Color3f(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(Vec3f(-3, -2, 5), Vec3f(0, 0, -4), Vec3f(0, 4, 0), left_red));
    world.add(make_shared<triangle>(Vec3f(-2, -2, 0), Vec3f(-2, -2, 0) + Vec3f(4, 0, 0), Vec3f(-2, -2, 0) + Vec3f(4, 4, 0), back_green));
    world.add(make_shared<quad>(Vec3f(3, -2, 1),  Vec3f(0, 0, 4),  Vec3f(0, 4, 0), right_blue));
    world.add(make_shared<quad>(Vec3f(-2, 3, 1),  Vec3f(4, 0, 0),  Vec3f(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(Vec3f(-2, -3, 5), Vec3f(4, 0, 0),  Vec3f(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 80;
    cam.lookfrom = Vec3f(0, 0, 9);
    cam.lookat =   Vec3f(0, 0, 0);
    cam.vup =      Vec3f(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);

    return 0;
}