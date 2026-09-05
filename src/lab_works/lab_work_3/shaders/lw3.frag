#version 450

layout( location = 0 ) out vec4 fragmentColor;
layout( location = 1 ) in vec4 aVertexColor;	// variable d'entrée pour la couleur

uniform float uLuminosite;

void main() {
	fragmentColor = vec4(aVertexColor * uLuminosite);
}
