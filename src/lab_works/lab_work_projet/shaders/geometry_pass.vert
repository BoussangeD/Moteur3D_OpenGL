#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;     // Projection * View * Model
uniform mat4 uMVMatrix;	     // View * Model
uniform mat4 uNormalMatrix;

out vec3 normal;
out vec3 fragPositionView;
out vec3 lightPositionView;
out vec2 texCoords;
out vec4 FragPosLightSpace;


void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.0 );

    fragPositionView = vec3(uMVMatrix * vec4(aVertexPosition, 1.0));
    normal = normalize(mat3(uNormalMatrix) * aVertexNormal);

    texCoords = aVertexTexCoords;

    FragPosLightSpace = uMVPMatrix * vec4( aVertexPosition, 1.0 );  // meme chose pour la lumiere vu qu'elle est positionné sur la caméra
}
