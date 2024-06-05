#pragma once

#include "../common/common.h"
#include "../bvh/aabb.h"

class material;

class hit_record {
public:
	Vec3f point;
	Vec3f normal;
	shared_ptr<material> mat;
	float t;
	float u, v;
	bool front_face;

	void set_face_normal(const Ray3f& r, const Vec3f& outward_normal) {
		front_face = dot(r.direction, normalize(outward_normal)) < 0.f;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class surface {
public:
	virtual ~surface() = default;

	virtual bool intersect(Ray3f& r, hit_record& rec) const = 0;

	virtual aabb bounding_box() const = 0;
};