#pragma once

#include "../math/math.h"
#include <limits>

template <size_t N, typename T>
struct Ray {
	
	static constexpr float epsilon = T(0.0001);
	static constexpr float infinity = std::numeric_limits<T>::infinity();

	Vec<N, T> o;
	Vec<N, T> d;
	T mint;
	T maxt;

	Ray() : mint(epsilon), maxt(inifinity) {}

	Ray(const Vec<N, T>& o, const Vec<N, T>& d, T mint = Ray::epsilon, T maxt = Ray::infinity) :
		o(o), d(d), mint(mint), maxt(maxt) {}

	Ray(const Ray& ray, T mint, T maxt) : o(ray.o), d(ray.d), mint(mint), maxt(maxt) {}

	Vec<N, T> operator()(T t) const {
		return o + t * d;
	}
};

template<T>
using Ray2 = Ray<2, T>;
template<T>
using Ray3 = Ray<3, T>;

using Ray2f = Ray2<float>;
using Ray2d = Ray2<double>;

using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;
