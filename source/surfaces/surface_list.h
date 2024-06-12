#pragma once

#include "surface.h"

#include "../common/common.h"
#include "../bvh/aabb.h"
#include <vector>

class surface_list : public surface {
public:
	std::vector<shared_ptr<surface>> objects;

	surface_list() {}
	surface_list(shared_ptr<surface> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<surface> object) {
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		hit_record temp_rec;
		bool hit_anything = false;

		for (const auto& object : objects) {
			if (object->intersect(r, temp_rec)) {
				hit_anything = true;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

	aabb bounding_box() const override { return bbox; }

private:
	aabb bbox;
};

class transformed_surface_list : public surface {
public:
	std::vector<shared_ptr<surface>> objects;

	transformed_surface_list(const Mat44f& m) : transf(m) {}
	transformed_surface_list(shared_ptr<surface> object, const Mat44f& m) : transf(m) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<surface> object) {
		objects.push_back(object);
		aabb temp_box = transf.bounding_box(object->bounding_box());
		bbox = aabb(bbox, temp_box);
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		hit_record temp_rec;
		bool hit_anything = false;
		Ray3f transformed_ray = transf.inv_ray(r);

		for (const auto& object : objects) {
			if (object->intersect(transformed_ray, temp_rec)) {
				hit_anything = true;
				rec = temp_rec;
				rec.point = transf.point(rec.point);
				rec.normal = normalize(transf.normal(rec.normal));
				r.maxT = rec.t;
			}
		}

		return hit_anything;
	}

	aabb bounding_box() const override { return bbox; }

private:
	transform transf;
	aabb bbox;
};