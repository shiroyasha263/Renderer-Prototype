#pragma once

#include "../common/common.h"
#include "../bvh/aabb.h"
#include "../math/transform.h"

class material;

class hit_record {
public:
	Vec3f point;
	Vec3f normal;
	shared_ptr<material> mat;
	float t;
	float u, v;
	bool front_face;

	void set_face_normal(const Ray3f& r, const Vec3f& outward_normal) {
		front_face = dot(r.direction, normalize(outward_normal)) < 0.f;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class surface {
public:
	virtual ~surface() = default;

	virtual bool intersect(Ray3f& r, hit_record& rec) const = 0;

	virtual aabb bounding_box() const = 0;
};

class translate : public surface {
public:

	translate(shared_ptr<surface> object, const Vec3f& offset) :
		object(object), offset(offset) {
		bbox = object->bounding_box() + offset;
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		// Move the ray backwards by the offset
		Ray3f offset_ray(r.origin - offset, r.direction, r.tm, r.minT, r.maxT);

		// Determine whether an intersection exists along the offset ray (and if so, where)
		if (!object->intersect(offset_ray, rec))
			return false;

		rec.point += offset;
		r.maxT = rec.t;

		return true;
	}

	aabb bounding_box() const override { return bbox; }
private:
	shared_ptr<surface> object;
	Vec3f offset;
	aabb bbox;
};

class rotate_y : public surface {
public:

	rotate_y(shared_ptr<surface> object, float angle) : object(object) {
		float radians = deg_to_rad(angle);
		sin_theta = sin(radians);
		cos_theta = cos(radians);
		bbox = object->bounding_box();

		Vec3f min(infinity, infinity, infinity);
		Vec3f max(-infinity, -infinity, -infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

					auto newx = cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					Vec3f tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		Vec3f origin = r.origin;
		Vec3f direction = r.direction;

		origin[0] = cos_theta * r.origin[0] - sin_theta * r.origin[2];
		origin[2] = sin_theta * r.origin[0] + cos_theta * r.origin[2];

		direction[0] = cos_theta * r.direction[0] - sin_theta * r.direction[2];
		direction[2] = sin_theta * r.direction[0] + cos_theta * r.direction[2];

		Ray3f rotated(origin, direction, r.tm, r.minT, r.maxT);

		if (!object->intersect(rotated, rec))
			return false;

		Vec3f point = rec.point;
		point[0] =  cos_theta * rec.point[0] + sin_theta * rec.point[2];
		point[2] = -sin_theta * rec.point[0] + cos_theta * rec.point[2];

		Vec3f normal = rec.normal;
		normal[0] =  cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
		normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

		rec.point = point;
		rec.normal = normal;
		r.maxT = rec.t;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<surface> object;
	float sin_theta;
	float cos_theta;
	aabb bbox;
};

class transform_surface : public surface {
public:
	transform_surface(shared_ptr<surface> object, Mat44f m) :
		object(object), transf(m) {
		bbox = object->bounding_box();
		bbox = transf.bounding_box(bbox);
	}

	transform_surface(shared_ptr<surface> object) :
		object(object), transf(la::identity) {
		bbox = object->bounding_box();
	}

	bool intersect(Ray3f& r, hit_record& rec) const override {
		hit_record temp_rec;
		Ray3f transformed_ray = transf.inv_ray(r);
		if (!object->intersect(transformed_ray, temp_rec))
			return false;

		rec = temp_rec;
		rec.point = transf.point(rec.point);
		rec.normal = normalize(transf.normal(rec.normal));
		r.maxT = rec.t;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<surface> object;
	transform transf;
	aabb bbox;
};