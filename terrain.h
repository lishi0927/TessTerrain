#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include "shaderuniform.h"
#include "VTex.h"
#include "light.h"
#include "framebuffer.h"
#include "shadervt.h"
#include "shaderspecular.h"

#define MaxBlockNumber 64 * 64 * 4

class TerrainBlock {
public:
	TerrainBlock(int index)
	{
		blockID = index;
		BRDF = 0;
		visible = true;
		isparent = false;
		ishorizon = false;

		for (int i = 0; i < 4; i++)
			isboundary[i] = true;

	}

	~TerrainBlock()
	{

	}

	void ChangeBRDF()
	{
		if (visible && !isparent && centerlevel <= 0 && !ishorizon)
		{
			//	BRDF = 1;
		}
	}

	glm::vec3 vertices[4];
	int BRDF;
	int blockID;
	bool visible;
	bool isparent;
	float regionarea;
	bool ishorizon;
	int boundary[4];
	bool isboundary[4];
	int level;
	int centerlevel;
	int lx, rx, ly, ry;
};

class Terrain {
public:
	Terrain()
	{
		m_SGlight.lambda = 1.0;
		m_SGlight.p = glm::vec3(1.0,1.0,1.0);
		m_SGlight.mu = glm::vec3(1.0, 1.0, 1.0);
	}

	~Terrain()
	{

	}

	bool Init();
	void Render();

	glm::mat4 m_WVP;
	glm::mat4 m_Projection;
	glm::mat4 m_World;
	glm::mat4 m_View;

	float m_ar;
	float m_fov;

	glm::vec3 currentPos;
	glm::vec3 currentDir;
	glm::vec3 m_camerapos;

	glm::vec3 lastPos;
	glm::vec3 lastDir;

	glm::vec3 needlastPos;
	glm::vec3 needlastDir;

	SGLight m_SGlight;

	int m_linemode;
	bool lineon;
	bool geometryclipmap;
	int m_Textureflag;

private:
	bool InitTexture();
	bool InitShader();
	void InitHeightLevel();
	void updateHtex(float currentX, float currentY, float currentZ);
	void updateBtex();
	void InitVertices();

	void updateBlock(float currentX, float currentZ, TerrainBlock& tblock);

	void updateBlock();

	void ChildBorderCrack(TerrainBlock &tblock);
	void BorderCrack(TerrainBlock &tblock);

	void ViewFrustum();
	void NeedTransferData();

	void VTPassGenerateVertices();
	void RenderPassGenerateVertices();
	
	VTex htex, btex, ntex, vistex;

	std::vector<TerrainBlock> m_Block;
	std::vector<TerrainBlock> m_Child;

	std::vector<glm::vec3> m_vtvertices;
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_bumpvertices;
	std::vector<glm::vec3> m_bordervertices;

	GLuint vao[4];
	GLuint vbo[4];

	GLuint hLevelTex, hLevelTex1;
	unsigned char hLevel1[CHUNKNUMBER * CHUNKNUMBER];
	float hLevel[CHUNKNUMBER * CHUNKNUMBER];

	FrameBuffer feedback;

	bool needUpdate[CHUNKNUMBER][CHUNKNUMBER];

	bool transferdata;
	bool needtranfersdata;

	GLuint colortexture;

	Shadervt m_vtshader;
	Shader_Specular m_specular;
};

#endif
