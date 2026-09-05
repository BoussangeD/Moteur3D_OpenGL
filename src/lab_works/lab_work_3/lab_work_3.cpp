#include "lab_work_3.hpp"
#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "imgui.h"
#include "utils/random.hpp" // pour les couleurs random
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() // destructeur
	{
		glDeleteProgram( idProgram );

		glDeleteBuffers( 1, &_cube.idVbo );
		glDisableVertexArrayAttrib( _cube.idVao, 0 );
		glDeleteVertexArrays( 1, &_cube.idVao );
		glDeleteBuffers( 1, &_cube.idEbo );
		glDeleteBuffers( 1, &_cube.idVboCouleur );

		glDeleteBuffers( 1, &_cube2.idVbo );
		glDisableVertexArrayAttrib( _cube2.idVao, 0 );
		glDeleteVertexArrays( 1, &_cube2.idVao );
		glDeleteBuffers( 1, &_cube2.idEbo );
		glDeleteBuffers( 1, &_cube2.idVboCouleur );

		glDisable( GL_DEPTH_TEST );
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initProgram();

		_initCamera();
		_createCube();
		_createCube2();
		_initBuffers();
		_initBuffers2();

		glEnable( GL_DEPTH_TEST ); // test de profondeur

		// ModelViewProjection Matrix
		idMVPMatrix = glGetUniformLocation( idProgram, "uMVPMatrix" ); // récupère l'adresse de la variable uniform uMVPMatrix

		glUseProgram( idProgram ); // on specifie le programme à utiliser

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::_createCube()
	{
		// FRONT FACE
		_cube.positionSommets.emplace_back( Vec3f( -0.5f, -0.5f, 0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( 0.5f, -0.5f, 0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( 0.5f, 0.5f, 0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( -0.5f, 0.5f, 0.5f ) );

		// BACK FACE
		_cube.positionSommets.emplace_back( Vec3f( -0.5f, -0.5f, -0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( 0.5f, -0.5f, -0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( 0.5f, 0.5f, -0.5f ) );
		_cube.positionSommets.emplace_back( Vec3f( -0.5f, 0.5f, -0.5f ) );

		_cube.indicesSommets = {
			0, 1, 2, 2, 3, 0, // FRONT
			4, 5, 6, 6, 7, 4, // BACK
			1, 5, 6, 6, 2, 1, // RIGHT
			0, 4, 7, 7, 3, 0, // LEFT
			3, 2, 6, 6, 7, 3, // TOP
			0, 1, 5, 5, 4, 0  // BOTTOM
		};

		// partie interpolation de couleurs
		for ( int i = 0; i < _cube.positionSommets.size(); i++ )
		{
			_cube.couleursSommets.emplace_back( getRandomVec3f() );
		}

		_cube.transfoMatrices = glm::scale( _cube.transfoMatrices, Vec3f( 0.8f, 0.8f, 0.8f ) ); // applique l'échelle
	}

	void LabWork3::_createCube2()
	{
		_cube2.positionSommets = _cube.positionSommets; // copie des positions des sommets de _cube1

		// ajoutez les valeurs de translation voulues en x, y ou z
		Vec3f translationCube = Vec3f( 4.0f, 0.0f, 0.0f );
		for ( Vec3f & vertex : _cube2.positionSommets )
		{
			vertex += translationCube;
		} 

		_cube2.indicesSommets = _cube.indicesSommets; // copie des indices des sommets de _cube1

		// partie interpolation de couleurs
		for ( int i = 0; i < _cube2.positionSommets.size(); i++ )
		{
			_cube2.couleursSommets.emplace_back( getRandomVec3f() );
		}

		_cube2.transfoMatrices = glm::scale( _cube2.transfoMatrices, Vec3f( 0.8f, 0.8f, 0.8f ) ); // applique l'échelle
	}

	void LabWork3::_initBuffers()
	{
		// partie Vbo
		glCreateBuffers( 1, &_cube.idVbo );
		glNamedBufferData(
			_cube.idVbo, _cube.positionSommets.size() * sizeof( Vec3f ), _cube.positionSommets.data(), GL_STATIC_DRAW );

		// partie Vao
		glCreateVertexArrays( 1, &_cube.idVao );
		glEnableVertexArrayAttrib( _cube.idVao, 0 );
		glVertexArrayAttribFormat( _cube.idVao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _cube.idVao, 0, _cube.idVbo, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube.idVao, 0, 0 );

		// partie ebo
		glCreateBuffers( 1, &_cube.idEbo );
		glNamedBufferData(
			_cube.idEbo, _cube.indicesSommets.size() * sizeof( GLuint ), _cube.indicesSommets.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _cube.idVao, _cube.idEbo );

		glCreateBuffers( 1, &_cube.idVboCouleur );
		glNamedBufferData( _cube.idVboCouleur,
						   _cube.couleursSommets.size() * sizeof( Vec3f ),
						   _cube.couleursSommets.data(),
						   GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( _cube.idVao, 1 );
		glVertexArrayAttribFormat(
			_cube.idVao, 1, 3, GL_FLOAT, GL_FALSE, 0 ); // format à 3 pour traiter les couleurs RGB
		glVertexArrayVertexBuffer( _cube.idVao, 1, _cube.idVboCouleur, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube.idVao, 1, 1 );
	}

	void LabWork3::_initBuffers2() 
	{
		// partie Vbo
		glCreateBuffers( 1, &_cube2.idVbo );
		glNamedBufferData(
			_cube2.idVbo, _cube2.positionSommets.size() * sizeof( Vec3f ), _cube2.positionSommets.data(), GL_STATIC_DRAW );

		// partie vao
		glCreateVertexArrays( 1, &_cube2.idVao );
		glEnableVertexArrayAttrib( _cube2.idVao, 0 );
		glVertexArrayAttribFormat( _cube2.idVao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _cube2.idVao, 0, _cube2.idVbo, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube2.idVao, 0, 0 );

		// partie ebo
		glCreateBuffers( 1, &_cube2.idEbo );
		glNamedBufferData(
			_cube2.idEbo, _cube2.indicesSommets.size() * sizeof( GLuint ), _cube2.indicesSommets.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _cube2.idVao, _cube2.idEbo );

		glCreateBuffers( 1, &_cube2.idVboCouleur );
		glNamedBufferData( _cube2.idVboCouleur,
						   _cube2.couleursSommets.size() * sizeof( Vec3f ),
						   _cube2.couleursSommets.data(),
						   GL_STATIC_DRAW );

		glEnableVertexArrayAttrib( _cube2.idVao, 1 );
		glVertexArrayAttribFormat( _cube2.idVao, 1, 3, GL_FLOAT, GL_FALSE, 0 ); // format à 3 pour traiter les couleurs RGB
		glVertexArrayVertexBuffer( _cube2.idVao, 1, _cube2.idVboCouleur, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _cube2.idVao, 1, 1 );
	}

	void LabWork3::animate( const float p_deltaTime )
	{
		// Rotation du premier cube autour de son propre axe.
		_cube.transfoMatrices = rotate( _cube.transfoMatrices, glm::radians( p_deltaTime * 90 ), _rotationCube );

		_cube2.transfoMatrices = rotate( _cube2.transfoMatrices, glm::radians( p_deltaTime * 90 ), Vec3f( 0.0f, 1.0f, 1.0f ));
	}

	void LabWork3::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // efface la couleur de fond et le buffer de profondeur

		_cube.MVPMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube.transfoMatrices;
		glBindVertexArray( _cube.idVao );					  // lien entre vao et le programme
		glDrawElements( GL_TRIANGLES, (int)_cube.indicesSommets.size(), GL_UNSIGNED_INT, 0 );
		glProgramUniformMatrix4fv( idProgram, idMVPMatrix, 1, GL_FALSE, glm::value_ptr( _cube.MVPMatrix ) );
		glBindVertexArray( 0 );

		_cube2.MVPMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube2.transfoMatrices;
		glBindVertexArray( _cube2.idVao );
		glDrawElements( GL_TRIANGLES, (int)_cube2.indicesSommets.size(), GL_UNSIGNED_INT, 0 );
		glProgramUniformMatrix4fv( idProgram, idMVPMatrix, 1, GL_FALSE, glm::value_ptr( _cube2.MVPMatrix ) );
		glBindVertexArray( 0 ); // delie le vao et le programme
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );

		ImGui::Text( "Luminosite" );
		GLint idLumi = glGetUniformLocation( idProgram, "uLuminosite" );
		glProgramUniform1f( idProgram, idLumi, _luminosite );
		if ( ImGui::SliderFloat( "1", &_luminosite, 0.0f, 1.0f ) )
		{
			glProgramUniform1f( idProgram, idLumi, _luminosite );
		}

		ImGui::Text( "Background Color" );
		if ( ImGui::ColorEdit3( "2", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		ImGui::Text( "FOVY" );
		float fovy = _camera.getFovy();
		if ( ImGui::SliderFloat( "3", &fovy, 1.0f, 120.0f ) )
		{
			_camera.setFovy( fovy );
		}

		ImGui::Text( "Rotation Axis for cube" );
		if ( ImGui::SliderFloat( "X", &_rotationCube.x, -1.0f, 1.0f )
			 || ImGui::SliderFloat( "Y", &_rotationCube.y, -1.0f, 1.0f )
			 || ImGui::SliderFloat( "Z", &_rotationCube.z, -1.0f, 1.0f ) )
		{
			_rotationCube = Vec3f( _rotationCube.x, _rotationCube.y, _rotationCube.z );
		}

		ImGui::End();
	}

	void LabWork3::_updateViewMatrix()
	{
		const Mat4f viewMatrix = _camera.getViewMatrix();
		glProgramUniformMatrix4fv( idProgram, idViewMatrix, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
	}

	void LabWork3::_updateProjectionMatrix()
	{
		const Mat4f projectionMatrix = _camera.getProjectionMatrix();
		glProgramUniformMatrix4fv( idProgram, idProjectionMatrix, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
	}

	void LabWork3::_initCamera()
	{
		_camera.setPosition( Vec3f( 0.0f, 1.0f, 3.0f ) );
		_camera.setScreenSize( getWindowWidth(), getWindowHeight() );
	}

	bool LabWork3::_initProgram() 
	{
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw3.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw3.frag" );

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
