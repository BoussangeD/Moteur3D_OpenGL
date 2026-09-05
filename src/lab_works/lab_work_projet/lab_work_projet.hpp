#ifndef __LAB_WORK_PROJET_HPP__
#define __LAB_WORK_PROJET_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "shadow_map.hpp"
#include "bloom.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWorkProjet : public BaseLabWork
	{
	  public:
		LabWorkProjet() : BaseLabWork()
		{
			_geometryPassProgram = GL_INVALID_INDEX;
			_shadingPassProgram	 = GL_INVALID_INDEX;
			_shadowMapProgram	 = GL_INVALID_INDEX;
			_gaussianBlurProgram = GL_INVALID_INDEX;
			fboId				 = GL_INVALID_INDEX;

			idVbo = GL_INVALID_INDEX;
			idVao = GL_INVALID_INDEX;
			idEbo = GL_INVALID_INDEX;

			idModelMatrix	   = GL_INVALID_INDEX;
			idViewMatrix	   = GL_INVALID_INDEX;
			idProjectionMatrix = GL_INVALID_INDEX;
			idMVPMatrix		   = GL_INVALID_INDEX;
			idMVMatrix		   = GL_INVALID_INDEX;
			idNormalMatrix	   = GL_INVALID_INDEX;
			idLightPosition	   = GL_INVALID_INDEX;
			idSelectTexture	   = GL_INVALID_INDEX;
			idGammaCorrection  = GL_INVALID_INDEX;

			idImageForBlur	   = GL_INVALID_INDEX;
			idHorizontal	   = GL_INVALID_INDEX;

			idLightSpaceMatrix = GL_INVALID_INDEX;
		}
		~LabWorkProjet();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void setLightPosition( const Vec3f & position ) { _lightPosition = position; }

	  private:
		// ================ Scene data.
		void _initCamera();				// initialise les paramètres de la caméra
		void _updateViewMatrix();		// mettre à jour la variable uniform uViewMatrix
		void _updateProjectionMatrix(); // mettre à jour la variable uniform uProjectionMatrix

		void _initGBuffer();			 // initialisation du G-buffer
		void _geometryPass();			 // appel le shader de la geometry pass
		void _initQuad();	 // initialisation du quad pour le pipeline graphique
		void _shadingPass(); // appel le shader de la shading pass

		// une fonction pour créer n'importe quel programme
		bool _initAnyPassProgram( GLuint & p_glProgram, const std::string vertexName, const std::string fragmentName ); 
		bool _initShadingPassProgram(); // création du shading pass program
		// ================

		// ================ GL data.
		GLuint _geometryPassProgram;
		GLuint _shadingPassProgram;
		GLuint _shadowMapProgram;
		GLuint _gaussianBlurProgram;

		GLuint idVbo; // buffers pour le quad
		GLuint idVao;
		GLuint idEbo;

		GLuint idModelMatrix;
		GLuint idViewMatrix;
		GLuint idProjectionMatrix;

		GLuint idMVPMatrix;
		GLuint idMVMatrix;
		GLuint idNormalMatrix;
		GLuint idLightPosition;
		GLuint idGammaCorrection;

		GLuint fboId;				  // framebuffer object
		GLuint _gBufferTextures[ 6 ]; // tableau pour les textures
		GLuint idSelectTexture;		  // pour sélectionner la texture souhaitée

		GLuint idImageForBlur;		// uniforms rendu sur une image
		GLuint idHorizontal;

		GLuint idLightSpaceMatrix;	// pour la shadow map
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color

		Mat4f MVPMatrix	   = Mat4f( 1.0f );
		Mat4f MVMatrix	   = Mat4f( 1.0f );
		Mat4f normalMatrix = Mat4f( 1.0f );
		Mat4f modelMatrix  = Mat4f( 1.0f );

		Mat4f lightSpaceMatrix = Mat4f( 1.0f );

		Vec3f _lightPosition = VEC3F_ZERO; // Position de la source lumineuse

		Camera _camera;
		float  _cameraSpeed		  = 0.1f; // vitesse de la camera
		float  _cameraSensitivity = 0.1f; // sensibilité de la camera

		TriangleMeshModel _triangleMM; // variable de type triangleMeshModel

		std::vector<Vec2f>	sommetsTriangle; // vecteur pour les sommets d'un triangle
		std::vector<GLuint> indicesSommets;	 // vecteur pour les indices des sommets

		Bloom	  _bloom;
		ShadowMap _shadowMap;

		float gamma = 2.2f;
		// ================

		static const std::string _shaderFolder;
		static const std::string _modelsFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_PROJET_HPP__
