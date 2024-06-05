#pragma once

#include "../common/common.h"
#include "texture.h"

class hit_record;

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(
		const Ray3f& r_in, const hit_record& rec, Color3f& attenuation, Ray3f& scattered
	) const {
		return false;
	}
};

class lambertian : public material {
public:
	lambertian(const Color3f& albedo) : tex(make_shared<solid_color>(albedo)) {}
	lambertian(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const Ray3f& r_in, const hit_record& rec, Color3f& attenuation, Ray3f& scattered) const override {
		Vec3f scatter_direction = rec.normal + random_Vec3f_unit_vector();

		if (near_zero(scatter_direction))
			scatter_direction = rec.normal;

		scattered = Ray3f(rec.point, scatter_direction, r_in.tm);
		attenuation = tex->value(rec.u, rec.v, rec.point);
		return true;
	}

private:
	shared_ptr<texture> tex;
};

class metal : public material {
public:
	metal(const Color3f& albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

	bool scatter(const Ray3f& r_in, const hit_record& rec, Color3f& attenuation, Ray3f& scattered) const override {
		Vec3f reflected = reflect(r_in.direction, rec.normal);
		reflected = normalize(reflected) + fuzz * random_Vec3f_unit_vector();
		scattered = Ray3f(rec.point, reflected, r_in.tm);
		attenuation = albedo;
		return (dot(scattered.direction, rec.normal) > 0);
	}
private:
	Color3f albedo;
	float fuzz;
};

class dielectric : public material {
public:
	dielectric(float refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const Ray3f& r_in, const hit_record& rec, Color3f& attenuation, Ray3f& scattered) const override {
		attenuation = Color3f(1.f);
		
		float ri = rec.front_face ? (1.f / refraction_index) : refraction_index;

		Vec3f unit_direction = normalize(r_in.direction);
		float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0f);
		float sin_theta = sqrt(1.f - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.f;
		Vec3f direction;

		if (cannot_refract || reflectance(cos_theta, ri) > random_float())
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, ri);

		scattered = Ray3f(rec.point, direction, r_in.tm);
		return true;
	}
private:
	float refraction_index;

	static float reflectance(float cosine, float refraction_index) {
		float r0 = (1.f - refraction_index) / (1.f + refraction_index);
		r0 = r0 * r0;
		return r0 + (1.f - r0) * pow((1.f - cosine), 5);
	}
};