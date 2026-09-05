#version 450

layout( location = 0 ) out vec4 fragColor;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uAmbientMap;
layout( binding = 3 ) uniform sampler2D uSpecularMap;
layout( binding = 4 ) uniform sampler2D uShininessMap;
layout( binding = 5 ) uniform sampler2D uNormalMap;

uniform bool uHasDiffuseMap;
uniform bool uHasAmbientMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;

in vec3 normal;
in vec3 fragPosition;
in vec3 lightPositionView;
in vec2 texCoords;

in vec3 lightPositionTangent;
in vec3 fragPositionTangent;

void main()
{
    vec3 lightDir = normalize(lightPositionView - fragPosition);   // lumière incidente Li
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - fragPosition);  // lumière qui arrive vers le point de vue (caméra) Lo

    vec3 lightDirTangent = normalize(lightPositionTangent - fragPositionTangent);  // lumière en tangeant space
    vec3 viewDirTangent = normalize(vec3(0.0, 0.0, 0.0) - fragPositionTangent);    // lumière qui arrive vers le point de vue (caméra) Lo en Tangent space

    vec3 lightDirFinal = lightDir;
    vec3 viewDirFinal = viewDir;
    vec3 normalFinal = normal;

    // ---------------------------- Normal map --------------------------- //
    if(uHasNormalMap) { // calcul de la normal map uniquement si elle est présente
        vec3 normalMap = normalize((texture(uNormalMap, texCoords).xyz) * 2.0 - 1.0);     // remappage sur [-1,1]

        lightDirFinal = lightDirTangent;
        viewDirFinal = viewDirTangent;
        normalFinal = normalMap;
    }

    if (dot(normalFinal, viewDirFinal) < 0.0) {
        normalFinal = -normalFinal; // inverser la normale si elle ne pointe pas vers la caméra
    }
    // ---------------------------- Normal map --------------------------- //


    // ------------------------- Couleur diffuse ------------------------- //
    float diffuse = max(0.0, dot(normalFinal, lightDirFinal));    // éclairage diffus (Phong)

    vec4 diffuseColor;
    if(uHasDiffuseMap) {
        diffuseColor = texture(uDiffuseMap, texCoords);
    } else {
        diffuseColor = vec4(uDiffuseColor, 1.0);
    }

    // test de l'opacité
    if (diffuseColor.a < 0.5) {
        discard;  // ignorer le fragment
    }

    vec3 finalColor = diffuse * diffuseColor.rgb;
    // ------------------------- Couleur diffuse ------------------------- //


    // ------------------------- Couleur ambiante ------------------------ //
    vec3 ambientColor;
    if(uHasAmbientMap) {
        ambientColor = texture(uAmbientMap, texCoords).xyz;
    } else {
        ambientColor = uAmbientColor;
    }
    // ------------------------- Couleur ambiante ------------------------ //


    // ---------------------------- Shininess ---------------------------- //
    float shininess;
    if(uHasShininessMap) {
        shininess = texture(uShininessMap, texCoords).x;
    } else {
        shininess = uShininess;
    }
    // ---------------------------- Shininess ---------------------------- //


    // ------------------------ Couleur spéculaire ----------------------- //
    vec3 H = normalize(viewDirFinal + lightDirFinal);                          // Demi-vecteur (H)
    float specular = pow(max(0.0, dot(normalFinal, H)), shininess);            // éclairage spéculaire avec H (Blinn-Phong)

    vec3 reflectDir = reflect(-lightDirFinal, normalFinal);                    // Direction de réflexion
    float specular2 = pow(max(0.0, dot(viewDirFinal, reflectDir)), shininess); // éclairage spéculaire avec reflect (Phong)
    
    vec3 specularColor;
    if(uHasSpecularMap) {                                                      // calcul final de la couleur spéculaire
        specularColor = specular * texture(uSpecularMap, texCoords).xxx;       
    } else {
        specularColor = specular * uSpecularColor;
    }
    // ------------------------ Couleur spéculaire ----------------------- //

    finalColor += specularColor + ambientColor;
    fragColor = vec4(finalColor, 1.0);
}
