#ifndef SHADER_SPECULAR
#define SHADER_SPECULAR
#include "shaderuniform.h"
#include "light.h"
#include <glm/glm.hpp>


class Shader_Specular : public ShaderUniform {
public:

	virtual bool Init();

	void SetWVP(const glm::mat4 &WVP);

	void SetWorldMatrix(const glm::mat4 &WorldMatrix);

	void SetCurrentPosition(const glm::vec4 &currentPos);

	void SetEyeWorldPos(const glm::vec3& EyeWorldPos);

	void SetHeightmap(unsigned int TextureUnit);

	void SetVtex(unsigned int TextureUnit);

	void SetBtex(unsigned int TextureUint);

	void SetColortex(unsigned int TextureUnit);

	void SetNtex(unsigned int TextureUnit);

	void SetNormaltex(unsigned int TextureUnit);

	void SetSatTex(unsigned int TextureUnit);

	void SetMaxMinTex(unsigned int TextureUnit);

	void SetShowTexture(int flag);

	void SetHLevel(unsigned int TextureUnit);

	void SetHLevel1(unsigned int TextureUnit);

	void SetSGLights(unsigned int NumLights, const SGLight* pLights);

	void SetVisTexture(unsigned int TextureUnit);

	void SetVisPageTexture(unsigned int TextureUnit);

private:
	GLuint m_WVPLocation;
	GLuint m_WorldMatrixLocation;
	GLuint m_eyeWorldPosLocation;

	GLuint m_vtexLocation;
	GLuint m_heightmapLocation;
	GLuint m_currentPosLocation;

	GLuint m_ShowTextureLocation;

	GLuint m_texLocation;

	GLuint m_satLocation;
	GLuint m_maxminLocation;

	GLuint m_hlevelLocation;
	GLuint m_hlevel1Location;

	GLuint m_ntexLocation;
	GLuint m_btexLocation;
	GLuint m_normalmapLocation;

	GLuint m_vistexLocation;
	GLuint m_vispagetexLocation;

	GLuint m_numSGLightsLocation;

	struct {
		GLuint axis;
		GLuint sharpness;
		GLuint amplitude;
	} m_sgLightsLocation[MAX_SG_LIGHTS];
};

#endif
