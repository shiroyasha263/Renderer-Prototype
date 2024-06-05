#pragma once

class interval {
public:
	float min, max;

	interval() : min(+infinity), max(-infinity) {}

	interval(float min, float max) : min(min), max(max) {}

	interval(const interval& a, const interval& b) {
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	bool contains(float x) const {
		return min <= x && max >= x;
	}

	bool surrounds(float x) const {
		return min < x && max > x;
	}

	float clamp(float x) const {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	interval expand(float delta) {
		float padding = delta / 2.f;
		return interval(min - padding, max + padding);
	}

	float size() {
		return max - min;
	}

	static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);