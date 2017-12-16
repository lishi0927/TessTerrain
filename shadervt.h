#ifndef SHADER_vt
#define SHADER_vt
#include "shaderuniform.h"
#include "light.h"
#include <glm/glm.hpp>


class Shadervt : public ShaderUniform {
public:
	virtual bool Init();

	void SetWVP(const glm::mat4 &WVP);

	void SetCurrentPosition(const glm::vec4 &currentPos);

	void SetHeightmap(unsigned int TextureUnit);

	void SetVtex(unsigned int TextureUnit);

	void SetBlockSize(float size);

	void SetHLevel(unsigned int TextureUnit);

	void SetHLevel1(unsigned int TextureUnit);
private:
	GLuint m_vtexLocation;
	GLuint m_heightmapLocation;
	GLuint m_currentPosLocation;
	GLuint m_gWVPLocation;
	GLuint m_gBlockSizeLocation;
	GLuint m_hlevelLocation;
	GLuint m_hlevel1Location;
};


#endif
