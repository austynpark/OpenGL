/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: Object contains model name and position, scale, rotation, color. In the rendering loop-
model file is loaded by the model name in Object class and pass uniform variables
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: austyn.park_CS300_1
Author: austyn.park / kyungook.park / 180001621
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#include "Object.h"

namespace OG 
{
	Object* OBJECT = nullptr;

	Object::Object(const std::string& fileName)
	{
		OBJECT = this;
		position_ = glm::vec3(0.0f);
		rotation_axis_ = glm::vec3(0.0f);
		scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
		name_ = fileName;
	}

	Object* Object::CreateObject(const std::string& fileName, glm::vec3 position, glm::vec3 scale,
		glm::vec3 color, glm::vec3 rotation_axis)
	{
		Object* obj = new Object(fileName);
		obj->position_ = position;
		obj->scale_ = scale;
		obj->color_ = color;
		obj->rotation_axis_ = rotation_axis;
		obj->name_ = fileName;

		return obj;
	}

	const std::string& Object::getName() const
	{
		return name_;
	}

	const glm::vec3& Object::getPosition() const
	{
		return position_;
	}

	const glm::vec3& Object::getScale() const
	{
		return scale_;
	}

	const glm::vec3& Object::getRotation() const
	{
		return rotation_axis_;
	}

	const glm::vec3& Object::getColor() const
	{
		return color_;
	}

	void Object::setPosition(const glm::vec3& pos)
	{
		position_ = pos;
	}

	void Object::setRotation(const glm::vec3& rot)
	{
		rotation_axis_ = rot;
	}

	void Object::setScale(const glm::vec3& s)
	{
		scale_ = s;
	}

	void Object::setColor(const glm::vec3& col)
	{
		color_ = col;
	}
	void Object::setName(const char* name)
	{
		name_ = name;
	}
}