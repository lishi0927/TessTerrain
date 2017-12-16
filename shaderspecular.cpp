#include "shaderspecular.h"
#include "shader.hpp"

bool Shader_Specular::Init()
{
	shader = LoadTessShaders("shader/fspecular_vert.glsl", "shader/fspecular_tess_cs.glsl", "shader/fspecular_tess_es.glsl", "shader/fspecular_fragment.glsl");

	m_WVPLocation = glGetUniformLocation(shader, "gWVP");
	m_WorldMatrixLocation = glGetUniformLocation(shader, "gWorld");
	m_eyeWorldPosLocation = glGetUniformLocation(shader, "gEyeWorldPos");


	m_currentPosLocation = glGetUniformLocation(shader, "currentPos");
	m_vtexLocation = glGetUniformLocation(shader, "vtex");
	m_heightmapLocation = glGetUniformLocation(shader, "texHeightmap");
	m_texLocation = glGetUniformLocation(shader, "blendtex");
	
	m_vistexLocation = glGetUniformLocation(shader, "vistex");
	m_vispagetexLocation = glGetUniformLocation(shader, "vispagetex");

	//m_satLocation = glGetUniformLocation(shader, "Satmap");
	//	m_maxminLocation = glGetUniformLocation(shader, "maxmintexture");

	m_ShowTextureLocation = glGetUniformLocation(shader, "gShowTexture");

	m_hlevelLocation = glGetUniformLocation(shader, "texHLevel");
	m_hlevel1Location = glGetUniformLocation(shader, "texHLevel1");

	m_btexLocation = glGetUniformLocation(shader, "btex");
	m_ntexLocation = glGetUniformLocation(shader, "ntex");
	m_normalmapLocation = glGetUniformLocation(shader, "normaltex");

	if (m_WVPLocation == 0xFFFFFFFF ||
	//	m_WorldMatrixLocation == 0xFFFFFFFF ||
	//	m_eyeWorldPosLocation == 0xFFFFFFFF ||
		m_currentPosLocation == 0xFFFFFFFF ||
		m_vtexLocation == 0xFFFFFFFF ||
		m_heightmapLocation == 0xFFFFFFFF ||
		//m_AOLocation == 0xFFFFFFFF ||
		//m_satLocation == 0xFFFFFFFF ||
		//m_maxminLocation == 0xFFFFFFFF ||
		m_hlevelLocation == 0xFFFFFFFF ||
		m_hlevel1Location == 0xFFFFFFFF) {
		//return false;
	}

	m_numSGLightsLocation = glGetUniformLocation(shader, "gNumSGLights");

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_sgLightsLocation); i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		SNPRINTF(Name, sizeof(Name), "gSGLights[%d].Axis", i);
		m_sgLightsLocation[i].axis = glGetUniformLocation(shader,Name);

		SNPRINTF(Name, sizeof(Name), "gSGLights[%d].Sharpness", i);
		m_sgLightsLocation[i].sharpness = glGetUniformLocation(shader, Name);

		SNPRINTF(Name, sizeof(Name), "gSGLights[%d].Amplitude", i);
		m_sgLightsLocation[i].amplitude = glGetUniformLocation(shader, Name);

		if (m_sgLightsLocation[i].axis == 0xFFFFFFFF ||
			m_sgLightsLocation[i].sharpness == 0xFFFFFFFF ||
			m_sgLightsLocation[i].amplitude == 0xFFFFFFFF) {
			return false;
		}
	}

	return true;
}

void Shader_Specular::SetWVP(const glm::mat4 &WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, &WVP[0][0]);
}

void Shader_Specular::SetWorldMatrix(const glm::mat4 &WorldMatrix)
{
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
}

void Shader_Specular::SetSGLights(unsigned int NumLights, const SGLight* pLights)
{
	glUniform1i(m_numSGLightsLocation, NumLights);
	for (unsigned int i = 0; i < NumLights; i++) {
		glUniform3f(m_sgLightsLocation[i].axis, pLights[i].p.x, pLights[i].p.y, pLights[i].p.z);
		glUniform1f(m_sgLightsLocation[i].sharpness, pLights[i].lambda);
		glUniform3f(m_sgLightsLocation[i].amplitude, pLights[i].mu.x, pLights[i].mu.y, pLights[i].mu.z);
	}
}
void Shader_Specular::SetEyeWorldPos(const glm::vec3& EyeWorldPos)
{
	glUniform3f(m_eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}


void Shader_Specular::SetCurrentPosition(const glm::vec4 &currentPos)
{
	glUniform4f(m_currentPosLocation, currentPos.x, currentPos.y, currentPos.z, currentPos.w);
}

void Shader_Specular::SetHeightmap(unsigned int TextureUnit)
{
	glUniform1i(m_heightmapLocation, TextureUnit);
}

void Shader_Specular::SetVtex(unsigned int TextureUnit)
{
	glUniform1i(m_vtexLocation, TextureUnit);
}

void Shader_Specular::SetColortex(unsigned int TextureUnit)
{
	glUniform1i(m_texLocation, TextureUnit);
}


void Shader_Specular::SetSatTex(unsigned int TextureUnit)
{
	glUniform1i(m_satLocation, TextureUnit);
}

void Shader_Specular::SetMaxMinTex(unsigned int TextureUnit)
{
	glUniform1i(m_maxminLocation, TextureUnit);
}


void Shader_Specular::SetShowTexture(int flag)
{
	glUniform1i(m_ShowTextureLocation, flag);
}


void Shader_Specular::SetHLevel(unsigned int TextureUnit)
{
	glUniform1i(m_hlevelLocation, TextureUnit);
}

void Shader_Specular::SetHLevel1(unsigned int TextureUnit)
{
	glUniform1i(m_hlevel1Location, TextureUnit);
}

void Shader_Specular::SetNtex(unsigned int TextureUnit)
{
	glUniform1i(m_ntexLocation, TextureUnit);
}

void Shader_Specular::SetBtex(unsigned int TextureUnit)
{
	glUniform1i(m_btexLocation, TextureUnit);
}

void Shader_Specular::SetNormaltex(unsigned int TextureUnit)
{
	glUniform1i(m_normalmapLocation, TextureUnit);
}

void Shader_Specular::SetVisTexture(unsigned int TextureUnit)
{
	glUniform1i(m_vistexLocation, TextureUnit);
}

void Shader_Specular::SetVisPageTexture(unsigned int TextureUnit)
{
	glUniform1i(m_vispagetexLocation, TextureUnit);
}
