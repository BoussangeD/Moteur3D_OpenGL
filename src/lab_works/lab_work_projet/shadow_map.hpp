#ifndef __SHADOW_MAP_HPP__
#define __SHADOW_MAP_HPP__

#include "GL/gl3w.h"
#include "define.hpp"

namespace M3D_ISICG
{
	class ShadowMap
	{
	  public:
		ShadowMap() 
		{ 
			idFboSM = GL_INVALID_INDEX;
			idDepthBuffer = GL_INVALID_INDEX;
		}

		void cleanGL();
		void _initDepthBuffer();

		void shadowMapPass( const GLuint p_glProgram, GLuint idlightSpaceMatrix, const Mat4f & lightSpaceMatrix );
		void bindShadowMap();

	  private:
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		GLuint idFboSM;
		GLuint idDepthBuffer;
	};
} // namespace M3D_ISICG

#endif // __SHADOW_MAP_HPP__
