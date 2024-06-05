#pragma once

#include "../common/common.h"

class perlin {
public:

	perlin() {
		randvec = new Vec3f[point_count];
		
		for (int i = 0; i < point_count; i++)
			randvec[i] = normalize(random_Vec3f(-1, 1));

		permx = perlin_generate_perm();
		permy = perlin_generate_perm();
		permz = perlin_generate_perm();
	}

	~perlin() {
		delete[] randvec;
		delete[] permx;
		delete[] permy;
		delete[] permz;
	}

	float noise(const Vec3f& p) const {
		float u = p.x - floor(p.x);
		float v = p.y - floor(p.y);
		float w = p.z - floor(p.z);
		
		auto i = int(floor(p.x));
		auto j = int(floor(p.y));
		auto k = int(floor(p.z));
		Vec3f c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
					c[di][dj][dk] = randvec[
						permx[(i + di) & 255] ^ 
						permy[(j + dj) & 255] ^ 
						permz[(k + dk) & 255]
					];

		return perlin_interp(c, u, v, w);
	}

	float turb(const Vec3f& p, int depth) const {
		float accum = 0.f;
		Vec3f temp_p = p;
		float weight = 1.f;

		for (int i = 0; i < depth; i++) {
			accum += weight * noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}

		return fabs(accum);
	}

private:
	static const int point_count = 256;
	Vec3f* randvec;
	int* permx;
	int* permy;
	int* permz;

	static int* perlin_generate_perm() {
		int* p = new int[point_count];

		for (int i = 0; i < point_count; i++)
			p[i] = i;

		permute(p, point_count);
		return p;
	}

	static void permute(int* p, int n) {
		for (int i = n - 1; i > 0; i--) {
			int target = random_int(0, i);
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}

	static float perlin_interp(const Vec3f c[2][2][2], float u, float v, float w) {
		float uu = u * u * (3.f - 2.f * u);
		float vv = v * v * (3.f - 2.f * v);
		float ww = w * w * (3.f - 2.f * w);

		float accum = 0.f;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					Vec3f weight_vec(u - i, v - j, w - k);
					accum += (i * uu + (1.f - i) * (1.f - uu))
						   * (j * vv + (1.f - j) * (1.f - vv))
						   * (k * ww + (1.f - k) * (1.f - ww))
						   * dot(c[i][j][k], weight_vec);
				}
		return accum;
	}
};