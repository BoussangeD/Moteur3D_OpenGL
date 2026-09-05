#include "glm/gtc/type_ptr.hpp" // pour glm::value_ptr
#include "shadow_map.hpp"
#include <iostream>

namespace M3D_ISICG
{
	void ShadowMap::cleanGL()
	{
		glDeleteBuffers( 1, &idFboSM );
		glDeleteBuffers( 1, &idDepthBuffer );
	}

	void ShadowMap::_initDepthBuffer()
	{
		// Cf. https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

		glCreateFramebuffers( 1, &idFboSM );	// FBO

		glCreateTextures( GL_TEXTURE_2D, 1, &idDepthBuffer ); // depth buffer
		glTextureStorage2D( idDepthBuffer, 1, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT );
		glTextureParameteri( idDepthBuffer, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( idDepthBuffer, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTextureParameteri( idDepthBuffer, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTextureParameteri( idDepthBuffer, GL_TEXTURE_WRAP_T, GL_REPEAT );

		glBindFramebuffer( GL_FRAMEBUFFER, idFboSM );
		glNamedFramebufferTexture( idFboSM, GL_DEPTH_ATTACHMENT, idDepthBuffer, 0 );

		glNamedFramebufferDrawBuffer( idFboSM, GL_NONE );	// aucun rendu des couleurs
		glNamedFramebufferReadBuffer( idFboSM, GL_NONE );

		if ( glCheckNamedFramebufferStatus( idFboSM, GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "Erreur sur le FBO !" << std::endl;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void ShadowMap::shadowMapPass( const GLuint p_glProgram, GLuint idlightSpaceMatrix, const Mat4f & lightSpaceMatrix )
	{ 
		glUseProgram( p_glProgram ); 

		glBindFramebuffer( GL_FRAMEBUFFER, idFboSM );
		glViewport( 0, 0, SHADOW_WIDTH, SHADOW_HEIGHT ); 
		glClear( GL_DEPTH_BUFFER_BIT );					 

		glProgramUniformMatrix4fv( p_glProgram, idlightSpaceMatrix, 1, GL_FALSE, glm::value_ptr( lightSpaceMatrix ) );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void ShadowMap::bindShadowMap() {
		glBindTextureUnit( 12, idDepthBuffer );
	}
} // namespace M3D_ISICG
