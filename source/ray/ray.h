#pragma once

#include "../math/vecmath.h"
#include <limits>

template <typename T, size_t N>
struct Ray {
	static constexpr float epsilon = T(0.0001);
	static constexpr float infinity = std::numeric_limits<T>::infinity();

	Vec<T, N> origin;
	Vec<T, N> direction;
	T		  minT;
	T		  maxT;

	Ray() : minT(epsilon), maxT(infinity) {
	
	}

	Ray(const Vec<T, N>& origin, const Vec<T, N>& direction, T minT = Ray::epsilon, T maxT = Ray::infinity) :
		origin(origin), direction(direction), minT(minT), maxT(maxT) {

	}

	Ray(const Ray<T, N>& ray, T minT = Ray::epsilon, T maxT = Ray::infinity) :
		origin(ray.origin), direction(ray.direction), minT(minT), maxT(maxT) {

	}

	Vec<T, N> operator()(T t) const {
		return origin + t * direction;
	}

	Vec<T, N> at(T t) const {
		return origin + t * direction;
	}
};

template <typename T>
using Ray2 = Ray<T, 2>;
template <typename T>
using Ray3 = Ray<T, 3>;

using Ray2f = Ray2<float>;
using Ray2d = Ray2<double>;

using Ray3f = Ray3<float>;
using Ray3d = Ray3<double>;