#version 450

layout( location = 0 ) in vec2 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
layout( location = 1 ) out vec4 sortieColor;	// variable de sortie pour la couleur

uniform float uTranslationX;

void main() {
	gl_Position = vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y, 1.0, 1.0);
	sortieColor = aVertexColor;	// passe la couleur au fragment shader
}