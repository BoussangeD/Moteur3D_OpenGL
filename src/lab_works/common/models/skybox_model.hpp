#ifndef __SKYBOX_MODEL_HPP__
#define __SKYBOX_MODEL_HPP__

#include "base_model.hpp"
#include "triangle_mesh.hpp"
#include "utils/file_path.hpp"
#include "utils/image.hpp"
#include <string>

namespace M3D_ISICG
{
	class SkyboxModel : public BaseModel
	{
	  public:
		SkyboxModel()		   = default;
		virtual ~SkyboxModel() = default;

		void load( const FilePath & p_directoryPath );
		void render( const GLuint p_glProgram ) const override;
		void cleanGL() override;
		bool _initSkyboxProgram( GLuint & p_glProgram, const std::string & _shaderFolder );

	  private:
		std::vector<Vec3f>	 skyboxSommets;
		std::vector<GLuint>	 skyboxIndices;

		float taille = 1.0f; // taille de la skybox

		GLuint _vao			   = GL_INVALID_INDEX;
		GLuint _vbo			   = GL_INVALID_INDEX;
		GLuint _ebo			   = GL_INVALID_INDEX;
		GLuint _cubemapTexture = GL_INVALID_INDEX;

		GLuint _loadTextures( const FilePath & p_directoryPath );
		void	_setupSkybox();
	};
} // namespace M3D_ISICG

#endif // __SKYBOX_MODEL_HPP__
