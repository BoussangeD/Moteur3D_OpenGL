#version 450

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube uSkybox;

void main()
{    
    FragColor = texture(uSkybox, TexCoords);
}
