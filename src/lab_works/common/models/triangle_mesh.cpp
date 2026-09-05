#include "triangle_mesh.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace M3D_ISICG
{
	TriangleMesh::TriangleMesh( const std::string &				  p_name,
								const std::vector<Vertex> &		  p_vertices,
								const std::vector<unsigned int> & p_indices,
								const Material &				  p_material ) :
		_name( p_name ),
		_vertices( p_vertices ), _indices( p_indices ), _material( p_material )
	{
		_vertices.shrink_to_fit();
		_indices.shrink_to_fit();
		_setupGL();
	}

	void TriangleMesh::render( const GLuint p_glProgram ) const
	{
		GLuint idAmbientColor  = glGetUniformLocation( p_glProgram, "uAmbientColor" );
		GLuint idDiffuseColor  = glGetUniformLocation( p_glProgram, "uDiffuseColor" );
		GLuint idSpecularColor = glGetUniformLocation( p_glProgram, "uSpecularColor" );
		GLuint idShininess	   = glGetUniformLocation( p_glProgram, "uShininess" );

		GLuint idHasDiffuseMap	 = glGetUniformLocation( p_glProgram, "uHasDiffuseMap" );
		GLuint idDiffuseMap		 = glGetUniformLocation( p_glProgram, "uDiffuseMap" );
		GLuint idHasAmbientMap	 = glGetUniformLocation( p_glProgram, "uHasAmbientMap" );
		GLuint idAmbientMap		 = glGetUniformLocation( p_glProgram, "uAmbientMap" );
		GLuint idHasSpecularMap	 = glGetUniformLocation( p_glProgram, "uHasSpecularMap" );
		GLuint idSpecularMap	 = glGetUniformLocation( p_glProgram, "uSpecularMap" );
		GLuint idHasShininessMap = glGetUniformLocation( p_glProgram, "uHasShininessMap" );
		GLuint idShininessMap	 = glGetUniformLocation( p_glProgram, "uShininessMap" );

		GLuint idHasNormalMap = glGetUniformLocation( p_glProgram, "uHasNormalMap" );
		GLuint idNormalMap	  = glGetUniformLocation( p_glProgram, "uNormalMap" );

		glProgramUniform3fv( p_glProgram, idAmbientColor, 1, glm::value_ptr( _material._ambient ) );
		glProgramUniform3fv( p_glProgram, idDiffuseColor, 1, glm::value_ptr( _material._diffuse ) );
		glProgramUniform3fv( p_glProgram, idSpecularColor, 1, glm::value_ptr( _material._specular ) );
		glProgramUniform1f( p_glProgram, idShininess, _material._shininess );

		glProgramUniform1i( p_glProgram, idHasDiffuseMap, _material._hasDiffuseMap );
		glProgramUniform1i( p_glProgram, idHasAmbientMap, _material._hasAmbientMap );
		glProgramUniform1i( p_glProgram, idHasSpecularMap, _material._hasSpecularMap );
		glProgramUniform1i( p_glProgram, idHasShininessMap, _material._hasShininessMap );
		glProgramUniform1i( p_glProgram, idHasNormalMap, _material._hasNormalMap );

		if ( _material._hasDiffuseMap )
		{
			glProgramUniform1i( p_glProgram, idDiffuseMap, 1 );
			glBindTextureUnit( 1, _material._diffuseMap._id );
		}

		if ( _material._hasAmbientMap )
		{
			glProgramUniform1i( p_glProgram, idAmbientMap, 2 );
			glBindTextureUnit( 2, _material._ambientMap._id );
		}

		if ( _material._hasSpecularMap )
		{
			glProgramUniform1i( p_glProgram, idSpecularMap, 3 );
			glBindTextureUnit( 3, _material._specularMap._id );
		}

		if ( _material._hasShininessMap )
		{
			glProgramUniform1i( p_glProgram, idShininessMap, 4 );
			glBindTextureUnit( 4, _material._shininessMap._id );
		}

		if ( _material._hasNormalMap )
		{
			glProgramUniform1i( p_glProgram, idNormalMap, 5 );
			glBindTextureUnit( 5, _material._normalMap._id );
		}

		glBindVertexArray( _vao );
		glDrawElements( GL_TRIANGLES, (int)_indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 ); // delie le vao et le programme

		if ( _material._hasDiffuseMap )
		{
			glBindTextureUnit( 1, 0 );
		}
		if ( _material._hasAmbientMap )
		{
			glBindTextureUnit( 2, 0 );
		}
		if ( _material._hasSpecularMap )
		{
			glBindTextureUnit( 3, 0 );
		}
		if ( _material._hasShininessMap )
		{
			glBindTextureUnit( 4, 0 );
		}
		if ( _material._hasNormalMap )
		{
			glBindTextureUnit( 5, 0 );
		}
	}

	void TriangleMesh::cleanGL()
	{
		glDisableVertexArrayAttrib( _vao, 0 );
		glDisableVertexArrayAttrib( _vao, 1 );
		glDisableVertexArrayAttrib( _vao, 2 );
		glDisableVertexArrayAttrib( _vao, 3 );
		glDisableVertexArrayAttrib( _vao, 4 );
		glDeleteVertexArrays( 1, &_vao );
		glDeleteBuffers( 1, &_vbo );
		glDeleteBuffers( 1, &_ebo );
	}

	void TriangleMesh::_setupGL()
	{
		// partie VBO
		glCreateBuffers( 1, &_vbo );
		glNamedBufferData( _vbo, _vertices.size() * sizeof( Vertex ), _vertices.data(), GL_STATIC_DRAW );

		// partie VAO
		glCreateVertexArrays( 1, &_vao );

		// indice 0 : aVertexPosition
		glEnableVertexArrayAttrib( _vao, 0 );
		glVertexArrayAttribFormat( _vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _position ) );
		glVertexArrayAttribBinding( _vao, 0, 0 );

		// indice 1 : aVertexNormal
		glEnableVertexArrayAttrib( _vao, 1 );
		glVertexArrayAttribFormat( _vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _normal ) );
		glVertexArrayAttribBinding( _vao, 1, 0 );

		// indice 2 : aVertexTexCoords
		glEnableVertexArrayAttrib( _vao, 2 );
		glVertexArrayAttribFormat( _vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof( Vertex, _texCoords ) ); // 2 car Vec2f ici
		glVertexArrayAttribBinding( _vao, 2, 0 );

		// indice 3 : aVertexTangent
		glEnableVertexArrayAttrib( _vao, 3 );
		glVertexArrayAttribFormat( _vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _tangent ) );
		glVertexArrayAttribBinding( _vao, 3, 0 );

		// indice 4 : aVertexBitagent
		glEnableVertexArrayAttrib( _vao, 4 );
		glVertexArrayAttribFormat( _vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof( Vertex, _bitangent ) );
		glVertexArrayAttribBinding( _vao, 4, 0 );

		glVertexArrayVertexBuffer( _vao, 0, _vbo, 0, sizeof( Vertex ) );

		// partie EBO
		glCreateBuffers( 1, &_ebo );
		glNamedBufferData( _ebo, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );
		glVertexArrayElementBuffer( _vao, _ebo );
	}
} // namespace M3D_ISICG
