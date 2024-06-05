#pragma once

#include "../common/common.h"
#include "../surfaces/surface.h"
#include "../surfaces/surface_list.h"

#include <algorithm>

class bvh_node : public surface {
public:
	bvh_node(surface_list list) : bvh_node(list.objects, 0, list.objects.size()) {
		// There's a C++ subtlety here. This constructor (without span indices) creates an
		// implicit copy of the hittable list, which we will modify. The lifetime of the copied
		// list only extends until this constructor exits. That's OK, because we only need to
		// persist the resulting bounding volume hierarchy.
	}

	bvh_node(std::vector<shared_ptr<surface>>& objects, size_t start, size_t end) {

		bbox = aabb::empty;

		for (size_t object_index = start; object_index < end; object_index++)
			bbox = aabb(bbox, objects[object_index]->bounding_box());

		int axis = bbox.longest_axis();

		auto comparator = (axis == 0) ? box_x_compare
						: (axis == 1) ? box_y_compare
						: box_z_compare;

		size_t object_span = end - start;

		if (object_span == 1) {
			left = right = objects[start];
		}
		else if (object_span == 2) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			std::sort(objects.begin() + start, objects.begin() + end, comparator);

			auto  mid = start + object_span / 2;
			left = make_shared<bvh_node>(objects, start, mid);
			right = make_shared<bvh_node>(objects, mid, end);
		}
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		if (!bbox.intersect(r))
			return false;

		bool intersect_left = left->intersect(r, rec);
		bool intersect_right = right->intersect(r, rec);

		return intersect_left || intersect_right;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<surface> left;
	shared_ptr<surface> right;
	aabb bbox;

	static bool box_compare(const shared_ptr<surface> a, const shared_ptr<surface> b, int axis_index) {
		interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
		interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
		return a_axis_interval.min < b_axis_interval.min;
	}

	static bool box_x_compare(const shared_ptr<surface> a, const shared_ptr<surface> b) {
		return box_compare(a, b, 0);
	}

	static bool box_y_compare(const shared_ptr<surface> a, const shared_ptr<surface> b) {
		return box_compare(a, b, 1);
	}

	static bool box_z_compare(const shared_ptr<surface> a, const shared_ptr<surface> b) {
		return box_compare(a, b, 2);
	}
};