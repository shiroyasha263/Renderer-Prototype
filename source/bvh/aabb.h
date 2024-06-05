#pragma once

#include "../common/common.h"

class aabb {
public:
	interval x, y, z;

	aabb() {}

	aabb(interval x, interval y, interval z) : x(x), y(y), z(z) {
		pad_to_minimums();
	}

	aabb(Vec3f a, Vec3f b) {
		x = (a.x <= b.x) ? interval(a.x, b.x) : interval(b.x, a.x);
		y = (a.y <= b.y) ? interval(a.y, b.y) : interval(b.y, a.y);
		z = (a.z <= b.z) ? interval(a.z, b.z) : interval(b.z, a.z);

		pad_to_minimums();
	}

	aabb(const aabb& box1, const aabb& box2) {
		x = interval(box1.x, box2.x);
		y = interval(box1.y, box2.y);
		z = interval(box1.z, box2.z);
	}

	const interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;
	}

	bool intersect(const Ray3f& r) const {
		const Vec3f ray_origin = r.origin;
		const Vec3f ray_direction = r.direction;
		float ray_tmin = r.minT;
		float ray_tmax = r.maxT;

		for (int axis = 0; axis < 3; axis++) {
			const interval& ax = axis_interval(axis);
			const float adinv = 1.f / ray_direction[axis];

			float t0 = (ax.min - ray_origin[axis]) * adinv;
			float t1 = (ax.max - ray_origin[axis]) * adinv;

			if (t0 < t1) {
				if (t0 > ray_tmin) ray_tmin = t0;
				if (t1 < ray_tmax) ray_tmax = t1;
			}
			else {
				if (t1 > ray_tmin) ray_tmin = t1;
				if (t0 < ray_tmax) ray_tmax = t0;
			}

			if (ray_tmax < ray_tmin)
				return false;
		}

		return true;
	}

	int longest_axis() {
		if (x.size() > y.size())
			return x.size() > z.size() ? 0 : 2;
		else
			return y.size() > z.size() ? 1 : 2;
	}

	static const aabb empty, universe;

private:

	void pad_to_minimums() {
		if (x.size() < epsilon) x = x.expand(epsilon);
		if (y.size() < epsilon) y = y.expand(epsilon);
		if (z.size() < epsilon) z = z.expand(epsilon);
	}
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);