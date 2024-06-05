#pragma once

#include "../common/common.h"
#include "../image/image.h"
#include "perlin.h"

class texture {
public:
	virtual ~texture() = default;

	virtual Color3f value(float u, float v, const Vec3f& p) const = 0;
};

class solid_color : public texture {
public:
	solid_color (const Color3f& albedo) : albedo(albedo) {}

	solid_color (float red, float blue, float green) : solid_color(Color3f(red, blue, green)) {}

	Color3f value(float u, float v, const Vec3f& p) const override {
		return albedo;
	}

private:
	Color3f albedo;
};

class checker_texture : public texture {
public:

	checker_texture(float scale, shared_ptr<texture> even, shared_ptr<texture> odd) :
		inv_scale(1.f / scale), even(even), odd(odd) {}

	checker_texture(float scale, const Color3f& c1, const Color3f& c2) :
		inv_scale(1.f / scale), even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	Color3f value(float u, float v, const Vec3f& p) const override {
		int xInt = int(std::floor(inv_scale * p.x));
		int yInt = int(std::floor(inv_scale * p.y));
		int zInt = int(std::floor(inv_scale * p.z));

		bool isEven = (xInt + yInt + zInt) % 2 == 0;

		return isEven ? even->value(u, v, p) : odd->value(u, v, p);
	}

private:
	float inv_scale;
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

class image_texture : public texture {
public:
	image_texture(const char* filename) : image(filename) {}

	Color3f value(float u, float v, const Vec3f& p) const override {
		if (image.height() <= 0) return Color3f(0, 1, 1);

		u = interval(0.f, 1.f).clamp(u);
		v = 1.0f - interval(0.f, 1.f).clamp(v);

		int i = int(u * image.width());
		int j = int(v * image.height());
		const unsigned char* pixel = image.pixel_data(i, j);

		float color_scale = 1.f / 255.f;
		return Color3f(pixel[0] * color_scale, pixel[1] * color_scale, pixel[2] * color_scale);
	}

private:
	read_image image;
};

class noise_texture : public texture {
public:
	noise_texture() {}

	noise_texture(float scale) : scale(scale) {}

	Color3f value(float u, float v, const Vec3f& p) const override {
		return Color3f(.5f) * (1.f + sin(scale * p.z + 10.f * noise.turb(p, 7)));
	}
private:
	perlin noise;
	float scale;
};