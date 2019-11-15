#include "imageio.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb\stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <stb\stb_image_write.h>

#define TINYEXR_IMPLEMENTATION
#include <tinyexr\tinyexr.h>

namespace pol {

	bool ImageIO::LoadTexture(const char* filename, int& width, int& height, bool srgb, bool flip, vector<Vector3f>& output) {
		int component;
		stbi_set_flip_vertically_on_load(flip);
		unsigned char* tex = stbi_load(filename, &width, &height, &component, 0);
		if (tex) {
			output.resize(width * height);
			float inv = 1.f / 255.f;
			for (int s = 0; s < height; ++s) {
				for (int t = 0; t < width; ++t) {
					//flip
					int idx = s * width + t;
					//int inverseIdx = (height - s - 1)*width + t;

					Vector3f texel;
					if (component == 1) {
						float r = tex[idx] * inv;
						texel = Vector3f(r, r, r);
					}
					else if (component == 3) {
						float r = tex[3 * idx] * inv;
						float g = tex[3 * idx + 1] * inv;
						float b = tex[3 * idx + 2] * inv;
						texel = Vector3f(r, g, b);
					}
					else if (component == 4) {
						float r = tex[4 * idx] * inv;
						float g = tex[4 * idx + 1] * inv;
						float b = tex[4 * idx + 2] * inv;
						float a = tex[4 * idx + 3] * inv;
						texel = Vector3f(r, g, b);
					}

					//convert from srgb space to linear space
					if (srgb)
						output[idx] = Vector3f(powf(texel.X(), 2.2f), powf(texel.Y(), 2.2f), powf(texel.Z(), 2.2f));
					else
						output[idx] = texel;
				}
			}

			stbi_image_free(tex);
		}
		else {
			fprintf(stderr, "Error when load texture [%s]\n", filename);
			return false;
		}

		return true;
	}

	bool ImageIO::SavePng(const char* filename, int width, int height, const vector<Vector3f>& input) {
		unsigned char* transform = new unsigned char[width * height * 3];
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				unsigned pixel = i * width + j;
				unsigned inverse = (height - i - 1) * width + j;
				transform[3 * pixel] = unsigned(Clamp(input[inverse].X(), 0.f, 1.f) * 255.f);
				transform[3 * pixel + 1] = unsigned(Clamp(input[inverse].Y(), 0.f, 1.f) * 255.f);
				transform[3 * pixel + 2] = unsigned(Clamp(input[inverse].Z(), 0.f, 1.f) * 255.f);
			}
		}

		stbi_write_png(filename, width, height, 3, transform, 0);

		delete[] transform;

		return true;
	}

	bool ImageIO::SavePng(const char* filename, int width, int height, const Vector3f* input) {
		unsigned char* transform = new unsigned char[width * height * 3];
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				unsigned pixel = i * width + j;
				unsigned inverse = (height - i - 1) * width + j;
				transform[3 * pixel] = unsigned(Clamp(input[inverse].X(), 0.f, 1.f) * 255.f);
				transform[3 * pixel + 1] = unsigned(Clamp(input[inverse].Y(), 0.f, 1.f) * 255.f);
				transform[3 * pixel + 2] = unsigned(Clamp(input[inverse].Z(), 0.f, 1.f) * 255.f);
			}
		}

		stbi_write_png(filename, width, height, 3, transform, 0);

		delete[] transform;

		return true;
	}

	bool ImageIO::LoadExr(const char* filename, int& width, int& height, vector<Vector3f>& output) {
		const char* err = NULL; // or nullptr in C++11

		float* out;
		int ret = LoadEXR(&out, &width, &height, filename, &err);

		if (ret != TINYEXR_SUCCESS) {
			if (err) {
				fprintf(stderr, "ERR : %s\n", err);
				FreeEXRErrorMessage(err); // release memory of error message.
				return false;
			}
		}
		else {
			output.resize(width*height);
			for (int i = 0; i < width*height; ++i){
				output[i] = Vector3f(out[4 * i + 0], out[4 * i + 1], out[4 * i + 2]);
			}
			free(out); // relase memory of image data
		}

		return true;
	}

	bool ImageIO::SaveExr(const char* filename, int width, int height, const vector<Vector3f>& input) {
		EXRHeader header;
		InitEXRHeader(&header);

		EXRImage image;
		InitEXRImage(&image);

		image.num_channels = 3;

		std::vector<float> images[3];
		images[0].resize(width * height);
		images[1].resize(width * height);
		images[2].resize(width * height);

		// Split RGBRGBRGB... into R, G and B layer
		for (int i = 0; i < width * height; i++) {
			images[0][i] = input[i].X();
			images[1][i] = input[i].Y();
			images[2][i] = input[i].Z();
		}

		float* image_ptr[3];
		image_ptr[0] = &(images[2].at(0)); // B
		image_ptr[1] = &(images[1].at(0)); // G
		image_ptr[2] = &(images[0].at(0)); // R

		image.images = (unsigned char**)image_ptr;
		image.width = width;
		image.height = height;

		header.num_channels = 3;
		header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
		// Must be (A)BGR order, since most of EXR viewers expect this channel order.
		strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
		strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
		strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

		header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
		header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
		for (int i = 0; i < header.num_channels; i++) {
			header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
			header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
		}

		const char* err = NULL; // or nullptr in C++11 or later.
		int ret = SaveEXRImageToFile(&image, &header, filename, &err);
		if (ret != TINYEXR_SUCCESS) {
			fprintf(stderr, "Save EXR err: %s\n", err);
			FreeEXRErrorMessage(err); // free's buffer for an error message 
			return false;
		}
		printf("Saved exr file. [ %s ] \n", filename);

		free(header.channels);
		free(header.pixel_types);
		free(header.requested_pixel_types);

		return true;
	}
}