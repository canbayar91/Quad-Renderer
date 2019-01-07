#include "Quadrilateral.h"
#include "MeshReader.h"

#include <algorithm>

using namespace Angel;

#define EPSILON 0.00001

size_t vertex_count;
size_t quad_count;

GLfloat zoom_multiplier = 1.6f;

GLfloat scale = 1.0f;
GLfloat angles[3] = {0.0f, 0.0f, 0.0f};
point4 location(0.0f, 0.0f, 0.0f, 1.0f);

GLfloat x_max = -100.0f;
GLfloat x_min = 100.0f;
GLfloat y_max = -100.0f;
GLfloat y_min = 100.0f;
GLfloat z_max = -100.0f;
GLfloat z_min = 100.0f;

point4 center;
GLfloat radius;

GLfloat projection_handle;
GLfloat angle_handle;
GLfloat scale_handle;
GLfloat position_handle;

const float MIN_VALUE = -1.0f;
const float MAX_VALUE = -1.0f;

bool useStandardDiagonals = true;

std::string mesh_file;
std::string stats_file;

std::vector<Quadrilateral> quadrilateralList;

void calculate_bounding_box() {

	// Calculate the middle point of the mesh
	center = point4((x_max + x_min) / 2.0f, (y_max + y_min) / 2.0f, (z_max + z_min) / 2.0f, 1.0f);

	// Calculate the radius of the mesh on the bigger side
	radius = abs(x_max - center.x);
	if (abs(x_min - center.x) > radius) radius = abs(x_min - center.x);
	if (abs(y_max - center.y) > radius) radius = abs(y_max - center.y);
	if (abs(y_min - center.y) > radius) radius = abs(y_min - center.y);
	if (abs(z_max - center.z) > radius) radius = abs(z_max - center.z);
	if (abs(z_min - center.z) > radius) radius = abs(z_min - center.z);
}

void reset_options() {

	// Reset the rotation
	angles[0] = 0.0f;
	angles[1] = 0.0f;
	angles[2] = 0.0f;

	// Reset the scale
	scale = 1.0f;

	// Reset the position
	location = point4(0.0f, 0.0f, 0.0f, 1.0f);

	// Reset the bounding box
	x_max = -100.0f;
	x_min = 100.0f;
	y_max = -100.0f;
	y_min = 100.0f;
	z_max = -100.0f;
	z_min = 100.0f;
}

void initialize() {

	// Read the mesh from input file
	quadrilateralList = MeshReader::getInstance()->readMesh(mesh_file);

	// Get the quadrilateral and vertex count in the mesh
	quad_count = quadrilateralList.size();
	vertex_count = quad_count * 6;

	// Read the statistics from the input file
	std::vector<float> statisticsList = MeshReader::getInstance()->readStatistics(stats_file, quad_count);
	
	// Get the minimum and maximum metric values in the statistics
	float minimumValue = *std::min_element(statisticsList.begin(), statisticsList.end());
	float maximumValue = *std::max_element(statisticsList.begin(), statisticsList.end());

	// Calculate the normalization factor of the gradient function
	float range = maximumValue - minimumValue;
	float normalizationFactor = 1.0f / range;

	// Read the point and normal list from the mesh
	point4* points = new point4[vertex_count];
	color4* colors = new color4[vertex_count];
	// vec3* normals = new vec3[vertex_count];
	for (size_t i = 0; i < quad_count; i++) {

		// Get the current quadrilateral from the list
		Quadrilateral quadrilateral = quadrilateralList[i];

		// Add the vertices for the first triangle depending on which diagonal is used
		if (useStandardDiagonals) {
			points[i * 6] = quadrilateral.getVertexA();
			points[i * 6 + 1] = quadrilateral.getVertexB();
			points[i * 6 + 2] = quadrilateral.getVertexD();
		} else {
			points[i * 6] = quadrilateral.getVertexA();
			points[i * 6 + 1] = quadrilateral.getVertexB();
			points[i * 6 + 2] = quadrilateral.getVertexC();
		}

		// Add the vertices for the second triangle
		if (useStandardDiagonals) {
			points[i * 6 + 3] = quadrilateral.getVertexC();
			points[i * 6 + 4] = quadrilateral.getVertexD();
			points[i * 6 + 5] = quadrilateral.getVertexB();
		} else {
			points[i * 6 + 3] = quadrilateral.getVertexD();
			points[i * 6 + 4] = quadrilateral.getVertexA();
			points[i * 6 + 5] = quadrilateral.getVertexC();
		}

		// Calculate the color of the quadrilateral by normalizing the metric values to range between 0 and 1
		float colorValue = statisticsList[i] * normalizationFactor;
		color4 currentColor = color4(1.0f - colorValue, colorValue, 0.0f, 1.0f);

		// Store the color information on the corresponding quadrilateral
		quadrilateralList[i].setColor(currentColor);

		// Add the colors for the first triangle
		colors[i * 6] = currentColor;
		colors[i * 6 + 1] = currentColor;
		colors[i * 6 + 2] = currentColor;

		// Add the colors for the first triangle
		colors[i * 6 + 3] = currentColor;
		colors[i * 6 + 4] = currentColor;
		colors[i * 6 + 5] = currentColor;

		// Add the normals for the first triangle
		// normals[i * 6] = quadrilateral.getNormalA();
		// normals[i * 6 + 1] = quadrilateral.getNormalA();
		// normals[i * 6 + 2] = quadrilateral.getNormalA();

		// Add the normals for the second triangle
		// normals[i * 6 + 3] = quadrilateral.getNormalC();
		// normals[i * 6 + 4] = quadrilateral.getNormalC();
		// normals[i * 6 + 5] = quadrilateral.getNormalC();
	}

	// Pick 100 random points and find the approximate bounding box of the object
	for (unsigned int i = 0; i < 100; i++) {

		// Pick a random vertex
		int index = rand() % vertex_count;
		point4 current = points[index];

		// Decide the maximum and minimum values of x
		if (current.x > x_max) {
			x_max = current.x;
		} else if (current.x < x_min) {
			x_min = current.x;
		}

		// Decide the maximum and minimum values of y
		if (current.y > y_max) {
			y_max = current.y;
		} else if (current.y < y_min) {
			y_min = current.y;
		}

		// Decide the maximum and minimum values of z
		if (current.z > z_max) {
			z_max = current.z;
		} else if (current.z < z_min) {
			z_min = current.z;
		}
	}

	// Find the center and the radius of the bounding box
	calculate_bounding_box();

	// Create and bind a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Calculate the buffer size needed to store all vertices
	unsigned int buffer_size = sizeof(point4) * vertex_count;

	// Create a buffer object in GPU and place the data into the buffer
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// glBufferData(GL_ARRAY_BUFFER, buffer_size + sizeof(vec3) * vertex_count, NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 2 * buffer_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, points);
	glBufferSubData(GL_ARRAY_BUFFER, buffer_size, buffer_size, colors);
	// glBufferSubData(GL_ARRAY_BUFFER, buffer_size, sizeof(vec3) * vertex_count, normals);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint location = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint color = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(color);
	glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(buffer_size));

	// Initialize the normal attribute from the vertex shader
	// GLuint normal = glGetAttribLocation(program, "vNormal");
	// glEnableVertexAttribArray(normal);
	// glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(buffer_size));

	// Generate random material color
	// float red = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX));
	// float green = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX));
	// float blue = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX));
	// color4 material(red, green, blue, 1.0);

	// Specify the position of light source
	// point4 light_position(0.0, 0.0, -20.0, 0.0);

	// Specify the components of light source
	// color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	// color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	// color4 light_specular(1.0, 1.0, 1.0, 1.0);

	// Specify the shininess coefficient for specular reflectivity
	// float material_shininess = 100.0;

	// Calculate the products of lighting components
	// color4 ambient_product = light_ambient * material;
	// color4 diffuse_product = light_diffuse * material;
	// color4 specular_product = light_specular * material;

	// Get the light values to the shader
	// glUniform4fv(glGetUniformLocation(program, "ambientProduct"), 1, ambient_product);
	// glUniform4fv(glGetUniformLocation(program, "diffuseProduct"), 1, diffuse_product);
	// glUniform4fv(glGetUniformLocation(program, "specularProduct"), 1, specular_product);
	// glUniform4fv(glGetUniformLocation(program, "lightPosition"), 1, light_position);
	// glUniform1f(glGetUniformLocation(program, "shininess"), material_shininess);

	// Get the uniform variables from the shader
	projection_handle = glGetUniformLocation(program, "projection");
	angle_handle = glGetUniformLocation(program, "theta");
	scale_handle = glGetUniformLocation(program, "scale");
	position_handle = glGetUniformLocation(program, "location");

	// Enable z-buffer algorithm for hidden surface removal
	glEnable(GL_DEPTH_TEST);

	// Use flat shading
	glShadeModel(GL_FLAT);

	// White background
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Release the memory
	delete[] points;
	delete[] colors;
	// delete[] normals;
}

void display() {

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load current angle and position to the vertex shader
	glUniform3fv(angle_handle, 1, angles);
	glUniform1f(scale_handle, scale);
	glUniform4fv(position_handle, 1, location);

	// Calculate the zoom amount
	GLfloat zoom = radius * zoom_multiplier;

	// Set the projection matrix in shaders
	mat4 projection_matrix = Ortho(center.x - zoom, center.x + zoom, center.y - zoom, center.y + zoom, center.z - zoom, center.z + zoom);
	glUniformMatrix4fv(projection_handle, 1, GL_TRUE, projection_matrix);

	// Render the points in GPU
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	// Double buffering
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {

	// Keyboard mapping
	switch (key) {
	case 'x':
		location.x += radius / 100;
		break;
	case 'X':
		location.x -= radius / 100;
		break;
	case 'y':
		location.y += radius / 100;
		break;
	case 'Y':
		location.y -= radius / 100;
		break;
	case 'z':
		location.z += radius / 100;
		break;
	case 'Z':
		location.z -= radius / 100;
		break;
	case 'c':
		useStandardDiagonals = !useStandardDiagonals;
		initialize();
		break;
	case 'r':
		reset_options();
		break;
	}

	// Render the scene again
	glutPostRedisplay();
}

void searchQuadrilateral(float greenValue) {

	// Iterate through the quadrilateral list
	for (size_t i = 0; i < quadrilateralList.size(); i++) {

		// Get the current quadrilateral from the list
		Quadrilateral quadrilateral = quadrilateralList[i];

		// Check if the quadrilateral color matches with the value read
		if (abs(quadrilateral.getColor().y - greenValue) <= EPSILON) {
			std::cout << "Index of the selected quadrilateral: " << i << std::endl;
			quadrilateral.outputCoordinates();
		}
	}
}

void mouse(int button, int state, int x, int y) {

	// When left mouse is clicked
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		// Read the color of the pixel
		float pixel[4];
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);

		// Output the pixel values for test
		std::cout << "R: " << pixel[0] << std::endl;
		std::cout << "G: " << pixel[1] << std::endl;
		std::cout << "B: " << pixel[2] << std::endl;

		// Search the quadrilateral list for selected element (skip white areas)
		if ((int) pixel[0] == 0.0f) {
			searchQuadrilateral(pixel[1]);
		}
	}
}

void mouseWheel(int wheel, int direction, int x, int y) {

	// Zoom in/out
	if (direction == GLUT_UP) {
		zoom_multiplier -= 0.2f;
	} else {
		zoom_multiplier += 0.2f;
	}

	// Render the scene again
	glutPostRedisplay();
}

void arrow(int key, int x, int y) {

	// Arrow keys
	switch (key) {
	case GLUT_KEY_UP:
		angles[0] += 2.0f;
		break;
	case GLUT_KEY_DOWN:
		angles[0] -= 2.0f;
		break;
	case GLUT_KEY_LEFT:
		angles[1] -= 2.0f;
		break;
	case GLUT_KEY_RIGHT:
		angles[1] += 2.0f;
		break;
	}

	// Render the scene again
	glutPostRedisplay();
}

int main(int argc, char **argv) {

	// Read the file name from command line
	// First input is the input quad mesh file (off format)
	// Second input is the statistics file for the mesh (there shall be a single value for each quad)
	mesh_file = argv[1];
	stats_file = argv[2];

	// Initialize window using GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);

	// The lines below are used to check the version in case freeglut is used
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Create the window
	glutCreateWindow("Mesh Renderer");

	// Initialize GLEW
	glewInit();

	// Initialize the application with the first mesh
	initialize();

	// Create a display function callback
	glutDisplayFunc(display);

	// Create a keyboard event listener
	glutKeyboardFunc(keyboard);

	// Create a mouse event listener
	glutMouseFunc(mouse);

	// Create a mouse wheel event listener
	glutMouseWheelFunc(mouseWheel);

	// Create a special event listener for arrow keys
	glutSpecialFunc(arrow);

	// Keep the window open
	glutMainLoop();

	return 0;
}