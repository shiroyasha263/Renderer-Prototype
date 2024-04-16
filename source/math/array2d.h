#pragma once

#include "math.h"
#include <utility>
#include <vector>

// Resizable 2D class
template<typename T>
class Array2D {
public:
	Array2D();
	Array2D(int size_x, int size_y, const T& value = T(0.));
	Array2D(const Array2D<T>&);
	Array2D& operator = (const Array2D&);
	~Array2D();

	// Element accessing functions
	T		&at(int x, int y);
	const T &at(int x, int y) const;
	T		&operator()(int x, int y);
	const T	&operator()(int x, int y) const;
	T		&at(int i);
	const T &at(int i) const;
	T		&operator()(int i);
	const T	&operator()(int i) const;
	const T *row(int y) const;

	int index_1D(int x, int y) const {
		return y * m_size.x + x;
	}

	std::pair<int, int> index_2D(int i) const {
		return std::make_pair(i % m_size.x, i / m_size.x);
	}

	// Access size related variables
	int width() const { return m_size.x; }
	int height() const { return m_size.y; }
	Vec2i size() const { return m_size; }
	int length() const { return m_size.x * m_size.y; }

	void resize(const Vec2i& size);
	void reset(const T& value = T(0));
	void operator=(const T&);

protected:
	std::vector<T> m_data;
	Vec2i		   m_size;
};

template<typename T>
inline Array2D<T>::Array2D() : m_data(0), m_size(0, 0) {}

template<typename T>
inline Array2D<T>::Array2D(int size_x, int size_y, const T& value) : m_data(size_x * size_y, value), m_size(size_x, size_y) {}

template <class T>
Array2D<T>::Array2D(const Array2D<T>& other) : m_data(other.m_data), m_size(other.m_size) {}

template <class T>
Array2D<T>& Array2D<T>::operator=(const Array2D<T>& other) {
	m_size = other.m_size;
	m_data = other.m_data;
	return *this;
}

template <typename T>
inline Array2D<T>::~Array2D() {}

template <typename T>
inline T& Array2D<T>::operator()(int x, int y) {
	return m_data[index_1D(x, y)];
}

template <typename T>
inline const T& Array2D<T>::operator()(int x, int y) const {
	return m_data[index_1D(x, y)];
}

template <typename T>
inline T& Array2D<T>::at(int x, int y) {
	return m_data.at(index_1D(x, y));
}

template <typename T>
inline const T& Array2D<T>::at(int x, int y) const {
	return m_data.at(index_1D(x, y));
}

template <typename T>
inline T& Array2D<T>::operator()(int i) {
	return m_data[i];
}

template <typename T>
inline const T& Array2D<T>::operator()(int i) const {
	return m_data[i];
}

template <typename T>
inline T& Array2D<T>::at(int i) {
	return m_data.at(i);
}

template <typename T>
inline const T& Array2D<T>::at(int i) const {
	return m_data.at(i);
}

template <typename T>
inline const T* Array2D<T>::row(int y) const {
	return &m_data[index_1D(0, y)];
}

template <typename T>
inline void Array2D<T>::resize(const Vec2i& size) {
	if (size == m_size)
		return;
	m_data.resize(size.x * size.y);
	m_size = size;
}

template <typename T>
inline void Array2D<T>::reset(const T& value) {
	for (auto& e : m_data)
		e = value;
}

template <typename T>
inline void Array2D<T>::operator=(const T& value) {
	reset(value);
}