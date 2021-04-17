#ifndef MTLLOADER_H_INCLUDED
#define MTLLOADER_H_INCLUDED

#include <map>
#include "TextureLoader.h"

class Material {
public:
	std::string name; 

	Math::Point4f ambientColor;
	Math::Point4f difuseColor;
	Math::Point4f specularColor;  
	float specularWeight; 

	Texture ambientTexture; 
	Texture difuseTexture;
	Texture specularTexture;
	Texture highlightTexture;
	Texture transparencyTexture;
	Texture bumpMapTexture; 
	Texture dislpacementTexture;
};

class MTLLoader {
public: 
	std::vector<Material> materials;
	std::map<std::string, int> materialsMap;

	std::string currentDirectory;

	void addMaterial (Material& material) {
		materials.push_back(material);
		materialsMap[material.name] = materials.size() - 1; // we set it's code 
	}

	int getMaterialIndex (std::string name) {
		if (materialsMap.find(name) != materialsMap.end())
			return materialsMap[name];
		else 
			return -1; 
	}

	void loadMtl (std::string directory, std::string filename) {
		currentDirectory = directory;
		std::string path = currentDirectory + filename; 

		std::ifstream file(path.c_str()); 

		Material currentMaterial;
		bool materialPending = false; 

		std::string line; 
		while (getline(file, line)) { 
			std::stringstream lineStream(line);

			std::string lineHeader = ""; 
			lineStream >> lineHeader; 

			if (lineHeader == "newmtl") {
				std::string name; 
				lineStream >> name;

				if (materialPending) {
					addMaterial(currentMaterial);
				}

				currentMaterial.name = name; 
				materialPending = true; 
			}
			else if (lineHeader == "Ka") {
				colorAmbientParse(currentMaterial, lineStream); 
			}
			else if (lineHeader == "Kd") {
				colorDifuseParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "Ks") {
				colorSpecularParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "Tr") {
				transparencyParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "d") {
				dissolvedParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_Ka") {
				textureAmbientParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_Kd") {
				textureDifuseParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_Ks") {
				textureSpecularParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_Ns") {
				textureHighlinghtParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_d") {
				textureTransparencyParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "map_bump") {
				textureBumpMapParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "bump") {
				textureBumpMapParse(currentMaterial, lineStream);
			}
			else if (lineHeader == "disp") {
				textureDisplacementParse(currentMaterial, lineStream);
			}
		}

		if (materialPending) {
			addMaterial(currentMaterial);
		}

		file.close(); 
	}

	void colorAmbientParse (Material& material, std::stringstream& stream) {
		Math::Point3f color; 

		if (stream >> color)
			material.ambientColor = color; 
	}

	void colorDifuseParse (Material& material, std::stringstream& stream) {
		Math::Point3f color; 
		
		if (stream >> color)
			material.difuseColor = color; 
	}

	void colorSpecularParse (Material& material, std::stringstream& stream) {
		Math::Point3f color; 
		
		if (stream >> color)
			material.specularColor = color; 
	}

	void specularWeightParse (Material& material, std::stringstream& stream) {
		float specularWeight; 

		if (stream >> specularWeight)
			material.specularWeight = specularWeight;
	}

	void transparencyParse (Material& material, std::stringstream& stream) {
		float transparency; 
		stream >> transparency;

		if (stream >> transparency)
			material.ambientColor.a = material.difuseColor.a = material.specularColor.a =
										transparency;  
	}

	void dissolvedParse (Material& material, std::stringstream& stream) {
		float disolved; 
		stream >> disolved;

		if (stream >> disolved)
			material.ambientColor.a = material.difuseColor.a = material.specularColor.a =
										(1.0 - disolved);  
	}

	/// not parsing options of the textures

	void textureAmbientParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 

		std::string path = currentDirectory + name; 

		if (name != "")
			material.ambientTexture = TextureLoader::load(path); 
	}

	void textureDifuseParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 

		std::string path = currentDirectory + name;

		if (name != "")
			material.difuseTexture = TextureLoader::load(path);
	}

	void textureSpecularParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 

		std::string path = currentDirectory + name;

		if (name != "")
			material.specularTexture = TextureLoader::load(path);
	}

	void textureHighlinghtParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 

		std::string path = currentDirectory + name;

		if (name != "")
			material.highlightTexture = TextureLoader::load(path);
	}

	void textureTransparencyParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 

		std::string path = currentDirectory + name;

		if (name != "")
			material.transparencyTexture = TextureLoader::load(path);
	}

	void textureBumpMapParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 
		
		std::string path = currentDirectory + name;

		if (name != "")
			material.bumpMapTexture = TextureLoader::load(path);
	}

	void textureDisplacementParse (Material& material, std::stringstream& stream) {
		std::string name; 
		stream >> name;

		while (name.size() >= 1 && name[0] == '-')
			stream >> name; 
		
		std::string path = currentDirectory + name;

		if (name != "")
			material.dislpacementTexture = TextureLoader::load(path);
	}
};

#endif