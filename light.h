#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <cmath>
#include <stdio.h>

#define SNPRINTF _snprintf_s
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

const int BASICTEXNUM = 6;
const std::string texnames[] = { "xsc_db001_1748.tga", "fb_jjc_cd002_2161.tga",
"fb_jjc_cd001_2161.tga", "xsc_db002_1748.tga",
"xsc_dbcd007_1201.tga", "xsc_dbsk006_1201.tga",
"xsc_db001_1748_n.tga", "xsc_dbcd007_1201_n.tga",
"xsc_dbsk006_1201_n.tga" };

#define MAX_SG_LIGHTS 2

struct BaseLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = glm::vec3(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};

struct DirectionalLight : public BaseLight
{
	glm::vec3 Direction;

	DirectionalLight()
	{
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
	}
};

struct PointLight : public BaseLight
{
	glm::vec3 Position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};

struct SpotLight : public PointLight
{
	glm::vec3 Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

struct SGLight
{
	glm::vec3 p; // lobe axis
	float lambda; // sharpness
	glm::vec3 mu; // amplitude
};

#endif
