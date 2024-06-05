#pragma once

#include "surface.h"

class sphere : public surface {
public:
	sphere(const Vec3f& center, float radius, shared_ptr<material> mat) : 
        center(center), radius(fmax(0, radius)), mat(mat), is_moving(false) {
        Vec3f rad_vec(radius);
        bbox = aabb(center - rad_vec, center + rad_vec);
    }

	sphere(const Vec3f& center, const Vec3f& center_final, float radius, shared_ptr<material> mat) : 
        center(center), radius(fmax(0, radius)), mat(mat), is_moving(true) {
        Vec3f rad_vec(radius);
        aabb box1(center - radius, center + radius);
        aabb box2(center_final - radius, center_final + radius);

        bbox = aabb(box1, box2);

        center_movement = center_final - center;
    }

	bool intersect(Ray3f& r, hit_record& rec) const override {

        Vec3f center_at_t = is_moving ? sphere_center_at_t(r.tm) : center;

        Vec3f origin_to_center = center_at_t - r.origin;
        float a = dot(r.direction, r.direction);
        float h = dot(r.direction, origin_to_center);
        float c = dot(origin_to_center, origin_to_center) - radius * radius;
        float discriminant = h * h - a * c;

        if (discriminant < 0)
            return false;
        
        float sqrtd = sqrt(discriminant);

        float root = (h - sqrtd) / a;
        if (!r.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!r.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.point = r(root);
        Vec3f outward_normal = (rec.point - center_at_t) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;
        r.maxT = root;

        return true;
	}

    aabb bounding_box() const override { return bbox; }

private:
	Vec3f center;
	float radius;
    shared_ptr<material> mat;
    bool is_moving;
    Vec3f center_movement;
    aabb bbox;

    Vec3f sphere_center_at_t(float time) const {
        return center + time * center_movement;
    }

    static void get_sphere_uv(const Vec3f& p, float& u, float& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        float theta = acos(-p.y);
        float phi = atan2(-p.z, p.x) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
};