#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "bloom.hpp"
#include <iostream>

namespace M3D_ISICG
{
	void Bloom::cleanGL()
	{
		glDeleteBuffers( 1, &idBloomFbo );
		glDeleteBuffers( 2, pingpongFBO );
	}

	void Bloom::_initBloomBuffer(const int width, const int height)
	{
		// Cf. https://learnopengl.com/Advanced-Lighting/Bloom

		glCreateTextures( GL_TEXTURE_2D, 1, &_bloomTexture );

		glTextureStorage2D( _bloomTexture, 1, GL_RGB16F, width, height );
		glTextureParameteri( _bloomTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( _bloomTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTextureParameteri( _bloomTexture, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTextureParameteri( _bloomTexture, GL_TEXTURE_WRAP_T, GL_REPEAT );

		glCreateFramebuffers( 1, &idBloomFbo );

		glNamedFramebufferTexture( idBloomFbo, GL_COLOR_ATTACHMENT1, _bloomTexture, 0 );

		GLenum drawBuffers[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glNamedFramebufferDrawBuffers( idBloomFbo, 2, drawBuffers );

		if ( glCheckNamedFramebufferStatus( idBloomFbo, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "Erreur sur le FBO !" << std::endl;
		}
	}

	void Bloom::_gaussianBlurPass( const GLuint p_glProgram, GLuint idVAO, std::vector<GLuint> indicesSommets, GLuint idImageForBlur )
	{
		glUseProgram( p_glProgram );
		glBindFramebuffer( GL_FRAMEBUFFER, pingpongFBO[ 0 ] );

		// lier la texture à flouter à l'unité de texture 0 (par exemple)
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, _bloomTexture ); // bloom texture à flouter

		// indiquer au shader que uImage doit utiliser l'unité de texture 0
		glProgramUniform1i( p_glProgram, idImageForBlur, 0 );

		// redessiner le quad pour appliquer le flou
		glBindVertexArray( idVAO );
		glDrawElements( GL_TRIANGLES, (int)indicesSommets.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void Bloom::_initPingPongBuffer( const int width, const int height )
	{
		glCreateFramebuffers( 2, pingpongFBO );

		glCreateTextures( GL_TEXTURE_2D, 2, pingpongBuffer );

		for ( int i = 0; i < 2; i++ )
		{
			// attacher la texture au Framebuffer
			glBindFramebuffer( GL_FRAMEBUFFER, pingpongFBO[ i ] );

			glTextureStorage2D( pingpongBuffer[ i ], 1, GL_RGBA16F, width, height );
			glTextureParameteri( pingpongBuffer[ i ], GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTextureParameteri( pingpongBuffer[ i ], GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTextureParameteri( pingpongBuffer[ i ], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTextureParameteri( pingpongBuffer[ i ], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			// attacher la texture au Framebuffer
			glNamedFramebufferTexture( pingpongFBO[ i ], GL_COLOR_ATTACHMENT0, pingpongBuffer[ i ], 0 );

			if ( glCheckNamedFramebufferStatus( pingpongFBO[ i ], GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			{
				std::cerr << "Erreur sur le FBO !" << std::endl;
			}
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Bloom::_pingPongPass( const GLuint p_glProgram, GLuint idVAO, std::vector<GLuint> indicesSommets, GLuint idHorizontal )
	{
		glUseProgram( p_glProgram );

		bool horizontal = true, first_iteration = true;
		int	 amount = 10; // nombre de fois que le flou gaussien est appliqué

		// boucle pour appliquer le flou gaussien de manière itérative
		for ( int i = 0; i < amount; i++ )
		{
			// sélectionner le framebuffer pour la passe de flou actuelle
			glBindFramebuffer( GL_FRAMEBUFFER, pingpongFBO[ horizontal ] );
			glProgramUniform1i( p_glProgram, idHorizontal, horizontal );

			// sélectionner la texture appropriée comme source de flou
			glBindTexture( GL_TEXTURE_2D, first_iteration ? _bloomTexture : pingpongBuffer[ !horizontal ] );

			// redessiner le quad pour appliquer le flou
			glBindVertexArray( idVAO );
			glDrawElements( GL_TRIANGLES, (int)indicesSommets.size(), GL_UNSIGNED_INT, 0 );
			glBindVertexArray( 0 );

			// basculer l'orientation du flou pour la prochaine passe
			horizontal = !horizontal;
			if ( first_iteration )
			{
				first_iteration = false;
			}
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Bloom::bindBloom() { glBindTextureUnit( 11, _bloomTexture ); }
} // namespace M3D_ISICG
