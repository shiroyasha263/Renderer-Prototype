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
#include "surfaces/constant_medium.h"
#include "surfaces/mesh.h"

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    surface_list boxes1;
    auto ground = make_shared<lambertian>(Color3f(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_float(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(Vec3f(x0, y0, z0), Vec3f(x1, y1, z1), ground));
        }
    }

    surface_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(Color3f(7, 7, 7));
    world.add(make_shared<quad>(Vec3f(123, 554, 147), Vec3f(300, 0, 0), Vec3f(0, 0, 265), light));

    auto center1 = Vec3f(400, 400, 200);
    auto center2 = center1 + Vec3f(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(Vec3f(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(Vec3f(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        Vec3f(0, 150, 145), 50, make_shared<metal>(Color3f(0.8, 0.8, 0.9), 1.0)
        ));

    auto boundary = make_shared<sphere>(Vec3f(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, Color3f(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(Vec3f(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, Color3f(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(Vec3f(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(Vec3f(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    surface_list boxes2;
    auto white = make_shared<lambertian>(Color3f(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(random_Vec3f(0, 165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
        Vec3f(-100, 270, 395)
        )
    );

    world = surface_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = Color3f(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = Vec3f(478, 278, -600);
    cam.lookat = Vec3f(278, 278, 0);
    cam.vup = Vec3f(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void scene_something() {
    surface_list world;

    auto checker = make_shared<checker_texture>(0.32f, Color3f(0.2f, 0.3f, 0.1f), Color3f(0.9f));
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto pertext = make_shared<noise_texture>();

    auto material_box = make_shared<lambertian>(Color3f(0.f, 1.f, 0.f));
    auto material_ground = make_shared<lambertian>(pertext);
    auto matG_2 = make_shared<lambertian>(checker);
    auto material_center = make_shared<lambertian>(earth_texture);
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(Color3f(0.8, 0.6, 0.2), 0.2f);
    auto material_metal = make_shared<metal>(Color3f(1.f), 0.f);
    auto material_difflight = make_shared<diffuse_light>(pertext);

    auto triangle_center = make_shared<triangle>(Vec3f(-0.5f, -0.5f, 0.5f - 1.2f), Vec3f(0.5f, -0.5f, 0.5f - 1.2f), Vec3f(-0.5f, 0.5f, 0.5f - 1.2f), material_metal);

    Mat44f transformation_matrix = Mat44f(
        { 10.f, 0.f, 0.f, 0.f },
        { 0.0f, 10.0f, 0.f, 0.f },
        { 0.0f, 0.f, 10.0f, 0.f },
        { 0.0f, -0.5f, -1.2f, 1.f });

    transformation_matrix = mul(transformation_matrix, rotate_along_x(-90));

    auto sphere_c = make_shared<sphere>(Vec3f(0.f, 0.f, 0.f), 0.5f, material_metal);
    auto shifted_sphere = make_shared<transformed_surface_list>(sphere_c, transformation_matrix);
    auto center_box = box(Vec3f(-0.25, -0.25, -0.25), Vec3f(0.25, 0.25, 0.25), material_metal);
    auto transformed_center_box = make_shared<transformed_surface_list>(center_box, transformation_matrix);

    auto bunny = make_shared<Mesh>("./assets/torsofemale.obj");
    auto transformed_bunny = make_shared<transformed_surface_list>(bunny, transformation_matrix);

    //world.add(shifted_sphere);

    world.add(make_shared<sphere>(Vec3f(0.0, -100.5, -1.0), 100.0, matG_2));
    //world.add(make_shared<sphere>(Vec3f(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(transformed_bunny);
    world.add(make_shared<sphere>(Vec3f(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(Vec3f(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(Vec3f(1.0, 0.0, -1.0), 0.5, material_right));

    //world = surface_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color3f(1.f, 1.f, 1.f);

    cam.vfov = 20;
    cam.lookfrom = Vec3f(-2, 2, 1);
    cam.lookat = Vec3f(0, 0, -1);
    cam.vup = Vec3f(0, 1, 0);

    cam.defocus_angle = 0.f;
    cam.focus_dist = 3.4;

    cam.render(world);
}

int main(int argc, char* argv[]) {

    /*std::ifstream scene_file;
    scene_file.open("scene.json");
    json j = json::parse(scene_file);
    scene s(j);
    s.render();*/

    scene_something();

    //final_scene(800, 1000, 40);

    return 0;
}