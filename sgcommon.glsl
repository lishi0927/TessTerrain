#version 410

const int MAX_SG_LIGHTS = 2;  

struct SG
{
	vec3 Axis; // lobe axis
	float Sharpness; // sharpness
	vec3 Amplitude; // amplitude
};

SG SGProduct(const SG& x, const SG& y)
{
   vec3 um = (x.Sharpness * x.Axis + y.Sharpness * y.Axis) /
                (x.Sharpness + y.Sharpness);
   float umLength = length(um);
   float lm = x.Sharpness + y.Sharpness;
 
   SphericalGaussian res;
   res.Axis = um * (1.0f / umLength);
   res.Sharpness = lm * umLength;
   res.Amplitude = x.Amplitude * y.Amplitude * exp(lm * (umLength - 1.0f));
 
   return res;
}

