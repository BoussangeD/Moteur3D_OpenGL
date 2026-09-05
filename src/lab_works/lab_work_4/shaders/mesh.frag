#version 450

layout( location = 0 ) out vec4 fragColor;

uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;    

in vec3 normal;
in vec3 fragPosition;
in vec3 lightPositionView;

void main()
{
    vec3 lightDir = normalize(lightPositionView - fragPosition);   // lumière incidente Li
    vec3 viewDir = normalize(vec3(0.0, 0.0, 0.0) - fragPosition);  // lumière qui arrive vers le point de vue (caméra) Lo
    vec3 normalFinal = normal;

    if (dot(normal, viewDir) < 0.0) {
        normalFinal = -normalFinal; // Inversez la normale si elle ne pointe pas vers la caméra
    }

    float diffuse = max(0.0, dot(normalFinal, lightDir));  // éclairage diffus (Phong)
    vec3 diffuseColor = diffuse * uDiffuseColor;

    vec3 H = normalize(viewDir + lightDir);                // Demi-vecteur (H)
    vec3 reflectDir = reflect(-lightDir, normalFinal);     // Direction de réflexion

    float specular = pow(max(0.0, dot(viewDir, reflectDir)), uShininess);   // éclairage spéculaire avec reflect (Phong)
    float specular2 = pow(max(0.0, dot(normalFinal, H)), uShininess);       // éclairage spéculaire avec H (Blinn-Phong)

    vec3 specularColor = specular2 * uSpecularColor;

    fragColor = vec4(specularColor + diffuseColor + uAmbientColor, 1.0);
}
