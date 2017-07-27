#include "tessbumpvt.h"
#include "shader.hpp"

bool TessBumpShadervt::Init()
{
	shader = LoadTessShaders("shader/tbvt_vert.glsl", "shader/tbvt_tess_cs.glsl", "shader/tbvt_tess_es.glsl", "shader/tbvt_fragment.glsl");

	m_gWVPLocation = glGetUniformLocation(shader, "gWVP");
	m_currentPosLocation = glGetUniformLocation(shader, "currentPos");
	m_vtexLocation = glGetUniformLocation(shader, "vtex");
	m_heightmapLocation = glGetUniformLocation(shader, "texHeightmap");
	m_gBlockSizeLocation = glGetUniformLocation(shader, "blocksize");

	if (m_gWVPLocation == 0xFFFFFFFF ||
		m_currentPosLocation == 0xFFFFFFFF ||
		m_vtexLocation == 0xFFFFFFFF ||
		m_heightmapLocation == 0xFFFFFFFF ||
		m_gBlockSizeLocation == 0xFFFFFFFF)
		return false;

	return true;
}

void TessBumpShadervt::SetWVP(const glm::mat4 &WVP)
{
	glUniformMatrix4fv(m_gWVPLocation, 1, GL_FALSE, &WVP[0][0]);
}

void TessBumpShadervt::SetCurrentPosition(const glm::vec4 &currentPos)
{
	glUniform4f(m_currentPosLocation, currentPos.x, currentPos.y, currentPos.z, currentPos.w);
}

void TessBumpShadervt::SetHeightmap(unsigned int TextureUnit)
{
	glUniform1i(m_heightmapLocation, TextureUnit);
}

void TessBumpShadervt::SetVtex(unsigned int TextureUnit)
{
	glUniform1i(m_vtexLocation, TextureUnit);
}

void TessBumpShadervt::SetBlockSize(float size)
{
	glUniform1f(m_gBlockSizeLocation, size);
}