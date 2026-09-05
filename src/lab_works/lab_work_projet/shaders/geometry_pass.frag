#version 450

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragAmbient;
layout(location = 3) out vec3 fragDiffuse;
layout(location = 4) out vec4 fragSpecular;

layout(binding = 1) uniform sampler2D uDiffuseMap;
layout(binding = 2) uniform sampler2D uAmbientMap;
layout(binding = 3) uniform sampler2D uSpecularMap;
layout(binding = 4) uniform sampler2D uShininessMap;
layout(binding = 5) uniform sampler2D uNormalMap;

uniform bool uHasDiffuseMap;
uniform bool uHasAmbientMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPositionView;

void main()
{
    // ------------------------- Couleur ambiante ------------------------ //
    vec3 ambientColor;
    if (uHasAmbientMap) {
        fragAmbient = texture(uAmbientMap, texCoords).xyz;
    }
    else {
        fragAmbient = uAmbientColor;
    }
    // ------------------------- Couleur ambiante ------------------------ //


    // ------------------------- Couleur diffuse ------------------------- //
    vec4 diffuseColor;
    if (uHasDiffuseMap) {
        diffuseColor = texture(uDiffuseMap, texCoords);
    } else {
        diffuseColor = vec4(uDiffuseColor, 1.0);
    }

    // test de l'opacité
    if (diffuseColor.a < 0.5) {
        discard;  // ignorer le fragment
    }

    fragDiffuse = diffuseColor.rgb;
    // ------------------------- Couleur diffuse ------------------------- //


    // ---------------------------- Shininess ---------------------------- //
    float shininess;
    if (uHasShininessMap) {
        shininess = texture(uShininessMap, texCoords).x;
    } else {
        shininess = uShininess;
    }
    // ---------------------------- Shininess ---------------------------- //


    // ------------------------ Couleur spéculaire ----------------------- //
    vec3 specularColor;
    if (uHasSpecularMap) {
        specularColor = texture(uSpecularMap, texCoords).xxx;       
    } else {
        specularColor = uSpecularColor;
    }
    // ------------------------ Couleur spéculaire ----------------------- //

    fragSpecular = vec4(specularColor, shininess);
    fragNormal = normal;
    fragPosition = fragPositionView;
}
