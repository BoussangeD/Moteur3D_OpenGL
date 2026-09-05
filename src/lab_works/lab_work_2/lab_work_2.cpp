#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "utils/random.hpp"		// pour les couleurs random
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2() // destructeur
	{
		glDeleteProgram( idProgram );
		glDeleteBuffers( 1, &idVbo );
		glDisableVertexArrayAttrib( idVao, 0 );
		glDeleteVertexArrays( 1, &idVao );
		glDeleteBuffers( 1, &idEbo );
		glDeleteBuffers( 1, &idVboCouleur );
	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initProgram();
			
		_drawQuad();		// dessin du rectangle avec couleurs interpolées
		disque( Vec2f( 0.0f, 0.0f ), 40, 0.5f ); // disque de centre 0,0 contenant 5 triangles et avec un rayon = 0.5

		_initBuffers();	// initialisation des buffers

		// partie variable de contrôle
		translationX = glGetUniformLocation( idProgram, "uTranslationX" ); // récupère l'index de la variable uniform uTranslationX

		glUseProgram( idProgram );								// on specifie le programme à utiliser

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::_drawQuad() 
	{
		sommetsTriangle.emplace_back( Vec2f( -0.5f, 0.5f ) );
		sommetsTriangle.emplace_back( Vec2f( 0.5f, 0.5f ) );
		sommetsTriangle.emplace_back( Vec2f( 0.5f, -0.5f ) );
		sommetsTriangle.emplace_back( Vec2f( -0.5f, -0.5f ) );

		indicesSommets = { 0, 1, 2, 0, 2, 3 }; // Les indices forment deux triangles (0-1-2 et 0-2-3)
	}

	void LabWork2::disque( const Vec2f & C, int N, float R )
	{
		sommetsTriangle.clear();
		couleursSommets.clear();
		indicesSommets.clear();

		sommetsTriangle.emplace_back( C );  // sommet central qu'on ne positionne qu'une fois

		for ( int i = 0; i < N; i++ )
		{
			float angle = ( (float)i / N ) * 2.0f * glm::pi<float>(); // 2pi pour un tour de cercle

			Vec2f sommetsTriangleCourant = C + Vec2f( cos( angle ) * R, sin( angle ) * R ); // calcul des composantes x et y
			Vec3f randomColor = getRandomVec3f();

			sommetsTriangle.emplace_back( sommetsTriangleCourant );
			couleursSommets.emplace_back( randomColor, 1.0f );

			indicesSommets.emplace_back( 0 );		   // indice du sommet central toujours à 0 
			indicesSommets.emplace_back( i + 1 );	   // premier sommet du triangle courant
			if ( i != N - 1 ) {
				indicesSommets.emplace_back( i + 2 );  // deuxième sommet du triangle courant
			}
			else {
				indicesSommets.emplace_back( 1 );      // à la fin de cercle il faut revenir à 1
			}
		}
	}

	void LabWork2::animate( const float p_deltaTime )
	{
		_time += p_deltaTime;					   // à chaque appel on incrémente le temps
		float translationTime = glm::sin( _time ); // pour trouver une valeur comprise entre -0,5 et 0,5
		glProgramUniform1f( idProgram, translationX, translationTime ); // maj de la valeur translationX
	}

	void LabWork2::render()
	{
		glClear( GL_COLOR_BUFFER_BIT ); // efface la couleur de fond
		glBindVertexArray( idVao );		// lien entre vao et le programme
		glDrawElements( GL_TRIANGLES, (int)indicesSommets.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 ); // delie le vao et le programme
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );

		ImGui::Text( "Luminosite" );
		GLint idLumi = glGetUniformLocation( idProgram, "uLuminosite" );
		glProgramUniform1f( idProgram, idLumi, _luminosite );
		if ( ImGui::SliderFloat( "", &_luminosite, 0.0f, 1.0f ) )
		{
			glProgramUniform1f( idProgram, idLumi, _luminosite );
		}

		ImGui::Text( "Background Color" );
		if ( ImGui::ColorEdit3( "", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		ImGui::End();
	}

	void LabWork2::_initBuffers() 
	{
		// partie Vbo
		glCreateBuffers( 1, &idVbo );
		glNamedBufferData( idVbo, sommetsTriangle.size() * sizeof( Vec2f ), sommetsTriangle.data(), GL_STATIC_DRAW );

		// partie Vao
		glCreateVertexArrays( 1, &idVao );
		glEnableVertexArrayAttrib( idVao, 0 );
		glVertexArrayAttribFormat( idVao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( idVao, 0, idVbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( idVao, 0, 0 );

		// partie ebo
		glCreateBuffers( 1, &idEbo );
		glNamedBufferData( idEbo, indicesSommets.size() * sizeof( GLuint ), indicesSommets.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( idVao, idEbo );

		// partie interpolation de couleurs
		couleursSommets.emplace_back( Vec4f( 1.0f, 0.0f, 0.0f, 1.0f ) ); // rouge
		couleursSommets.emplace_back( Vec4f( 0.0f, 1.0f, 0.0f, 1.0f ) ); // vert
		couleursSommets.emplace_back( Vec4f( 0.0f, 0.0f, 1.0f, 1.0f ) ); // bleu
		couleursSommets.emplace_back( Vec4f( 1.0f, 0.0f, 1.0f, 1.0f ) ); // magenta

		glCreateBuffers( 1, &idVboCouleur );
		glNamedBufferData(
			idVboCouleur, couleursSommets.size() * sizeof( Vec4f ), couleursSommets.data(), GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( idVao, 1 );
		glVertexArrayAttribFormat( idVao, 1, 4, GL_FLOAT, GL_FALSE, 0 ); // format à 4 pour traiter les couleurs RGBA
		glVertexArrayVertexBuffer( idVao, 1, idVboCouleur, 0, sizeof( Vec4f ) );
		glVertexArrayAttribBinding( idVao, 1, 1 );
	}

	bool LabWork2::_initProgram() 
	{
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw2.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw2.frag" );

		GLuint vertexShader	  = glCreateShader( GL_VERTEX_SHADER ); // lecture des shaders
		GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glShaderSource( fragmentShader, 1, &fSrc, NULL ); // association des shaders
		glCompileShader( vertexShader );				  // compilation des shaders
		glCompileShader( fragmentShader );

		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		// Check if compilation is ok.
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		idProgram = glCreateProgram(); // creation du programme

		glAttachShader( idProgram, vertexShader ); // lie les shaders avec le programme
		glAttachShader( idProgram, fragmentShader );
		glLinkProgram( idProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( idProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( idProgram, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( vertexShader ); // suppression des shaders
		glDeleteShader( fragmentShader );

		return true;
	}

} // namespace M3D_ISICG
