#pragma once

#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb/stb_image.h>

#include <cstdlib>
#include <iostream>

class read_image {
public:
	read_image() {}

	read_image(const char* image_filename) {
		// Loads image data from the specified file. If the RTW_IMAGES environment variable is
		// defined, looks only in that directory for the image file. If the image was not found,
		// searches for the specified image file first from the current directory, then in the
		// images/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
		// parent, on so on, for six levels up. If the image was not loaded successfully,
		// width() and height() will return 0.

		std::string filename = std::string(image_filename);
		auto image_directory = getenv("RTW_IMAGES");

		// Hunt for the image file in some likely locations.
		if (image_directory && load(std::string(image_directory) + "/" + image_filename)) return;
		if (load(filename)) return;
		if (load("images/" + filename)) return;
		if (load("../images/" + filename)) return;
		if (load("../../images/" + filename)) return;
		if (load("../../../images/" + filename)) return;
		if (load("../../../../images/" + filename)) return;
		if (load("../../../../../images/" + filename)) return;
		if (load("../../../../../../images/" + filename)) return;

		std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
	}

	~read_image() {
		delete[] bdata;
		STBI_FREE(fdata);
	}

	bool load(const std::string& filename) {
		// Loads the linear (gamma=1) image data from the given file name. Returns true if the
		// load succeeded. The resulting data buffer contains the three [0.0, 1.0]
		// floating-point values for the first pixel (red, then green, then blue). Pixels are
		// contiguous, going left to right for the width of the image, followed by the next row
		// below, for the full height of the image.

		int n = bytes_per_pixel;
		fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
		if (fdata == nullptr) false;

		bytes_per_scanline = image_width * bytes_per_pixel;
		convert_to_bytes();
		return true;
	}

	int width()  const { return (fdata == nullptr) ? 0 : image_width; }
	int height() const { return (fdata == nullptr) ? 0 : image_height; }

	const unsigned char* pixel_data(int x, int y) const{
		// Return the address of the three RGB bytes of the pixel at x,y. If there is no image
		// data, returns magenta.
		static unsigned char magenta[] = { 255, 0, 255 };
		if (bdata == nullptr) return magenta;

		x = clamp(x, 0, image_width);
		y = clamp(y, 0, image_height);

		return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
	}

private:
	const int		bytes_per_pixel = 3;
	float			*fdata = nullptr;
	unsigned char	*bdata = nullptr;
	int				image_width = 0;
	int				image_height = 0;
	int				bytes_per_scanline = 0;

	static int clamp(int x, int low, int high) {
		if (x < low) return low;
		if (x < high) return x;
		return high - 1;
	}

	static unsigned char float_to_bytes(float value) {
		if (value <= 0.f)
			return 0;
		if (value >= 1.f)
			return 255;
		return static_cast<unsigned int>(265.0f * value);
	}

	void convert_to_bytes() {
		// Convert the linear floating point pixel data to bytes, storing the resulting byte
		// data in the `bdata` member.

		int total_bytes = image_width * image_height * bytes_per_pixel;
		bdata = new unsigned char[total_bytes];

		auto *fptr  = fdata;
		auto *bptr = bdata;

		for (int i = 0; i < total_bytes; i++, fptr++, bptr++)
			*bptr = float_to_bytes(*fptr);
	}
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif