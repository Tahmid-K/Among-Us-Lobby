#ifndef _SPHERE_H_
#define _SPHERE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class sphere {
	public:
		GLuint vao = 0, vbo = 0, vbo_n = 0, ebo = 0;
		glm::mat4 model;
		int stack_count = 40;
		int sector_count = 40;
		int nums_to_draw;
		sphere(glm::vec3 translation, glm::vec3 scale_factor);
		void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	    glm::mat4 get_model();
	    void set_model(glm::mat4 a_model);
        void scale_and_translate(glm::vec3 translation, glm::vec3 scale_factor);
};

#endif