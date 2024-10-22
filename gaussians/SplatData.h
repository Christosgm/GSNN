#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "../utils/Progress.h"
class SplatData
{
	public:
		SplatData(const std::string &path);
		bool loadFromPLY(const std::string &path);

		const glm::vec3 &getCenter() const;
		int getNumberOfGaussians() const;

		const std::vector<glm::vec4> &getPositions() const;
		const std::vector<glm::vec4> &getColors() const;
		const std::vector<glm::vec4> &getScales() const;
		const std::vector<glm::vec4> &getRotations() const;
		const std::vector<float> &getOpacities() const; 
		const std::vector<float> &getCovariances3D() const;

	protected:
		glm::vec3 _center;
		int _numberOfGaussians;

		std::vector<glm::vec4> _positions;
		std::vector<glm::vec4> _colors;
		std::vector<glm::vec4> _scales;
		std::vector<glm::vec4> _rotations;
		std::vector<float> _opacities;
		std::vector<int> _ply_indexes;
		std::vector<float> _covariances3D;
};
