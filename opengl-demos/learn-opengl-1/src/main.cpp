/*

main notes for this application:
- will be building for x86 only (DEBUG and RELEASE), can leave x64 build configs untouched
- will be targeting the /MT & /MTd runtimes, and thus preferring static-linking
- middleware folder will contain all middleware (then each project can choose which versions to use)
- will be targetting OpenGL 3.3 Core Context (can increase version for more advanced demos)

- MY CONVENTIONS:
- m_MemberVar
- s_StaticVar
- g_GlobalVar
- int *ptr not int* ptr, due to int* a, b being misleading. But note that const int x=5, y=7; declares both x and y as constants.
- CONST goes on the right of the type, TYPES SIGNATURES ARE READ RIGHT TO LEFT, e.g. my_class:: * const & is a reference to a const pointer to a member of my_class
- use #pragma once instead of include-guards (simpler, would only break in specific situations (e.g. same file in multiple places), but avoids typos and is supported by the big compilers)
- never "using namespace std;"
- use .cpp/.h
- headers shouldn't depend on others being included first, but anyway, include headers from local scope to global scope (e.g. my headers before system headers)
- if (NULL/constant/local var == var) (rather than reverse to prevent = vs == bugs)
- single line if statements such as if (cond) single-line;
- use "const" local variables and references as much as possible
- braces on same line

*/



//NOTE: must include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int const SCR_WIDTH = 800;
unsigned int const SCR_HEIGHT = 600;

int main(int argc, char const *argv[]) {
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit()) return -1;

	// TARGET = OpenGL 3.3 CORE context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	std::cout << "OpenGL target version: " << GLVersion.major << "." << GLVersion.minor << "+" << std::endl;



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


glm::mat4 camera(float Translate, glm::vec2 const &Rotate) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}