#pragma once

#include "../common/common.h"

class material;

class hit_record {
public:
	Vec3f point;
	Vec3f normal;
	shared_ptr<material> mat;
	float t;
	bool front_face;

	void set_face_normal(const Ray3f& r, const Vec3f& outward_normal) {
		front_face = dot(r.direction, normalize(outward_normal)) < 0.f;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class surface {
public:
	virtual ~surface() = default;

	virtual bool intersect(const Ray3f& r, interval ray_t, hit_record& rec) const = 0;
};