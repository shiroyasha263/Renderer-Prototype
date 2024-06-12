#pragma once

#include <linalg/linalg.h>
#include <algorithm>
#include <cstdlib>

#undef M_PI
#define M_PI         3.14159265358979323846f ///< \f$\pi\f$
#define INV_PI       0.31830988618379067154f ///< \f$\frac{1}{\pi}\f$
#define INV_TWOPI    0.15915494309189533577f ///< \f$\frac{1}{2\pi}\f$
#define INV_FOURPI   0.07957747154594766788f ///< \f$\frac{1}{4\pi}\f$
#define SQRT_TWO     1.41421356237309504880f ///< \f$\sqrt{2}\f$
#define INV_SQRT_TWO 0.70710678118654752440f ///< \f$\frac{1}{\sqrt{2}}\f$

namespace la = linalg;

template <class T, int N>
using Vec = la::vec<T, N>;
template <class T, int M, int N>
using Mat = la::mat<T, M, N>;
template <class T, int N>
using Color = la::vec<T, N>;


template <class T>
using Vec1 = Vec<T, 1>;
template <class T>
using Vec2 = Vec<T, 2>;
template <class T>
using Vec3 = Vec<T, 3>;
template <class T>
using Vec4 = Vec<T, 4>;


template <class T>
using Color3 = Color<T, 3>;
template <class T>
using Color4 = Color<T, 4>;

using Vec1f = Vec1<float>;
using Vec1d = Vec1<double>;
using Vec1i = Vec1<std::int32_t>;
using Vec1u = Vec1<std::uint32_t>;
using Vec1c = Vec1<std::uint8_t>;

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<std::int32_t>;
using Vec2u = Vec2<std::uint32_t>;
using Vec2c = Vec2<std::uint8_t>;

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<std::int32_t>;
using Vec3u = Vec3<std::uint32_t>;
using Vec3c = Vec3<std::uint8_t>;

using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
using Vec4i = Vec4<std::int32_t>;
using Vec4u = Vec4<std::uint32_t>;
using Vec4c = Vec4<std::uint8_t>;

using Color3f = Color3<float>;
using Color3d = Color3<double>;
using Color3u = Color3<std::uint32_t>;
using Color3c = Color3<std::uint8_t>;

using Color4f = Color4<float>;
using Color4d = Color4<double>;
using Color4u = Color4<std::uint32_t>;
using Color4c = Color4<std::uint8_t>;

template <class T>
using Mat22 = Mat<T, 2, 2>;
template <class T>
using Mat33 = Mat<T, 3, 3>;
template <class T>
using Mat44 = Mat<T, 4, 4>;

using Mat22f = Mat22<float>;
using Mat33f = Mat33<float>;
using Mat44f = Mat44<float>;
using Mat22d = Mat22<double>;
using Mat33d = Mat33<double>;
using Mat44d = Mat44<double>;

template <class T, int N>
inline std::ostream& operator<< (std::ostream& out, const Vec<T, N>& v) {
	for (int i = 0; i < N; i++)	{
		if (i == N - 1)
			out << v[i];
		else
			out << v[i] << ' ';
	}
	return out;
}

template <class T, int M, int N>
inline std::ostream& operator<< (std::ostream& out, const Mat<T, M, N>& m) {
	for (int i = 0; i < M; i++)	{
		out << m[i] << '\n';
	}
	return out;
}

inline Vec3f random_Vec3f() {
	return Vec3f(random_float(), random_float(), random_float());
}

inline Vec3f random_Vec3f(float min, float max) {
	return Vec3f(random_float(min, max), random_float(min, max), random_float(min, max));
}

inline Vec3f random_Vec3f_in_unitsphere() {
	while (true) {
		Vec3f random = random_Vec3f(-1.f, 1.f);
		if (length2(random) < 1.f)
			return random;
	}
}

inline Vec3f random_Vec3f_unit_vector() {
	return normalize(random_Vec3f_in_unitsphere());
}

inline Vec3f random_Vec3f_on_hemisphere(const Vec3f& normal) {
	Vec3f on_unit_sphere = random_Vec3f_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.f)
		return on_unit_sphere;
	else
		return -on_unit_sphere;
}

inline bool near_zero(const Vec3f& v) {
	float s = 1e-8;
	return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

inline Vec3f reflect(const Vec3f& v, const Vec3f& n) {
	return v - 2 * dot(v, n) * n;
}

inline Vec3f refract(const Vec3f& v, const Vec3f& n, float etai_over_etat) {
	float cos_theta = fmin(dot(-v, n), 1.0);
	Vec3f r_out_perp = etai_over_etat * (v + cos_theta * n);
	Vec3f r_out_parallel = -sqrt(fabs(1.0f - length2(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

inline Vec3f random_Vec3f_in_unit_disk() {
	while (true) {
		Vec3f p = Vec3f(random_float(-1.f, 1.f), random_float(-1.f, 1.f), 0.f);
		if (length2(p) < 1.f)
			return p;
	}
}

inline Mat44f vector_translate(const Vec3f& v) {
	return Mat44f(
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ v.x, v.y, v.z, 1.f });
}

inline Mat44f rotate_along_x(float degrees) {
	float theta = deg_to_rad(degrees);
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	return Mat44f(
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, cos_theta, sin_theta, 0.f },
		{ 0.f, -sin_theta, cos_theta, 0.f },
		{ 0.f, 0.f, 0.f, 1.f });
}

inline Mat44f rotate_along_y(float degrees) {
	float theta = deg_to_rad(degrees);
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	return Mat44f(
		{ cos_theta, 0.f, -sin_theta, 0.f },
		{ 0.f, 1.f, 0.f, 0.f },
		{ sin_theta, 0.f, cos_theta, 0.f },
		{ 0.f, 0.f, 0.f, 1.f });
}

inline Mat44f rotate_along_z(float degrees) {
	float theta = deg_to_rad(degrees);
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);
	return Mat44f(
		{ cos_theta, sin_theta, 0.f, 0.f },
		{ -sin_theta, cos_theta, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f });
}

inline Mat44f scale(const Vec3f& s) {
	return Mat44f(
		{ s.x, 0.f, 0.f, 0.f },
		{ 0.f, s.y, 0.f, 0.f },
		{ 0.f, 0.f, s.z, 0.f },
		{ 0.f, 0.f, 0.f, 1.f });
}