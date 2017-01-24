#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
uniform vec3 material_color;
uniform float material_reflectivity;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform float material_emission;

uniform int has_emission_texture;
layout(binding = 5) uniform sampler2D emissiveMap;

///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 50.0;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;

// Fresnel term
float F(vec3 n, vec3 wi, float f)
{
    return f + (1 - f) * pow(1 - dot(n, wi), 5);
}

// // Microfacet Distribution Function
float D(vec3 n, vec3 wh, float s)
{
    return (s + 2) / (2 * PI) * (pow(dot(n, wh), s));
}

// // Shadowing/masking function
float G(vec3 n, vec3 wi, vec3 wo, vec3 wh)
{
    return min(1.0, min(2 * dot(n, wh) * dot(n, wo) / dot(wo, wh), 2 * dot(n, wh) * dot(n, wi) / dot(wo, wh)));
}

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n)
{
    vec3 wi = normalize(viewSpaceLightPosition - viewSpacePosition);
    vec3 wh = normalize(wi + wo);

    if (dot(n, wi) <= 0)
        return vec3(0.0);

    float d = distance(viewSpaceLightPosition, viewSpacePosition);
    vec3 Li = point_light_intensity_multiplier * point_light_color / pow(d, 2);

    float Fwi   = F(n, wi, material_fresnel);
    float Dwh   = D(n, wh, material_shininess);
    float Gwiwo = G(n, wi, wo, wh);
    float brdf  = Fwi * Dwh * Gwiwo / (4 * dot(n, wo) * dot(n, wi));

    vec3 diffuse_term    = material_color * (1.0 / PI) * abs( dot(n, wi) ) * Li;
    vec3 dielectric_term = brdf * dot(n, wi) * Li + (1 - Fwi) * diffuse_term;
    vec3 metal_term      = brdf * material_color * dot(n, wi) * Li;
    vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

    return material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term;
}

vec2 sphericalLookup(vec3 dir)
{
	// Calculate the spherical coordinates of the direction
	float theta = acos(max(-1.0f, min(1.0f, dir.y)));
	float phi = atan(dir.z, dir.x);
	if (phi < 0.0f) phi = phi + 2.0f * PI;
	return vec2(phi / (2.0 * PI), theta / PI);
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n)
{
    vec3 n_ws = (viewInverse * vec4(n, 0.0)).xyz;

    vec3 irradiance = environment_multiplier * texture(irradianceMap, sphericalLookup(n_ws)).xyz;
    vec3 diffuse_term = material_color * (1.0 / PI) * irradiance;

    vec3 wi = reflect(-wo, n);
    vec3 wi_ws = (viewInverse * vec4(wi, 0.0)).xyz;
    float roughness = sqrt(sqrt(2 / (material_shininess + 2)));
    vec3 Li = environment_multiplier * textureLod(reflectionMap, sphericalLookup(wi_ws), roughness * 7.0).xyz;

    float Fwi = F(n_ws, wi_ws, material_fresnel);
    vec3 dielectric_term = Fwi * Li + (1 - Fwi) * diffuse_term;
    vec3 metal_term      = Fwi * material_color * Li;
    vec3 microfacet_term = material_metalness * metal_term + (1 - material_metalness) * dielectric_term;

    return material_reflectivity * microfacet_term + (1 - material_reflectivity) * diffuse_term;
}

void main()
{
	vec3 wo = -normalize(viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	// Direct illumination
	vec3 direct_illumination_term = calculateDirectIllumiunation(wo, n);

	// Indirect illumination
	vec3 indirect_illumination_term = calculateIndirectIllumination(wo, n);

	///////////////////////////////////////////////////////////////////////////
	// Add emissive term. If emissive texture exists, sample this term.
	///////////////////////////////////////////////////////////////////////////
	vec3 emission_term = material_emission * material_color;
	if (has_emission_texture == 1) {
		emission_term = texture(emissiveMap, texCoord).xyz;
	}
	fragmentColor.xyz = material_color;
		// direct_illumination_term +
		// indirect_illumination_term +
		// emission_term;
}
