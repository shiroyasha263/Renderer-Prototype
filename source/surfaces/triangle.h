#pragma once

#include "../common/common.h"
#include "surface.h"

class triangle : public surface {
public:

	triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, shared_ptr<material> mat)
		: v0(v0), v1(v1), v2(v2), mat(mat) {
		
		v10 = v1 - v0;
		v20 = v2 - v0;
		Vec3f n = cross(v10, v20);
		normal = normalize(n);
		D = dot(normal, v0);
		w = n / dot(n, n);

		n0 = normal;
		n1 = normal;
		n2 = normal;

		t0 = Vec2f(0.f);
		t1 = Vec2f(1.f, 0.f);
		t2 = Vec2f(0.f, 1.f);

		set_bounding_box();
	}

	triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2,
		const Vec3f& n0, const Vec3f& n1, const Vec3f& n2,
		shared_ptr<material> mat) :
		v0(v0), v1(v1), v2(v2), n0(normalize(n0)), n1(normalize(n1)), n2(normalize(n2)), mat(mat) {
		
		v10 = v1 - v0;
		v20 = v2 - v0;
		Vec3f n = cross(v10, v20);
		normal = normalize(n);
		D = dot(normal, v0);
		w = n / dot(n, n);

		t0 = Vec2f(0.f);
		t1 = Vec2f(1.f, 0.f);
		t2 = Vec2f(0.f, 1.f);

		set_bounding_box();
	}

	triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2,
		const Vec3f& n0, const Vec3f& n1, const Vec3f& n2,
		const Vec2f& t0, const Vec2f& t1, const Vec2f& t2,
		shared_ptr<material> mat) :
		v0(v0), v1(v1), v2(v2),
		n0(normalize(n0)), n1(normalize(n1)), n2(normalize(n2)),
		t0(t0), t1(t1), t2(t2), mat(mat) {
		
		v10 = v1 - v0;
		v20 = v2 - v0;
		Vec3f n = cross(v10, v20);
		normal = normalize(n);
		D = dot(normal, v0);
		w = n / dot(n, n);

		set_bounding_box();
	}

	virtual void set_bounding_box() {
		aabb bbox_diag_1 = aabb(v0, v0 + v10);
		aabb bbox_diag_2 = aabb(v0, v0 + v20);
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
		Vec3f planar_hit_point = point_of_intersection - v0;
		float alpha = dot(w, cross(planar_hit_point, v20));
		float beta = dot(w, cross(v10, planar_hit_point));

		if (!is_interior(alpha, beta, rec))
			return false;

		rec.t = t;
		rec.point = point_of_intersection;
		rec.mat = mat;
		rec.set_face_normal(r, rec.normal);
		r.maxT = t;

		return true;
	}

	virtual bool is_interior(float a, float b, hit_record& rec) const {
		interval unit_interval = interval(0, 1);
		// Given the hit point in plane coordinates, return false if it is outside the
		// primitive, otherwise set the hit record UV coordinates and return true.

		if (a <= 0 || b <= 0 || !unit_interval.contains(a + b))
			return false;

		rec.u = t0.x * (1 - a - b) + t1.x * a + t2.x * b;
		rec.v = t0.y * (1 - a - b) + t1.y * a + t2.y * b;
		rec.normal = n0 * (1 - a - b) + n1 * a + n2 * b;
		return true;
	}
private:
	Vec3f v0;
	Vec3f v1;
	Vec3f v2;
	Vec3f n0;
	Vec3f n1;
	Vec3f n2;
	Vec2f t0;
	Vec2f t1;
	Vec2f t2;
	shared_ptr<material> mat;
	Vec3f v10;
	Vec3f v20;
	Vec3f w;
	aabb bbox;
	Vec3f normal;
	float D;
};