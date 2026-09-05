#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;     // Projection * View * Model
uniform mat4 uMVMatrix;	     // View * Model
uniform mat4 uNormalMatrix;
uniform vec3 uLightPosition; // Nouvelle position de la source lumineuse

out vec3 normal;
out vec3 fragPosition;
out vec3 lightPositionView;
out vec2 texCoords;

out vec3 lightPositionTangent; 
out vec3 fragPositionTangent;

void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.0 );

    fragPosition = vec3(uMVMatrix * vec4(aVertexPosition, 1.0));
    normal = normalize(mat3(uNormalMatrix) * aVertexNormal);

    lightPositionView = vec3(uLightPosition); // passage de la lumière dans le view space

    texCoords = aVertexTexCoords;

    vec3 T = normalize(mat3(uNormalMatrix) * aVertexTangent);        // tangeant en view space
    vec3 B = normalize(mat3(uNormalMatrix) * aVertexBitangent);      // bitangent en view space
    vec3 N = normal;                                      // normal en view space

    mat3 inv_TBN = transpose(mat3(T, B, N));
    fragPositionTangent = inv_TBN * vec3(fragPosition);    // calcul de la position du sommet en tangent space
    lightPositionTangent = inv_TBN * vec3(uLightPosition); // calcul de la position de la lumière en tangent space
}
