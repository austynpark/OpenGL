#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>

class Shape;
class Plane;
class Triangle;
class Sphere;
class Aabb;
struct Ray;
/*
typedef struct Ray {
	glm::vec3 Q;
	glm::vec3 D;

	__forceinline glm::vec3 eval(float t) const {
		return Q + t * D;
	};
} Ray;

/*
* Storage of a ray intersection with any Shape object
*
* @param t		Parameter value on ray of the point of intersection
* @param object A pointer to the Shape intersected
* @param P		Point of intersection (in world coordinates)
* @param N		Normal of surface at interesection point (in world coordinates)
*/
typedef struct Intersection {

	float t;
	Shape* object;
	glm::vec3 P;
	glm::vec3 N;
	// other 2D or 3D texture coord

	float distance() const { return t; }

} Intersection;

class Shape {
public:
	Shape() {};
	virtual ~Shape() = 0 {};
	
	virtual bool Intersect(const glm::vec3& point) = 0;
	virtual bool Intersect(const Ray& ray, Intersection& intersect) = 0;
};

class Plane : Shape{
public:
	Plane(const glm::vec4& n) : norm(n) {};
	~Plane() override {};

	bool Intersect(const glm::vec3& point) override;
	bool Intersect(const Ray& ray, Intersection& intersect) override;
	bool Intersect(const Triangle& triangle);
	bool Intersect(const Aabb& aabb);

	// (normal.x, normal.y, normal.z, d) ax+by+cz+d = 0
	glm::vec4 norm;
};

class Triangle : Shape {
public:
	Triangle(
		const glm::vec3& v0,
		const glm::vec3& v1,
		const glm::vec3& v2
	) {
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	};

	~Triangle() override {};

	bool Intersect(const glm::vec3& point) override;
	bool Intersect(const Ray& ray, Intersection& intersect) override;
	bool Intersect(const Plane& plane);

	glm::vec3 v[3];
};

class Sphere : Shape {
public:
	Sphere(glm::vec3 position, float radius) : pos(position), r(radius) {};
	~Sphere() override {};


	bool Intersect(const glm::vec3& point) override;
	bool Intersect(const Ray& ray, Intersection& intersect) override;
	bool Intersect(const Aabb& aabb);
	bool Intersect(const Sphere& sphere);
	


	glm::vec3 pos;
	float r;
};

class Aabb : Shape {
public:
	Aabb(const glm::vec3& _min, const glm::vec3& _max) : min(_min), max(_max) {};
	~Aabb() override {};

	glm::vec3 min;
	glm::vec3 max;

	bool Intersect(const glm::vec3& point) override;
	bool Intersect(const Ray& ray, Intersection& intersect) override;
	bool Intersect(const Aabb& aabb);
	bool Intersect(const Sphere& sphere);

	// glm::vec3 center;
	// glm::vec3 half_extents;
};

struct Ray {
	glm::vec3 pos; // Ps
	glm::vec3 dir; // Dr

	__forceinline glm::vec3 eval(float t) const {
		return pos + t * dir; // Pr(t)
	};

};

#endif // !INTERSECTION_H
