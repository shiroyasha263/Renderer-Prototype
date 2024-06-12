#pragma once

#include "vecmath.h"
#include "../bvh/aabb.h"
#include "../ray/ray.h"

struct transform {
	Mat44f m;
	Mat44f m_inv;

	transform() : m(la::identity), m_inv(la::identity) {}

	transform(const Mat44f& m) : m(m), m_inv(la::inverse(m)) {}

	transform(const Mat44f& m, const Mat44f& m_inv) : m(m), m_inv(m_inv) {}

	transform inverse() const {
		return transform(m_inv);
	}

	transform operator* (const transform& t) const {
		return transform(mul(m, t.m), mul(t.m_inv, m_inv));
	}

	Vec3f vector(const Vec3f& v) const {
		Vec4f transformed_vector = Vec4f(v, 0.f);
		transformed_vector = mul(m, transformed_vector);
		return transformed_vector.xyz();
	}

	Vec3f inv_vector(const Vec3f& v) const {
		Vec4f transformed_vector = Vec4f(v, 0.f);
		transformed_vector = mul(m_inv, transformed_vector);
		return transformed_vector.xyz();
	}

	Vec3f normal(const Vec3f& n) const {
		Vec4f transformed_normal = Vec4f(n, 0.f);
		transformed_normal = mul(transpose(m_inv), transformed_normal);
		return transformed_normal.xyz();
	}

	Vec3f inv_normal(const Vec3f& n) const {
		Vec4f transformed_normal = Vec4f(n, 0.f);
		transformed_normal = mul(transpose(m), transformed_normal);
		return transformed_normal.xyz();
	}

	Vec3f point(const Vec3f& p) const {
		Vec4f transformed_point = Vec4f(p, 1.f);
		transformed_point = mul(m, transformed_point);
		return transformed_point.xyz() / transformed_point.w;
	}

	Vec3f inv_point(const Vec3f& p) const {
		Vec4f transformed_point = Vec4f(p, 1.f);
		transformed_point = mul(m_inv, transformed_point);
		return transformed_point.xyz() / transformed_point.w;
	}

	Ray3f ray(const Ray3f& r) const {
		Vec3f transformed_origin = point(r.origin);
		Vec3f transformed_direction = vector(r.direction);
		return Ray3f(transformed_origin, transformed_direction, r.tm, r.minT, r.maxT);
	}

	Ray3f inv_ray(const Ray3f& r) const {
		Vec3f transformed_origin = inv_point(r.origin);
		Vec3f transformed_direction = inv_vector(r.direction);
		return Ray3f(transformed_origin, transformed_direction, r.tm, r.minT, r.maxT);
	}

	aabb bounding_box(const aabb& bbox) const {
		Vec3f min(infinity, infinity, infinity);
		Vec3f max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					Vec3f new_point = Vec3f(bbox.x.min * (1 - i % 2) + bbox.x.max * (i % 2),
						bbox.y.min * (1 - j % 2) + bbox.y.max * (j % 2),
						bbox.z.min * (1 - k % 2) + bbox.z.max * (k % 2));

					Vec3f tester = point(new_point);

					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		return aabb(min, max);
	}

	static transform translate(const Vec3f& t) {
		return transform(
			Mat44f({ 1.f, 0.f, 0.f, 0.f },
				   { 0.f, 1.f, 0.f, 0.f },
				   { 0.f, 0.f, 1.f, 0.f },
				   { t.x, t.y, t.z, 1.f }));
	}

	static transform axisOffset(const Vec3f& x, const Vec3f& y, const Vec3f& z, const Vec3f& o) {
		return transform(Mat44f({ x, 0 },
								{ y, 0 },
								{ z, 0 },
								{ o, 1 }));
	}
};