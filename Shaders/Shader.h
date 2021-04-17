#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class Shader {
public:
	bool isShaderValid = false; 
	GLuint shader; 

	Shader() {}

	Shader(GLenum shaderType) {
		createShader(shaderType);
	}

	Shader(GLenum shaderType, std::string fileName) {
		createShader(shaderType);
		loadShader(fileName);
	}

	void clear() {
		deleteShader();
	}

	GLint getShader() {
		return shader;
	}

	void createShader(GLenum shaderType) {
		shader = glCreateShader(shaderType);
		isShaderValid = true; 
	}

	void loadShader(std::string fileName) {
		std::ifstream file(fileName.c_str()); 
		
		if (!file)
			EXCEPTION("can't open shader source");
		// slow but... ehhh
		std::string content((std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>()));

		const char *stringPtr = content.c_str();

		glShaderSource(shader, 1, &stringPtr, NULL);
		glCompileShader(shader);

		if (hasErrors())
			EXCEPTION("shader creation failed");
	}

	bool hasErrors() {
		GLint succes = 0; 
		glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);

		if (succes == true)
			return false; 
		else {
			GLint maxLength = 0; 
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
			
			std::unique_ptr<char[]> infoLog(new char[maxLength]);  
			glGetShaderInfoLog(shader, (GLsizei)maxLength, NULL, infoLog.get());

			std::cout << "Shader log:" << std::endl;
			std::cout << infoLog.get() << std::endl;

			deleteShader();
			return true; 
		}
	}

	void deleteShader() {
		if (isShaderValid) {
			glDeleteShader(shader);
			isShaderValid = false; 
		}
	}
};


#endif