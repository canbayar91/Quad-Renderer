#include "Quadrilateral.h"

Quadrilateral::Quadrilateral() {
	this->a = point4(0.0, 0.0, 0.0, 0.0);
	this->b = point4(0.0, 0.0, 0.0, 0.0);
	this->c = point4(0.0, 0.0, 0.0, 0.0);
	this->d = point4(0.0, 0.0, 0.0, 0.0);
}

Quadrilateral::Quadrilateral(const point4 a, const point4 b, const point4 c, const point4 d) {
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}

void Quadrilateral::setColor(const color4 color) {
	this->color = color;
}

const point4 Quadrilateral::getVertexA() {
	return a;
}

const point4 Quadrilateral::getVertexB() {
	return b;
}

const point4 Quadrilateral::getVertexC() {
	return c;
}

const point4 Quadrilateral::getVertexD() {
	return d;
}

const vec3 Quadrilateral::getNormalA() {
	const vec4 u = a - d;
	const vec4 v = a - b;
	return normalize(cross(u, v));
}

const vec3 Quadrilateral::getNormalB() {
	const vec4 u = b - a;
	const vec4 v = b - c;
	return normalize(cross(u, v));
}

const vec3 Quadrilateral::getNormalC() {
	const vec4 u = c - b;
	const vec4 v = c - d;
	return normalize(cross(u, v));
}

const vec3 Quadrilateral::getNormalD() {
	const vec4 u = d - c;
	const vec4 v = d - a;
	return normalize(cross(u, v));
}

const color4 Quadrilateral::getColor() {
	return color;
}

void Quadrilateral::outputCoordinates() {
	std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ") - ";
	std::cout << "(" << b.x << ", " << b.y << ", " << b.z << ") - ";
	std::cout << "(" << c.x << ", " << c.y << ", " << c.z << ") - ";
	std::cout << "(" << d.x << ", " << d.y << ", " << d.z << ")" << std::endl;
}
