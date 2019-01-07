#ifndef QUADRILATERAL
#define QUADRILATERAL

#include "Angel.h"

using namespace Angel;

typedef vec4 point4;
typedef vec4 color4;

class Quadrilateral {
public:

	Quadrilateral();
	Quadrilateral(const point4 a, const point4 b, const point4 c, const point4 d);

	void setColor(const color4 color);

	const point4 getVertexA();
	const point4 getVertexB();
	const point4 getVertexC();
	const point4 getVertexD();

	const vec3 getNormalA();
	const vec3 getNormalB();
	const vec3 getNormalC();
	const vec3 getNormalD();

	const color4 getColor();

	void outputCoordinates();

private:

	point4 a;
	point4 b;
	point4 c;
	point4 d;

	color4 color;

};

#endif
