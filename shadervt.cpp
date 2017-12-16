#include "shadervt.h"
#include "shader.hpp"

bool Shadervt::Init()
{
	shader = LoadTessShaders("shader/fvt_vert.glsl", "shader/fvt_tess_cs.glsl", "shader/fvt_tess_es.glsl", "shader/fvt_fragment.glsl");

	m_gWVPLocation = glGetUniformLocation(shader, "gWVP");
	m_currentPosLocation = glGetUniformLocation(shader, "currentPos");
	m_vtexLocation = glGetUniformLocation(shader, "vtex");
	m_heightmapLocation = glGetUniformLocation(shader, "texHeightmap");
	m_gBlockSizeLocation = glGetUniformLocation(shader, "blocksize");

	m_hlevelLocation = glGetUniformLocation(shader, "texHLevel");
	m_hlevel1Location = glGetUniformLocation(shader, "texHLevel1");

	if (m_gWVPLocation == 0xFFFFFFFF ||
		m_currentPosLocation == 0xFFFFFFFF ||
		m_vtexLocation == 0xFFFFFFFF ||
		m_heightmapLocation == 0xFFFFFFFF ||
		m_gBlockSizeLocation == 0xFFFFFFFF)
		return false;

	return true;
}

void Shadervt::SetWVP(const glm::mat4 &WVP)
{
	glUniformMatrix4fv(m_gWVPLocation, 1, GL_FALSE, &WVP[0][0]);
}

void Shadervt::SetCurrentPosition(const glm::vec4 &currentPos)
{
	glUniform4f(m_currentPosLocation, currentPos.x, currentPos.y, currentPos.z, currentPos.w);
}

void Shadervt::SetHeightmap(unsigned int TextureUnit)
{
	glUniform1i(m_heightmapLocation, TextureUnit);
}

void Shadervt::SetVtex(unsigned int TextureUnit)
{
	glUniform1i(m_vtexLocation, TextureUnit);
}

void Shadervt::SetBlockSize(float size)
{
	glUniform1f(m_gBlockSizeLocation, size);
}

void Shadervt::SetHLevel(unsigned int TextureUnit)
{
	glUniform1i(m_hlevelLocation, TextureUnit);
}

void Shadervt::SetHLevel1(unsigned int TextureUnit)
{
	glUniform1i(m_hlevel1Location, TextureUnit);
}