#pragma once

#include "../common/common.h"
#include "../camera/camera.h"
#include "../materials/material.h"
#include "../surfaces/surface.h"
#include "../surfaces/sphere.h"
#include "../surfaces/surface_list.h"
#include <map>

class scene {
public:
    scene(const json& j) {
        if (j.contains("camera")) {
            cam.params_init(j["camera"]);
            std::clog << "Added Camera from Json" << '\n';
        }

        if (j.contains("materials")) {
            json mats = j["materials"];
            for (int i = 0; i < mats.size(); i++) {
                if (mats[i]["type"] == "lambertian") {
                    Color3f albedo;
                    json_to_vector(mats[i]["albedo"], albedo);
                    mat_list[mats[i]["name"]] = make_shared<lambertian>(albedo);
                    std::clog << "Added Material : " << mats[i]["name"] << '\n';
                    continue;
                }
                if (mats[i]["type"] == "metal") {
                    Color3f albedo;
                    json_to_vector(mats[i]["albedo"], albedo);
                    float fuzz = mats[i]["fuzz"];
                    mat_list[mats[i]["name"]] = make_shared<metal>(albedo, fuzz);
                    std::clog << "Added Material : " << mats[i]["name"] << '\n';
                    continue;
                }
                if (mats[i]["type"] == "dielectric") {
                    float refractive_index = mats[i]["refraction_index"];
                    mat_list[mats[i]["name"]] = make_shared<dielectric>(refractive_index);
                    std::clog << "Added Material : " << mats[i]["name"] << '\n';
                    continue;
                }
                std::clog << "Invalid Material for material " << i << '\n';
            }
        }

        if (j.contains("surfaces")) {
            json surf = j["surfaces"];
            for (int i = 0; i < surf.size(); i++) {
                if (surf[i]["type"] == "sphere") {
                    Vec3f center;
                    json_to_vector(surf[i]["center"], center);
                    float radius = surf[i]["radius"];
                    world.add(make_shared<sphere>(center, radius, mat_list[surf[i]["material"]]));
                    continue;
                }
                std::clog << "Invalid Surface for Surface " << i << '\n';
            }
        }
    }

    void render() {
        cam.render(world);
    }

private:
    camera cam;
    std::map<std::string, shared_ptr<material>> mat_list;
    surface_list world;
};