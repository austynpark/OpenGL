/* Start Header -------------------------------------------------------
File Name: ogpch.h
Purpose: Precompiled Header.
Language: C++
Platform: Windows x64 / OpenGL 4.5
Project: OGEngine
Author: austyn.park / kyungook.park
Creation date: 2021/09/29
End Header --------------------------------------------------------*/
#ifndef __OGPCH_H
#define __OGPCH_H

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>

#include <algorithm>
#include <memory>

#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <chrono>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

using std::cout;
using std::endl;
#endif // __OGPCH_H