#include "skybox_model.hpp"
#include "GL/gl3w.h"
#include "skybox_model.hpp"
#include "utils/image.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include "utils/read_file.hpp"


namespace M3D_ISICG
{
	void SkyboxModel::load( const FilePath & p_directoryPath )
	{
		std::cout << "Loading model from: " << p_directoryPath << std::endl;
		_setupSkybox();
		_cubemapTexture = _loadTextures( p_directoryPath );
	}

	void SkyboxModel::render( const GLuint p_glProgram ) const
	{
		GLuint idCubeMap = glGetUniformLocation( p_glProgram, "uSkybox" );
		glProgramUniform1i( p_glProgram, idCubeMap, 0 ); 

		glBindTexture( GL_TEXTURE_CUBE_MAP, _cubemapTexture );
		glDepthFunc( GL_LEQUAL ); // ajuster la fonction de profondeur pour éviter des problèmes avec le skybox

		// dessiner la skybox
		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, (int)skyboxIndices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );

		glDepthFunc( GL_LESS ); // remettre la fonction de profondeur par défaut
		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	}

	void SkyboxModel::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
		glDeleteTextures( 1, &_cubemapTexture );
	}

	GLuint SkyboxModel::_loadTextures( const FilePath & p_directoryPath )
	{
		const FilePath skyboxFaces[]
			= { p_directoryPath + "right.png",	p_directoryPath + "left.png", p_directoryPath + "top.png",
				p_directoryPath + "bottom.png", p_directoryPath + "front.png", p_directoryPath + "back.png" };

		Image image;	// on récupère la taille en largeur et hauteur d'une des images pour connaître la taille des autres images
		image.load( skyboxFaces[ 0 ] );
		int width  = image._width; 
		int height = image._height;

		// Create a texture on the GPU.
		Texture texture;
		glCreateTextures( GL_TEXTURE_CUBE_MAP, 1, &texture._id );
		glTextureStorage2D( texture._id, 1, GL_RGBA8, width, height );

		for ( int i = 0; i < 6; i++ )
		{
			GLenum format		  = GL_INVALID_ENUM;
			if ( image._nbChannels == 1 )
			{
				format		   = GL_RED;
			}
			else if ( image._nbChannels == 2 )
			{
				format		   = GL_RG;
			}
			else if ( image._nbChannels == 3 )
			{
				format		   = GL_RGB;
			}
			else
			{
				format		   = GL_RGBA;
			}

			if ( image.load( skyboxFaces[ i ] ) )
			{
				glTextureSubImage3D(
					texture._id, 0, 0, 0, i, image._width, image._height, 1, format, GL_UNSIGNED_BYTE, image._pixels );
			}
			else
			{
				std::cerr << "Failed to load skybox texture: " << skyboxFaces[ i ] << std::endl;
			}
		}

		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		// on clamp les textures dans les 3 directions car on a un cube 3d
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

		return texture._id;
	}

	void SkyboxModel::_setupSkybox()
	{
		// BOTTOM FACE
		skyboxSommets.emplace_back( Vec3f( -taille, -taille, taille ) );
		skyboxSommets.emplace_back( Vec3f( taille, -taille, taille ) );
		skyboxSommets.emplace_back( Vec3f( taille, -taille, -taille ) );
		skyboxSommets.emplace_back( Vec3f( -taille, -taille, -taille ) );

		// TOP FACE
		skyboxSommets.emplace_back( Vec3f( -taille, taille, taille ) );
		skyboxSommets.emplace_back( Vec3f( taille, taille, taille ) );
		skyboxSommets.emplace_back( Vec3f( taille, taille, -taille ) );
		skyboxSommets.emplace_back( Vec3f( -taille, taille, -taille ) );

		skyboxIndices = {
			1, 2, 6, 6, 5, 1, 
			0, 4, 7, 7, 3, 0, 
			4, 5, 6, 6, 7, 4, 
			0, 3, 2, 2, 1, 0, 
			0, 1, 5, 5, 4, 0, 
			3, 7, 6, 6, 2, 3 
		};

		// partie vbo
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, skyboxSommets.size() * sizeof( Vec3f ), skyboxSommets.data(), GL_STATIC_DRAW );

		// partie vao
		glCreateVertexArrays( 1, &_vao );
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// partie ebo
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, skyboxIndices.size() * sizeof( GLuint ), skyboxIndices.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _vao, _ebo );
	}

	bool SkyboxModel::_initSkyboxProgram( GLuint & p_glProgram, const std::string & _shaderFolder )
	{
		const std::string vertexShaderStr	= readFile( _shaderFolder + "skybox.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "skybox.frag" );

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

		p_glProgram = glCreateProgram(); // creation du programme

		glAttachShader( p_glProgram, vertexShader ); // lie les shaders avec le programme
		glAttachShader( p_glProgram, fragmentShader );
		glLinkProgram( p_glProgram );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( p_glProgram, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( p_glProgram, sizeof( log ), NULL, log );
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
