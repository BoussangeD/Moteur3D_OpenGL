#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork()
		{
			idProgram	 = GL_INVALID_INDEX;
			idVbo		 = GL_INVALID_INDEX;
			idVao		 = GL_INVALID_INDEX;
			idEbo		 = GL_INVALID_INDEX;
			idVboCouleur = GL_INVALID_INDEX;
			translationX = GL_INVALID_VALUE;
		}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		bool _initProgram();
		void _initBuffers();
		void _drawQuad();
		void disque( const Vec2f & C, int N, float R );

		// ================ Scene data.
		std::vector<Vec2f> sommetsTriangle;		// vecteur pour les sommets d'un triangle
		std::vector<GLuint> indicesSommets;		// vecteur pour les indices des sommets
		std::vector<Vec4f>	couleursSommets;	// vecteur pour les couleurs d'un sommet
		// ================

		// ================ GL data.
		GLuint   idProgram;
		GLuint   idVbo;
		GLuint   idVao;
		GLuint   idEbo;
		GLuint   idVboCouleur;
		GLint	 translationX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time = 0.0f;								 // temps global
		float _luminosite = 1.0f;						 // luminosité de la forme
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
