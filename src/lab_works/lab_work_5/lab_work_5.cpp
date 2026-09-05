#include "lab_work_5.hpp"
#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "imgui.h"
#include "utils/random.hpp" // pour les couleurs random
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork5::_shaderFolder = "src/lab_works/lab_work_5/shaders/";
	const std::string LabWork5::_modelsFolder = "data/models/";

	LabWork5::~LabWork5() // destructeur
	{
		glDeleteProgram( idProgram );
		glDeleteProgram( _skyboxProgram );

		_triangleMM.cleanGL();
		_skyboxM.cleanGL();

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_BLEND );
	}

	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initProgram();
		_skyboxM._initSkyboxProgram( _skyboxProgram, LabWork5::_shaderFolder ); // initialisation du programme pour la skybox

		_initCamera();

		_triangleMM.load( "", _modelsFolder + "sponza.obj" );
		_skyboxM.load( _modelsFolder + "skybox-20231120/" );

		glEnable( GL_DEPTH_TEST ); // test de profondeur
		glEnable( GL_BLEND );								 // active le mélange des couleurs

		idMVPMatrix		= glGetUniformLocation( idProgram, "uMVPMatrix" );
		idMVMatrix		= glGetUniformLocation( idProgram, "uMVMatrix" );
		idNormalMatrix	= glGetUniformLocation( idProgram, "uNormalMatrix" );
		idLightPosition = glGetUniformLocation( idProgram, "uLightPosition" );

		_skyboxProjMatrix = glGetUniformLocation( _skyboxProgram, "uProjectionMatrix" ); // uniform pour la skybox
		_skyboxViewMatrix = glGetUniformLocation( _skyboxProgram, "uViewMatrix" );

		modelMatrix = glm::scale( Mat4f( 1.0f ), Vec3f( 0.003f, 0.003f, 0.003f ) );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork5::animate( const float p_deltaTime ) {}

	void LabWork5::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // efface la couleur de fond

		MVPMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix() * modelMatrix; // model/view/projection matrix
		MVMatrix	 = _camera.getViewMatrix() * modelMatrix;						 // model/view matrix
		normalMatrix = glm::transpose( glm::inverse( MVMatrix ) );

		_lightPosition = _camera.getViewMatrix() * Vec4f( _camera.getPosition(), 1.0f ); // passage de la camera du world space au view
		setLightPosition( _lightPosition );

		glUseProgram( idProgram );
		glProgramUniformMatrix4fv( idProgram, idMVPMatrix, 1, GL_FALSE, glm::value_ptr( MVPMatrix ) );
		glProgramUniformMatrix4fv( idProgram, idMVMatrix, 1, GL_FALSE, glm::value_ptr( MVMatrix ) );
		glProgramUniformMatrix4fv( idProgram, idNormalMatrix, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
		glProgramUniform3fv( idProgram, idLightPosition, 1, glm::value_ptr( _lightPosition ) );

		_triangleMM.render( idProgram );

		projectionMatrix = _camera.getProjectionMatrix();			  // projection et view matrix pour la skybox
		viewMatrix		 = Mat4f( Mat3f( _camera.getViewMatrix() ) ); // agrandir la skybox

		glUseProgram( _skyboxProgram );
		glProgramUniformMatrix4fv( _skyboxProgram, _skyboxProjMatrix, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
		glProgramUniformMatrix4fv( _skyboxProgram, _skyboxViewMatrix, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );

		_skyboxM.render( _skyboxProgram );
	}

	void LabWork5::handleEvents( const SDL_Event & p_event )
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

	void LabWork5::displayUI()
	{
		ImGui::Begin( "Settings lab work 5" );

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

		ImGui::End();
	}

	void LabWork5::_updateViewMatrix()
	{
		const Mat4f & viewMatrix = _camera.getViewMatrix();
		glProgramUniformMatrix4fv( idProgram, idViewMatrix, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
	}

	void LabWork5::_updateProjectionMatrix()
	{
		const Mat4f & projectionMatrix = _camera.getProjectionMatrix();
		glProgramUniformMatrix4fv( idProgram, idProjectionMatrix, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
	}

	void LabWork5::_initCamera()
	{
		_camera.setPosition( Vec3f( -0.185386f, 0.701571f, -0.0812992f ) );
		_camera.setScreenSize( getWindowWidth(), getWindowHeight() );
	}

	bool LabWork5::_initProgram() {
		const std::string vertexShaderStr	= readFile( _shaderFolder + "mesh_texture.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "mesh_texture.frag" );

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
