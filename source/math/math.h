#pragma once

#include "linalg/linalg.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

#if defined(_WIN32)
#pragma warning(disable : 4305) // double constant assigned to float
#pragma warning(disable : 4244) // int -> float conversion
#pragma warning(disable : 4843) // double -> float conversion
#pragma warning(disable : 4267) // size_t -> int
#pragma warning(disable : 4838) // another double -> int
#endif

//Define the Vectors and Matrices that will be used

namespace la = linalg;

template <int N, class T>
using Vec = la::vec<T, N>;
template <class T, int M, int N>
using Mat = la::mat<T, M, N>;
template<int N, class T>
using Color = la::vec<T, N>;

template<class T>
using Vec1 = Vec<1, T>;
template<class T>
using Vec2 = Vec<2, T>;
template<class T>
using Vec3 = Vec<3, T>;
template<class T>
using Vec4 = Vec<4, T>;

template<class T>
using Color3 = Color<3, T>;
template<class T>
using Color4 = Color<4, T>;

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
using Color3c = Color3<uint8_t>;

using Color4f = Color4<float>;
using Color4d = Color4<double>;
using Color4u = Color4<std::uint32_t>;
using Color4c = Color4<uint8_t>;

template<class T>
using Mat22 = Mat<T, 2, 2>;
template<class T>
using Mat33 = Mat<T, 3, 3>;
template<class T>
using Mat44 = Mat<T, 4, 4>;

using Mat22f = Mat22<float>;
using Mat22d = Mat22<double>;
using Mat33f = Mat33<float>;
using Mat33d = Mat33<double>;
using Mat44f = Mat44<float>;
using Mat44d = Mat44<double>;


//
// Some useful constants
//

#undef M_PI
#define M_PI         3.14159265358979323846f ///< \f$\pi\f$
#define INV_PI       0.31830988618379067154f ///< \f$\frac{1}{\pi}\f$
#define INV_TWOPI    0.15915494309189533577f ///< \f$\frac{1}{2\pi}\f$
#define INV_FOURPI   0.07957747154594766788f ///< \f$\frac{1}{4\pi}\f$
#define SQRT_TWO     1.41421356237309504880f ///< \f$\sqrt{2}\f$
#define INV_SQRT_TWO 0.70710678118654752440f ///< \f$\frac{1}{\sqrt{2}}\f$

//
// Some useful functions
//

template<typename T>
inline T pow2(T x) {
	return x * x;
}

template<typename T>
inline T pow3(T x) {
	return x * x * x;
}

template<typename T>
inline T pow4(T x) {
	T x2 = x * x;
	return x2 * x2;
}

template<typename T>
inline T pow5(T x) {
	T x2 = x * x;
	return x2 * x2 * x;
}

template<typename T>
inline T sqr(T x) {
	return pow2(x);
}

template<typename T>
inline T cube(T x) {
	return pow3(x);
}


//
// Allow applying isfinite and isNan to vectors
//

namespace linalg {
	namespace detail {
		
		struct std_isfinite {
			template <class A>
			auto operator()(A a) const -> decltype(std::isfinite(a)) {
				return std::isfinite(a);
			}
		};

		struct std_isnan {
			template <class A>
			auto operator()(A a) const -> decltype(std::isnan(a)) {
				return std::isnan(a);
			}
		};
	}

	template <class A>
	apply_t <detail::std_isfinite, A> isfinite(const A& a) {
		return apply(detail::std_isfinite{}, a);
	}
	template <class A>
	apply_t <detail::std_isnan, A> isnan(const A& a) {
		return apply(detail::std_isnan{}, a);
	}
}

/**
	Linear interpolation.

	Linearly interpolates between \p a and \p b, using parameter \p t.

	\tparam T    type for start and end points, and return value
	\tparam S    type for interpolation parameter
	\param a     Start point
	\param b     End point
	\param t     A blending factor of \p a and \p b.
	\return      Linear interpolation of \p a and \p b -
				 a value between \p a and \p b if \p t is between 0 and 1.
*/
using la::lerp;

/// Always-positive modulo operation
template <typename T>
inline T mod(T a, T b)
{
	int n = (int)(a / b);
	a -= n * b;
	if (a < 0)
		a += b;
	return a;
}

/**
	Construct an orthonormal coordinate system given one vector \p a.

	\param [in]  z  The coordinate system's local z axis direction.
	\return         The local x and y-axes orthogonal to z.
*/
template <typename T>
std::pair<Vec3<T>, Vec3<T>> coordinate_system(const Vec3<T>& z)
{
	if (std::abs(z.x) > std::abs(z.y))
	{
		T       inv_len = T(1) / std::sqrt(z.x * z.x + z.z * z.z);
		Vec3<T> y{ z.z * inv_len, 0, -z.x * inv_len };
		return { cross(y, z), y };
	}
	else
	{
		T       inv_len = T(1) / std::sqrt(z.y * z.y + z.z * z.z);
		Vec3<T> y{ 0, z.z * inv_len, -z.y * inv_len };
		return { cross(y, z), y };
	}
}

//
// Color Utilities
//

/// Convert from linear RGB to sRGB
inline Color3f to_sRGB(const Color3f& c)
{
	return la::select(la::lequal(c, 0.0031308f), c * 12.92f, (1.0f + 0.055f) * la::pow(c, 1.0f / 2.4f) - 0.055f);
}

/// Convert from sRGB to linear RGB
inline Color3f to_linear_RGB(const Color3f& c)
{
	return la::select(la::lequal(c, 0.04045f), c * (1.0f / 12.92f), la::pow((c + 0.055f) * (1.0f / 1.055f), 2.4f));
}

/// Return the associated luminance
inline float luminance(const Color3f& c)
{
	return dot(c, { 0.212671f, 0.715160f, 0.072169f });
}

inline float luminance(const Color4f& c)
{
	return luminance(c.xyz());
}

inline float rad2degree(const float& rad)
{
	return rad * 180.f * INV_PI;
}

inline float deg2rad(const float& deg)
{
	return deg * M_PI / 180.f;
}

// Matplotlib-style false-color maps
Color3f viridis(float t);
Color3f inferno(float t);
Color3f magma(float t);
Color3f plasma(float t);

template <class C, int N, class T>
std::basic_ostream<C>& operator<<(std::basic_ostream<C>& out, const Vec<N, T>& v)
{
	std::ios_base::fmtflags oldFlags = out.flags();
	auto                    width = out.precision() + 2;

	out.setf(std::ios_base::right);
	if (!(out.flags() & std::ios_base::scientific))
		out.setf(std::ios_base::fixed);
	width += 5;

	out << '{';
	for (size_t i = 0; i < N - 1; ++i)
		out << std::setw(width) << v[i] << ',';
	out << std::setw(width) << v[N - 1] << '}';

	out.flags(oldFlags);
	return out;
}

template <class C, class T>
std::basic_ostream<C>& operator<<(std::basic_ostream<C>& s, const Mat44<T>& m)
{
	return s << "{" << m[0] << ",\n " << m[1] << ",\n " << m[2] << ",\n " << m[3] << "}";
}