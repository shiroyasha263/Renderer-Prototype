#pragma once

#include "../common/common.h"
#include "surface.h"
#include "../materials/material.h"
#include "../materials/texture.h"

class constant_medium : public surface {
public:

	constant_medium(shared_ptr<surface> boundary, float density, shared_ptr<texture> tex) :
		boundary(boundary), neg_inv_density(-1.f / density), phase_function(make_shared<isotropic>(tex)) {}

	constant_medium(shared_ptr<surface> boundary, float density, const Color3f& albedo) :
		boundary(boundary), neg_inv_density(-1.f / density), phase_function(make_shared<isotropic>(albedo)) {}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		// Print occasional samples when debugging. To enable, set enableDebug true.
		const bool enableDebug = false;
		const bool debugging = enableDebug && random_float() < 0.00001;

		Ray3f temp_ray(r, -infinity, infinity);

		hit_record rec1, rec2;

		if (!boundary->intersect(temp_ray, rec1))
			return false;

		temp_ray.minT = rec1.t + epsilon;
		temp_ray.maxT = infinity;

		if (!boundary->intersect(temp_ray, rec2))
			return false;

		if (debugging) std::clog << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

		if (rec1.t < r.minT) rec1.t = r.minT;
		if (rec2.t > r.maxT) rec2.t = r.maxT;

		if (rec1.t >= rec2.t)
			return false;

		if (rec1.t < 0.f)
			rec1.t = 0.f;

		float ray_length = length(r.direction);
		float distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
		float hit_distance = neg_inv_density * log(random_float());

		if (hit_distance > distance_inside_boundary)
			return false;

		rec.t = rec1.t + hit_distance / ray_length;
		rec.point = r.at(rec.t);
		r.maxT = rec.t;

		if (debugging) {
			std::clog << "hit_distance = " << hit_distance << '\n'
				<< "rec.t = " << rec.t << '\n'
				<< "rec.p = " << rec.point << '\n';
		}

		rec.normal = Vec3f(1.f, 0.f, 0.f);
		rec.front_face = true;
		rec.mat = phase_function;

		return true;
	}

	aabb bounding_box() const override { return boundary->bounding_box(); }

private:
	shared_ptr<surface> boundary;
	float neg_inv_density;
	shared_ptr<material> phase_function;
};