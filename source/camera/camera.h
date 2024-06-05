#pragma once

#include "../common/common.h"
#include "../surfaces/surface.h"
#include "../materials/material.h"

class camera {
public:

	float aspect_ratio = 1.f;
	int image_width = 100;
	int samples_per_pixel = 10;
	int max_depth = 10;

	float vfov = 90;
	Vec3f lookfrom = Vec3f(0.f);
	Vec3f lookat = Vec3f(0.f, 0.f, -1.f);
	Vec3f vup = Vec3f(0.f, 1.f, 0.f);

	float defocus_angle = 0.f;
	float focus_dist = 10.f;

	void render(const surface& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				Color3f pixel_color(0.f);
				for (int k = 0; k < samples_per_pixel; k++) {
					Ray3f ray = get_ray(i, j);
					pixel_color += ray_color(ray, world, max_depth);
				}
				write_color(std::cout, pixel_samples_scale * pixel_color);
			}
		}

		std::clog << "\nDone\n";
	}

	void params_init(const json& j) {
		aspect_ratio = j.value("aspect_ratio", aspect_ratio);
		image_width = j.value("image_width", image_width);
		samples_per_pixel = j.value("samples_per_pixel", samples_per_pixel);
		max_depth = j.value("max_depth", max_depth);

		vfov = j.value("vfov", vfov);
		json_to_vector(j["lookfrom"], lookfrom);
		json_to_vector(j["lookat"], lookat);
		json_to_vector(j["vup"], vup);

		defocus_angle = j.value("defocus_angle", defocus_angle);
		focus_dist = j.value("focus_dist", focus_dist);
	}

private:

	int image_height;
	float pixel_samples_scale;
	Vec3f center;
	Vec3f pixel00_loc;
	Vec3f pixel_delta_u;
	Vec3f pixel_delta_v;
	Vec3f u, v, w;
	Vec3f defocus_disk_u;
	Vec3f defocus_disk_v;

	void initialize() {
		// Calculate height
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixel_samples_scale = 1.f / samples_per_pixel;

		center = lookfrom;

		// Camera
		float theta = deg_to_rad(vfov);
		float h = tan(theta / 2.f);
		float viewport_height = 2.0f * h * focus_dist;

		// Calculate width of viewport
		float viewport_width = viewport_height * (float(image_width) / image_height);

		w = normalize(lookfrom - lookat);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		// Calculate the Vectors representing the edges of the viewport
		Vec3f viewport_u = viewport_width * u;
		Vec3f viewport_v = viewport_height * -v;

		// Calculate per pixel width and height
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Location of the upper left pixel
		Vec3f viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.f - viewport_v / 2.f;
		pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

		float defocus_radius = focus_dist * tan(deg_to_rad(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	Ray3f get_ray(int i, int j) const {
		Vec3f offset = sample_square();
		Vec3f pixel_sample = pixel00_loc
							 + ((i + offset.x) * pixel_delta_u)
							 + ((j + offset.y) * pixel_delta_v);
		Vec3f ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		Vec3f ray_direction = pixel_sample - ray_origin;
		float ray_time = random_float();

		return Ray3f(ray_origin, ray_direction, ray_time);
	}

	Vec3f sample_square() const {
		return Vec3f(random_float() - 0.5f, random_float() - 0.5f, 0.f);
	}

	Color3f ray_color(Ray3f& r, const surface& world, int depth) const {

		if (depth <= 0)
			return Color3f(0.f);

		hit_record rec;

		if (world.intersect(r, rec)) {
			Ray3f scattered;
			Color3f attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, world, depth - 1);
			return Color3f(0.f);
		}

		Vec3f unit_direction = normalize(r.direction);
		float a = 0.5f * (unit_direction.y + 1.0f);
		return (1.0f - a) * Color3f(1.0f) + a * Color3f(0.5f, 0.7f, 1.0f);
	}

	Vec3f defocus_disk_sample() const {
		Vec3f p = random_Vec3f_in_unit_disk();
		return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
	}
};