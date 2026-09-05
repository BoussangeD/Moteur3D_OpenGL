#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "common/models/skybox_model.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() : BaseLabWork()
		{
			idProgram		   = GL_INVALID_INDEX;
			idModelMatrix	   = GL_INVALID_INDEX;
			idViewMatrix	   = GL_INVALID_INDEX;
			idProjectionMatrix = GL_INVALID_INDEX;
			idMVPMatrix		   = GL_INVALID_INDEX;
			idMVMatrix		   = GL_INVALID_INDEX;
			idNormalMatrix	   = GL_INVALID_INDEX;
			idLightPosition	   = GL_INVALID_INDEX;

			_skyboxProgram	   = GL_INVALID_INDEX; 
			_skyboxProjMatrix  = GL_INVALID_INDEX; 
			_skyboxViewMatrix  = GL_INVALID_INDEX; 
		}
		~LabWork5();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void setLightPosition( const Vec3f & position ) { _lightPosition = position; }

	  private:
		// ================ Scene data.
		bool _initProgram();
		void _initCamera();				// initialise les paramètres de la caméra
		void _updateViewMatrix();		// mettre à jour la variable uniform uViewMatrix
		void _updateProjectionMatrix(); // mettre à jour la variable uniform uProjectionMatrix
		// ================

		// ================ GL data.
		GLuint idProgram;
		GLuint idModelMatrix;
		GLuint idViewMatrix;
		GLuint idProjectionMatrix;

		GLuint idMVPMatrix;
		GLuint idMVMatrix;
		GLuint idNormalMatrix;
		GLuint idLightPosition;

		GLuint _skyboxProgram;
		GLuint _skyboxProjMatrix;
		GLuint _skyboxViewMatrix;
		// ================

		// ================ Settings.
		Vec4f _bgColor	  = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color

		Mat4f MVPMatrix	   = Mat4f( 1.0f );
		Mat4f MVMatrix	   = Mat4f( 1.0f );
		Mat4f normalMatrix = Mat4f( 1.0f );
		Mat4f modelMatrix  = Mat4f( 1.0f );

		Mat4f projectionMatrix = Mat4f( 1.0f );		// pour la skybox
		Mat4f viewMatrix = Mat4f( 1.0f );

		Vec3f _lightPosition = VEC3F_ZERO; // Position de la source lumineuse

		Camera _camera;
		float  _cameraSpeed		  = 0.1f; // vitesse de la camera
		float  _cameraSensitivity = 0.1f; // sensibilité de la camera

		TriangleMeshModel _triangleMM;	// variable de type triangleMeshModel
		SkyboxModel		  _skyboxM;		// variable de type skyboxModel
		// ================

		static const std::string _shaderFolder;
		static const std::string _modelsFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_5_HPP__
