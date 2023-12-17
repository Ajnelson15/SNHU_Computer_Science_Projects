
//Andrew Nelson
//CS330_Comp_Graphics_Final_Project

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "meshes.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "Shape Meshes Example"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nIndices;    // Number of indices of the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;
	//Texture ID
	GLuint gTextureBookFCover;
	GLuint gTexturePages;
	GLuint gTextureTable;
	GLuint gTextureCanPlain;
	GLuint gTextureCan;
	GLuint gTextureCanLid;
	GLuint gTextureCanLid2;
	GLuint gTextureRed;
	GLuint gTexturePlastic;
	GLuint gTextureBackCover;
	GLuint gTextureBinding;
	// Shader program
	GLuint gProgramId;

	//Shape Meshes from Professor Brian
	Meshes meshes;

	// camera
	Camera gCamera(glm::vec3(-1.5f, 5.0f, 4.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	//Used to determine camera perspective
	bool gCameraPerspective = true;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;

}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);

// Surface Vertex Shader Source Code

const GLchar* vertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 vertexPosition; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 vertexNormal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexFragmentNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	//Uniform / Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0f); // Transforms vertices into clip coordinates

	vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

	vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal; // get normal vectors in world space only and exclude normal translation properties
	vertexTextureCoordinate = textureCoordinate;
}
);

// Surface Fragment Shader Source Code
const GLchar* fragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentNormal; // For incoming normals
	in vec3 vertexFragmentPos; // For incoming fragment position
	in vec2 vertexTextureCoordinate;

	out vec4 fragmentColor; // For outgoing cube color to the GPU

	// Uniform / Global variables for object color, light color, light position, and camera/view position
	uniform vec4 objectColor;
	uniform vec3 ambientColor;
	uniform vec3 light1Color;
	uniform vec3 light1Direction;
	uniform vec3 light2Color;
	uniform vec3 light2Position;
	uniform vec3 light3Color;
	uniform vec3 light3Position;
	uniform vec3 light4Color;
	uniform vec3 light4Position;
	uniform vec3 light5Color;
	uniform vec3 light5Position;
	uniform float constant;
	uniform float linear;
	uniform float quadratic;
	uniform float attentuation;
	uniform vec3 viewPosition;
	uniform sampler2D uTexture; // Useful when working with multiple textures
	uniform bool ubHasTexture;
	uniform float ambientStrength = 0.1f; // Set ambient or global lighting strength
	uniform float specularIntensity1 = 0.8f;
	uniform float highlightSize1 = 16.0f;
	uniform float specularIntensity2 = 0.8f;
	uniform float highlightSize2 = 16.0f;
	uniform float specularIntensity3 = 0.8f;
	uniform float highlightSize3 = 16.0f;
	uniform float specularIntensity4 = 0.8f;
	uniform float highlightSize4 = 16.0f;
	uniform float specularIntensity5 = 0.8f;
	uniform float highlightSize5 = 16.0f;

void main()
{
	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	//Calculate Ambient lighting
	vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color
	vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit
	
//---------------------------------------------------------------------------------------------------------------------- Directional Light 1

	//Light 1 Diffuse 
	vec3 light1Direc = normalize(-light1Direction); // Calculate light direction from the light source
	float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse1 = impact1 * light1Color; // Generate diffuse light color
	
	//Light 1 Specular
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
	vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector
	float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize1);
	vec3 specular1 = specularIntensity1 * specularComponent1 * light1Color;

//---------------------------------------------------------------------------------------------------------------------- Point Light 2

	//Light 2 Diffuse
	vec3 light2Direction = normalize(light2Position - vertexFragmentPos); 
	float impact2 = max(dot(norm, light2Direction), 0.0);
	vec3 diffuse2 = impact2 * light2Color; 

	//Light 2 Specular
	vec3 reflectDir2 = reflect(-light2Direction, norm);
	float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
	vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;
	float distance = length(light2Position - vertexFragmentPos);
	float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

//---------------------------------------------------------------------------------------------------------------------- Point Light 3

	//Light 3 Diffuse
	vec3 light3Direction = normalize(light3Position - vertexFragmentPos); 
	float impact3 = max(dot(norm, light3Direction), 0.0);
	vec3 diffuse3 = impact3 * light3Color; 

	//Light 3 Specular
	vec3 reflectDir3 = reflect(-light3Direction, norm);
	float specularComponent3 = pow(max(dot(viewDir, reflectDir3), 0.0), highlightSize3);
	vec3 specular3 = specularIntensity3 * specularComponent3 * light3Color;

//---------------------------------------------------------------------------------------------------------------------- Point Light 4

	//Light 4 Diffuse
	vec3 light4Direction = normalize(light4Position - vertexFragmentPos); 
	float impact4 = max(dot(norm, light4Direction), 0.0);
	vec3 diffuse4 = impact4 * light3Color;

	//Light 4 Specular
	vec3 reflectDir4 = reflect(-light4Direction, norm);
	float specularComponent4 = pow(max(dot(viewDir, reflectDir4), 0.0), highlightSize4);
	vec3 specular4 = specularIntensity4 * specularComponent4 * light4Color;

//---------------------------------------------------------------------------------------------------------------------- Point Light 5

	//Light 5 Diffuse
	vec3 light5Direction = normalize(light5Position - vertexFragmentPos);
	float impact5 = max(dot(norm, light5Direction), 0.0);
	vec3 diffuse5 = impact5 * light5Color;

	//Light 5 Specular
	vec3 reflectDir5 = reflect(-light5Direction, norm);
	float specularComponent5 = pow(max(dot(viewDir, reflectDir5), 0.0), highlightSize5);
	vec3 specular5 = specularIntensity5 * specularComponent5 * light5Color;


	//**Calculate phong result**
	//Texture holds the color to be used for all three components
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate);
	vec3 phong1;
	vec3 phong2;
	vec3 phong3;
	vec3 phong4;
	vec3 phong5;

	if (ubHasTexture == true)
	{
		phong1 = ((ambient * attenuation) + (diffuse1 * attenuation) + (specular1 * attenuation)) * textureColor.xyz;
		phong2 = ((ambient * attenuation) + (diffuse2 * attenuation) + (specular2 * attenuation)) * textureColor.xyz;
		phong3 = ((ambient * attenuation) + (diffuse3 * attenuation) + (specular3 * attenuation)) * textureColor.xyz;
		phong4 = ((ambient * attenuation) + (diffuse4 * attenuation) + (specular4 * attenuation)) * textureColor.xyz;
		phong5 = ((ambient * attenuation) + (diffuse5 * attenuation) + (specular5 * attenuation)) * textureColor.xyz;
	}
	else
	{
		phong1 = ((ambient * attenuation) + (diffuse1 * attenuation) + (specular1 * attenuation)) * objectColor.xyz;
		phong2 = ((ambient * attenuation) + (diffuse2 * attenuation) + (specular2 * attenuation)) * objectColor.xyz;
		phong3 = ((ambient * attenuation) + (diffuse3 * attenuation) + (specular3 * attenuation)) * objectColor.xyz;
		phong4 = ((ambient * attenuation) + (diffuse4 * attenuation) + (specular4 * attenuation)) * objectColor.xyz;
		phong5 = ((ambient * attenuation) + (diffuse5 * attenuation) + (specular5 * attenuation)) * objectColor.xyz;
	}

	fragmentColor = vec4(phong1 + phong2 + phong3 + phong4 + phong5, 1.0); // Send lighting results to GPU

}
);

void flipImage(unsigned char* image, int width, int height, int channels) {
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}


int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	//UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object
	meshes.CreateMeshes();

	// Create the shader program
	if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
		return EXIT_FAILURE;

//-------------------------------------------------------------------------------------- Load textures

	const char* texFilename = "textures/colorful.jpg";

	if (!UCreateTexture(texFilename, gTextureBookFCover))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "textures/pages.jpeg";

	if (!UCreateTexture(texFilename, gTexturePages))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "textures/wood.jpg";

	if (!UCreateTexture(texFilename, gTextureTable))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/can.jpg";

	if (!UCreateTexture(texFilename, gTextureCan))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/plaincan.jpg";

	if (!UCreateTexture(texFilename, gTextureCanPlain))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/red.jpg";

	if (!UCreateTexture(texFilename, gTextureRed))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/plastic.jpg";

	if (!UCreateTexture(texFilename, gTexturePlastic))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/backcover.jpg";

	if (!UCreateTexture(texFilename, gTextureBackCover))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/binding.jpg";

	if (!UCreateTexture(texFilename, gTextureBinding))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/canlid.jpg";

	if (!UCreateTexture(texFilename, gTextureCanLid))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	texFilename = "textures/canlid2.jpg";

	if (!UCreateTexture(texFilename, gTextureCanLid2))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	//Bind Textures on corresponding texture units in uTexture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureBookFCover);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gTexturePages);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTextureTable);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gTextureCan);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gTextureCanPlain);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, gTextureRed);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, gTexturePlastic);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, gTextureBackCover);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, gTextureBinding);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, gTextureCanLid);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, gTextureCanLid2);

	// Sets the background color of the window to black (it will be implicitely used by glClear)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(gWindow))
	{
		//Per-frame timing

		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// Processes User Input
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	//UDestroyMesh(gMesh);
	meshes.DestroyMeshes();

	// Release shader program
	UDestroyShaderProgram(gProgramId);

	//Releases textures
	UDestroyTexture(gTextureBookFCover);
	UDestroyTexture(gTexturePages);
	UDestroyTexture(gTextureTable);
	UDestroyTexture(gTextureCan);
	UDestroyTexture(gTextureCanLid);
	UDestroyTexture(gTextureCanLid2);
	UDestroyTexture(gTextureCanPlain);
	UDestroyTexture(gTexturePlastic);
	UDestroyTexture(gTextureRed);
	UDestroyTexture(gTextureBinding);
	UDestroyTexture(gTextureBackCover);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: Window Creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);

	//Checks for mouse and keyboard input
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetKeyCallback(*window, key_callback);
	
	// Tell GLFW to capture our mouse cursor
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW: initialize

	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

//Key call back for user input to toggle orthographic and perspective camera views
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double xpos, ypos;
	// O = Ortho View
	if (key == GLFW_KEY_O)
		switch (action)
		{
		case GLFW_PRESS:
			if (gCameraPerspective) {
				gCameraPerspective = false;

			}

		}

	// P = Perspective view
	if (key == GLFW_KEY_P)
		switch (action)
		{
		case GLFW_PRESS:
			if (!gCameraPerspective) {
				gCameraPerspective = true;
			}

		}
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}


// Functioned called to render a frame
void URender()
{
	// Initialize variables
	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;
	GLint viewPosLoc;
	GLint ambStrLoc;
	GLint ambColLoc;
	GLint light1ColLoc;
	GLint light1DirLoc;
	GLint light2ColLoc;
	GLint light2PosLoc;
	GLint light3ColLoc;
	GLint light3PosLoc;
	GLint light4ColLoc;
	GLint light4PosLoc;
	GLint light5ColLoc;
	GLint light5PosLoc;
	GLint constantLoc;
	GLint linearLoc;
	GLint quadraticLoc;
	GLint objColLoc;
	GLint specInt1Loc;
	GLint highlghtSz1Loc;
	GLint specInt2Loc;
	GLint highlghtSz2Loc;
	GLint specInt3Loc;
	GLint highlghtSz3Loc;
	GLint specInt4Loc;
	GLint highlghtSz4Loc;
	GLint specInt5Loc;
	GLint highlghtSz5Loc;
	GLint uHasTextureLoc;
	bool ubHasTextureVal;
	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 translation;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.83f, 0.79f, 0.74f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera/view transformation
	if (gCameraPerspective) {
		//Sets camera to Persective View
		view = gCamera.GetViewMatrix();
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	}									
	else {
		// Sets camera at fixed Front Orthographic View
		Camera OrthoCamera = Camera(-1.5f, 0.0f, 4.0, -0.122788f, 0.707107f, -0.696364f, -100, 0.0f);
		view = OrthoCamera.GetViewMatrix();
		projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.1f, 100.0f);
	}

	// Set the shader to be used
	glUseProgram(gProgramId);

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	viewPosLoc = glGetUniformLocation(gProgramId, "viewPosition");
	ambStrLoc = glGetUniformLocation(gProgramId, "ambientStrength");
	ambColLoc = glGetUniformLocation(gProgramId, "ambientColor");
	light1ColLoc = glGetUniformLocation(gProgramId, "light1Color");
	light1DirLoc = glGetUniformLocation(gProgramId, "light1Direction");
	light2ColLoc = glGetUniformLocation(gProgramId, "light2Color");
	light2PosLoc = glGetUniformLocation(gProgramId, "light2Position");
	light3ColLoc = glGetUniformLocation(gProgramId, "light3Color");
	light3PosLoc = glGetUniformLocation(gProgramId, "light3Position");
	light4ColLoc = glGetUniformLocation(gProgramId, "light4Color");
	light4PosLoc = glGetUniformLocation(gProgramId, "light4Position");
	light5ColLoc = glGetUniformLocation(gProgramId, "light5Color");
	light5PosLoc = glGetUniformLocation(gProgramId, "light5Position");
	objColLoc = glGetUniformLocation(gProgramId, "objectColor");
	specInt1Loc = glGetUniformLocation(gProgramId, "specularIntensity1");
	highlghtSz1Loc = glGetUniformLocation(gProgramId, "highlightSize1");
	specInt2Loc = glGetUniformLocation(gProgramId, "specularIntensity2");
	highlghtSz2Loc = glGetUniformLocation(gProgramId, "highlightSize2");
	specInt3Loc = glGetUniformLocation(gProgramId, "specularIntensity3");
	highlghtSz3Loc = glGetUniformLocation(gProgramId, "highlightSize3");
	specInt4Loc = glGetUniformLocation(gProgramId, "specularIntensity4");
	highlghtSz4Loc = glGetUniformLocation(gProgramId, "highlightSize4");
	specInt5Loc = glGetUniformLocation(gProgramId, "specularIntensity5");
	highlghtSz5Loc = glGetUniformLocation(gProgramId, "highlightSize5");
	uHasTextureLoc = glGetUniformLocation(gProgramId, "ubHasTexture");
	constantLoc = glGetUniformLocation(gProgramId, "constant");
	linearLoc = glGetUniformLocation(gProgramId, "linear");
	quadraticLoc = glGetUniformLocation(gProgramId, "qaudratic");

	// Loads view and projection matrices
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

//-------------------------------------------------------------------------------------------Lighting Setup

	//Set Camera View Location
	glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);

	//Set Ambient Light Strength
	glUniform1f(ambStrLoc, 0.1f);

	//Set Ambient Color
	glUniform3f(ambColLoc, 1.0f, 1.0f, 1.0f);

	//Light 1 (Direction Light)
	glUniform3f(light1ColLoc, 0.1f, 0.1f, 0.1f);			//Set Light Color
	glUniform3f(light1DirLoc, 0.0f, 2.0f, 0.0f);			//Set Light location
	glUniform1f(specInt1Loc, 0.1f);							//Set Light specular intensity
	glUniform1f(highlghtSz1Loc, 0.1f);						//Set Light specular highlight size

	//Light 2 (Point Light Top Red)
	glUniform3f(light2ColLoc, 0.72f, 0.36f, 0.27f);			//Set Light Color 
	glUniform3f(light2PosLoc, -2.252f, 0.75f, 0.5f);		//Set Light location 
	glUniform1f(specInt2Loc, 1.0f);							//Set Light specular intensity
	glUniform1f(highlghtSz2Loc,10.0f);						//Set Light specular highlight size
	glUniform1f(constantLoc, 1.0f);							//Set constant value
	glUniform1f(linearLoc, 0.07f);							//Set linear value
	glUniform1f(quadraticLoc, 1.8);							//Set quadratic value

	////Light 3 (Point Light Left Side)
	glUniform3f(light3ColLoc, 0.452f, 0.59f, 0.5762f);		//Set Light Color
	glUniform3f(light3PosLoc, -8.0f, 4.0f, 0.0f);			//Set Light location
	glUniform1f(specInt3Loc, 0.1f);							//Set Light specular intensity
	glUniform1f(highlghtSz3Loc, 1.0f);						//Set Light specular highlight size
	glUniform1f(constantLoc, 1.0f);							//Set constant value
	glUniform1f(linearLoc, 0.007f);							//Set linear value
	glUniform1f(quadraticLoc, 0.0017);						//Set quadratic value

	//Light 5 (Point Light Front)
	glUniform3f(light5ColLoc, 0.452f, 0.59f,0.5762f);		//Set Light Color
	glUniform3f(light5PosLoc, -1.5f, 1.0f, 5.0f);			//Set Light location
	glUniform1f(specInt5Loc, 0.1f);							//Set Light specular intensity
	glUniform1f(highlghtSz5Loc, 20.0f);						//Set Light specular highlight size
	glUniform1f(constantLoc, 1.0f);							//Set constant value
	glUniform1f(linearLoc, 0.007f);							//Set linear value
	glUniform1f(quadraticLoc, 0.0017);						//Set quadratic value

	//Light 4 (Point Light Right Side)
	glUniform3f(light4ColLoc, 0.452f, 0.59f, 0.5762f);		//Set Light Color
	glUniform3f(light4PosLoc, 4.0f, 4.0f, -2.0f);			//Set Light location
	glUniform1f(specInt4Loc, 0.1f);							//Set Light specular intensity
	glUniform1f(highlghtSz4Loc, 1.0f);						//Set Light specular highlight size
	glUniform1f(constantLoc, 1.0f);							//Set constant value
	glUniform1f(linearLoc, 0.007f);							//Set linear value
	glUniform1f(quadraticLoc, 0.0017);						//Set quadratic value

//------------------------------------------------------------------------------------------ Plane for 3D Scene	
	
	// Enables textures to be used in shader
	ubHasTextureVal = true;
	glUniform1i(uHasTextureLoc, ubHasTextureVal);

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPlaneMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(4.0f, 1.0f, 4.0f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(60.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.2f, 0.0f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 2);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	//Unbinds the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//------------------------------------------------------------------------------------------- Back Book Cover

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.05f, 2.5f, 2.0f));

	// 2. Rotate the object
	rotation = glm::rotate(2.2f, glm::vec3(0.0, 1.0f, 0.0f));

	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.282f, 1.2495f, -0.496f));

	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 7);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	//Unbinds the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//----------------------------------------------------------------------------------------- Pages of the book

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.4f, 2.2f, 1.8f));

	// 2. Rotate the object
	rotation = glm::rotate(2.2f, glm::vec3(0.0, 1.0f, 0.0f));

	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.25f, 1.3f, -0.3f));

	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 1);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	//Unbinds the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	//----------------------------------------------------------------------------------------- Front Cover of Book

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.05f, 2.5f, 2.0f));
	// 2. Rotate the object
	rotation = glm::rotate(2.195f, glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.042f, 1.25f, -0.1615f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	//Unbinds the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	//------------------------------------------------------------------------------------------ Book Binder

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.05f, 2.5f, 0.455f));
	// 2. Rotate the object
	rotation = glm::rotate(0.62f, glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.952f, 1.249f, 0.2442f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 8);
	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	//Unbinds the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//---------------------------------------------------------------------------------------------- Can Body

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.6f, 2.5f, 0.6f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(-35.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.0f, 0.0f, -0.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 3);

	// Draws the triangles sides
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 4);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//---------------------------------------------------------------------------------------------- Can Flat Top
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5999f, 0.1f, 0.5999f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(-35.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.0f, 2.41f, -0.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the bottom triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 9);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0); 

//---------------------------------------------------------------------------------------Dumbbell Square 1
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.45f, 0.82f, 0.7f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(100.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.2f, 0.41f, 1.1035f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 6);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//-------------------------------------------------------------------------------------- Dumbbell Triangle 1
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPrismMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.82f, 0.7f, 0.2f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(190.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.2564f, 0.4103f, 0.7832f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;

	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPrismMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//-------------------------------------------------------------------------------------- Dumbbell Triangle 2
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPrismMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.82f, 0.7f, 0.25f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation,glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.139f, 0.4105f, 1.448f)); //-0.915f, 0.291f, 1.5185f
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPrismMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//--------------------------------------------------------------------------------------- Dumbbell Handle

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gCylinderMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.1f, 1.5f, 0.1f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-1.4f, 0.41f, 1.14f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//---------------------------------------------------------------------------------------Dumbbell Square 2

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gBoxMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.45f, 0.82f, 0.7f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(100.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.0f, 0.41f, 1.4225f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//-------------------------------------------------------------------------------------- Dumbbell Triangle 3
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPrismMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.82f, 0.7f, 0.2f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(190.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-3.0564f, 0.4103f, 1.1022f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPrismMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//-------------------------------------------------------------------------------------- Dumbbell Triangle 4

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPrismMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.82f, 0.7f, 0.25f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.939f, 0.4105f, 1.767f)); //-0.915f, 0.291f, 1.5185f
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	//Loads current model for rendering
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPrismMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

//-------------------------------------------------------------------------------------------------- Wooden Block

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(meshes.gPrismMesh.vao);

	// 1. Scales the object
	scale = glm::scale(glm::vec3(0.5f, 0.5f, 1.0f));
	// 2. Rotate the object
	rotation = glm::rotate(glm::radians(140.0f), glm::vec3(0.0, 1.0f, 0.0f));
	// 3. Position the object
	translation = glm::translate(glm::vec3(-2.1f, 0.25f, 0.2f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//Loads current model for rendering
	glProgramUniform4f(gProgramId, objColLoc, 0.0f, 0.0f, 0.5f, 1.0f);

	//Loads current texture for rendering
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 5);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPrismMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrieve the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImage(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}