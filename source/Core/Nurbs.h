#pragma once

#include <algorithm>
#include <vector>

#include "glm/glm.hpp"


class NURBS
{
public:
	enum Dim : int { U, V };

	static const float  DEFAULT_STEP;
	static const size_t DEFAULT_DIM = 3;
	static const size_t MIN_DIM = 2;
	static const size_t MAX_DIM = 20;
	size_t dim[2];

	static const size_t DEFAULT_DEGREE = 3;
	static const size_t MIN_DEGREE = 1;
	static const size_t MAX_DEGREE = 10;
	size_t degree[2];
	
	std::vector<float> knots[2];
	std::vector<glm::vec3> controlPoints;
	
public:
	NURBS(size_t dimU, size_t dimV);
	inline NURBS(size_t dimUV=DEFAULT_DIM) : NURBS(dimUV, dimUV) {}

	inline static constexpr Dim reverseDim(Dim dim) { return dim == U ? V : U; }
	
	void setKnots(Dim d, bool clampStart=false, bool clampEnd=false);
	
	inline size_t getOrder(Dim d)     { return degree[d] + 1; }
	inline size_t getMaxDegree(Dim d) { return std::min<size_t>(MAX_DEGREE, dim[d]-1); }
	void setDegree(Dim d, size_t value=DEFAULT_DEGREE);

	void setDim(Dim d, size_t value);

	void insertDim(Dim d, size_t place, std::vector<glm::vec3> newCP);
	void insertDim(Dim d, size_t place);
	inline void insertDim(Dim d) { insertDim(d, dim[d]); }

	void output();
};