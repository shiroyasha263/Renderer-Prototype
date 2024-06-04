#pragma once

#include "surface.h"

#include "../common/common.h"
#include <vector>

class surface_list : public surface {
public:
	std::vector<shared_ptr<surface>> objects;

	surface_list() {}
	surface_list(shared_ptr<surface> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<surface> object) {
		objects.push_back(object);
	}

	bool intersect(const Ray3f& r, interval ray_t, hit_record& rec) const override {
		hit_record temp_rec;
		bool hit_anything = false;
		float closest_sofar = ray_t.max;

		for (const auto& object : objects) {
			if (object->intersect(r, interval(ray_t.min, closest_sofar), temp_rec)) {
				hit_anything = true;
				closest_sofar = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}
};