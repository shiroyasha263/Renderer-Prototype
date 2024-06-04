#pragma once

#include "surface.h"

class sphere : public surface {
public:
	sphere(const Vec3f& center, float radius, shared_ptr<material> mat) : center(center), radius(fmax(0, radius)), mat(mat) {}

	bool intersect(const Ray3f& r, interval ray_t, hit_record& rec) const override {
        Vec3f origin_to_center = center - r.origin;
        float a = dot(r.direction, r.direction);
        float h = dot(r.direction, origin_to_center);
        float c = dot(origin_to_center, origin_to_center) - radius * radius;
        float discriminant = h * h - a * c;

        if (discriminant < 0)
            return false;
        
        float sqrtd = sqrt(discriminant);

        float root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.point = r(root);
        Vec3f outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
	}

private:
	Vec3f center;
	float radius;
    shared_ptr<material> mat;
};