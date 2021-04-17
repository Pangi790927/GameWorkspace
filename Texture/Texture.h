#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <cstring>

#include "MathLib.h"

class Texture {
public:
	enum {
		TEX_LINEAR = 1,
		TEX_REPEAT = 2,
		TEX_NEAREST = 4,
		TEX_DEFAULT = 8,
	};

	template <typename Type>
	struct array_deleter {
		void operator () ( Type const * p) {
			delete[] p;
		}
	};

	int width; 
	int height; 
	int pixelSize;
	
	/* TODO: add code to destroy texture */
	GLuint openglTexture = 0; 
	std::shared_ptr<unsigned char> textureData; 

	Texture (int width = 0, int height = 0, int pixelSize = 3) 
	: width(width), height(height), pixelSize(pixelSize)
	{
		if (width != 0 && height != 0) {
			alocateSpace(width, height, pixelSize);	
		}
	}

	unsigned char& get_r (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 0];
	}

	unsigned char& get_g (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 1];
	}

	unsigned char& get_b (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 2];
	}

	unsigned char& get_a (int row, int col) {
		return textureData.get()[(row * width + col) * pixelSize + 3];
	}

	Math::Point3f get_rgb(int row, int col) {
		return Math::Point3f(
			get_r(row, col) / 255.,
			get_g(row, col) / 255.,
			get_b(row, col) / 255.
		);
	}

	Math::Point4f get_rgba(int row, int col) {
		return Math::Point4f(
			get_r(row, col) / 255.,
			get_g(row, col) / 255.,
			get_b(row, col) / 255.,
			get_a(row, col) / 255.
		);
	}

	void set_rgb(int row, int col, Math::Point3f color) {
		get_r(row, col) = color.x * 255.;
		get_g(row, col) = color.y * 255.;
		get_b(row, col) = color.z * 255.;
	}

	void set_rgba(int row, int col, Math::Point4f color) {
		get_r(row, col) = color.x * 255.;
		get_g(row, col) = color.y * 255.;
		get_b(row, col) = color.z * 255.;
		get_a(row, col) = color.w * 255.;
	}

	void alocateSpace(int width, int height, int pixelSize = 3) {
		this->width = width;
		this->height = height;
		this->pixelSize = pixelSize;
		alocateSpace();
	}

	void alocateSpace() {
		if (width % 4 != 0)
			width = width + (4 - width % 4);
		textureData = std::shared_ptr<unsigned char>(
				new unsigned char[width * height * pixelSize],
				array_deleter<unsigned char>());
		memset(textureData.get(), width * height * pixelSize, 0);
	}

	int dataSize() {
		return width * height * pixelSize;
	}

	void bind() {
		glBindTexture(GL_TEXTURE_2D, openglTexture);
	}

	void generateOpenGLTexture(int flags = TEX_DEFAULT) {
		glGenTextures(1, &openglTexture);

		glBindTexture(GL_TEXTURE_2D, openglTexture);

		if (flags & TEX_DEFAULT) {
			flags = TEX_NEAREST | TEX_REPEAT;
		}

		if (flags & TEX_REPEAT) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);	
		}
		if (flags & TEX_LINEAR) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		if (flags & TEX_NEAREST) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		GLint format = GL_RGB;

		if (pixelSize == 0) {
			throw std::runtime_error("Invalid Pixel Format");
		}
		else if (pixelSize == 1) {
			format = GL_RED;
		}
		else if (pixelSize == 2) {
			format = GL_RG;
		}
		else if (pixelSize == 3) {
			format = GL_RGB;
		}
		else if (pixelSize == 4) {
			format = GL_RGBA;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format,
			width,
			height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			textureData.get()
		);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void destroyTexture() {
		glDeleteTextures(1, &openglTexture);
	}

	~Texture() {
	}
}; 

#endif