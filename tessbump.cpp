#include "tessbump.h"
#include "shader.hpp"

bool TessBumpShader_Bump::Init()
{
	shader = LoadShaders("shader/tb_vert.glsl", "shader/tb_fragment.glsl");

	m_WVPLocation = glGetUniformLocation(shader, "gWVP");
	m_WorldMatrixLocation = glGetUniformLocation(shader, "gWorld");
	m_eyeWorldPosLocation = glGetUniformLocation(shader, "gEyeWorldPos");
	m_dirLightLocation.Color = glGetUniformLocation(shader, "gDirectionalLight.Base.Color");
	m_dirLightLocation.AmbientIntensity = glGetUniformLocation(shader, "gDirectionalLight.Base.AmbientIntensity");
	m_dirLightLocation.Direction = glGetUniformLocation(shader, "gDirectionalLight.Direction");
	m_dirLightLocation.DiffuseIntensity = glGetUniformLocation(shader, "gDirectionalLight.Base.DiffuseIntensity");
	m_matSpecularIntensityLocation = glGetUniformLocation(shader, "gMatSpecularIntensity");
	m_matSpecularPowerLocation = glGetUniformLocation(shader, "gSpecularPower");

	m_currentPosLocation = glGetUniformLocation(shader, "currentPos");
	m_vtexLocation = glGetUniformLocation(shader, "vtex");
	m_heightmapLocation = glGetUniformLocation(shader, "texHeightmap");
	m_texLocation = glGetUniformLocation(shader, "colormap");

	m_AOLocation = glGetUniformLocation(shader, "AOmap");

	m_ShowAOLocation = glGetUniformLocation(shader, "gShowAO");
	m_ShowTextureLocation = glGetUniformLocation(shader, "gShowTexture");

	m_hlevelLocation = glGetUniformLocation(shader, "HLevel");
	m_hlevel1Location = glGetUniformLocation(shader, "HLevel1");

	if (m_dirLightLocation.AmbientIntensity == 0xFFFFFFFF ||
		m_WVPLocation == 0xFFFFFFFF ||
		m_WorldMatrixLocation == 0xFFFFFFFF ||
		m_eyeWorldPosLocation == 0xFFFFFFFF ||
		m_dirLightLocation.Color == 0xFFFFFFFF ||
		m_dirLightLocation.DiffuseIntensity == 0xFFFFFFFF ||
		m_dirLightLocation.Direction == 0xFFFFFFFF ||
		m_matSpecularIntensityLocation == 0xFFFFFFFF ||
		m_matSpecularPowerLocation == 0xFFFFFFFF ||
		m_currentPosLocation == 0xFFFFFFFF ||
		m_vtexLocation == 0xFFFFFFFF ||
		m_heightmapLocation == 0xFFFFFFFF ||
		m_AOLocation == 0xFFFFFFFF ||
		m_hlevelLocation == 0xFFFFFFFF ||
		m_hlevel1Location == 0xFFFFFFFF) {
		return false;
	}

	return true;
}

void TessBumpShader_Bump::SetWVP(const glm::mat4 &WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, &WVP[0][0]);
}

void TessBumpShader_Bump::SetWorldMatrix(const glm::mat4 &WorldMatrix)
{
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
}

void TessBumpShader_Bump::SetDirectionalLight(const DirectionalLight& Light)
{
	glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
	glm::vec3 Direction = Light.Direction;
	Direction = glm::normalize(Direction);
	glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
	glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}

void TessBumpShader_Bump::SetEyeWorldPos(const glm::vec3& EyeWorldPos)
{
	glUniform3f(m_eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}

void TessBumpShader_Bump::SetMatSpecularIntensity(float Intensity)
{
	glUniform1f(m_matSpecularIntensityLocation, Intensity);
}

void TessBumpShader_Bump::SetMatSpecularPower(float Power)
{
	glUniform1f(m_matSpecularPowerLocation, Power);
}

void TessBumpShader_Bump::SetCurrentPosition(const glm::vec4 &currentPos)
{
	glUniform4f(m_currentPosLocation, currentPos.x, currentPos.y, currentPos.z, currentPos.w);
}

void TessBumpShader_Bump::SetHeightmap(unsigned int TextureUnit)
{
	glUniform1i(m_heightmapLocation, TextureUnit);
}

void TessBumpShader_Bump::SetVtex(unsigned int TextureUnit)
{
	glUniform1i(m_vtexLocation, TextureUnit);
}

void TessBumpShader_Bump::SetColortex(unsigned int TextureUnit)
{
	glUniform1i(m_texLocation, TextureUnit);
}

void TessBumpShader_Bump::SetAOTex(unsigned int TextureUnit)
{
	glUniform1i(m_AOLocation, TextureUnit);
}

void TessBumpShader_Bump::SetShowAO(int flag)
{
	glUniform1i(m_ShowAOLocation, flag);
}

void TessBumpShader_Bump::SetShowTexture(int flag)
{
	glUniform1i(m_ShowTextureLocation, flag);
}

void TessBumpShader_Bump::SetHLevel(unsigned int TextureUnit)
{
	glUniform1i(m_hlevelLocation, TextureUnit);
}

void TessBumpShader_Bump::SetHLevel1(unsigned int TextureUnit)
{
	glUniform1i(m_hlevel1Location, TextureUnit);
}