#pragma once

#include <memory>
#include <vector>

namespace OG {
	class Texture;
	class Model;
	class Shader;

	class CubeMap {
	public:
		CubeMap();
		~CubeMap();
		bool Init(const char** faces_file_path, int first_tex_Num);
		void Render(const glm::mat4& view, const glm::mat4& projection);
	private:
		std::vector<std::unique_ptr<Texture>> faces;
		std::unique_ptr<Model> skybox;
		std::unique_ptr<Shader> shader;
	};

}
