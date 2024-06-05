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