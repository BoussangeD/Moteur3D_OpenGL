#version 450

layout( location = 0 ) in vec3 aVertexPosition;

out vec3 TexCoords;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main()
{
    TexCoords = aVertexPosition;
    vec4 position = uProjectionMatrix * uViewMatrix * vec4(aVertexPosition, 1.0);
    gl_Position = position.xyww; // pour s'assurer que la skybox reste à une distance constante
}
