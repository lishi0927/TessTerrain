#include "terrain.h"
#include <SOIL/SOIL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

int boundary_index[13][4] = { { 8,16,8,16 },{ 8,16,16,16 },{ 8,16,16,16 },{ 8,16,16,8 },
{ 16,16,8,16 },{ 16,16,16,8 },{ 16,16,8,16 },{ 16,16,16,8 },
{ 16,8,8,16 },{ 16,8,16,16 },{ 16,8,16,16 },{ 16,8,16,8 },{ 16,16,16,16 } };

int child_index[4][4] = { { 2,1,2,1 },{ 1,2,2,1 },{ 1,2,1,2 },{ 2,1,1,2 } };

void TessBumpTerrain::InitVertices() {

	int index = 1;
	float r = 1.0 * MAXSCALE;

	for (int l = 0; l < LEVEL; l++) {
		for (int i = -2; i < 2; i++)
			for (int j = -2; j < 2; j++) {
				if (l != LEVEL - 1 && i != -2 && i != 1 && j != -2 && j != 1)
					continue;
				TessBumpBlock new_block(index++);
				new_block.vertices[0] = glm::vec3(r*i, 0, r*j);
				new_block.vertices[1] = glm::vec3(r*(i + 1), 0, r*j);
				new_block.vertices[2] = glm::vec3(r*(i + 1), 0, r*(j + 1));
				new_block.vertices[3] = glm::vec3(r*i, 0, r*(j + 1));

				new_block.level = 0;
				new_block.centerlevel = l;

				m_Block.push_back(new_block);
			}
		r *= 0.5;
	}

	glGenVertexArrays(4, vao);
	glGenBuffers(4, vbo);

	for (int i = 0; i < 4; i++)
	{
		glBindVertexArray(vao[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);

		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxBlockNumber, 0, GL_DYNAMIC_DRAW);

		// set up generic attrib pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));
	}

	for (int i = 0; i < m_Block.size(); i++)
	{
		if (i >= 12 * (LEVEL - 1))
		{
			int index = i - 12 * (LEVEL - 1);
			if (index <= 4) memcpy(m_Block[i].boundary, boundary_index[index], 4 * sizeof(int));
			else if (index >= 11) memcpy(m_Block[i].boundary, boundary_index[index - 4], 4 * sizeof(int));
			else if (index == 7) memcpy(m_Block[i].boundary, boundary_index[5], 4 * sizeof(int));
			else if (index == 8) memcpy(m_Block[i].boundary, boundary_index[6], 4 * sizeof(int));
			else memcpy(m_Block[i].boundary, boundary_index[12], 4 * sizeof(int));
		}
		else
		{
			int index = i % 12;
			memcpy(m_Block[i].boundary, boundary_index[index], 4 * sizeof(int));
		}
	}

}

bool TessBumpTerrain::InitTexture()
{
	string filename;
	int w, h, channels, type;
	unsigned char *img;

	filename = "ps_texture_16k.png";
	glGenTextures(1, &colortexture);
	glBindTexture(GL_TEXTURE_2D, colortexture);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	img = SOIL_load_image(filename.c_str(), &w, &h, &channels, SOIL_LOAD_AUTO);
	type = (channels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(img);

	return true;
}

void TessBumpTerrain::InitHeightLevel()
{
	memset(hLevel, 0, sizeof(hLevel));
	glGenTextures(1, &hLevelTex);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &hLevelTex1);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool TessBumpTerrain::InitShader()
{
	m_specular.Init();
	m_specular.Enable();
	m_specular.SetVtex(0);
	m_specular.SetHeightmap(1);
	m_specular.SetColortex(2);
	m_specular.SetHLevel(3);
	m_specular.SetHLevel1(4);
	glUseProgram(0);

	m_bump.Init();
	m_bump.Enable();
	m_bump.SetVtex(0);
	m_bump.SetHeightmap(1);
	m_bump.SetColortex(2);
	m_bump.SetHLevel(3);
	m_bump.SetHLevel1(4);
	glUseProgram(0);

	m_border.Init();
	m_border.Enable();
	m_border.SetVtex(0);
	m_border.SetHeightmap(1);
	m_border.SetColortex(2);
	m_border.SetHLevel(3);
	m_border.SetHLevel1(4);
	glUseProgram(0);
}

bool TessBumpTerrain::Init()
{
	InitVertices();

	InitShader();

	//初始化，合成texture
//	if (!InitTexture()) return false;

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	htex.init(CHUNKSIZE, CHUNKNUMBER, "dem", LEVELOFTERRAIN);
	btex.init(BLENDSIZE, CHUNKNUMBER, "texture", LEVELOFBLENDTEX);
	ntex.init(CHUNKSIZE, CHUNKNUMBER, "normal", LEVELOFTERRAIN);

	m_directionalLight.AmbientIntensity = 0.9f;
	m_directionalLight.DiffuseIntensity = 0.5f;
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.Direction = glm::vec3(-1.0f, -1.0f, 0.0f);
}

void TessBumpTerrain::updateBlock(float currentX, float currentZ, TessBumpBlock &tblock)
{
	int xs, xb, ys, yb, zs, zb;
	xs = xb = ys = yb = zs = zb = 0;

	float minx, miny, maxx, maxy;
	minx = miny = 1;
	maxx = maxy = -1;

	for (int i = 0; i < 4; i++)
		for (int k = 0; k <= 1; k++) {
			glm::vec4 pos = glm::vec4(tblock.vertices[i], 1.0) + glm::vec4(0.0, k * 200, 0.0, 0.0);
			pos = m_WVP * pos;
			xs = (pos.x < -pos.w) ? xs + 1 : xs;
			xb = (pos.x > pos.w) ? xb + 1 : xb;
			ys = (pos.y < -pos.w) ? ys + 1 : ys;
			yb = (pos.y > pos.w) ? yb + 1 : yb;
			zs = (pos.z < -pos.w) ? zs + 1 : zs;
			zb = (pos.z > pos.w) ? zb + 1 : zb;
//			minx = clampf(Min(pos.x / pos.w, minx), -1, 1);
//			miny = clampf(Min(pos.y / pos.w, miny), -1, 1);
//			maxx = clampf(Max(pos.x / pos.w, maxx), -1, 1);
//			maxy = clampf(Max(pos.y / pos.w, maxy), -1, 1);
		}
	if (xs == 8 || xb == 8 || ys == 8 || yb == 8 || zs == 8 || zb == 8)
	{
		tblock.visible = false;
		return;
	}

	if (tblock.level >= 0)
	{
		return;
	}

//	tblock.regionarea = (maxx - minx) * (maxy - miny);

	float sidelength = tblock.vertices[3].x - tblock.vertices[0].x;
	float heightresolution = sidelength * (VIEWCHUNKNUMBER * CHUNKSIZE / 4.0 / MAXSCALE);

	if (heightresolution > 64)
	{
		tblock.isparent = true;
		//tblock.child = m_Block.size() + m_Child.size() + 1;
		TessBumpBlock temperror = tblock;

		TessBumpBlock tempchild1(m_Block.size() + m_Child.size() + 1);
		tempchild1.level = tblock.level + 1;
		tempchild1.centerlevel = tblock.centerlevel;
		//tempchild1.parent = tblock.blockID;
		for (int j = 0; j < 4; j++)
		{
			tempchild1.boundary[j] = tblock.boundary[j] / child_index[0][j];
		}
		tempchild1.vertices[0] = tblock.vertices[0];
		tempchild1.vertices[1] = float(0.5) * (tblock.vertices[0] + tblock.vertices[1]);
		tempchild1.vertices[2] = float(0.25) * (tblock.vertices[0] + tblock.vertices[1] + tblock.vertices[2] + tblock.vertices[3]);
		tempchild1.vertices[3] = float(0.5) * (tblock.vertices[0] + tblock.vertices[3]);
		m_Child.push_back(tempchild1);

		if (tblock.blockID < 0) tblock = temperror;
		TessBumpBlock tempchild2(m_Block.size() + m_Child.size() + 1);
		tempchild2.level = tblock.level + 1;
		//tempchild2.parent = tblock.blockID;
		for (int j = 0; j < 4; j++)
		{
			tempchild2.boundary[j] = tblock.boundary[j] / child_index[1][j];
		}
		tempchild2.centerlevel = tblock.centerlevel;
		tempchild2.vertices[0] = float(0.5) * (tblock.vertices[0] + tblock.vertices[1]);
		tempchild2.vertices[1] = tblock.vertices[1];
		tempchild2.vertices[2] = float(0.5) * (tblock.vertices[1] + tblock.vertices[2]);
		tempchild2.vertices[3] = float(0.25) * (tblock.vertices[0] + tblock.vertices[1] + tblock.vertices[2] + tblock.vertices[3]);
		m_Child.push_back(tempchild2);

		if (tblock.blockID < 0) tblock = temperror;
		TessBumpBlock tempchild3(m_Block.size() + m_Child.size() + 1);
		tempchild3.level = tblock.level + 1;
		//tempchild3.parent = tblock.blockID;
		for (int j = 0; j < 4; j++)
		{
			tempchild3.boundary[j] = tblock.boundary[j] / child_index[2][j];
		}
		tempchild3.centerlevel = tblock.centerlevel;
		tempchild3.vertices[0] = float(0.25) * (tblock.vertices[0] + tblock.vertices[1] + tblock.vertices[2] + tblock.vertices[3]);
		tempchild3.vertices[1] = float(0.5) * (tblock.vertices[1] + tblock.vertices[2]);
		tempchild3.vertices[2] = tblock.vertices[2];
		tempchild3.vertices[3] = float(0.5) * (tblock.vertices[2] + tblock.vertices[3]);
		m_Child.push_back(tempchild3);

		if (tblock.blockID < 0) tblock = temperror;
		TessBumpBlock tempchild4(m_Block.size() + m_Child.size() + 1);
		tempchild4.level = tblock.level + 1;
		//tempchild4.parent = tblock.blockID;
		for (int j = 0; j < 4; j++)
		{
			tempchild4.boundary[j] = tblock.boundary[j] / child_index[3][j];
		}
		tempchild4.centerlevel = tblock.centerlevel;
		tempchild4.vertices[0] = float(0.5) * (tblock.vertices[0] + tblock.vertices[3]);
		tempchild4.vertices[1] = float(0.25) * (tblock.vertices[0] + tblock.vertices[1] + tblock.vertices[2] + tblock.vertices[3]);
		tempchild4.vertices[2] = float(0.5) * (tblock.vertices[2] + tblock.vertices[3]);
		tempchild4.vertices[3] = tblock.vertices[3];
		m_Child.push_back(tempchild4);
	}
}

void TessBumpTerrain::updateHtex(float currentX, float currentY, float currentZ)
{
	m_Child.clear();

	memset(needUpdate, 0, sizeof(needUpdate));

	for (int i = 0; i < m_Block.size(); i++)
	{
		m_Block[i].isparent = false;
		updateBlock(currentX, currentZ, m_Block[i]);
		if (m_Block[i].visible && !m_Block[i].isparent)
		{
			int lx, rx, ly, ry;
			lx = m_Block[i].vertices[0].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			rx = m_Block[i].vertices[1].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			ly = m_Block[i].vertices[0].y / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;
			ry = m_Block[i].vertices[2].y / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;

			int maxCoor = CHUNKNUMBER;
			clamp(lx, 0, maxCoor);
			clamp(ly, 0, maxCoor);
			clamp(rx, 0, maxCoor);
			clamp(ry, 0, maxCoor);

			for (int k1 = lx; k1 <= rx; k1++)
				for (int k2 = ly; k2 <= ry; k2++) {
					needUpdate[k1][k2] |= 1;
				}
		}
	}

	for (int i = 0; i < m_Child.size(); i++)
	{
		m_Child[i].isparent = false;
		updateBlock(currentX, currentZ, m_Child[i]);
		if (m_Child[i].visible && !m_Child[i].isparent)
		{
			int lx, rx, ly, ry;
			lx = m_Child[i].vertices[0].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			rx = m_Child[i].vertices[1].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			ly = m_Child[i].vertices[0].y / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;
			ry = m_Child[i].vertices[2].y / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;

			int maxCoor = CHUNKNUMBER;
			clamp(lx, 0, maxCoor);
			clamp(ly, 0, maxCoor);
			clamp(rx, 0, maxCoor);
			clamp(ry, 0, maxCoor);

			for (int k1 = lx; k1 <= rx; k1++)
				for (int k2 = ly; k2 <= ry; k2++) {
					needUpdate[k1][k2] |= 1;
				}
		}
	}
	int geolevel[1<<LEVEL-1][1 << LEVEL - 1];

	htex.clear();
	htex.update(currentX, currentZ, currentY, hLevel, hLevel1, m_WVP,geolevel,false,needUpdate);
	htex.checkThreadState();
	htex.generateTex();

	glBindTexture(GL_TEXTURE_2D, hLevelTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_FLOAT, hLevel);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, hLevelTex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, hLevel1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TessBumpTerrain::updateBtex()
{
	static float data[FEEDBACK_WIDTH * FEEDBACK_HEIGHT * 4];
	static set<pair<int, int> > bSet;
	bSet.clear();
	feedback.begin();
	glReadPixels(0, 0, FEEDBACK_WIDTH, FEEDBACK_HEIGHT, GL_RGBA, GL_FLOAT, (void*)&data);
	feedback.end();
	btex.clear();
	ntex.clear();
	ntex.checkThreadState();
	btex.checkThreadState();

	float horizondata[FEEDBACK_WIDTH];
#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < FEEDBACK_WIDTH; i++)
			horizondata[i] = 0.0;
	}
	int tot = 0;
#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < FEEDBACK_HEIGHT; i++)
			for (int j = 0; j < FEEDBACK_WIDTH; j++) {
				float *p = data + (i * FEEDBACK_WIDTH + j) * 4;
				if (fabs(p[3]) < 1e-6)
					continue;

				horizondata[j] = p[3];

				p[2] += log2((float)FEEDBACK_WIDTH / (float)WIDTH);
				int l = (int)round(p[2] - 0.5);
				int r = (int)round(p[2] + 0.5);
				int x, y;
				x = floor(p[0] / (1.0 / CHUNKNUMBER) - 1e-6);
				y = floor(p[1] / (1.0 / CHUNKNUMBER) - 1e-6);
				clamp(x, 0, CHUNKNUMBER - 1);
				clamp(y, 0, CHUNKNUMBER - 1);
				clamp(l, 0, btex.getMaxLevel() - 1);
				clamp(r, 0, btex.getMaxLevel() - 1);
				int tx, ty;
				tx = x >> l;
				ty = y >> l;
				if (bSet.find(make_pair(l, ty * (CHUNKNUMBER >> l) + tx)) == bSet.end()) {
					ntex.update(l, tx, ty, false);
					btex.update(l, tx, ty, false);
					bSet.insert(make_pair(l, ty * (CHUNKNUMBER >> l) + tx));
					tot++;
				}
				if (l != r) {
					tx = x >> r;
					ty = y >> r;
					if (bSet.find(make_pair(l, ty * (CHUNKNUMBER >> r) + tx)) == bSet.end()) {
						tot++;
						ntex.update(l, tx, ty, false);
						btex.update(l, tx, ty, false);
						bSet.insert(make_pair(l, ty * (CHUNKNUMBER >> r) + tx));
					}
				}
			}
	}
	btex.generateTex();
	ntex.generateTex();

#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < FEEDBACK_WIDTH; i++)
		{
			int id = horizondata[i] * (m_Block.size() + m_Child.size());
			if (id <= m_Block.size())
				m_Block[id - 1].ishorizon = true;
			else
				m_Child[id - m_Block.size() - 1].ishorizon = true;
		}
	}
}

void TessBumpTerrain::updateBlock()
{
	for (int i = 0; i < m_Block.size(); i++)
	{
		m_Block[i].ChangeBRDF();
	}
	for (int i = 0; i < m_Child.size(); i++)
	{
		m_Child[i].ChangeBRDF();
	}

}

void TessBumpTerrain::ChildBorderCrack(TessBumpBlock& tblock)
{
	int id = tblock.blockID - m_Block.size() - 1;
	int remain = id % 4;
	if (remain == 0)
	{
		if (m_Child[id + 1].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}

		if (m_Child[id + 3].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 1)
	{
		if (m_Child[id - 1].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Child[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 2)
	{
		if (m_Child[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Child[id + 1].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}
	}
	else if (remain == 3)
	{
		if (m_Child[id - 1].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}

		if (m_Child[id - 3].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}
	}
}

void TessBumpTerrain::BorderCrack(TessBumpBlock& tblock)
{
	int id = tblock.blockID - 1;
	if (id >= LEVEL * 12)
	{
		return;
	}
	int remain = id % 12;
	if (remain == 0)
	{
		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}

		if (m_Block[id + 4].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	else if (remain == 1)
	{
		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 2)
	{
		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 3)
	{
		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Block[id + 2].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	if (remain == 4)
	{
		if (m_Block[id - 4].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id + 2].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	else if (remain == 5)
	{
		if (m_Block[id - 2].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id + 2].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	else if (remain == 6)
	{
		if (m_Block[id - 2].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id + 2].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	else if (remain == 7)
	{
		if (m_Block[id - 2].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id + 4].BRDF == 1)
		{
			tblock.isboundary[1] = false;
		}
	}
	if (remain == 8)
	{
		if (m_Block[id - 2].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 9)
	{
		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 10)
	{
		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}

		if (m_Block[id + 1].BRDF == 1)
		{
			tblock.isboundary[3] = false;
		}
	}
	else if (remain == 11)
	{
		if (m_Block[id - 4].BRDF == 1)
		{
			tblock.isboundary[0] = false;
		}

		if (m_Block[id - 1].BRDF == 1)
		{
			tblock.isboundary[2] = false;
		}
	}
}


void TessBumpTerrain::ViewFrustum()
{
	glm::vec3 deltaPos = currentPos - lastPos;
	glm::vec3 deltaDir = currentDir - lastDir;

	if (sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y + deltaPos.z * deltaPos.z) > 50) transferdata = true;
	else
	{
		if (sqrt(deltaDir.x * deltaDir.x + deltaDir.y * deltaDir.y + deltaDir.z * deltaDir.z) > 0.5) transferdata = true;
		else transferdata = false;
	}

	if (transferdata)
	{
		lastPos = currentPos;
		lastDir = currentDir;
	}
}

void TessBumpTerrain::VTPassGenerateVertices()
{
	m_vtvertices.clear();
	for (int i = 0; i < m_Block.size(); i++)
	{
		if (m_Block[i].visible && !m_Block[i].isparent)
		{
			for (int j = 0; j < 4; j++)
			{
				glm::vec3 vertices = m_Block[i].vertices[j] + glm::vec3(0.0, m_Block[i].blockID, 0.0);
				m_vtvertices.push_back(vertices);
			}
		}
	}

	for (int i = 0; i < m_Child.size(); i++)
	{
		if (m_Child[i].visible && !m_Child[i].isparent)
		{
			for (int j = 0; j < 4; j++)
			{
				glm::vec3 vertices = m_Child[i].vertices[j] + glm::vec3(0.0, m_Child[i].blockID, 0.0);
				m_vtvertices.push_back(vertices);
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxBlockNumber, 0, GL_DYNAMIC_DRAW);

	glm::vec3 *vtmapped = reinterpret_cast<glm::vec3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_vtvertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	std::copy(m_vtvertices.begin(), m_vtvertices.end(), vtmapped);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TessBumpTerrain::RenderPassGenerateVertices()
{
	updateBlock();
	m_vertices.clear();
	m_bumpvertices.clear();
	m_bordervertices.clear();

	int changeindex[4] = { 2,1,3,0 };

	for (int i = 0; i < m_Block.size(); i++)
	{
		if (m_Block[i].visible && !m_Block[i].isparent)
		{
			if (m_Block[i].BRDF == 0)
			{
				for (int j = 0; j < 4; j++)
				{
					glm::vec3 vertices = m_Block[i].vertices[j] + glm::vec3(0.0, m_Block[i].boundary[j], 0.0);
					m_vertices.push_back(vertices);
				}
			}
			else
			{
				for (int j = 0; j < 3; j++)
				{
					m_bumpvertices.push_back(m_Block[i].vertices[j]);
				}
				m_bumpvertices.push_back(m_Block[i].vertices[0]);
				m_bumpvertices.push_back(m_Block[i].vertices[2]);
				m_bumpvertices.push_back(m_Block[i].vertices[3]);

				BorderCrack(m_Block[i]);

				for (int j = 0; j < 4; j++)
				{
					if (m_Block[i].isboundary[changeindex[j]])
					{
						glm::vec3 vertices = m_Block[i].vertices[j] + glm::vec3(0.0, m_Block[i].boundary[changeindex[j]], 0.0);
						m_bordervertices.push_back(vertices);
						m_bordervertices.push_back(m_Block[i].vertices[(j + 1) % 4]);
						m_bordervertices.push_back(m_Block[i].vertices[(j + 1) % 4]);
						m_bordervertices.push_back(m_Block[i].vertices[j]);
					}
				}
			}
		}
	}

	for (int i = 0; i < m_Child.size(); i++)
	{
		if (m_Child[i].visible && !m_Child[i].isparent)
		{
			if (m_Child[i].BRDF == 0)
			{
				for (int j = 0; j < 4; j++)
				{
					glm::vec3 vertices = m_Child[i].vertices[j] + glm::vec3(0.0, m_Child[i].boundary[j], 0.0);
					m_vertices.push_back(vertices);
				}
			}
			else
			{
				for (int j = 0; j < 3; j++)
				{
					m_bumpvertices.push_back(m_Child[i].vertices[j]);
				}
				m_bumpvertices.push_back(m_Child[i].vertices[0]);
				m_bumpvertices.push_back(m_Child[i].vertices[2]);
				m_bumpvertices.push_back(m_Child[i].vertices[3]);

				ChildBorderCrack(m_Child[i]);

				for (int j = 0; j < 4; j++)
				{
					if (m_Child[i].isboundary[changeindex[j]])
					{
						glm::vec3 vertices = m_Child[i].vertices[j] + glm::vec3(0.0, m_Child[i].boundary[changeindex[j]], 0.0);
						m_bordervertices.push_back(vertices);
						m_bordervertices.push_back(m_Child[i].vertices[(j + 1) % 4]);
						m_bordervertices.push_back(m_Child[i].vertices[(j + 1) % 4]);
						m_bordervertices.push_back(m_Child[i].vertices[j]);
					}
				}
			}
		}
	}


	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxBlockNumber, 0, GL_DYNAMIC_DRAW);

	glm::vec3 *mapped = reinterpret_cast<glm::vec3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_vertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	std::copy(m_vertices.begin(), m_vertices.end(), mapped);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxBlockNumber, 0, GL_DYNAMIC_DRAW);

	glm::vec3 *bumpmapped = reinterpret_cast<glm::vec3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_bumpvertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	std::copy(m_bumpvertices.begin(), m_bumpvertices.end(), bumpmapped);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MaxBlockNumber, 0, GL_DYNAMIC_DRAW);

	glm::vec3 *bordermapped = reinterpret_cast<glm::vec3*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_bordervertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	std::copy(m_bordervertices.begin(), m_bordervertices.end(), bordermapped);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TessBumpTerrain::Render()
{
	ViewFrustum();

	glClearDepth(1.0f);
	glClearColor(0.46f, 0.46f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (transferdata)
		RenderPassGenerateVertices();

	if (m_linemode)
	{
		m_Textureflag = 3;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		lineon = true;
	}
	else
	{
		if (lineon)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_Textureflag = 1;
			lineon = false;
		}
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	m_specular.Enable();

	m_specular.SetCurrentPosition(glm::vec4(currentPos.x / CHUNKNUMBER / CHUNKSIZE, currentPos.z / CHUNKNUMBER / CHUNKSIZE, currentPos.y, 0.0));
	m_specular.SetWVP(m_WVP);
	m_specular.SetWorldMatrix(m_World);
	m_specular.SetDirectionalLight(m_directionalLight);
	m_specular.SetEyeWorldPos(m_camerapos);
	m_specular.SetMatSpecularIntensity(1.0f);
	m_specular.SetMatSpecularPower(1.0f);
	m_specular.SetShowAO(AOflag);
	m_specular.SetShowTexture(m_Textureflag);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, htex.getTex());

	vector<PTex*> ptexes;
	if (!transferdata)ptexes = htex.getPtexes();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptexes[0]->getTex());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, Maxmintexture);

	glBindVertexArray(vao[1]);

	glDrawArrays(GL_PATCHES, 0, m_vertices.size());

	glBindVertexArray(0);
	glUseProgram(0);

	m_bump.Enable();
	m_bump.SetWVP(m_WVP);

	m_bump.SetCurrentPosition(glm::vec4(currentPos.x / CHUNKNUMBER / CHUNKSIZE, currentPos.z / CHUNKNUMBER / CHUNKSIZE, currentPos.y, 0.0));

	m_bump.SetWorldMatrix(m_World);
	m_bump.SetDirectionalLight(m_directionalLight);
	m_bump.SetEyeWorldPos(m_camerapos);
	m_bump.SetMatSpecularIntensity(1.0f);
	m_bump.SetMatSpecularPower(1.0f);
	m_bump.SetShowAO(AOflag);
	m_bump.SetShowTexture(m_Textureflag);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, htex.getTex());

	if (!transferdata)ptexes = htex.getPtexes();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptexes[0]->getTex());

	glBindVertexArray(vao[2]);

	glDrawArrays(GL_TRIANGLES, 0, m_bumpvertices.size());

	glBindVertexArray(0);
	glUseProgram(0);

	m_border.Enable();
	m_border.SetWVP(m_WVP);

	m_border.SetCurrentPosition(glm::vec4(currentPos.x / CHUNKNUMBER / CHUNKSIZE, currentPos.z / CHUNKNUMBER / CHUNKSIZE, currentPos.y, 0.0));

	m_border.SetWorldMatrix(m_World);
	m_border.SetDirectionalLight(m_directionalLight);
	m_border.SetEyeWorldPos(m_camerapos);
	m_border.SetMatSpecularIntensity(1.0f);
	m_border.SetMatSpecularPower(1.0f);
	m_border.SetShowAO(AOflag);
	m_border.SetShowTexture(m_Textureflag);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, htex.getTex());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptexes[0]->getTex());

	glBindVertexArray(vao[3]);

	glDrawArrays(GL_PATCHES, 0, m_bordervertices.size());

	glBindVertexArray(0);
	glUseProgram(0);
}