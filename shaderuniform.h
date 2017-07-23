#ifndef SHADER_UNIFORM
#define SHADER_UNIFORM
#include "GL/glew.h"
#include <vector>

class ShaderUniform
{
public:
	ShaderUniform()
	{
	}

	~ShaderUniform()
	{
	}

	void Enable()
	{
		glUseProgram(shader);
	}

	void Disable()
	{
		glUseProgram(0);
	}

	virtual bool Init() = 0;

	GLuint shader;
};

#endif
