#pragma once

#include "../common/vecmath.h"

enum FitResolutionGate { kFill = 0, kOverscan };

class Camera {
public:

	Camera() {
		focalLength = 55; // in mm // 35mm Full Aperture in inches
		filmApertureWidth = 0.980;
		filmApertureHeight = 0.735;
		nearClippingPlane = 0.1;
		farClippingPlane = 1000;
		// image resolution in pixels
		imageWidth = 640;
		imageHeight = 480;
		fitFilm = kOverscan;
		Update();
	}

	void Update() {

		const float inchToMm = 25.4f;

		filmAspectRatio = filmApertureWidth / filmApertureHeight;
		deviceAspectRatio = imageWidth / (float)imageHeight;

		top = ((filmApertureHeight * inchToMm / 2) / focalLength) * nearClippingPlane;
		right = ((filmApertureWidth * inchToMm / 2) / focalLength) * nearClippingPlane;

		float xscale = 1;
		float yscale = 1;

		switch (fitFilm) {
		default:
		case kFill:
			if (filmAspectRatio > deviceAspectRatio) {
				xscale = deviceAspectRatio / filmAspectRatio;
			}
			else {
				yscale = filmAspectRatio / deviceAspectRatio;
			}
			break;
		case kOverscan:
			if (filmAspectRatio > deviceAspectRatio) {
				yscale = filmAspectRatio / deviceAspectRatio;
			}
			else {
				xscale = deviceAspectRatio / filmAspectRatio;
			}
			break;
		}

		right *= xscale;
		top *= yscale;

		bottom = -top;
		left = -right;
	}

	float focalLength;
	float filmApertureWidth;
	float filmApertureHeight;
	float nearClippingPlane;
	float farClippingPlane;
	uint32_t imageWidth;
	uint32_t imageHeight;
	FitResolutionGate fitFilm;
	float filmAspectRatio;
	float deviceAspectRatio;
	float top, right, bottom, left;
};