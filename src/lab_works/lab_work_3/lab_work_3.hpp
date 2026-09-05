#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork()
		{
			idProgram		   = GL_INVALID_INDEX;
			idModelMatrix	   = GL_INVALID_INDEX;
			idViewMatrix	   = GL_INVALID_INDEX;
			idProjectionMatrix = GL_INVALID_INDEX;
			idMVPMatrix		   = GL_INVALID_INDEX;
		}
		~LabWork3();

		struct Mesh
		{
			std::vector<Vec3f>	positionSommets;
			std::vector<Vec3f>	couleursSommets;
			std::vector<GLuint> indicesSommets;
			Mat4f				transfoMatrices;
			Mat4f				MVPMatrix;
			GLuint				idVbo;
			GLuint				idVao;
			GLuint				idEbo;
			GLuint				idVboCouleur;	

			Mesh()
			{
				transfoMatrices  = Mat4f( 1.0f );
				MVPMatrix		 = Mat4f( 1.0f );
				idVbo			 = GL_INVALID_INDEX;
				idVao			 = GL_INVALID_INDEX;
				idEbo			 = GL_INVALID_INDEX;
				idVboCouleur	 = GL_INVALID_INDEX;
			}
		};

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		bool _initProgram();

		void _createCube();
		void _createCube2();
		void _initBuffers();
		void _initBuffers2();

		void _initCamera();				// initialise les paramètres de la caméra
		void _updateViewMatrix();		// mettre à jour la variable uniform uViewMatrix
		void _updateProjectionMatrix(); // mettre à jour la variable uniform uProjectionMatrix
		// ================

		// ================ GL data.
		GLuint idProgram;
		GLuint idModelMatrix;
		GLuint idViewMatrix;
		GLuint idProjectionMatrix;
		GLuint idMVPMatrix; // Model/View/Projection Matrix
		// ================

		// ================ Settings.
		Vec4f _bgColor	  = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time		  = 0.0f;							// temps global
		float _luminosite = 1.0f;							// luminosité de la forme

		Mesh   _cube;					  // attribut de type mesh pour accéder aux vecteurs et buffers
		Mesh   _cube2;
		Vec3f  _rotationCube = Vec3f( 0.0f, 1.0f, 0.0f );

		Camera _camera;					  // attribut camera pour récupérer les données voulues
		float  _cameraSpeed		  = 0.1f; // vitesse de la camera
		float  _cameraSensitivity = 0.1f; // sensibilité de la camera
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
