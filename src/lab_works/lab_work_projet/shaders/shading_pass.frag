#version 450

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

layout(binding = 6) uniform sampler2D fragPosition;
layout(binding = 7) uniform sampler2D fragNormal;
layout(binding = 8) uniform sampler2D fragAmbient;
layout(binding = 9) uniform sampler2D fragDiffuse;
layout(binding = 10) uniform sampler2D fragSpecular;
layout(binding = 11) uniform sampler2D bloomTexture;
layout(binding = 12) uniform sampler2D shadowMap;

in vec4 FragPosLightSpace;

uniform int uSelectTexture;
uniform float uGammaCorrection;
uniform vec3 uLightPosition; // nouvelle position de la source lumineuse

float calculShadowFactor(vec3 normal, vec3 lightDir) {
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;   // coordonnées de l'espace lumière en coordonnées de texture
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;       // profondeur la plus proche pour les coordonnées actuelles

    float currentDepth = projCoords.z;      // profondeur actuelle du fragment dans l'espace de la lumière

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  // biais pour améliorer l'effet

    // si la profondeur actuelle moins le biais est supérieure à la profondeur la plus proche, c'est dans l'ombre
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return shadow;
}


vec3 toneMapping(vec3 hdrColor, float exposure) {
    return vec3(1.0) - exp(-hdrColor * exposure);
}

void main() {
    vec3 lightPositionView = vec3(uLightPosition); // passage de la lumière dans le view space

    ivec2 coord = ivec2(gl_FragCoord.xy); // coordonnées du pixel courant

    vec3 position = texelFetch(fragPosition, coord, 0).xyz;
    vec3 normal = normalize(texelFetch(fragNormal, coord, 0).xyz); // Normalize normal
    vec3 ambient = texelFetch(fragAmbient, coord, 0).xyz;
    vec3 diffuse = texelFetch(fragDiffuse, coord, 0).xyz;
    vec3 specular = texelFetch(fragSpecular, coord, 0).xyz; // xyz couleur
    float shininess = texelFetch(fragSpecular, coord, 0).w; // w shininess
    vec3 bloomColor = texelFetch(bloomTexture, coord, 0).xyz;

    // Calcul de l'éclairage
    vec3 lightDir = normalize(lightPositionView - position);
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - position);

    vec3 normalFinal = normal;
    if (dot(normalFinal, viewDir) < 0.0) {
        normalFinal = -normalFinal; // inverser la normale si elle ne pointe pas vers la caméra
    }

    float exposure = 1.0;

    float diff = max(0.0, dot(normalFinal, lightDir));
    vec3 diffuseColor = diff * diffuse;

    vec3 H = normalize(viewDir + lightDir);  
    float spec = pow(max(0.0, dot(normalFinal, H)), shininess);
    vec3 specularColor = spec * specular;

    float shadow = calculShadowFactor(normalFinal, lightDir);

    vec3 hdrColor = ambient + (1.0 - shadow) * (diffuseColor + specularColor) + bloomColor;
    hdrColor = toneMapping(hdrColor, exposure);

    float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = vec4(hdrColor, 1.0);
    } else {
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    if(uSelectTexture == 0)
        fragColor = vec4(pow(hdrColor, vec3(1.0f / uGammaCorrection)), 1.0); // gamma correction
    else if(uSelectTexture == 1)
        fragColor = vec4(position, 1.0);
    else if(uSelectTexture == 2)
        fragColor = vec4(normalFinal, 1.0);
    else if(uSelectTexture == 3)
        fragColor = vec4(ambient, 1.0);
    else if(uSelectTexture == 4)
        fragColor = vec4(diffuseColor, 1.0);
    else if(uSelectTexture == 5)
        fragColor = vec4(specularColor, 1.0);
}