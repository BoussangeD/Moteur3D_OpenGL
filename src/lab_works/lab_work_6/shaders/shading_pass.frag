#version 450

layout(location = 0) out vec4 fragColor;

layout(binding = 6) uniform sampler2D fragPosition;
layout(binding = 7) uniform sampler2D fragNormal;
layout(binding = 8) uniform sampler2D fragAmbient;
layout(binding = 9) uniform sampler2D fragDiffuse;
layout(binding = 10) uniform sampler2D fragSpecular;

uniform vec3 uLightPosition; // Nouvelle position de la source lumineuse
uniform int uSelectTexture;

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy); // coordonnées du pixel courant

    vec3 position = texelFetch(fragPosition, coord, 0).xyz;
    vec3 normal = normalize(texelFetch(fragNormal, coord, 0).xyz);
    vec3 ambient = texelFetch(fragAmbient, coord, 0).xyz;
    vec3 diffuse = texelFetch(fragDiffuse, coord, 0).xyz;
    vec3 specular = texelFetch(fragSpecular, coord, 0).xyz; // xyz couleur
    float shininess = texelFetch(fragSpecular, coord, 0).w; // w shininess

    vec3 lightPositionView = vec3(uLightPosition); // passage de la lumière dans le view space

    // Calcul de l'éclairage
    vec3 lightDir = normalize(lightPositionView - position);
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - position);

    vec3 normalFinal = normal;
    if (dot(normalFinal, viewDir) < 0.0) {
        normalFinal = -normalFinal; // inverser la normale si elle ne pointe pas vers la caméra
    }

    float diff = max(0.0, dot(normalFinal, lightDir));
    vec3 diffuseColor = diff * diffuse;

    vec3 H = normalize(viewDir + lightDir);  
    float spec = pow(max(0.0, dot(normalFinal, H)), shininess);
    vec3 specularColor = spec * specular.xyz;

    vec3 finalColor = ambient + diffuseColor + specularColor;

    if(uSelectTexture == 0)
        fragColor = vec4(finalColor, 1.0);
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