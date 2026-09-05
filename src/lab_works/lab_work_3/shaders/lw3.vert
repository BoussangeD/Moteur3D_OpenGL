#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
layout( location = 1 ) out vec4 sortieColor;		// variable de sortie pour la couleur

uniform mat4 uMVPMatrix;

void main() {
	gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);

	sortieColor = aVertexColor;		// passe la couleur au fragment shader
}