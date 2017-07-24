#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include "shaderuniform.h"
#include "VTex.h"
#include "light.h"
#include "tessbumpspecular.h"
#include "tessbump.h"
#include "tessbumpborder.h"

#define MaxBlockNumber 64 * 16 * 4
#define MaxRegionScan  1.0 / 2.0
#define Bump_Area  1.0 / 10.0

class TessBumpBlock {
public:
	TessBumpBlock(int index)
	{
		blockID = index;

		BRDF = 0;
		visible = true;
		isparent = false;
		ishorizon = false;

		for (int i = 0; i < 4; i++)
			isboundary[i] = true;
	}

	~TessBumpBlock()
	{

	}

	void ChangeBRDF()
	{
		if (visible && !isparent && centerlevel <= 0 && !ishorizon)
		{
			BRDF = 1;
		}
	}

	glm::vec3 vertices[4];
	int blockID;
	int BRDF;
	float regionarea;
	bool visible;
	bool isparent;
	bool ishorizon;
	int boundary[4];
	bool isboundary[4];
	int level;
	int centerlevel;
};


class TessBumpTerrain {
public:
	TessBumpTerrain()
	{
		AOflag = 1;
		m_Textureflag = 1;
		m_linemode = 0;
		lineon = false;
		transferdata = true;

		lastPos = glm::vec3(-50.0, -50.0, -50.0);
		lastDir = glm::vec3(0.0, 0.0, 0.0);

		m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
		m_directionalLight.AmbientIntensity = 0.0f;
		m_directionalLight.DiffuseIntensity = 0.0f;
		m_directionalLight.Direction = glm::vec3(1.0f, -1.0, 0.0);
	}

	~TessBumpTerrain()
	{
		m_Block.clear();
		m_vertices.clear();
	}

	bool Init();
	void Render();

	glm::mat4 m_WVP;
	glm::mat4 m_Projection;
	glm::mat4 m_World;
	glm::mat4 m_View;

	float m_ar;
	float m_fov;

	DirectionalLight m_directionalLight;
	glm::vec3 currentPos;
	glm::vec3 currentDir;
	glm::vec3 m_camerapos;

	glm::vec3 lastPos;
	glm::vec3 lastDir;

	int AOflag;
	int m_Textureflag;
	int m_Shadowflag;
	int m_linemode;
	bool lineon;
	bool geometryclipmap;

private:
	bool InitTexture();
	bool InitShader();
	void InitHeightLevel();
	void updateHtex(float currentX, float currentY, float currentZ);
	void InitVertices();
	void ViewFrustum();
	void ChildBorderCrack(TessBumpBlock &tblock);
	void BorderCrack(TessBumpBlock &tblock);

	void updateBlock(float currentX, float currentZ, TessBumpBlock& tblock);

	void updateBlock();

	void RenderPassGenerateVertices();

	void InitMaxMin();

	VTex htex; // height map
	VTex btex; // blend map

	bool transferdata;

	std::vector<TessBumpBlock> m_Block;
	std::vector<TessBumpBlock> m_Child;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_bumpvertices;
	std::vector<glm::vec3> m_bordervertices;

	GLuint vao[3];
	GLuint vbo[3];

	GLuint colortexture;

	GLuint Maxmintexture;

	GLuint hLevelTex, hLevelTex1;
	unsigned char hLevel1[CHUNKNUMBER * CHUNKNUMBER];
	float hLevel[CHUNKNUMBER * CHUNKNUMBER];

	TessBumpShader_Specular m_specular;
	TessBumpShader_Bump m_bump;
	TessBumpShader_Border m_border;
};

#endif

