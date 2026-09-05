#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1() 
	{ 
		glDeleteProgram( idProgram );		// destruction du programme
		glDeleteBuffers( 1, &idVbo );
		glDisableVertexArrayAttrib( idVao, 0 );
		glDeleteVertexArrays( 1, &idVao );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initProgram();		// initialisation du programme

		_drawTriangle();	// dessin du triangle

		glUseProgram( idProgram );		// on specifie le programme à utiliser

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::_drawTriangle() 
	{
		sommetsTriangle.emplace_back( Vec2f( -0.5f, 0.5f ) ); // valeurs des sommets du triangle
		sommetsTriangle.emplace_back( Vec2f( 0.5f, 0.5f ) );  // emplace_back au lieu de push_back
		sommetsTriangle.emplace_back( Vec2f( 0.5f, -0.5f ) );

		// partie Vbo
		glCreateBuffers( 1, &idVbo );
		glNamedBufferData( idVbo, sommetsTriangle.size() * sizeof( Vec2f ), sommetsTriangle.data(), GL_STATIC_DRAW );

		// partie Vao
		glCreateVertexArrays( 1, &idVao );
		glEnableVertexArrayAttrib( idVao, 0 );
		glVertexArrayAttribFormat( idVao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( idVao, 0, idVbo, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( idVao, 0, 0 );
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() 
	{ 
		glClear( GL_COLOR_BUFFER_BIT );		// efface la couleur de fond
		glBindVertexArray( idVao );			// lien entre vao et le programme
		glDrawArrays( GL_TRIANGLES, 0, (int)sommetsTriangle.size() ); // dessine le triangle
		glBindVertexArray( 0 );				// delie le vao et le programme
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

	bool LabWork1::_initProgram() 
	{
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw1.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw1.frag" );

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
