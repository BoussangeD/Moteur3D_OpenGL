#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6() : BaseLabWork()
		{
			_geometryPassProgram = GL_INVALID_INDEX;
			_shadingPassProgram	 = GL_INVALID_INDEX;
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

			_gBufferTextures[ 6 ] = { GL_INVALID_INDEX };
		}
		~LabWork6();

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
		bool _initGeometryPassProgram(); // création du geometry pass program
		bool _initShadingPassProgram();	 // création du shading pass program
		void _geometryPass();			 // appel le shader de la geometry pass

		void _initQuad();	 // initialisation du quad pour le pipeline graphique
		void _shadingPass(); // appel le shader de la shading pass
		// ================

		// ================ GL data.
		GLuint _geometryPassProgram;
		GLuint _shadingPassProgram;
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

		GLuint fboId;				  // framebuffer object
		GLuint _gBufferTextures[ 6 ]; // tableau pour les textures

		GLuint idSelectTexture;		  // pour sélectionner la texture souhaitée
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color

		Mat4f MVPMatrix	   = Mat4f( 1.0f );
		Mat4f MVMatrix	   = Mat4f( 1.0f );
		Mat4f normalMatrix = Mat4f( 1.0f );
		Mat4f modelMatrix  = Mat4f( 1.0f );

		Vec3f _lightPosition = VEC3F_ZERO; // Position de la source lumineuse

		Camera _camera;
		float  _cameraSpeed		  = 0.1f; // vitesse de la camera
		float  _cameraSensitivity = 0.1f; // sensibilité de la camera

		TriangleMeshModel _triangleMM; // variable de type triangleMeshModel

		std::vector<Vec2f>	sommetsTriangle; // vecteur pour les sommets d'un triangle
		std::vector<GLuint> indicesSommets;	 // vecteur pour les indices des sommets
		// ================

		static const std::string _shaderFolder;
		static const std::string _modelsFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
