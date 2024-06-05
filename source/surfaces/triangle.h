#pragma once

#include "../common/common.h"
#include "surface.h"

class triangle : public surface {
public:

	triangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, shared_ptr<material> mat)
		: A(A), B(B), C(C), mat(mat) {
		u = B - A;
		v = C - A;
		Vec3f n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, A);
		w = n / dot(n, n);

		set_bounding_box();
	}

	virtual void set_bounding_box() {
		aabb bbox_diag_1 = aabb(A, A + u);
		aabb bbox_diag_2 = aabb(A, A + v);
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
		Vec3f planar_hit_point = point_of_intersection - A;
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

		if (a <= 0 || b <= 0 || !unit_interval.contains(a + b))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
	}
private:
	Vec3f A;
	Vec3f B;
	Vec3f C;
	Vec3f u;
	Vec3f v;
	Vec3f w;
	shared_ptr<material> mat;
	aabb bbox;
	Vec3f normal;
	float D;
};