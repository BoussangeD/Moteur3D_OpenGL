#version 450

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D uImage;
uniform bool uHorizontal;    // vrai pour horizontal, faux pour vertical
float uWeight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); 

void main() { 
    // calcul du décalage d'une texel
    vec2 tex_offset = 1.0 / textureSize(uImage, 0); 

    // poids central à la couleur de la texture courante
    vec3 result = texture(uImage, texCoords).rgb * uWeight[0];
    
    // si flou  horizontal, appliquer le flou sur l'axe x
    if (uHorizontal) {
        for (int i = 1; i < 5; ++i) {
            // ajouter les couleurs des texels horizontaux avec leur poids respectifs
            result += texture(uImage, texCoords + vec2(tex_offset.x * i, 0.0)).rgb * uWeight[i];
            result += texture(uImage, texCoords - vec2(tex_offset.x * i, 0.0)).rgb * uWeight[i];
        }
    } else {
        // sinon appliquer flou sur axe y
        for (int i = 1; i < 5; ++i) {
            result += texture(uImage, texCoords + vec2(0.0, tex_offset.y * i)).rgb * uWeight[i];
            result += texture(uImage, texCoords - vec2(0.0, tex_offset.y * i)).rgb * uWeight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}
