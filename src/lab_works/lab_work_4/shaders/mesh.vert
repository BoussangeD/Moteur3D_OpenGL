#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix;     // Projection * View * Model
uniform mat4 uMVMatrix;	     // View * Model
uniform mat4 uNormalMatrix;
uniform vec3 uLightPosition; // nouvelle position de la source lumineuse

out vec3 normal;
out vec3 fragPosition;
out vec3 lightPositionView;

void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.0 );

    fragPosition = vec3(uMVMatrix * vec4(aVertexPosition, 1.0));
    normal = normalize(mat3(uNormalMatrix) * aVertexNormal);

    lightPositionView = vec3(uLightPosition); // passage de la lumière dans le View space
}
