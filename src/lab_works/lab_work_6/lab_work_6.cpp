#include "lab_work_6.hpp"
#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "imgui.h"
#include "utils/random.hpp" // pour les couleurs random
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";
	const std::string LabWork6::_modelsFolder = "data/models/";
	
	LabWork6::~LabWork6() // destructeur
	{
		glDeleteProgram( _geometryPassProgram );
		glDeleteProgram( _shadingPassProgram );

		_triangleMM.cleanGL();

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_BLEND );

		glDeleteBuffers( 1, &idVbo );
		glDisableVertexArrayAttrib( idVao, 0 );
		glDeleteVertexArrays( 1, &idVao );
		glDeleteBuffers( 1, &idEbo );
		glDeleteBuffers( 1, &fboId );
		glDeleteBuffers( 1, _gBufferTextures );
	}


	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initGeometryPassProgram();
		_initShadingPassProgram();

		_initCamera();

		_triangleMM.load( "", _modelsFolder + "sponza.obj" );

		glEnable( GL_DEPTH_TEST ); // test de profondeur
		glEnable( GL_BLEND );	   // active le mélange des couleurs

		idMVPMatrix		= glGetUniformLocation( _geometryPassProgram, "uMVPMatrix" );
		idMVMatrix		= glGetUniformLocation( _geometryPassProgram, "uMVMatrix" );
		idNormalMatrix	= glGetUniformLocation( _geometryPassProgram, "uNormalMatrix" );
		idLightPosition = glGetUniformLocation( _shadingPassProgram, "uLightPosition" );

		modelMatrix = glm::scale( Mat4f( 1.0f ), Vec3f( 0.003f, 0.003f, 0.003f ) );

		_initGBuffer();
		_initQuad();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::_initGBuffer()
	{
		glCreateFramebuffers( 1, &fboId );

		GLenum drawBuffers[ 6 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
									GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_DEPTH_ATTACHMENT };

		for ( int i = 0; i < 6; i++ ) {
			glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ i ] );
		}

		// position
		glTextureStorage2D( _gBufferTextures[ 0 ], 1, GL_RGBA32F, getWindowWidth(), getWindowHeight() );

		// normales
		glTextureStorage2D( _gBufferTextures[ 1 ], 1, GL_RGBA32F, getWindowWidth(), getWindowHeight() );

		// couleure ambiante
		glTextureStorage2D( _gBufferTextures[ 2 ], 1, GL_RGBA8, getWindowWidth(), getWindowHeight() );

		// couleure diffuse
		glTextureStorage2D( _gBufferTextures[ 3 ], 1, GL_RGBA8, getWindowWidth(), getWindowHeight() );

		// couleure spéculaire
		glTextureStorage2D( _gBufferTextures[ 4 ], 1, GL_RGBA8, getWindowWidth(), getWindowHeight() );

		// depth
		glTextureStorage2D( _gBufferTextures[ 5 ], 1, GL_DEPTH_COMPONENT32F, getWindowWidth(), getWindowHeight() );

		for ( int i = 0; i < 6; i++ )	// liaison des textures au framebuffer
		{
			glNamedFramebufferTexture( fboId, drawBuffers[ i ], _gBufferTextures[ i ], 0 );
		}

		glNamedFramebufferDrawBuffers( fboId, 5, drawBuffers ); // associer les textures à la sortie du fragment shader
		if ( glCheckNamedFramebufferStatus( fboId, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "Erreur sur le FBO !" << std::endl;
		}
	}

	void LabWork6::_geometryPass()
	{
		glUseProgram( _geometryPassProgram ); // on specifie le programme à utiliser

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fboId );	  // binder fbo sur GL_DRAW_FRAMEBUFFER
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // efface la couleur de fond

		_triangleMM.render( _geometryPassProgram );			  // appel à glDraw dans triangleMesh

		glNamedFramebufferReadBuffer( fboId, GL_COLOR_ATTACHMENT0 );
		glBlitNamedFramebuffer( fboId,				 // buffer source
								0,					 // buffer défault
								0,					 // x0 src
								0,					 // y0 src
								getWindowWidth(),	 // x1 src
								getWindowHeight(),	 // y1 src
								0,					 // x0 dest
								0,					 // y0 dest
								getWindowWidth(),	 // x1 dest
								getWindowHeight(),	 // y1 dest
								GL_COLOR_BUFFER_BIT, // masque
								GL_NEAREST );		 // filtre

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	}

	void LabWork6::_initQuad()
	{
		float taille = 1.0f;

		sommetsTriangle.emplace_back( Vec2f( -taille, taille ) );
		sommetsTriangle.emplace_back( Vec2f( taille, taille ) );
		sommetsTriangle.emplace_back( Vec2f( taille, -taille ) );
		sommetsTriangle.emplace_back( Vec2f( -taille, -taille ) );

		indicesSommets = { 0, 1, 2, 0, 2, 3 }; // Les indices forment deux triangles (0-1-2 et 0-2-3)

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
	}

	void LabWork6::_shadingPass()
	{
		glUseProgram( _shadingPassProgram ); // on specifie le programme à utiliser
		glDisable( GL_DEPTH_TEST );

		// positions
		glBindTextureUnit( 6, _gBufferTextures[ 0 ] );

		// normales
		glBindTextureUnit( 7, _gBufferTextures[ 1 ] );

		// couleure ambiante
		glBindTextureUnit( 8, _gBufferTextures[ 2 ] );
		
		// couleure diffuse
		glBindTextureUnit( 9, _gBufferTextures[ 3 ] );
		
		// couleure spéculaire
		glBindTextureUnit( 10, _gBufferTextures[ 4 ] );
		
		glBindVertexArray( idVao ); // lien entre vao et le programme
		glDrawElements( GL_TRIANGLES, (int)indicesSommets.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 ); // delie le vao et le programme

		glBindTextureUnit( 6, 0 );
		glBindTextureUnit( 7, 0 );
		glBindTextureUnit( 8, 0 );
		glBindTextureUnit( 9, 0 );
		glBindTextureUnit( 10, 0 );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glEnable( GL_DEPTH_TEST );
	}

	void LabWork6::animate( const float p_deltaTime ) {}

	void LabWork6::render()
	{
		MVPMatrix
			= _camera.getProjectionMatrix() * _camera.getViewMatrix() * modelMatrix; // model/view/projection matrix
		MVMatrix	 = _camera.getViewMatrix() * modelMatrix;						 // model/view matrix
		normalMatrix = glm::transpose( glm::inverse( MVMatrix ) );

		_lightPosition = _camera.getViewMatrix()
						 * Vec4f( _camera.getPosition(), 1.0f ); // passage de la camera du world space au view space
		setLightPosition( _lightPosition );

		glProgramUniformMatrix4fv( _geometryPassProgram, idMVPMatrix, 1, GL_FALSE, glm::value_ptr( MVPMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, idMVMatrix, 1, GL_FALSE, glm::value_ptr( MVMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, idNormalMatrix, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
		glProgramUniform3fv( _shadingPassProgram, idLightPosition, 1, glm::value_ptr( _lightPosition ) );

		_geometryPass();
		_shadingPass();
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
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

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Settings lab work 6" );

		ImGui::Text( "Background Color" );
		if ( ImGui::ColorEdit3( "##Background Color", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		ImGui::Text( "FOVY" );
		float fovy = _camera.getFovy();
		if ( ImGui::SliderFloat( "##FOVY", &fovy, 1.0f, 120.0f ) )
		{
			_camera.setFovy( fovy );
		}

		// listBox pour choisir la texture à afficher
		ImGui::Text( "Select Texture" );
		idSelectTexture = glGetUniformLocation(_shadingPassProgram, "uSelectTexture"); // récupère l'index de la variable uniform uSelectTexture
		static const char * textures[] = { "All textures", "Position", "Normal", "Ambient", "Diffuse", "Specular" };
		int					selectTexture = 0; 
		if ( ImGui::ListBox("##Select Texture", &selectTexture, textures, IM_ARRAYSIZE(textures)) ) {
			// maj de la variable uniforme dans le shader avec la texture sélectionnée
			glProgramUniform1i( _shadingPassProgram, idSelectTexture, selectTexture );
		}

		ImGui::End();
	}

	void LabWork6::_updateViewMatrix()
	{
		const Mat4f & viewMatrix = _camera.getViewMatrix();
		glProgramUniformMatrix4fv( _geometryPassProgram, idViewMatrix, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
	}

	void LabWork6::_updateProjectionMatrix()
	{
		const Mat4f & projectionMatrix = _camera.getProjectionMatrix();
		glProgramUniformMatrix4fv(
			_geometryPassProgram, idProjectionMatrix, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
	}

	void LabWork6::_initCamera()
	{
		_camera.setPosition( Vec3f( -0.185386f, 0.701571f, -0.0812992f ) );
		_camera.setScreenSize( getWindowWidth(), getWindowHeight() );
	}

	bool LabWork6::_initGeometryPassProgram()
	{
		const std::string vertexShaderStr	= readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "geometry_pass.frag" );

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
			std ::cerr << " Error compiling fragment shader : " << log << std ::endl;
			return false;
		}

		_geometryPassProgram = glCreateProgram(); // creation du programme

		glAttachShader( _geometryPassProgram, vertexShader ); // lie les shaders avec le programme
		glAttachShader( _geometryPassProgram, fragmentShader );
		glLinkProgram( _geometryPassProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _geometryPassProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProgram, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( vertexShader ); // suppression des shaders
		glDeleteShader( fragmentShader );

		return true;
	}

	bool LabWork6::_initShadingPassProgram()
	{
		const std::string shading_pass = readFile( _shaderFolder + "shading_pass.frag" );

		GLuint fragmentShading = glCreateShader( GL_FRAGMENT_SHADER );

		const GLchar * sfSrc = shading_pass.c_str();
		glShaderSource( fragmentShading, 1, &sfSrc, NULL ); // association des shaders
		glCompileShader( fragmentShading );

		// Check if compilation is ok.
		GLint compiled1;
		glGetShaderiv( fragmentShading, GL_COMPILE_STATUS, &compiled1 );
		if ( !compiled1 )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShading, sizeof( log ), NULL, log );
			glDeleteShader( fragmentShading );
			std ::cerr << " Error compiling shading pass : " << log << std ::endl;
			return false;
		}

		_shadingPassProgram = glCreateProgram(); // creation du programme

		glAttachShader( _shadingPassProgram, fragmentShading );
		glLinkProgram( _shadingPassProgram );

		// Check if link is ok.
		GLint linked1;
		glGetProgramiv( _shadingPassProgram, GL_LINK_STATUS, &linked1 );
		if ( !linked1 )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProgram, sizeof( log ), NULL, log );
			glDeleteShader( fragmentShading );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( fragmentShading );

		return true;
	}

} // namespace M3D_ISICG
