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
#include <string>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
//void processInput(GLFWwindow *window);
glm::mat4 camera(float Translate, glm::vec2 const &Rotate);
void queryGLVersion();
void errorCallback(int error, char const *description);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);



// settings
//unsigned int const SCR_WIDTH = 800;
//unsigned int const SCR_HEIGHT = 600;



// inline-shaders
// NOTICE HOW THEY ARE NULL TERMINATED
char const *vertexShaderSource = "#version 330 core\n" // for OpenGL 3.3+, the GLSL version matches (e.g. 330 or 460)
    "layout (location = 0) in vec3 aPos;\n" // here we declare a variable called aPos of glsl type vec3 as an INPUT VERTEX ATTRIBUTE and set location of input variable (match location of it in VAO)
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // gl_Position is a pre-defined output of vert shader 
    "}\0";
char const *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n" // frag shader only needs 1 output variable which we declare here
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";






int main(int argc, char const *argv[]) {
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit()) {
		std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
		return -1;
	}

	//Set the custom error callback function
	//Errors will be printed to the console
	glfwSetErrorCallback(errorCallback);

	// TARGET = OpenGL 3.3 CORE context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	GLFWvidmode const *mode = glfwGetVideoMode(monitor);

	GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", monitor, NULL); // creates the window in full-screen on the primary montitor (w/ proper DPI)
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//glfwSetWindowPos(window, 1, 31); // set window top-left corner to be at top-left corner of screen


	//So that we can access this object on key callbacks...
	//glfwSetWindowUserPointer(window, this);

	//Set the custom function that tracks key presses
	glfwSetKeyCallback(window, keyCallback);

	//Bring the new window to the foreground (not strictly necessary but convenient)
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//Intialize GLAD (finds appropriate OpenGL configuration for your system)
	/*
	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return -1;
	}
	*/


	std::cout << "OpenGL target version: " << GLVersion.major << "." << GLVersion.minor << "+" << std::endl;

	//Query and print out information about our OpenGL environment
	queryGLVersion();






	// build and compile our shader program
	// opengl must dynamically compile the shader at RUNTIME
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // returns an ID (>0) to reference the created empty shader object of specified type (use this object to maintain the source code strings that define the shader)
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // here we actually attach the shader source string to the shader object. we pass 1 string and NULL specifies that each string is null-terminated.
	// note: opengl copies the strings we pass in, so we can free our copies if we wish now
    glCompileShader(vertexShader); // actually compile the shader for use by the GPU
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // NOTE: the "iv" describes the returned parameters (vector of ints)
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shader objects (into a SHADER PROGRAM that we can use for rendering)
    int shaderProgram = glCreateProgram(); // create an empty SHADER PROGRAM OBJECT and return the ID
    glAttachShader(shaderProgram, vertexShader); // attach both compiled shaders into the shader program
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // link all the attached shaders together in 1 final shader program object (makes a pipeline where outputs of previous shaders get linked to inputs of succesive shaders) - can get linking errors here if the input/output names don't match
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader); // IMPORTANT: delete the raw shader program objects once they have been successfully linked into a final shader program object - this frees memory and invalidates the name/ID
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
	// NOTE: the winding here is not CCW, so I could change that, but since its 2D, it doesnt really matter right now
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;

	// NOTE: the initialization code should be done ONCE (unless object changes freqeuntly)
    glGenVertexArrays(1, &VAO); // generate 1 VAO and return the ID in VAO

    glGenBuffers(1, &VBO); // generate 1 buffer object name (unique ID?), use glDeleteBuffers() to return ID to pool
	// NOTE: ID=0 is reserved as a way to unbind/clean memory

    glGenBuffers(1, &EBO); // generate 1 EBO and return the ID
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindVertexArray(VAO); // tell OpenGL to use this VAO for future storage of VBO, EBO, glVertexAttribPointer(), glEnableVertexAttribArray() calls - so we only have 1 VAO bound at a time for use

	// NOTE: OpenGL uses symbolic names (enums) to represent actual bound objects to the target
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // the VBO ID (buffer object name) isn't associated with an actual buffer object (VBO) until bound here
	// NOTE: we can only bind upto 1 buffer of each type at a time. 
	// NOTE: now any calls made on GL_ARRAY_BUFFER target will affect currently bound buffer, VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copies previously defined vertex data into buffer's memory

	// 4th param is how we want the GPU to manage the data we provided, (if dynamic/stream then data will be allocated in memory that is faster to write to)
	// GL_STATIC_DRAW - data will most likely not change at all or very rarely
	// GL_DYNAMIC_DRAW - data will change alot
	// GL_STREAM_DRAW - data will change every time it is drawn


	// in the tut, so far... we sent the input vertex data to the GPU and specified how to process it with our shader program,
	// but we still have to tell OpenGL how to interpret the vertex data in memory and how it should connect the vertex data to the vertex shader's atttributes.


	// here we do the same thing as a VBBO, except we use the INDICES instead of verts
	// NOTE: since we are using a different bind target, the previous one isnt unbound
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// tell opengl how to interpret the vertex buffer data when future drawing calls are made (current interpretation stored in currently bound VAO)
	// params:
	// index - index of vertex attrb. (this 0 matches the location=0 in the shader)
	// size - 1,2,3 or 4 (no. of components in the attrb.)
	// type - component type (e.g. type of pos.x)
	// normalized - should the data be clamped within -1 to 1 (signed) or 0 to 1 (unsigned)
	// stride - byte offset between consecutive vertex attributes (e.g. v1 to v2), if you put 0 then it assumes tight-packing
	// pointer - offset to first compoentnt of first vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // each attrib. takes its data from memory managed by a VBO (in particular the VBO currently bound to GL_ARRAY_BUFFER) when calling this fucntion. So now vertex attribute 0 is associated with our VBO
    glEnableVertexAttribArray(0); // enable vertex attribute of index=0 (disabled by default)


	// AT THIS POINT WHAT HAVE WE DONE?
	// 1. init vert data in a buffer using a VBO
	// 2. setup VERT SHADER and FRAG SHADER
	// 3. told OpenGL how to LINK vert data to the VERTEX SHADER'S VERTEX ATTRIBS

	// now drawing an object would look something like this...
	/*
	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 1. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 2. use our shader program when we want to render an object
	glUseProgram(shaderProgram);
	// 3. now draw the object 
	someOpenGLFunctionThatDrawsOurTriangle();
	*/
	// The above would be tedious to repeat for every draw call, so instead we will use a VAO to store all state configs and we can then bind this object to restore state


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); // we use 0 to unbind the VBO from this target 

	// WARNING....
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


	// NOTE: now that the VAO has been unbound, I believe we could unbind the EBO here if we wish


    // uncomment this call to draw in wireframe polygons.
	// WIREFRAME MODE (show individual POLYS)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // param1 - apply to both front and back of all triangles, param2 - draw them as lines
	// this changes the state for all future draw calls until changed
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // use this to reset back to DEFAULT



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// input
		// -----
		//processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);




		// draw our first triangle
		glUseProgram(shaderProgram); // here we specify OpenGL to use this specific shader program for future SHADER/RENDERING CALLS
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		// specify the draw mode (e.g. poitns, lines, tris, quads, etc.), the starting index in buffer, and the count (number of entries from the start index - so number of verts to render)
		//glDrawArrays(GL_TRIANGLES, 0, 6); // this function draws primitives using the currently active shader, current attrb config and VBO data (bound within current VAO)
		
		// here we use draw elements instead of draw arrays since we are using INDEXED DRAWING (via an EBO - index buffer), takes the currently bound VBO (via VAO) and EBO (via VAO), read the indices in EBO to draw the verts in the VBO in specified order (thus EBO order matters, and VBO order does not matter?)
		// advantage: using only a VBO - duplicate verts and order matters so we always get same shape for same draw primtive mode
		//		      using BOTH a VBO and EBO - no duplicate vert data and VBO order does not matter (so its basically a mathematical SET) and we can use different EBOs over 1 VBO to use the same verts to draw different shapes
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // param1 is drawmode, param2 is number of verts to draw (1 square = 2 tris = 6 verts), param3 is the INDEX TYPE, param4 is the starting offset into the index array (or ptr to container if an EBO is not used)
		// glBindVertexArray(0); // no need to unbind it every time
		// NOTE: I guess we would unbind it if we had another VAO to bind, unless we would just bind the new VAO (which would unbind the previous?)





		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window); // swap the front and back buffers so the stuff we rendered to the back in this frame will get displayed in the front
		glfwPollEvents();
	}





	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);





	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
/*
void processInput(GLFWwindow *window) {
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}
*/

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


void queryGLVersion() {
	// query opengl version and renderer information
	std::string version = reinterpret_cast<char const *>(glGetString(GL_VERSION));
	std::string glslver = reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::string renderer = reinterpret_cast<char const *>(glGetString(GL_RENDERER));

	std::cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << std::endl;
}



void errorCallback(int error, char const *description) {
	std::cout << "GLFW ERROR " << error << ":" << std::endl;
	std::cout << description << std::endl;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	//Key codes are often prefixed with GLFW_KEY_ and can be found on the GLFW website
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (GLFW_KEY_1 == key && GLFW_PRESS == action) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (GLFW_KEY_2 == key && GLFW_PRESS == action) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (GLFW_KEY_3 == key && GLFW_PRESS == action) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}









/*
NOTES:

MODEL (object's local coord-space -> world space) 
VIEW (world space -> camera space)
PROJECTION (camera space -> screen space)

FRUSTUM CULLING - determine what objects are at least partially visible within frustum and send ONLY these to the GPU, so that unnecessary vertex processing is saved.

GLFW windows are double-buffered by default (front - one displayed and back - one we render to)


GRAPHICS PIPELINE:
part 1 = 3D coords -> 2D coords
part 2 = 2D coords -> colored pixels

each part of pipeline is very speicialized so its easy to p[arallize so GPUs have tons of cores to run shaders (small programs on GPU)
- some of the default shaders can be overriden (vertex (must), geometry (usually left alone), fragment shaders (must))

Vertex Data[] (collection of data per coordinate) -> vertex shader -> shape assembly -> geometry shader -> rasterization -> fragment shader -> tests and blending

we represent the vertex data using VERTEX ATTRIBUTES (can contain any data we want), most important a 3D position + Color value

- we must provide a PRIMITIVE ("hint") on DRAW CALLS to give context to OpenGL what we want to do with the vertex data (e.g. GL_POINTS, GL_TRIANGLES, GL_LINE_STRIP)


SINGLE vertex -> Vertex shader (raw 3d coord -> transformed 3d coord / basic vertex attrb. processing) 

PRIMITVE ASSEMBLY stage takes in all vertices (or vertex if GL_POINTS) from vertex shader that form a PRIMITIVE -> assembles points into a shape

GEOMETRY SHADER takes in this output (collection of verts that form a primitive) -> can generate other shapes by adding in new vertices (e.g. make 2 triangles out of an existing triangle)

RASTERIZATION takes in this output -> maps primitives to corresponding pixels on screen (creates at least 1 fragment (sample-sized segment of rasterized primitive) per pixel for every primitive)

CLIPPING is performed before frag shader (discard all fragments outside screen view to increase performance)

fragment = all data needed to render a single pizel (in OpenGL)

FRAGEMENT SHADER = calculate final color of a pixel (advanced effects occur here)
- usually contains/privided scene info (e.g. lights, shadows, light color, etc.)

ALPHA TEST / BLENDING STAGE = checks depth/stencil values of a fragment relative to other previously drawn fragments to determine if it should be discarded. also blends based on alpha

DEPTH...
depth (z-buffer) = depth is an increasing function of the distance between screen plane and the fragment to be drawn (farther away = higher depth value)
- we keep track of the depth of every fragment (in a depth buffer) so that when we come to a new fragment, we determine if it is occluded by a nearer fragment and just ignore it
glEnbale(GL_DEPTH_TEST) // enable depth buffer + automatic fragment testing
glClear(GL_COLOR_BUFFER_BIT | GL_DPETH_BUFFER_BIT) // make sure to clear the default zeroed depth buffer (black screen would appear due to no fragment being <0)
- max depth of 1.0f at the far clipping plane of frustum

STENCIL...
stencil buffer is a custom extension of depth buffer, in that we can give meaning to the values and when they change. We determine when to draw a fragment based on semantics of the value
e.g. we can provide a matrix of 0/1 that outline a cut-out of our scene to render where 1 overlays,


Normalized Device Coordinates (NDC) between -1.0f and 1.0f on all 3 axes
- only corrds between these constraints are visible, sod efine all coords like this
- -1.0 -1.0 is bottom left

set z=0 for "2d rendering"


all coords coming out of vertex shader should be in NDC


glViewport + provided data = NDC coords (-1 to 1) -> screen-space coords (0 to screen width/height) (via VIEWPORT TRANSFORM) and then gets transformed into fragments to input into frag shader



VERTEX DATA gets stored in GPU memory, we configure opengl how to interpret the memory and specify how to send data to gpu. We tell vertex shader how much data to process 
- MANAGE MEMORY vis VBOS (vertex buffer objects) - type of opengl object - store a large number of verts. Use these to send batches of vertex data to the gpu (SLOW)
- vertex shader can process gpu memory very fast
- like other opengl objects, a VBO has a unique ID


VBO holds actual vertex data (all vertex attributes like pos, norms, uvs, etc.)
VAO stores information on location of each attribute type?

VECTORS...
- if we have a vec4, then vec.w is called the PERSPECTIVE DIVISION (later on...)

- note: usually the input data to the vert shader is not in NDCs yet, so we have to process it a bit in the vert shader

SHADER TYPES...
Compute (4.3+)
Vertex
Tess-control
Tess-eval
Geometry
Fragment

COLORS...
- we use 32-bit RGBA where each channel is within 0.0f and 1.0f

LINKING VERTEX ATTRIBUTES...
- vertex buffer data: each vertex has data stored in sequence (e.g. v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, ...)
- each coord is 4bytes (32bit float) thus each vertex has a STRIDE of 12bytes
- the initial offset is 0 and padding between data is 0

VERTEX ARRAY OBJECT (VAO)...
- bound similar to a VBO
- future vertex attrb calls will be stored inside the VAO
- only need to config vertex attrib pointers ONCE and then when we want to draw the object, we bind the corresponding VAO
- a VAO stores... 
- 1. calls to glenablevertextattribarray() / gldisablevertexattribarray()
- 2. vertex attrb configs via glvertexattribpointer()
- 3. VBOs associated with vertex attrbs by calls to glvertexattribpointer()

- NOTE: a single VBO can contain different types of attributes, e.g. pos[0], col[0], pos[1], col[1], ...

- I think that a VAO contains 16 ATTRIBUTE POINTERS (most will be unused), e.g. VAO has attrb ptr 0 -> pos[0] -> pos[1] -> ... (in a VBO) && attrb ptr 1 -> col[0] -> col[1] -> ... (in same VBO)
- when we want to draw an object, we will bind the VAO with the proper settings

CREATION vs BINDING (former is to generate empty opengl object and return unique >0 unsigned int ID (object name) to reference it and latter is to tell opengl we want to work with an existing object to read/modify it)

***- DRAW PROCESS:
- when we have multiples objects to draw, first generate/config a VAO for each? (and thus the VBOs / attrib ptrs) and store the VAOS for later use
- when we want to draw a specific object, we bind the proper VAO, draw object and then unbind the VAO for safety
***
- I believe the above process is what the 453 boilerplate uses since it creates a VAO for every Geoemtry instance and then loops over the VAO member fields in the draw time

- NOTE: we could put every vert of every object in 1 VBO w/o a VAO, but that is rediculous to manage so we use multiples of each

- NOTE: opengl mainly works with triangles as polygons!

ELEMENT BUFFER OBJECT (EBO)...
- NOTE: we use these for INDEXING (index buffers) to prevent duplicate vert data being in a buffer (e.g. we want to draw a square so we only need 4 verts not 6)
***- we store the unique verts and then we specify the ORDER to draw them in (IIRC, we should use COUNTERCLOCKWISE WINDING for OUTWARD NORMALS OF A FACE - FRONT FACING) - this is can be used for FACE CULLING, but we can change the default of "front-facing" via glFrontFace() 
- so while a VBO will now store UNIQUE VERT DATA, our EBO will store INDEX DATA
-***WARNING: OPENGL INDEXING STARTS FROM 0! (RECALL THAT WAVEFRONT OBJS FILES EXPORTED FROM BLENDER START FROM 1)

***STANDARD - TRI POLYS (since triangles can make any n-gons) w/ CCW WINDING

- NOTE: glDRAWELEMENTS() takes its indices from the EBO currently bound to GL_ELEMENT_ARRAY_BUFFER target, so we would have to bind proper EBO everytime we want to render object w/ indices (this would be cumbersome), so...
- like VBOS, EBOS are also stored withing a VAO (VAO keeps track of element bugger object BINDINGS)
- the current-bound EBO while a VAO is bound gets stores as the VAO's EBO, so all we need to do is bind to proper VAO to auto-bind its EBO!

- PROCESS... 1. config VBO, 2. config EBO, 3. config VAO ??


- DATA STORAGE...
- its seems that a VAO STORES...
- 16 ATTRIBUTE POINTERS (each points to a different starting sttribute in a VBO (single VBO per VAO?)
- EBO pointer -> EBO (contains index data)
- NOTE: ACTUALLY, A VAO CAN STORE BINDING INFO FOR 1 EBO AND AT LEAST 16 VBOs
***WARNING!- a VAO stores the glBindBuffer() calls when the target is GL_ELEMENT_ARRAY_BUFFER (i.e. for an EBO), but it also stores UNBIND calls so DO NOT UNBIND AN EBO BEFORE UNBINDING THE VAO, o/w THE VAO WONT HAVE A CONFIGURED EBO!!!


-**** SUPER IMPORTANT NOTE:
- in the comments of Hello Triangle page, John24ssj says that if we set glfwSwapInterval(1) we can enable ~VSYNC (match framerate to monitor refresh rate) so that we dont get enormous fps for small demos and get GPU COIL WHINE!!! - this has happened to me before








later...
read the additonal resources pages on the site
work through the exercises on the pages on the site


later2...
homogenous coords (1 = point, 0 = vector ?)
barycentric coords
slerp

*/