#include "frameBuffer.h"
#include "util.h"
#include "texture.hpp"

FrameBuffer::FrameBuffer() {
	valid = false;
	saved = false;
}

FrameBuffer::~FrameBuffer() {
	if (valid) {
		glDeleteBuffers(1, &frameBuffer);
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &quadVAO);
	}
}

void FrameBuffer::init(int screenWidth, int screenHeight) {
	if (valid)
		return;
	width = screenWidth;
	height = screenHeight;

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR:framebuffer is not complete.\n");
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shader.Init();

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	valid = true;
}

void FrameBuffer::begin() {
	if (valid)
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	else {
		printf("ERROR: frame buffer has not been initialized.\n");
		exit(0);
	}
}

void FrameBuffer::end() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::render() {
	//	glClearColor(0.37f, 0.37f, 0.37f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	glDisable(GL_DEPTH_TEST); // We don't care about depth information when rendering a single quad

	// Draw Screen
	m_shader.Enable();
	glBindVertexArray(quadVAO);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);	// Use the color attachment texture as the texture of the quad plane
	m_shader.SetTextureUnit(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void FrameBuffer::clear() {
	begin();
	glClearDepth(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	end();
}

void FrameBuffer::savetoImage()
{
	if (!saved)
	{
	    float* data = new float[width * height * 4];
		begin();
		glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, (void*)data);
		end();
		SaveImage(data, width, height, "test_lod.png");
	}
	saved = true;
}



