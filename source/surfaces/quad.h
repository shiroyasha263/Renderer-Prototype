#pragma once

#include "../common/common.h"
#include "surface.h"
#include "surface_list.h"

class quad : public surface {
public:

	quad(const Vec3f& Q, const Vec3f& u, const Vec3f& v, shared_ptr<material> mat)
		: Q(Q), u(u), v(v), mat(mat) {
		Vec3f n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, Q);
		w = n / dot(n, n);

		set_bounding_box();
	}

	virtual void set_bounding_box() {
		aabb bbox_diag_1 = aabb(Q, Q + u + v);
		aabb bbox_diag_2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox_diag_1, bbox_diag_2);
	}

	aabb bounding_box() const override { return bbox; }

	bool intersect(Ray3f& r, hit_record& rec) const override {

		float denom = dot(normal, r.direction);

		// No hit if the ray is parallel
		if (fabs(denom) < 1e-8)
			return false;

		// Return false if hit point param t is out of range
		float t = (D - dot(normal, r.origin)) / denom;
		if (!r.surrounds(t))
			return false;

		// Determine the hit point lies within the planar shape using its plane coordinates.
		Vec3f point_of_intersection = r.at(t);
		Vec3f planar_hit_point = point_of_intersection - Q;
		float alpha = dot(w, cross(planar_hit_point, v));
		float beta = dot(w, cross(u, planar_hit_point));

		if (!is_interior(alpha, beta, rec))
			return false;

		rec.t = t;
		rec.point = point_of_intersection;
		rec.mat = mat;
		rec.set_face_normal(r, normal);
		r.maxT = t;

		return true;
	}

	virtual bool is_interior(float a, float b, hit_record& rec) const {
		interval unit_interval = interval(0, 1);
		// Given the hit point in plane coordinates, return false if it is outside the
		// primitive, otherwise set the hit record UV coordinates and return true.

		if (!unit_interval.contains(a) || !unit_interval.contains(b))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
	}
private:
	Vec3f Q;
	Vec3f u;
	Vec3f v;
	Vec3f w;
	shared_ptr<material> mat;
	aabb bbox;
	Vec3f normal;
	float D;
};

inline shared_ptr<surface_list> box(const Vec3f& a, const Vec3f& b, shared_ptr<material> mat) {
	shared_ptr<surface_list> faces = make_shared<surface_list>();

	Vec3f min = Vec3f(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
	Vec3f max = Vec3f(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

	Vec3f dx = Vec3f(max.x - min.x, 0.f, 0.f);
	Vec3f dy = Vec3f(0.f, max.y - min.y, 0.f);
	Vec3f dz = Vec3f(0.f, 0.f, max.z - min.z);

	faces->add(make_shared<quad>(Vec3f(min.x, min.y, max.z), dx, dy, mat)); // front
	faces->add(make_shared<quad>(Vec3f(max.x, min.y, max.z), -dz, dy, mat)); // right
	faces->add(make_shared<quad>(Vec3f(max.x, min.y, min.z), -dx, dy, mat)); // back
	faces->add(make_shared<quad>(Vec3f(min.x, min.y, min.z), dz, dy, mat)); // left
	faces->add(make_shared<quad>(Vec3f(min.x, max.y, max.z), dx, -dz, mat)); // top
	faces->add(make_shared<quad>(Vec3f(min.x, min.y, min.z), dx, dz, mat)); // bottom

	return faces;
}