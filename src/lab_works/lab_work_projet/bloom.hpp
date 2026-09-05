#ifndef __BLOOM_HPP__
#define __BLOOM_HPP__

#include "GL/gl3w.h"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class Bloom
	{
	  public:
		Bloom()
		{
			_bloomTexture  = GL_INVALID_INDEX;
			idBloomFbo	   = GL_INVALID_INDEX;
		}

		void cleanGL();
		void _initBloomBuffer( const int width, const int height );
		void _gaussianBlurPass( const GLuint p_glProgram, GLuint idVAO, std::vector<GLuint> indicesSommets, GLuint idImageForBlur );

		void _initPingPongBuffer( const int width, const int height );
		void _pingPongPass( const GLuint p_glProgram, GLuint idVAO, std::vector<GLuint> indicesSommets, GLuint idHorizontal );

		void bindBloom();

	  private:
		GLuint _bloomTexture;
		GLuint idBloomFbo;	   // fbo pour le bloom
		GLuint pingpongFBO[ 2 ];
		GLuint pingpongBuffer[ 2 ];
	};
} // namespace M3D_ISICG

#endif // __BLOOM_HPP__
