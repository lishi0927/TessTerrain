#include "tessbumpspecular.h"
#include "shader.hpp"

bool TessBumpShader_Specular::Init()
{
	shader = LoadTessShaders("shader/tbspecular_vert.glsl", "shader/tbspecular_tess_cs.glsl", "shader/tbspecular_tess_es.glsl", "shader/tbspecular_fragment.glsl");
	
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
	m_texLocation = glGetUniformLocation(shader, "blendtex");

	m_AOtexLocation = glGetUniformLocation(shader, "aotex");
	m_AOleveltexLocation = glGetUniformLocation(shader, "texaolevel");
	m_AOmaptexLocation = glGetUniformLocation(shader, "texAOmap");
	m_AOmaxdiffLocation = glGetUniformLocation(shader, "texaomaxdiff");

	m_satLocation = glGetUniformLocation(shader, "Satmap");
//	m_maxminLocation = glGetUniformLocation(shader, "maxmintexture");

	m_ShowAOLocation = glGetUniformLocation(shader, "gShowAO");
	m_ShowTextureLocation = glGetUniformLocation(shader, "gShowTexture");

	m_hlevelLocation = glGetUniformLocation(shader, "texHLevel");
	m_hlevel1Location = glGetUniformLocation(shader, "texHLevel1");

	m_btexLocation = glGetUniformLocation(shader, "btex");
	m_ntexLocation = glGetUniformLocation(shader, "ntex");
	m_normalmapLocation = glGetUniformLocation(shader, "normaltex");
//	m_normallevelLocation = glGetUniformLocation(shader, "normallevel");


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
//		m_AOLocation == 0xFFFFFFFF ||
		m_satLocation == 0xFFFFFFFF ||
//		m_maxminLocation == 0xFFFFFFFF ||
		m_hlevelLocation == 0xFFFFFFFF ||
		m_hlevel1Location == 0xFFFFFFFF ||
		m_normallevelLocation == 0xFFFFFFFF) {
		return false;
	}

	return true;
}

void TessBumpShader_Specular::SetWVP(const glm::mat4 &WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_FALSE, &WVP[0][0]);
}

void TessBumpShader_Specular::SetWorldMatrix(const glm::mat4 &WorldMatrix)
{
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_FALSE, &WorldMatrix[0][0]);
}

void TessBumpShader_Specular::SetDirectionalLight(const DirectionalLight& Light)
{
	glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
	glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
	glm::vec3 Direction = Light.Direction;
	Direction = glm::normalize(Direction);
	glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
	glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}

void TessBumpShader_Specular::SetEyeWorldPos(const glm::vec3& EyeWorldPos)
{
	glUniform3f(m_eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}

void TessBumpShader_Specular::SetMatSpecularIntensity(float Intensity)
{
	glUniform1f(m_matSpecularIntensityLocation, Intensity);
}

void TessBumpShader_Specular::SetMatSpecularPower(float Power)
{
	glUniform1f(m_matSpecularPowerLocation, Power);
}

void TessBumpShader_Specular::SetCurrentPosition(const glm::vec4 &currentPos)
{
	glUniform4f(m_currentPosLocation, currentPos.x, currentPos.y, currentPos.z, currentPos.w);
}

void TessBumpShader_Specular::SetHeightmap(unsigned int TextureUnit)
{
	glUniform1i(m_heightmapLocation, TextureUnit);
}

void TessBumpShader_Specular::SetVtex(unsigned int TextureUnit)
{
	glUniform1i(m_vtexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetColortex(unsigned int TextureUnit)
{
	glUniform1i(m_texLocation, TextureUnit);
}

void TessBumpShader_Specular::SetAOtex(unsigned int TextureUnit)
{
	glUniform1i(m_AOtexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetAOlevelTex(unsigned int TextureUnit)
{
	glUniform1i(m_AOleveltexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetAOmaxdiffTex(unsigned int TextureUnit)
{
	glUniform1i(m_AOmaxdiffLocation, TextureUnit);
}

void TessBumpShader_Specular::SetAOMaptex(unsigned int TextureUnit)
{
	glUniform1i(m_AOmaptexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetSatTex(unsigned int TextureUnit)
{
	glUniform1i(m_satLocation, TextureUnit);
}

void TessBumpShader_Specular::SetMaxMinTex(unsigned int TextureUnit)
{
	glUniform1i(m_maxminLocation, TextureUnit);
}


void TessBumpShader_Specular::SetShowAO(int flag)
{
	glUniform1i(m_ShowAOLocation, flag);
}

void TessBumpShader_Specular::SetShowTexture(int flag)
{
	glUniform1i(m_ShowTextureLocation, flag);
}


void TessBumpShader_Specular::SetHLevel(unsigned int TextureUnit)
{
	glUniform1i(m_hlevelLocation, TextureUnit);
}

void TessBumpShader_Specular::SetHLevel1(unsigned int TextureUnit)
{
	glUniform1i(m_hlevel1Location, TextureUnit);
}

void TessBumpShader_Specular::SetNtex(unsigned int TextureUnit)
{
	glUniform1i(m_ntexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetBtex(unsigned int TextureUnit)
{
	glUniform1i(m_btexLocation, TextureUnit);
}

void TessBumpShader_Specular::SetNormaltex(unsigned int TextureUnit)
{
	glUniform1i(m_normalmapLocation, TextureUnit);
}

void TessBumpShader_Specular::SetNormallevel(unsigned int TextureUnit)
{
	glUniform1i(m_normallevelLocation, TextureUnit);
}