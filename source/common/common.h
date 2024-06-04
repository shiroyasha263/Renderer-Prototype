#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>

using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using json = nlohmann::json;

const float epsilon = 0.001;
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

inline float deg_to_rad(float degrees) {
	return (degrees * pi) / 180;
}

// Returns a real random number from [0, 1)
inline float random_float() {
	static std::uniform_real_distribution<float> distribution(0.f, 1.f);
	static std::mt19937 generator;
	return distribution(generator);
}

// Returns a real random number from [min, max)
inline float random_float(float min, float max) {
	return min + (max - min) * random_float();
}

#include "../math/vecmath.h"
#include "../ray/ray.h"
#include "interval.h"


inline float linear_to_gamma(float linear_component) {
	if (linear_component > 0)
		return sqrt(linear_component);
	return 0;
}

void write_color(std::ostream& out, const Color3f& color) {
	auto r = color.x;
	auto g = color.y;
	auto b = color.z;

	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	static const interval intensity(0.f, 0.999f);
	int rbyte = int(255.999 * intensity.clamp(r));
	int gbyte = int(255.999 * intensity.clamp(g));
	int bbyte = int(255.999 * intensity.clamp(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

template <typename T, int N>
inline void json_to_vector(const json& j, Vec<T, N>& v) {
	if (j.size() == N)
		for (int i = 0; i < N; i++)
			v[i] = j[i];
	else
		std::clog << "JSON length is probably wrong, check your json file" << '\n';
}