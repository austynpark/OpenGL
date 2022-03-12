#include "Intersection.h"

#include <algorithm>

constexpr unsigned int point_size = 3;
constexpr float EPSILON = 0.00001F;
using namespace glm;

/*
* Sphere Intersection
*/
bool Sphere::Intersect(const glm::vec3& point)
{
	float d = glm::dot(point - pos, point - pos);
	float r_sq = r * r;


	return d * d <= r_sq;
}

bool Sphere::Intersect(const Ray& ray, Intersection& intersect)
{
	vec3 center_to_origin = ray.dir - pos;

	const float b = dot(center_to_origin, ray.dir);
	// since a = dot(D, D) = 1
	const float discriminant_sqrt = sqrtf(b * b - dot(center_to_origin, center_to_origin) + r * r);

	if (-b - discriminant_sqrt > EPSILON) {
		intersect.t = -b - discriminant_sqrt;
	}
	else if (-b + discriminant_sqrt > EPSILON) {
		intersect.t = -b + discriminant_sqrt;
	}
	else {
		return false;
	}

	//intersect.P = ray.eval(intersect.t);
	//intersect.N = normalize(intersect.P - C);
	//intersect.object = this;

	return true;
}

bool Sphere::Intersect(const Aabb& aabb)
{
	vec3 closest_point;

	for (unsigned i = 0; i < point_size; ++i) {
		closest_point[i] = std::max(aabb.min[i], std::min(pos[i], aabb.max[i]));
	}

	float dist = dot(closest_point - pos, closest_point - pos);

	return dist < (r * r);
}

bool Sphere::Intersect(const Sphere& sphere)
{
	Sphere combined_sphere(pos, r + sphere.r);

	return combined_sphere.Intersect(sphere.pos);
}

/*
* AABB Intersection
*/
bool Aabb::Intersect(const glm::vec3& point)
{
	for (unsigned i = 0; i < point_size; ++i) {
		if ((point[i] < min[i]) || (point[i] > max[i])) {
			return false;
		}
	}

	return true;
}

bool Aabb::Intersect(const Ray& ray, Intersection& intersect)
{
	float tmin = (min.x - ray.pos.x) / ray.dir.x;
	float tmax = (max.x - ray.pos.x) / ray.dir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (min.y - ray.pos.y) / ray.dir.y;
	float tymax = (max.y - ray.pos.y) / ray.dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - ray.pos.z) / ray.dir.z;
	float tzmax = (max.z - ray.pos.z) / ray.dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

bool Aabb::Intersect(const Aabb& aabb)
{
	for (unsigned i = 0; i < point_size; ++i) {
		if (max[i] < aabb.min[i] || aabb.max[i] < min[i])
			return false;
	}

	return true;
}

bool Aabb::Intersect(const Sphere& sphere)
{
	vec3 closest_point;

	for (unsigned i = 0; i < point_size; ++i) {
		closest_point[i] = std::max(min[i], std::min(sphere.pos[i], max[i]));
	}

	float dist = dot(closest_point - sphere.pos, closest_point - sphere.pos);

	return dist < (sphere.r* sphere.r);
}

/*
* Plane Intersection
*/
bool Plane::Intersect(const glm::vec3& point)
{
	float ndotp = dot(glm::vec3(norm), point);

	ndotp -= norm.w;

	return ndotp < EPSILON;
}

bool Plane::Intersect(const Ray& ray, Intersection& intersect)
{
	float t = -(glm::dot(glm::vec3(norm), ray.pos) + norm.w) / glm::dot(glm::vec3(norm), ray.dir);

	if (t < EPSILON) {
		return false;
	}

	intersect.t = t;

	return true;
}

bool Plane::Intersect(const Triangle& triangle)
{
	
	for (unsigned i = 0; i < point_size; ++i)
	{
		float dist_to_plane = abs(dot(glm::vec3(norm), triangle.v[i]) - norm.w) / glm::length(glm::vec3(norm));

		if (dist_to_plane > EPSILON)
			return false;
	}

	return false;
}

bool Plane::Intersect(const Aabb& aabb)
{
	vec3 center = (aabb.max + aabb.min) * 0.5f; // Compute AABB center
	vec3 extents = aabb.max - center; // Compute positive extents

	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	float r = extents[0] * abs(norm[0]) + extents[1] * abs(norm[1]) + extents[2] * abs(norm[2]);

	// Compute distance of box center from plane
	float s = dot(vec3(norm), center) - norm.w;

	// Intersection occurs when distance s falls within [-r,+r] interval
	return abs(s) <= r;
}

/*
* Triangle Intersection
*/
bool Triangle::Intersect(const glm::vec3& point)
{
	float area_triangle = 1 / glm::length(glm::cross(v[1] - v[0], v[2] - v[0]));

	float a = glm::length(cross(v[1] - point, v[2] - point)) * area_triangle;
	float b = glm::length(cross(v[2] - point, v[0] - point)) * area_triangle;
	float c = 1 - a - b;

	vec3 barycentric = glm::vec3(a, b, c);

	if (a + b + c - 1 > EPSILON)
		return false;

	if (a < 0 || a > 1 ||
		b < 0 || b > 1 ||
		c < 0 || c > 1)
		return false;

	return false;
}

bool Triangle::Intersect(const Ray& ray, Intersection& intersect)
{
	/*
	glm::vec3 plane_norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
	float d = -glm::dot(plane_norm, v1);

	Plane plane(glm::vec4(plane_norm, d));

	if (!plane.Intersect(ray, intersect)) {
		return false;
	}
	*/

	vec3 E1 = v[1] - v[0];
	vec3 E2 = v[2] - v[0];
	vec3 p = cross(ray.dir, E2);
	float d = dot(p, E1);

	// NO INTERSECTION (RAY IS PARALLEL TO TRIANGLE)
	if (d == EPSILON) {
		return false;
	}

	vec3 S = ray.pos - v[0];
	float u = dot(p, S) / d;

	// RAY INTERSECTS PLANE, BUT OUTSIDE E2 edge
	if ((u < EPSILON) || (u > 1)) {
		return false;
	}

	vec3 q = cross(S, E1);
	float v = dot(ray.dir, q) / d;

	// RAY INTERSECTS PLANE, BUT OUTSIDE OTHER EDGES
	if ((v < EPSILON) || (u + v) > 1) {
		return false;
	}

	float t = dot(E2, q) / d;

	// RAY'S NEGATIVE HALF INTERSECTS TRIANGLE
	if (t < EPSILON) {
		return false;
	}

	intersect.t = t;
	//intersection.P = ray.eval(t);

	//TODO: If vertex normals are known: (1 - u - v)N0 + uN1 + vN2 and else
	//intersection.N = normalize(cross(E1, E2));
	//intersection.object = this;

	return true;
}

bool Triangle::Intersect(const Plane& plane)
{
	for (unsigned i = 0; i < point_size; ++i)
	{
		float dist_to_plane = abs(dot(glm::vec3(plane.norm), v[i]) - plane.norm.w) / glm::length(glm::vec3(plane.norm));

		if (dist_to_plane > EPSILON)
			return false;
	}

	return true;
}
