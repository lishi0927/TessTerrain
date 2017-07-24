#ifndef TESSBUMP_SHADER_SPECULAR
#define TESSBUMP_SHADER_SPECULAR
#include "shaderuniform.h"
#include "light.h"
#include <glm/glm.hpp>


class TessBumpShader_Specular : public ShaderUniform {
public:

	virtual bool Init();

	void SetWVP(const glm::mat4 &WVP);

	void SetWorldMatrix(const glm::mat4 &WorldMatrix);

	void SetDirectionalLight(const DirectionalLight& Light);

	void SetEyeWorldPos(const glm::vec3& EyeWorldPos);

	void SetMatSpecularIntensity(float Intensity);

	void SetMatSpecularPower(float Power);

	void SetCurrentPosition(const glm::vec4 &currentPos);

	void SetHeightmap(unsigned int TextureUnit);

	void SetVtex(unsigned int TextureUnit);

	void SetColortex(unsigned int TextureUnit);

	void SetAOTex(unsigned int TextureUnit);

	void SetSatTex(unsigned int TextureUnit);

	void SetMaxMinTex(unsigned int TextureUnit);

	void SetShowAO(int flag);

	void SetShowTexture(int flag);

	void SetHLevel(unsigned int TextureUnit);

	void SetHLevel1(unsigned int TextureUnit);

private:
	GLuint m_WVPLocation;
	GLuint m_WorldMatrixLocation;
	GLuint m_eyeWorldPosLocation;
	GLuint m_matSpecularIntensityLocation;
	GLuint m_matSpecularPowerLocation;

	GLuint m_vtexLocation;
	GLuint m_heightmapLocation;
	GLuint m_currentPosLocation;

	GLuint m_ShowAOLocation;
	GLuint m_ShowTextureLocation;

	GLuint m_texLocation;

	GLuint m_AOLocation;
	GLuint m_satLocation;
	GLuint m_maxminLocation;

	GLuint m_hlevelLocation;
	GLuint m_hlevel1Location;

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint DiffuseIntensity;
		GLuint Direction;
	} m_dirLightLocation;
};

#endif
