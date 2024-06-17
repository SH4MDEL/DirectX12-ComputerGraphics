#define MaxLight 30
#define Directional 0
#define Point 1
#define Spot 2
#define Last 3

struct MaterialData
{
	float3 fresnelR0;
	float roughness;
	float3 ambient;
};

cbuffer Material : register(b0, space1)
{
	MaterialData g_material[4];
};

struct LightData
{
	float3 strength;
	float fallOffStart;
	float3 direction;
	float fallOffEnd;
	float3 position;
	float spotPower;
	uint type;
	float3 padding;
};

cbuffer Light : register(b0, space2)
{
	LightData g_lights[MaxLight];
}

float CalcAttenuation(float d, float fallOffStart, float fallOffEnd)
{
	return saturate((fallOffEnd - d) / (fallOffEnd - fallOffStart));
}

// r0 = ((n-1)/(n+1))^2, n은 굴절 지수이다. 
float3 SchlickFresnel(float3 r0, float3 normal, float3 lightVector)
{
	float cosIncidentAngle = saturate(dot(normal, lightVector));
	
	float f0 = 1.f - cosIncidentAngle;
	float3 reflectPercent = r0 + (1.f - r0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVector, float3 normal, 
	float3 toEye, float3 diffuse, MaterialData material)
{
	float shininess = 1.f - material.roughness;
	const float m = shininess * 256.f;
	float3 halfVector = normalize(toEye + lightVector);

	float roughnessFactor = (m + 8.f) * pow(max(dot(halfVector, normal), 0.f), m) / 8.f;
	float3 fresnelFactor = SchlickFresnel(material.fresnelR0, halfVector, lightVector);

	float3 specular = fresnelFactor * roughnessFactor;
	specular = specular / (specular + 1.f);

	return (diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(LightData light, float3 normal, 
	float3 toEye, float3 diffuse, MaterialData material)
{
	float3 lightDirection = -light.direction;

	float ndotl = max(dot(lightDirection, normal), 0.0f);
	float3 lightStrength = light.strength * ndotl;

	return BlinnPhong(lightStrength, lightDirection, normal, toEye, diffuse, material);
}

float3 ComputePointLight(LightData light, float3 objectPosition, float3 normal, 
	float3 toEye, float3 diffuse, MaterialData material)
{
	float3 lightVector = light.position - objectPosition;
	float d = length(lightVector);

	if (d > light.fallOffEnd) return float3(0.f, 0.f, 0.f);

	lightVector /= d;
	float ndotl = max(dot(lightVector, normal), 0.f);
	float3 lightStrength = light.strength * ndotl;

	float attenuation = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
	lightStrength *= attenuation;

	return BlinnPhong(lightStrength, lightVector, normal, toEye, diffuse, material);
}

float3 ComputeSpotLight(LightData light, float3 objectPosition, float3 normal, 
	float3 toEye, float3 diffuse, MaterialData material)
{
	float3 lightVector = light.position - objectPosition;
	float d = length(lightVector);

	if (d > light.fallOffEnd) return 0.0f;

	lightVector /= d;
	float ndotl = max(dot(lightVector, normal), 0.0f);
	float3 lightStrength = light.strength * ndotl;

	float attenuation = CalcAttenuation(d, light.fallOffStart, light.fallOffEnd);
	lightStrength *= attenuation;

	float spotFactor = pow(max(dot(-lightVector, light.direction), 0.f), light.spotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVector, normal, toEye, diffuse, material);
}

float4 Lighting(float3 objectPosition, float3 normal, 
	float3 toEye, float4 diffuse, MaterialData material)
{
	float3 output = float3(0.f, 0.f, 0.f);
	
	for (int i = 0; i < MaxLight; ++i)
	{
		if (g_lights[i].type == Last) break;
		
		switch (g_lights[i].type) 
		{
		case Directional:
			output += ComputeDirectionalLight(g_lights[i], normal, toEye, diffuse.rgb, material);
			break;
		case Point:
			output += ComputePointLight(g_lights[i], objectPosition, normal, toEye, diffuse.rgb, material);
			break;
		case Spot:
			output += ComputeSpotLight(g_lights[i], objectPosition, normal, toEye, diffuse.rgb, material);
			break;
		}
	}
	
	float3 ambient = material.ambient * diffuse.rgb;
	output += ambient;
	
	return float4(output, diffuse.a);
}