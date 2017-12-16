#include "tessterrain.h"
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
/*
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
	*/
	r = MAXSCALE * 4 / 32;

	float offset = MAXSCALE * 2;

	for(int i = 0; i< 32; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			TessBumpBlock new_block(index++);
			new_block.vertices[0] = glm::vec3(r*i - offset, 0, r*j - offset);
			new_block.vertices[1] = glm::vec3(r*(i + 1) - offset, 0, r*j - offset);
			new_block.vertices[2] = glm::vec3(r*(i + 1) - offset, 0, r*(j + 1) - offset);
			new_block.vertices[3] = glm::vec3(r*i - offset, 0, r*(j + 1) - offset);

			new_block.level = 0;
			new_block.centerlevel = 0;

			m_Block.push_back(new_block);
		}
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
		for(int j = 0; j<4;  j++)
		m_Block[i].boundary[j] = 32;
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

	memset(hLevel, 0, sizeof(hLevel));
	memset(hLevel1, 0, sizeof(hLevel1));
}

bool TessBumpTerrain::InitShader()
{
	m_vtshader.Init();
	m_vtshader.Enable();
	m_vtshader.SetVtex(0);
	m_vtshader.SetHeightmap(1);
	m_vtshader.SetHLevel(2);
	m_vtshader.SetHLevel1(3);
	glUseProgram(0);

	m_specular.Init();
	m_specular.Enable();
	m_specular.SetVtex(0);
	m_specular.SetHeightmap(1);
	m_specular.SetHLevel(2);
	m_specular.SetHLevel1(3);
	m_specular.SetBtex(4);
	m_specular.SetColortex(5);
	m_specular.SetNtex(6);
	m_specular.SetNormaltex(7);
	m_specular.SetSatTex(8);
	m_specular.SetAOtex(9);
	m_specular.SetAOMaptex(10);
	m_specular.SetAOlevelTex(11);
	m_specular.SetAOmaxdiffTex(12);
//	m_specular.SetNormallevel(13);
	
	glUseProgram(0);

	m_bump.Init();
	m_bump.Enable();
	m_bump.SetVtex(0);
	m_bump.SetHeightmap(1);
	m_bump.SetHLevel(2);
	m_bump.SetHLevel1(3);
	m_bump.SetBtex(4);
	m_bump.SetColortex(5);
	m_bump.SetNtex(6);
	m_bump.SetNormaltex(7);
	m_bump.SetAOtex(9);
	m_bump.SetAOMaptex(10);
	m_bump.SetAOlevelTex(11);
//	m_bump.SetNormallevel(12);
	glUseProgram(0);

	m_border.Init();
	m_border.Enable();
	m_border.SetVtex(0);
	m_border.SetHeightmap(1);
	m_border.SetHLevel(2);
	m_border.SetHLevel1(3);
	m_border.SetBtex(4);
	m_border.SetColortex(5);
	m_border.SetNtex(6);
	m_border.SetNormaltex(7);
	m_border.SetAOtex(9);
	m_border.SetAOMaptex(10);
	m_border.SetAOlevelTex(11);
//	m_border.SetNormallevel(12);
	glUseProgram(0);

	return true;
}

void TessBumpTerrain::InitAOTexture()
{
	memset(AOlevel, LEVELOFTERRAIN - 1, CHUNKNUMBER*CHUNKNUMBER);
#pragma omp parallel for schedule(dynamic, 1)
	{
		for(int i = 0; i < CHUNKNUMBER; i++)
			for (int j = 0; j < CHUNKNUMBER; j++)
			{
				string filename,filename1;
				FILE *f, *f1;
				filename = "data/" + getChunkName(i, j) + "/AOerrList";
				filename1 = "data/" + getChunkName(i, j) + "/maxAOdiff";
				openfile(filename.c_str(), f);
				openfile(filename1.c_str(), f1);
				for (int k = 0; k < LEVELOFTERRAIN; k++) {
					fread(&AOerrorlist[i][j][k], sizeof(float), 1, f);
					fread(&maxAOdiff[i][j][k], sizeof(float), 1, f1);
				}
				fclose(f);
				fclose(f1);
			}
	}


	FILE *fp = fopen("data/maxAO.txt", "r");
	for (int i = 0; i < CHUNKNUMBER; i++)
		for (int j = 0; j < CHUNKNUMBER; j++)
		{ 
			fscanf(fp, "%f", &maxAO[i][j]);
		}
	fclose(fp);


	glGenTextures(1, &AOmaxdifftex);
	glBindTexture(GL_TEXTURE_2D, AOmaxdifftex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &AOlevelTex);
	glBindTexture(GL_TEXTURE_2D, AOlevelTex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void TessBumpTerrain::InitNormalTexture()
{
	glGenTextures(1, &NormallevelTex);
	glBindTexture(GL_TEXTURE_2D, NormallevelTex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

float TessBumpTerrain::CalculateFunction(float phi1, float phi2, float k)
{
	return glm::cos(glm::radians(phi1)) * glm::cos(glm::radians(phi2)) + k * glm::sin(glm::radians(phi1)) * glm::sin(glm::radians(phi2));
}

void TessBumpTerrain::CalculateAngle(int index1, int index2)
{
	int phi1 = index1 / NORMAL_THETA;
	int theta1 = index1 % NORMAL_THETA;

	int phi2 = index2 / NORMAL_THETA;
	int theta2 = index2 % NORMAL_THETA;

	float thetamin = glm::abs(theta1 * THETA_ANGLE - (theta2 + 1) * THETA_ANGLE);
	float thetamax = glm::abs((theta1 + 1)* THETA_ANGLE - theta2 * THETA_ANGLE);
	if (thetamax < thetamin)
	{
		thetamax += thetamin;
		thetamin = thetamax - thetamin;
		thetamax = thetamax - thetamin;
	}

	float kmax = -1, kmin = 1;
	if (thetamax > 0 && thetamin < 0)
	{
		kmax = 1;
	}
	if (thetamax > 180 && thetamin < 180)
	{
		kmin = -1;
	}

	thetamax = glm::cos(glm::radians(thetamax));
	thetamin = glm::cos(glm::radians(thetamin));

	if (thetamax > kmax)
	{
		kmax = thetamax;
	}

	if (thetamin > kmax)
	{
		kmax = thetamin;
	}

	if (thetamax < kmin)
	{
		kmin = thetamax;
	}

	if (thetamin < kmin)
	{
		kmin = thetamin;
	}

	float maxresult = -1, minresult = 1;
//function: y = cosa * cosb + k * sina * sinb;
	float phi1min = phi1 * PHI_ANGLE;
	float phi1max = (phi1 + 1) * PHI_ANGLE;
	float phi2min = phi2 * PHI_ANGLE;
	float phi2max = (phi2 + 1) * PHI_ANGLE;
	
	float result = CalculateFunction(phi1min, phi2min, kmax);
	if(result > maxresult) maxresult = result;

    result = CalculateFunction(phi1max, phi2min, kmax);
	if (result > maxresult) maxresult = result;

	result = CalculateFunction(phi1min, phi2max, kmax);
	if (result > maxresult) maxresult = result;

    result = CalculateFunction(phi1max, phi2max, kmax);
	if (result > maxresult) maxresult = result;

	if (phi1min == 90)
	{
		if (phi2min < 90 && phi2max > 90 && maxresult < kmax)
			maxresult = kmax;

		float tempphi = glm::tan(glm::radians(phi1max)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmax);
			if (result > maxresult) maxresult = result;
		}
	}
	else if (phi1max == 90)
	{
		if (phi2min < 90 && phi2max > 90 && maxresult < kmax)
			maxresult = kmax;

		float tempphi = glm::tan(glm::radians(phi1min)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmax);
			if (result > maxresult) maxresult = result;
		}
	}
	else
	{
		float tempphi = glm::tan(glm::radians(phi1min)) * kmax;
	    tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmax);
			if (result > maxresult) maxresult = result;
		}
	
	    tempphi = glm::tan(glm::radians(phi1max)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmax);
			if (result > maxresult) maxresult = result;
		}
	}

	if (phi2min == 90)
	{
		if (phi1min < 90 && phi1max > 90 && maxresult < kmax)
			maxresult = kmax;

		float tempphi = glm::tan(glm::radians(phi2max)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2max, kmax);
			if (result > maxresult) maxresult = result;
		}

	}
	else if (phi2max == 90)
	{
		float tempphi = glm::tan(glm::radians(phi2min)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2min, kmax);
			if (result > maxresult) maxresult = result;
		}
	}
	else {
		float tempphi = glm::tan(glm::radians(phi2min)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2min, kmax);
			if (result > maxresult) maxresult = result;
		}

		tempphi = glm::tan(glm::radians(phi2max)) * kmax;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2max, kmax);
			if (result > maxresult) maxresult = result;
		}
	}

    result = CalculateFunction(phi1min, phi2min, kmin);
	if (result < minresult) minresult = result;

	result = CalculateFunction(phi1max, phi2min, kmin);
	if (result < minresult) minresult = result;

	result = CalculateFunction(phi1min, phi2max, kmin);
	if (result < minresult) minresult = result;

	result = CalculateFunction(phi1max, phi2max, kmin);
	if (result < minresult) minresult = result;

	if (phi1min == 90 )
	{
		float tempphi = glm::tan(glm::radians(phi1max)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmin);
			if (result < minresult) minresult = result;
		}
	}
	else if (phi1max == 90)
	{
		float tempphi = glm::tan(glm::radians(phi1min)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmin);
			if (result < minresult) minresult = result;
		}
	}
	else
	{
		float tempphi = glm::tan(glm::radians(phi1min)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmin);
			if (result < minresult) minresult = result;
		}

		tempphi = glm::tan(glm::radians(phi1max)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi2min && tempphi < phi2max)
		{
			result = CalculateFunction(phi1min, tempphi, kmin);
			if (result < minresult) minresult = result;
		}
	}

	if (phi2min == 90 )
	{
		float tempphi = glm::tan(glm::radians(phi2max)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2max, kmin);
			if (result < minresult) minresult = result;
		}
	}
	else if (phi2max == 90)
	{
		float tempphi = glm::tan(glm::radians(phi2min)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2min, kmin);
			if (result < minresult) minresult = result;
		}
	}
	else {
		float tempphi = glm::tan(glm::radians(phi2min)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2min, kmin);
			if (result < minresult) minresult = result;
		}

		tempphi = glm::tan(glm::radians(phi2max)) * kmin;
		tempphi = glm::atan(tempphi);
		tempphi = tempphi >= 0 ? tempphi : tempphi + M_PI;
		tempphi = glm::degrees(tempphi);
		if (tempphi > phi1min && tempphi < phi1max)
		{
			result = CalculateFunction(tempphi, phi2max, kmin);
			if (result < minresult) minresult = result;
		}
	}

	spaceangle[index1][index2][0] = spaceangle[index2][index1][0] = minresult;
	spaceangle[index1][index2][0] = spaceangle[index2][index1][0] = maxresult;
}

void TessBumpTerrain::InitNormalData()
{
	for(int i = 0; i<NORMAL_PHI * NORMAL_THETA; i++)
		for (int j = i; j < NORMAL_PHI * NORMAL_THETA; j++)
		{
			if (j == i)
			{
				spaceangle[i][j][0] = 0;
				spaceangle[i][j][1] = 0;
				continue;
			}

			CalculateAngle(i, j);
		}

	memset(normaldistribution, 0, CHUNKNUMBER * CHUNKNUMBER * (NORMAL_PHI * NORMAL_THETA / 32));
	
	FILE *f;
	openfile("data/normaldistribution", f);
	fread(normaldistribution, sizeof(int), CHUNKNUMBER * CHUNKNUMBER * (NORMAL_PHI * NORMAL_THETA / 32), f);
	fclose(f);

	string filename;
	for (int j = 0; j < CHUNKNUMBER; j++)
		for (int i = 0; i< CHUNKNUMBER; i++)
		{
			filename = "data/" + getChunkName(i, j) + "/NormalerrList";
			openfile(filename.c_str(), f);
			fread(normaldeviation[i][j], sizeof(float), LEVELOFTERRAIN, f);
			fclose(f);
		}

	openfile("data/maxminheight", f);
	fread(maxminheight, sizeof(int), CHUNKNUMBER * CHUNKNUMBER * 2, f);
	fclose(f);

}

bool TessBumpTerrain::Init()
{
 	InitVertices();

	InitShader();

	InitHeightLevel();

	InitAOTexture();

	//初始化，合成texture
//	if (!InitTexture()) return false;

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	feedback.init(FEEDBACK_WIDTH, FEEDBACK_HEIGHT);

	htex.init(CHUNKSIZE, CHUNKNUMBER, "dem", LEVELOFTERRAIN);
	btex.init(BLENDSIZE, CHUNKNUMBER, "texture", LEVELOFBLENDTEX);
	ntex.init(CHUNKSIZE, CHUNKNUMBER, "newnormal", LEVELOFTERRAIN);
	AOtex.init(CHUNKSIZE,CHUNKNUMBER, "AO", LEVELOFTERRAIN);

	VTex::initThread();
	htex.clear();
	htex.loadCoarsest();
	btex.clear();
	btex.loadCoarsest();
	ntex.clear();
	ntex.loadCoarsest();
	AOtex.clear();
	AOtex.loadCoarsest();

	
	m_directionalLight.AmbientIntensity = 0.9f;
	m_directionalLight.DiffuseIntensity = 0.5f;
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.Direction = glm::vec3(-1.0f, -1.0f, 0.0f);
	return true;
}

void TessBumpTerrain::updateBlock(float currentX, float currentZ, TessBumpBlock &tblock)
{
	int xs, xb, ys, yb, zs, zb;
	xs = xb = ys = yb = zs = zb = 0;

	float minx, miny, maxx, maxy;
	minx = miny = 1;
	maxx = maxy = -1;

	for (int i = 0; i < 4; i++)
		for (int k = -1; k <= 1; k = k+2) {
			glm::vec4 pos = glm::vec4(tblock.vertices[i], 1.0) + glm::vec4(0.0, k * 4000, 0.0, 0.0);
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

	float sidelength = tblock.vertices[1].x - tblock.vertices[0].x;
	float heightresolution = sidelength * (VIEWCHUNKNUMBER * CHUNKSIZE / 4.0 / MAXSCALE);

	if (heightresolution > 32)
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
			if (child_index[0][j] == 1)tempchild1.boundary[j] = 16;
			else
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
			if (child_index[1][j] == 1)tempchild2.boundary[j] = 16;
			else
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
			if (child_index[2][j] == 1)tempchild3.boundary[j] = 16;
			else
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
			if (child_index[3][j] == 1)tempchild4.boundary[j] = 16;
			else
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
			m_Block[i].lx = lx = m_Block[i].vertices[0].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			m_Block[i].rx = rx = m_Block[i].vertices[1].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			m_Block[i].ly = ly = m_Block[i].vertices[0].z / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;
			m_Block[i].ry = ry = m_Block[i].vertices[2].z / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;

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
			m_Child[i].lx = lx = m_Child[i].vertices[0].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			m_Child[i].rx = rx = m_Child[i].vertices[1].x / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentX * CHUNKNUMBER;
			m_Child[i].ly = ly = m_Child[i].vertices[0].z / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;
			m_Child[i].ry = ry = m_Child[i].vertices[2].z / MAXSCALE / 4 * VIEWCHUNKNUMBER + currentZ * CHUNKNUMBER;

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

void TessBumpTerrain::UpdateAOLevel()
{
	if (m_directionalLight.AmbientIntensity == m_oldlight.AmbientIntensity)
	{
		return;
	}

	m_oldlight.AmbientIntensity = m_directionalLight.AmbientIntensity;
	float nowaoerror = AOERROR / m_directionalLight.AmbientIntensity;

#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < CHUNKNUMBER; i++)
			for (int j = 0; j < CHUNKNUMBER; j++)
				for (int k = LEVELOFTERRAIN - 1; k >= 0 ; k--) {
					if (AOerrorlist[i][j][k] <= nowaoerror)
					{
						AOlevel[i][j] = k;
						break;
					}
				}
	} 
	unsigned char tempmaxdiff[CHUNKNUMBER][CHUNKNUMBER];
#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < CHUNKNUMBER; i++)
			for (int j = 0; j < CHUNKNUMBER; j++)
			{
				tempmaxdiff[i][j] = int(maxAOdiff[i][j][AOlevel[i][j]] * 255);
			}
	}

	glBindTexture(GL_TEXTURE_2D, AOmaxdifftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, tempmaxdiff);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, AOlevelTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, AOlevel);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned char TessBumpTerrain::CalculateNormalLightLevel(float maxerror, float minerror, float derivation[LEVELOFTERRAIN])
{
	if (maxerror < minerror) return LEVELOFTERRAIN - 1;
	unsigned char ret;
	for (ret = LEVELOFTERRAIN - 1; ret > 0; ret--)
	{
		if (derivation[ret] < glm::cos(glm::radians(30.0))) continue;
		
		float maxerror = 0;
		float angle = glm::degrees(glm::acos(derivation[ret]));
		float mintheta = glm::degrees(glm::acos(maxerror));
		float maxtheta = glm::degrees(glm::acos(minerror));

	    float result = glm::abs(glm::clamp(glm::cos(glm::radians(maxtheta - angle)),(float)0,(float)1) - glm::clamp(glm::cos(glm::radians(maxtheta)), (float)0, (float)1));
		if (result > maxerror) maxerror = result;

	    result = glm::abs(glm::clamp(glm::cos(glm::radians(mintheta - angle)), (float)0, (float)1) - glm::clamp(glm::cos(glm::radians(mintheta)), (float)0, (float)1));
		if (result > maxerror) maxerror = result;


		if (maxtheta > 90 && mintheta < 90)
		{
			result = glm::cos(glm::radians(90 - angle));
			if (result > maxerror) maxerror = result;
		}

		if (angle > maxtheta)
		{
			result = glm::abs(1.0 - glm::clamp(glm::cos(glm::radians(maxtheta)), (float)0, (float)1));
			if (result > maxerror) maxerror = result;
		}

		if (angle > mintheta)
		{
			result = glm::abs(1.0 - glm::clamp(glm::cos(glm::radians(mintheta)), (float)0, (float)1));
			if (result > maxerror) maxerror = result;
		}

		if (angle > mintheta && angle < maxtheta)
		{
			result = glm::abs(1.0 - glm::clamp(glm::cos(glm::radians(angle)), (float)0, (float)1));
			if (result > maxerror) maxerror = result;
		}
		
		result = glm::abs(glm::clamp(glm::cos(glm::radians(maxtheta + angle)), (float)0, (float)1) - glm::clamp(glm::cos(glm::radians(maxtheta)), (float)0, (float)1));
		if (result > maxerror) maxerror = result;

		result = glm::abs(glm::clamp(glm::cos(glm::radians(mintheta + angle)), (float)0, (float)1) - glm::clamp(glm::cos(glm::radians(mintheta)), (float)0, (float)1));
		if (result > maxerror) maxerror = result;
		
		if (mintheta < 90 - angle && maxtheta > 90 - angle)
		{
			result = glm::clamp(glm::cos(glm::radians(90 - angle)), (float)0, (float)1);
			if (result > maxerror) maxerror = result;
		}

		if (maxerror < SHADING_ERROR) break;
	}
	return ret;
}

void TessBumpTerrain::UpdateNormalLight()
{
	if (m_directionalLight.Direction == m_oldlight.Direction)
	{
		return;
	}

	m_oldlight.Direction = m_directionalLight.Direction;
	glm::vec3 direction = glm::normalize(m_directionalLight.Direction);
	float phi = glm::acos(direction.y);
	float theta = glm::atan(direction.z, direction.x);
	int result1 = (int)(phi / M_PI * NORMAL_PHI);
	int result2 = (int)(theta / (2 * M_PI) * NORMAL_THETA);
	int result = result1 * NORMAL_THETA + result2;

	memset(normallevel, LEVELOFTERRAIN - 1, CHUNKNUMBER * CHUNKNUMBER);

	for (int i = 0; i < CHUNKNUMBER; i++)
		for (int j = 0; j < CHUNKNUMBER; j++)
		{
			float minerror = 1, maxerror = -1;
			for (int k = 0; k < NORMAL_PHI * NORMAL_THETA / 32; k++)
			{
				int distribution = normaldistribution[i][j][k];
				for (int s = 0; s < 32; s++)
				{
					if (distribution & (1 << s))
					{
						int index = k * 32 + s;
						if (spaceangle[result][index][0] < minerror) minerror = spaceangle[result][index][0];
						if (spaceangle[result][index][1] > maxerror) maxerror = spaceangle[result][index][1];
					}
				}
			}
			normallevel[i][j] = CalculateNormalLightLevel(maxerror, minerror, normaldeviation[i][j]);
		}
}

float TessBumpTerrain::CalculateHtheta(float viewphi, float viewtheta, float lightphi, float lighttheta)
{
	float y = glm::sin(glm::radians(viewphi)) * glm::sin(glm::radians(viewtheta)) + glm::sin(glm::radians(lightphi)) * glm::sin(glm::radians(lighttheta));
	float x = glm::sin(glm::radians(viewphi)) * glm::cos(glm::radians(viewtheta)) + glm::sin(glm::radians(lightphi)) * glm::cos(glm::radians(lighttheta));
	return glm::atan(y, x);
}

float TessBumpTerrain::CalculateHphi(float viewphi, float lightphi, float k)
{
	float y = glm::cos(glm::radians(viewphi)) + glm::cos(glm::radians(lightphi));
	float x = glm::sqrt(2 + 2 * glm::cos(glm::radians(viewphi)) * glm::cos(glm::radians(lightphi)) + 2 * k * glm::sin(glm::radians(viewphi)) * glm::sin(glm::radians(lightphi)));
	return glm::acos(y / x);
}

void TessBumpTerrain::UpdateHalfVector()
{

	ntex.clear();
	ntex.checkThreadState();

	float currentX = currentPos.x / CHUNKNUMBER / CHUNKSIZE;
	float currentY = currentPos.z / CHUNKNUMBER / CHUNKSIZE;
	float currentH = currentPos.y;

	int w = CHUNKNUMBER;
	int h = CHUNKNUMBER;

	float lx, rx, ly, ry;
	float r = VIEWCHUNKNUMBER;
	r /= CHUNKNUMBER;
	lx = currentX - r / 2;
	rx = currentX + r / 2;
	ly = currentY - r / 2;
	ry = currentY + r / 2;

	int plx, ply, prx, pry;
	plx = floor(lx * w + 1e-6);
	prx = floor(rx * w + 1e-6);
	ply = floor(ly * h + 1e-6);
	pry = floor(ry * h + 1e-6);
	clamp(plx, 0, w - 1);
	clamp(prx, 0, w - 1);
	clamp(ply, 0, h - 1);
	clamp(pry, 0, h - 1);

	glm::vec3 direction = glm::normalize(m_directionalLight.Direction);
	float lightphi = glm::degrees(glm::acos(direction.y));
	float lighttheta = glm::degrees(glm::atan(direction.z, direction.x));

	for (int x = plx; x <= prx; x++)
		for (int y = ply; y <= pry; y++) {
			if (!needUpdate[x][y])
				continue;

			if (normallevel[x][y] == 0)
			{
				ntex.update(0, x, y, false);
				continue;
			}

			float viewmintheta = M_PI, viewmaxtheta = -M_PI;

			glm::vec2 vertex_0 = glm::vec2(x * CHUNKREALSIZE - currentX * CHUNKNUMBER * CHUNKREALSIZE, y * CHUNKREALSIZE - currentY * CHUNKNUMBER * CHUNKREALSIZE);
			glm::vec2 vertex_1 = glm::vec2((x + 1) * CHUNKREALSIZE - currentX * CHUNKNUMBER * CHUNKREALSIZE, y * CHUNKREALSIZE - currentY * CHUNKNUMBER * CHUNKREALSIZE);
			glm::vec2 vertex_2 = glm::vec2((x + 1) * CHUNKREALSIZE - currentX * CHUNKNUMBER * CHUNKREALSIZE, (y + 1) * CHUNKREALSIZE - currentY * CHUNKNUMBER * CHUNKREALSIZE);
			glm::vec2 vertex_3 = glm::vec2(x * CHUNKREALSIZE - currentX * CHUNKNUMBER * CHUNKREALSIZE, (y + 1) * CHUNKREALSIZE - currentY * CHUNKNUMBER * CHUNKREALSIZE);

			float viewtheta = glm::degrees(glm::atan(-vertex_0.y, -vertex_0.x));
			if (viewtheta > viewmaxtheta) viewmaxtheta = viewtheta;
			if (viewtheta < viewmintheta) viewmintheta = viewtheta;

			viewtheta = glm::degrees(glm::atan(-vertex_1.y, -vertex_1.x));
			if (viewtheta > viewmaxtheta) viewmaxtheta = viewtheta;
			if (viewtheta < viewmintheta) viewmintheta = viewtheta;

			viewtheta = glm::degrees(glm::atan(-vertex_2.y, -vertex_2.x));
			if (viewtheta > viewmaxtheta) viewmaxtheta = viewtheta;
			if (viewtheta < viewmintheta) viewmintheta = viewtheta;

			viewtheta = glm::degrees(glm::atan(-vertex_3.y, -vertex_3.x));
			if (viewtheta > viewmaxtheta) viewmaxtheta = viewtheta;
			if (viewtheta < viewmintheta) viewmintheta = viewtheta;

			float minxylength, maxxylength;
			minxylength = 4 * MAXSCALE * MAXSCALE;
			maxxylength = 0;

			if (vertex_0.x * vertex_1.x < 0) minxylength = vertex_0.y;
			if (vertex_0.y * vertex_3.y < 0) minxylength = vertex_0.x;	
				
			float length = vertex_0.x * vertex_0.x + vertex_0.y * vertex_0.y;
			if (length > maxxylength) maxxylength = length;
			if (length < minxylength) minxylength = length;

		    length = vertex_1.x * vertex_1.x + vertex_1.y * vertex_1.y;
			if (length > maxxylength) maxxylength = length;
			if (length < minxylength) minxylength = length;

			length = vertex_2.x * vertex_2.x + vertex_2.y * vertex_2.y;
			if (length > maxxylength) maxxylength = length;
			if (length < minxylength) minxylength = length;

			length = vertex_3.x * vertex_3.x + vertex_3.y * vertex_3.y;
			if (length > maxxylength) maxxylength = length;
			if (length < minxylength) minxylength = length;

			float viewmaxphi = glm::degrees(glm::atan(maxxylength, currentH - maxminheight[x][y][1]));
			float viewminphi = glm::degrees(glm::atan(minxylength, currentH - maxminheight[x][y][0]));

//theta
			float hmaxtheta = -1, hmintheta = 1;
			float htheta = CalculateHtheta(viewminphi, viewmintheta, lightphi, lighttheta);
			if (htheta > hmaxtheta) hmaxtheta = htheta;
			if (htheta < hmintheta) hmintheta = htheta;

			htheta = CalculateHtheta(viewminphi, viewmaxtheta, lightphi, lighttheta);
			if (htheta > hmaxtheta) hmaxtheta = htheta;
			if (htheta < hmintheta) hmintheta = htheta;

			htheta = CalculateHtheta(viewmaxphi, viewmintheta, lightphi, lighttheta);
			if (htheta > hmaxtheta) hmaxtheta = htheta;
			if (htheta < hmintheta) hmintheta = htheta;

			htheta = CalculateHtheta(viewmaxphi, viewmaxtheta, lightphi, lighttheta);
			if (htheta > hmaxtheta) hmaxtheta = htheta;
			if (htheta < hmintheta) hmintheta = htheta;

			if (viewmaxphi > 90 && viewminphi < 90)
			{
				htheta = CalculateHtheta(90, viewmintheta, lightphi, lighttheta);
				if (htheta > hmaxtheta) hmaxtheta = htheta;
				if (htheta < hmintheta) hmintheta = htheta;

				htheta = CalculateHtheta(90, viewmaxtheta, lightphi, lighttheta);
				if (htheta > hmaxtheta) hmaxtheta = htheta;
				if (htheta < hmintheta) hmintheta = htheta;

				float ratio = glm::sin(glm::radians(lightphi));
				if (ratio < 1)
				{
					ratio = glm::degrees(glm::acos(-ratio));

					float temp = lighttheta + glm::degrees(glm::acos(-ratio));
					if (temp > viewmintheta && temp < viewmaxtheta);
					{
						htheta = CalculateHtheta(90, temp, lightphi, lighttheta);
						if (htheta > hmaxtheta) hmaxtheta = htheta;
						if (htheta < hmintheta) hmintheta = htheta;
					}

					temp = lighttheta - glm::degrees(glm::acos(-ratio));
					if (temp > viewmintheta && temp < viewmaxtheta);
					{
						htheta = CalculateHtheta(90, temp, lightphi, lighttheta);
						if (htheta > hmaxtheta) hmaxtheta = htheta;
						if (htheta < hmintheta) hmintheta = htheta;
					}
					temp = 360 + lighttheta - glm::degrees(glm::acos(-ratio));
					if (temp > viewmintheta && temp < viewmaxtheta);
					{
						htheta = CalculateHtheta(90, temp, lightphi, lighttheta);
						if (htheta > hmaxtheta) hmaxtheta = htheta;
						if (htheta < hmintheta) hmintheta = htheta;
					}

					temp = -360 + lighttheta + glm::degrees(glm::acos(-ratio));
					if (temp > viewmintheta && temp < viewmaxtheta);
					{
						htheta = CalculateHtheta(90, temp, lightphi, lighttheta);
						if (htheta > hmaxtheta) hmaxtheta = htheta;
						if (htheta < hmintheta) hmintheta = htheta;
					}
				}
			}

			float ratio = glm::sin(glm::radians(lightphi)) / glm::sin(glm::radians(viewminphi));
			if (ratio < 1)
			{
				ratio = glm::degrees(glm::acos(-ratio));

				float temp = lighttheta + glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewminphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}

				temp = lighttheta - glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewminphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}
				temp = 360 + lighttheta - glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewminphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}

				temp = -360 + lighttheta + glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewminphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}
			}

		    ratio = glm::sin(glm::radians(lightphi)) / glm::sin(glm::radians(viewmaxphi));
			if (ratio < 1)
			{
				ratio = glm::degrees(glm::acos(-ratio));

				float temp = lighttheta + glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewmaxphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}

				temp = lighttheta - glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewmaxphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}
				temp = 360 + lighttheta - glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewmaxphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}

				temp = -360 + lighttheta + glm::degrees(glm::acos(-ratio));
				if (temp > viewmintheta && temp < viewmaxtheta);
				{
					htheta = CalculateHtheta(viewmaxphi, temp, lightphi, lighttheta);
					if (htheta > hmaxtheta) hmaxtheta = htheta;
					if (htheta < hmintheta) hmintheta = htheta;
				}
			}

//phi
			float maxthetavar = viewmaxtheta - lighttheta;
			float minthetavar = viewmintheta - lighttheta;
			float kmax, kmin;

			kmax = kmin = glm::cos(glm::radians(maxthetavar));
			kmax = glm::cos(glm::radians(minthetavar)) > kmax ? glm::cos(glm::radians(minthetavar)) : kmax;
			kmin = glm::cos(glm::radians(minthetavar)) < kmin ? glm::cos(glm::radians(minthetavar)) : kmin;

			if (maxthetavar > 180 && minthetavar < 180 || maxthetavar > -180 && minthetavar < -180)
			{
				kmin = -1;
			}

			if (maxthetavar > 0 && minthetavar < 0)
			{
				kmax = 1;
			}
//max
			float hmaxphi = 0, hminphi = M_PI;
			float hphi = CalculateHphi(viewminphi, lightphi, kmin);
			if (hphi > hmaxphi) hmaxphi = hphi;
			if (hphi < hminphi) hminphi = hphi;

			hphi = CalculateHphi(viewminphi, lightphi, kmin);
			if (hphi > hmaxphi) hmaxphi = hphi;
			if (hphi < hminphi) hminphi = hphi;
//min
			hphi = CalculateHphi(viewminphi, lightphi, kmax);
			if (hphi > hmaxphi) hmaxphi = hphi;
			if (hphi < hminphi) hminphi = hphi;

			hphi = CalculateHphi(viewminphi, lightphi, kmax);
			if (hphi > hmaxphi) hmaxphi = hphi;
			if (hphi < hminphi) hminphi = hphi;

//angle variance
			float maxnhangle = -1, minnhangle = 1;
			int minphinumber = int(glm::degrees(hminphi) / (180 / NORMAL_PHI));
			int maxphinumber = int(glm::degrees(hmaxphi) / (180 / NORMAL_PHI));
			int minthetanumber = int(glm::degrees(hmintheta) / (360 / NORMAL_THETA));
			int maxthetanumber = int(glm::degrees(hmaxtheta) / (360 / NORMAL_THETA));

			float minerror = 1, maxerror = -1;
			for (int i = minphinumber; i < maxphinumber; i++)
				for(int j = minthetanumber; j < maxthetanumber; j++)
			{
				int result = i * NORMAL_THETA + j;
				for (int k = 0; k < NORMAL_PHI * NORMAL_THETA / 32; k++)
				{
					int distribution = normaldistribution[x][y][k];
					for (int s = 0; s < 32; s++)
					{
						if (distribution & (1 << s))
						{
							int index = k * 32 + s;
							if (spaceangle[result][index][0] < minerror) minerror = spaceangle[result][index][0];
							if (spaceangle[result][index][1] > maxerror) maxerror = spaceangle[result][index][1];
						}
					}
				}
			}
			normallevel[x][y] = min(normallevel[x][y], CalculateNormalLightLevel(maxerror, minerror, normaldeviation[x][y]));
			ntex.update(normallevel[x][y], x >> normallevel[x][y], y >> normallevel[x][y], false);
		}

		ntex.generateTex();

		glBindTexture(GL_TEXTURE_2D, NormallevelTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, normallevel);
		glBindTexture(GL_TEXTURE_2D, 0);
}

void TessBumpTerrain::updateBtex()
{
	static float data[FEEDBACK_WIDTH * FEEDBACK_HEIGHT * 4];
	static set<pair<int, int> > bSet;
	static set<pair<int, int> > AOSet;
	AOSet.clear();
	bSet.clear();
	feedback.begin();
	glReadPixels(0, 0, FEEDBACK_WIDTH, FEEDBACK_HEIGHT, GL_RGBA, GL_FLOAT, (void*)&data);
	feedback.end();
	btex.clear();
	ntex.clear();
	AOtex.clear();
	ntex.checkThreadState();
	btex.checkThreadState();
	AOtex.checkThreadState();

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

				l = AOlevel[x][y];
				tx = x >> l;
				ty = y >> l;
				if (AOSet.find(make_pair(l, ty * (CHUNKNUMBER >> r) + tx)) == AOSet.end()) {
					AOtex.update(l, tx, ty, false);
					AOSet.insert(make_pair(l, ty * (CHUNKNUMBER >> r) + tx));
				}
			}
	}
	btex.generateTex();
	ntex.generateTex(); 
	AOtex.generateTex();

#pragma omp parallel for schedule(dynamic, 1)
	{
		for (int i = 0; i < FEEDBACK_WIDTH; i++)
		{
			int id = horizondata[i] * (m_Block.size() + m_Child.size());
			if (id <= 0 || id >= m_Block.size() + m_Child.size()) continue;
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
		if (m_Block[i].BRDF == 1)
		{
			int lx = m_Block[i].lx;
			int rx = m_Block[i].rx;
			int ly = m_Block[i].ly;
			int ry = m_Block[i].ry;
			float maxaovalue = 0;
			for (int x = lx; x <= rx; x++)
				for (int y = ly; y <= ry; y++)
				{
					if (maxaovalue < maxAO[x][y]) maxaovalue = maxAO[x][y];
				}
			if (maxaovalue <= 0.1 / m_directionalLight.AmbientIntensity) m_Block[i].AOflag = 1;
		}
	}

	for (int i = 0; i < m_Child.size(); i++)
	{
		m_Child[i].ChangeBRDF();
		if (m_Child[i].BRDF == 1)
		{
			int lx = m_Child[i].lx;
			int rx = m_Child[i].rx;
			int ly = m_Child[i].ly;
			int ry = m_Child[i].ry;
			float maxaovalue = 0;
			for (int x = lx; x <= rx; x++)
				for (int y = ly; y <= ry; y++)
				{
					if (maxaovalue < maxAO[x][y]) maxaovalue = maxAO[x][y];
				}
			if (maxaovalue <= 0.1 / m_directionalLight.AmbientIntensity) m_Child[i].AOflag = 1;
		}
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

	if (sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y + deltaPos.z * deltaPos.z) > 0) transferdata = true;
	else
	{
		if (sqrt(deltaDir.x * deltaDir.x + deltaDir.y * deltaDir.y + deltaDir.z * deltaDir.z) > 0) transferdata = true;
		else transferdata = false;
	}

	if (transferdata)
	{
		lastPos = currentPos;
		lastDir = currentDir;
	}
}

void TessBumpTerrain::NeedTransferData()
{
	glm::vec3 deltaPos = currentPos - needlastPos;
	glm::vec3 deltaDir = currentDir - needlastDir;

	if (sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y + deltaPos.z * deltaPos.z) > 200) needtranfersdata = true;
	else
	{
		if (sqrt(deltaDir.x * deltaDir.x + deltaDir.y * deltaDir.y + deltaDir.z * deltaDir.z) > 0.2) needtranfersdata = true;
		else needtranfersdata = false;
	}

	if (needtranfersdata)
	{
		needlastPos = currentPos;
		needlastDir = currentDir;
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
					glm::vec3 vertices = m_Block[i].vertices[j] + glm::vec3(0.0, m_Block[i].AOflag, 0.0);
					m_bumpvertices.push_back(vertices);
				}

				glm::vec3 tvertices = m_Block[i].vertices[0] + glm::vec3(0.0, m_Block[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);
				tvertices = m_Block[i].vertices[2] + glm::vec3(0.0, m_Block[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);
				tvertices = m_Block[i].vertices[3] + glm::vec3(0.0, m_Block[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);

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
					glm::vec3 vertices = m_Child[i].vertices[j] + glm::vec3(0.0, m_Child[i].AOflag, 0.0);
					m_bumpvertices.push_back(vertices);
				}
				glm::vec3 tvertices = m_Child[i].vertices[0] + glm::vec3(0.0, m_Child[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);
				tvertices = m_Child[i].vertices[2] + glm::vec3(0.0, m_Child[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);
				tvertices = m_Child[i].vertices[3] + glm::vec3(0.0, m_Child[i].AOflag, 0.0);
				m_bumpvertices.push_back(tvertices);


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
	glEnable(GL_MULTISAMPLE);

	

	NeedTransferData();

	if(!needtranfersdata)
	ViewFrustum();

	if (needtranfersdata || transferdata)
	{
		glClearDepth(1.0f);
		glClearColor(0.46f, 0.46f, 1.0f, 1.0f);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);

		//vt render
		feedback.clear();
		feedback.begin();
		glViewport(0, 0, FEEDBACK_WIDTH, FEEDBACK_HEIGHT);

		m_vtshader.Enable();

		updateHtex(currentPos.x / CHUNKNUMBER / CHUNKSIZE, currentPos.y, currentPos.z / CHUNKNUMBER / CHUNKSIZE);
		m_vtshader.SetCurrentPosition(glm::vec4(currentPos.x / CHUNKNUMBER / CHUNKSIZE, currentPos.z / CHUNKNUMBER / CHUNKSIZE, currentPos.y, 0.0));
		m_vtshader.SetWVP(m_WVP);
		m_vtshader.SetBlockSize(m_Block.size() + m_Child.size());

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, htex.getTex());

		PTex* ptex = htex.getPtex();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ptex->getTex());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, hLevelTex);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, hLevelTex1);

		if(needtranfersdata)
		VTPassGenerateVertices();

		glBindVertexArray(vao[0]);

		glDrawArrays(GL_PATCHES, 0, m_vtvertices.size());

		glBindVertexArray(0);
		glUseProgram(0);
		feedback.end();

		updateBtex();
	}

	glClearDepth(1.0f);
	glClearColor(0.46f, 0.46f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

//	if (needtranfersdata)
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

	UpdateAOLevel();

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

	//if (!transferdata)
	PTex* ptex = htex.getPtex();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptex->getTex());

//	ptex = htex.getSatPtex();
//	glActiveTexture(GL_TEXTURE8);
//	glBindTexture(GL_TEXTURE_2D, ptex->getTex());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);

	PTex* bptex = btex.getPtex();
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, btex.getTex());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bptex->getTex());

	PTex* nptex = ntex.getPtex();
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, ntex.getTex());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, nptex->getTex());

	PTex* AOptex= AOtex.getPtex();
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, AOtex.getTex());

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, AOptex->getTex());

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, AOlevelTex);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, AOmaxdifftex);

	//glActiveTexture(GL_TEXTURE13);
	//glBindTexture(GL_TEXTURE_2D, NormallevelTex);

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

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptex->getTex());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, btex.getTex());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bptex->getTex());

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, ntex.getTex());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, nptex->getTex());

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, AOtex.getTex());

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, AOptex->getTex());

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, AOlevelTex);

//	glActiveTexture(GL_TEXTURE12);
	//glBindTexture(GL_TEXTURE_2D, NormallevelTex);

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
	glBindTexture(GL_TEXTURE_2D, ptex->getTex());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, btex.getTex());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bptex->getTex());

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, ntex.getTex());

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, nptex->getTex());

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, AOtex.getTex());

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, AOptex->getTex());

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, AOlevelTex);

//	glActiveTexture(GL_TEXTURE12);
//	glBindTexture(GL_TEXTURE_2D, NormallevelTex);

	glBindVertexArray(vao[3]);

	glDrawArrays(GL_PATCHES, 0, m_bordervertices.size());

	glBindVertexArray(0);
	glUseProgram(0);
}