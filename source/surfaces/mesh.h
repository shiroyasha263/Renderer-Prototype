#pragma once

#include "surface.h"
#include <map>
#include "../materials/material.h"
#include "../surfaces/triangle.h"
#include "../surfaces/surface_list.h"
#include "../bvh/bvh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

using std::string;
using std::vector;

class Mesh : public surface {
public:

	Mesh() {}

	Mesh(std::string filename) {

        std::ifstream is(filename);
        if (is.fail())
            std::clog << "Unable to load file";

        string warn;
        string err;

        struct UserData
        {
            Mesh* mesh;
            uint32_t         current_material_idx;
            std::map<string, int> material_map;
            string           material_prefix;
        } data;

        data.mesh = this;
        data.current_material_idx = 0;

        data.material_prefix = "";
        
        //if (data.material_prefix != "")
        //    spdlog::info("Prepending the string \"{}\" to all mesh material names", data.material_prefix);

        // create a default material used for any faces that don't have a material set
        // this will be the material with index 0
        auto default_material = make_shared<metal>(Color3f(0.8f, 0.8f, 0.8f), 0.2f);
        materials.push_back(default_material);

        tinyobj::callback_t cb;

        cb.vertex_cb = [](void* user_data, float x, float y, float z, float w)
        {
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;
            Vec3f     v{ x, y, z };
            mesh->vertex_position.push_back(v);
        };

        cb.normal_cb = [](void* user_data, float x, float y, float z)
        {
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;
            mesh->vertex_normal.push_back(normalize(Vec3f(x, y, z)));
        };

        cb.texcoord_cb = [](void* user_data, float x, float y, float z)
        {
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;
            mesh->vertex_texcoord.push_back(Vec2f(x, y));
        };

        cb.index_cb = [](void* user_data, tinyobj::index_t* indices, int num_indices)
        {
            // NOTE: the value of each index is the raw value. For example, the application must manually adjust the index
            // with offset (e.g. v_indices.size()) when the value is negative (which means relative index). Also, the first
            // index starts with 1, not 0. See fixIndex() function in tiny_obj_loader.h for details. Also, 0 is set for the
            // index value which does not exist in the .obj file
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;

            if (num_indices < 3)
                std::clog << "A triangle cannot have less than 3 indices";

            tinyobj::index_t idx0 = indices[0];
            tinyobj::index_t idx1 = indices[1];
            tinyobj::index_t idx2;

            for (int i = 2; i < num_indices; i++) {
                idx2 = indices[i];

                bool use_normal = false;
                bool use_texcoord = false;

                if (!mesh->vertex_position.size() || indices[0].vertex_index == 0 || indices[i - 1].vertex_index == 0 ||
                    indices[i].vertex_index == 0)
                    std::clog << "Missing Vertex index for triangle";

                mesh->vertex_indices_per_face.push_back(Vec3i(idx0.vertex_index - 1, idx1.vertex_index - 1, idx2.vertex_index - 1));
                mesh->material_indice_per_face.push_back(data->current_material_idx);

                if (mesh->vertex_normal.size() && indices[0].normal_index != 0 && indices[i - 1].normal_index != 0 &&
                    indices[i].normal_index != 0)
                {
                    mesh->normal_indices_per_face.push_back(Vec3i(idx0.normal_index - 1, idx1.normal_index - 1, idx2.normal_index - 1));
                    use_normal = true;
                }

                if (mesh->vertex_texcoord.size() && indices[0].texcoord_index != 0 && indices[i - 1].texcoord_index != 0 &&
                    indices[i].texcoord_index != 0) {
                    mesh->texture_indices_per_face.push_back(Vec3i(idx0.texcoord_index - 1, idx1.texcoord_index - 1, idx2.texcoord_index - 1));
                    use_texcoord = true;
                }

                idx1 = idx2;
            }
        };

        cb.usemtl_cb = [](void* user_data, const char* name, int material_idx)
        {
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;

            string full_name = data->material_prefix + name;

            // check if we've already added a material with this name to the mesh
            auto it = data->material_map.find(full_name);
            if (it != data->material_map.end())
                data->current_material_idx = it->second;
            //else
            //{
            //    // try to find a material with the given name in the scene description and add it to the mesh's materials
            //    try
            //    {
            //        data->mesh->materials.push_back(
            //            DartsFactory<Material>::find(json::object({ {"material", string(full_name)} })));
            //        data->material_map[full_name] = data->current_material_idx = data->mesh->materials.size() - 1;
            //    }
            //    catch (const std::exception& e)
            //    {
            //        spdlog::warn("When parsing OBJ file: {}\n\tUsing default material instead.\n", e.what());
            //        data->material_map[full_name] = data->current_material_idx = 0;
            //    }
            //}
        };

        cb.mtllib_cb = [](void* user_data, const tinyobj::material_t* materials, int num_materials)
        {
            UserData* data = reinterpret_cast<UserData*>(user_data);
            Mesh* mesh = data->mesh;
        };

        bool ret = tinyobj::LoadObjWithCallback(is, cb, &data, nullptr, &warn, &err);

        // compute object_to_texture space transform
        //auto d = bbox_o.diagonal();
        //auto m = mul(scaling_matrix(la::select(equal(d, 0.f), 1.f, 1.f / d)), translation_matrix(-bbox_o.min));
        //object_to_texture = Transform(m);

        if (!warn.empty())
            std::clog << warn;

        if (!err.empty() || !ret)
            std::clog << err;

        std::clog << "Vertices read : " << vertex_position.size() << "\n";
        std::clog << "Normals read : " << vertex_normal.size() << "\n";
        std::clog << "Texcoord read : " << vertex_texcoord.size() << "\n";
        std::clog << "Vertex Indices read : " << vertex_indices_per_face.size() << "\n";
        std::clog << "Normal Indices read : " << normal_indices_per_face.size() << "\n";
        std::clog << "Texcoord Indices read : " << texture_indices_per_face.size() << "\n";

        build();
	}

    bool intersect(Ray3f& r, hit_record& rec) const override {
        bool hit_anything = BVH->intersect(r, rec);
        return hit_anything;
    }

    void add(shared_ptr<surface> object) {
        mesh_list.add(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    void build() {
        bool use_normals;
        bool use_texcoords;

        for (int face = 0; face < vertex_indices_per_face.size(); face++) {
            Vec3f v0 = vertex_position[vertex_indices_per_face[face].x];
            Vec3f v1 = vertex_position[vertex_indices_per_face[face].y];
            Vec3f v2 = vertex_position[vertex_indices_per_face[face].z];

            Vec3f n0, n1, n2;
            if (normal_indices_per_face.size() > face)
            {
                auto in0 = normal_indices_per_face[face].x, in1 = normal_indices_per_face[face].y, in2 = normal_indices_per_face[face].z;
                if (in0 >= 0 && in1 >= 0 && in2 >= 0)
                {
                    use_normals = true;
                    n0 = vertex_normal[in0];
                    n1 = vertex_normal[in1];
                    n2 = vertex_normal[in2];
                }
            }

            Vec2f t0, t1, t2;
            if (texture_indices_per_face.size() > face)
            {
                auto it0 = texture_indices_per_face[face].x, it1 = texture_indices_per_face[face].y, it2 = texture_indices_per_face[face].z;
                if (it0 >= 0 && it1 >= 0 && it2 >= 0)
                {
                    use_texcoords = true;
                    t0 = vertex_texcoord[it0];
                    t1 = vertex_texcoord[it1];
                    t2 = vertex_texcoord[it2];
                }
            }

            int switch_mode = use_normals + use_texcoords * 2;

            if (switch_mode == 3) {
                auto tri = make_shared<triangle>(v0, v1, v2, n0, n1, n2, t0, t1, t2, materials[material_indice_per_face[face]]);
                add(tri);
            }
            else if (switch_mode == 1) {
                auto tri = make_shared<triangle>(v0, v1, v2, n0, n1, n2, materials[material_indice_per_face[face]]);
                add(tri);
            }
            else if (switch_mode == 2) {
                Vec3f v10 = v1 - v0;
                Vec3f v20 = v2 - v0;
                Vec3f n = cross(v10, v20);
                n = normalize(n);
                auto tri = make_shared<triangle>(v0, v1, v2, n, n, n, t0, t1, t2, materials[material_indice_per_face[face]]);
                add(tri);
            }
            else {
                auto tri = make_shared<triangle>(v0, v1, v2, materials[material_indice_per_face[face]]);
                add(tri);
            }
        }

        BVH = make_shared<bvh_node>(mesh_list);
    }

    aabb bounding_box() const override { return bbox; }

	vector<Vec3f> vertex_position;
	vector<Vec3f> vertex_normal;
	vector<Vec2f> vertex_texcoord;
	vector<Vec3i> vertex_indices_per_face;
	vector<Vec3i> normal_indices_per_face;
	vector<Vec3i> texture_indices_per_face;
	vector<uint32_t> material_indice_per_face;
	vector<shared_ptr<material>> materials;
    surface_list mesh_list;
    shared_ptr<bvh_node> BVH;
	transform transf;
	aabb bbox;
};