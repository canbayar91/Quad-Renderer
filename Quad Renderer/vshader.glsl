#version 150

in vec4 vPosition;
in vec4 vColor;
// in vec3 vNormal;

out vec4 color;

uniform mat4 projection;
uniform vec4 location;
uniform vec4 center;
uniform vec3 theta;

void main() {

	// Output color is the sum of all three components
	color = vColor;

	// Compute sine and cosine of angle theta
	vec3 angles = radians(theta);
	vec3 cosValue = cos(angles);
	vec3 sinValue = sin(angles);

	// Matrix that defines translation to according to the center of mesh
	mat4 center = mat4(1.0, 0.0, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							-center.x, -center.y, -center.z, 1.0);

	// Matrix that defines rotation about x-axis
	mat4 rotationX = mat4(1.0, 0.0, 0.0, 0.0,
						  0.0, cosValue.x, -sinValue.x, 0.0,
					      0.0, sinValue.x, cosValue.x, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	// Matrix that defines rotation about y-axis
	mat4 rotationY = mat4(cosValue.y, 0.0, sinValue.y, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  -sinValue.y, 0.0, cosValue.y, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	// Matrix that defines rotation about z-axis
	mat4 rotationZ = mat4(cosValue.z, -sinValue.z, 0.0, 0.0,
						  sinValue.z, cosValue.z, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0);

	// Matrix that defines translation
	mat4 translation = mat4(1.0, 0.0, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0,
							0.0, 0.0, 1.0, 0.0,
							location.x, location.y, location.z, 1.0);

	// Computation of vertex position
	gl_Position = projection * translation * rotationX * rotationY * rotationZ * center * vPosition;
}
