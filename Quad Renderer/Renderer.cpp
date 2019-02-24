#include "Quadrilateral.h"
#include "MeshReader.h"

#include <algorithm>
#include <limits>

using namespace Angel;

#define EPSILON 0.00001

const GLfloat FLOAT_MAX = std::numeric_limits<float>::max();
const GLfloat FLOAT_MIN = -FLOAT_MAX;

size_t vertex_count;
size_t quad_count;

GLfloat zoom_multiplier = 1.6f;

GLfloat angles[3] = {0.0f, 0.0f, 0.0f};
point4 location(0.0f, 0.0f, 0.0f, 1.0f);

GLfloat x_max = FLOAT_MIN;
GLfloat x_min = FLOAT_MAX;
GLfloat y_max = FLOAT_MIN;
GLfloat y_min = FLOAT_MAX;
GLfloat z_max = FLOAT_MIN;
GLfloat z_min = FLOAT_MAX;

point4 center;
GLfloat radius;

GLfloat projection_handle;
GLfloat angle_handle;
GLfloat position_handle;
GLfloat center_handle;

std::string mesh_file;
std::string stats_file;

std::vector<Quadrilateral> quadrilateralList;

void update_minmax(point4 vertex) {

	// Decide the maximum value of x
	if (vertex.x > x_max) {
		x_max = vertex.x;
	}

	// Decide the minimum value of x
	if (vertex.x < x_min) {
		x_min = vertex.x;
	}

	// Decide the maximum value of y
	if (vertex.y > y_max) {
		y_max = vertex.y;
	}

	// Decide the minimum value of y
	if (vertex.y < y_min) {
		y_min = vertex.y;
	}

	// Decide the maximum value of z
	if (vertex.z > z_max) {
		z_max = vertex.z;
	}

	// Decide the minimum value of z
	if (vertex.z < z_min) {
		z_min = vertex.z;
	}

}

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

void reset() {

	// Reset the rotation
	angles[0] = 0.0f;
	angles[1] = 0.0f;
	angles[2] = 0.0f;

	// Reset the position
	location = point4(0.0f, 0.0f, 0.0f, 1.0f);

	// Reset the bounding box
	x_max = FLOAT_MIN;
	x_min = FLOAT_MAX;
	y_max = FLOAT_MIN;
	y_min = FLOAT_MAX;
	z_max = FLOAT_MIN;
	z_min = FLOAT_MAX;
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

		// Add the vertices for the first triangle
		points[i * 6] = quadrilateral.getVertexA();
		points[i * 6 + 1] = quadrilateral.getVertexB();
		points[i * 6 + 2] = quadrilateral.getVertexD();

		// Add the vertices for the second triangle
		points[i * 6 + 3] = quadrilateral.getVertexC();
		points[i * 6 + 4] = quadrilateral.getVertexD();
		points[i * 6 + 5] = quadrilateral.getVertexB();

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
	}

	// Pick 100 random points and find the approximate bounding box of the object
	for (unsigned int i = 0; i < 100; i++) {

		// Pick a random vertex
		int index = rand() % vertex_count;
		point4 current = points[index];

		// Update minimum and maximum locations according to the point
		update_minmax(current);
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
	glBufferData(GL_ARRAY_BUFFER, 2 * buffer_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, points);
	glBufferSubData(GL_ARRAY_BUFFER, buffer_size, buffer_size, colors);

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

	// Get the uniform variables from the shader
	projection_handle = glGetUniformLocation(program, "projection");
	angle_handle = glGetUniformLocation(program, "theta");
	position_handle = glGetUniformLocation(program, "location");
	center_handle = glGetUniformLocation(program, "center");

	// Enable z-buffer algorithm for hidden surface removal
	glEnable(GL_DEPTH_TEST);

	// Use flat shading
	glShadeModel(GL_FLAT);

	// White background
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Release the memory
	delete[] points;
	delete[] colors;
}

void display() {

	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load current angle and position to the vertex shader
	glUniform3fv(angle_handle, 1, angles);
	glUniform4fv(position_handle, 1, location);
	glUniform4fv(center_handle, 1, center);

	// Calculate the zoom amount
	GLfloat zoom = radius * zoom_multiplier;

	// Set the projection matrix in shaders
	mat4 projection_matrix = Ortho(-zoom, zoom, -zoom, zoom, zoom, -zoom);
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
	case 'r':
		reset();
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
		angles[0] -= 2.0f;
		break;
	case GLUT_KEY_DOWN:
		angles[0] += 2.0f;
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