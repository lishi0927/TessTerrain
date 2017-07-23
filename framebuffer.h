#ifndef FRAMERBUFFER_H
#define FRAMERBUFFER_H
#include "util.h"
#include <GL/glew.h>
#include "shaderuniform.h"
#include "shader.hpp"



class FramebufferShader :public ShaderUniform {
public:
	virtual bool Init(){
		shader = LoadShaders("shader/screen_vert.glsl", "shader/screen_fragment.glsl");
		m_samplerLocation = glGetUniformLocation(shader, "screenTexture");

		if (m_samplerLocation == 0xFFFFFFFF) return false;

		return true;
	}

	void SetTextureUnit(unsigned int TextureUnit)
	{
		glUniform1i(m_samplerLocation, TextureUnit);
	}

	GLuint m_samplerLocation;
};

class FrameBuffer {
private:
	GLuint frameBuffer;
	GLuint colorBuffer;
	GLuint rbo;
	bool valid;
	FramebufferShader m_shader;
	GLuint quadVBO;
	GLuint quadVAO;
	bool saved;
	int width;
	int height;

public:
	FrameBuffer();
	~FrameBuffer();
	void init(int screenWidth, int screenHeight);
	void begin();
	void end();
	void render();
	void clear();
	void savetoImage();
};

const GLfloat quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f, 0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f
};

#endif