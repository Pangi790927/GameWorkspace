#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include "Texture.h"

#include <memory>
#include <cstring>

/// stb: not my library, credit goes to: Sean T. Barrett 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace TextureLoader {
	Texture load (std::string filename) {
		Texture rawTexture;

		unsigned char *data = stbi_load(
			filename.c_str(), 
				&rawTexture.width, 
			&rawTexture.height, 
			&rawTexture.pixelSize,
				0
			);

		if (!data)
			EXCEPTION("Could not load image: %s", filename.c_str());

		rawTexture.alocateSpace();
		std::copy(data, data + rawTexture.dataSize(), rawTexture.textureData.get()); 
		stbi_image_free(data);

		rawTexture.generateOpenGLTexture();
		return rawTexture; 
	}
}


#endif