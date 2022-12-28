#pragma once

#include <algorithm>
#include <vector>
#include "glm/glm.hpp"


class NURBS
{
public:
	using cp_t = std::vector<glm::vec3>;
public:
	enum Dim : int { U, V };
	static const char* dim_char[2];

	size_t dim[2];
	static const float DEFAULT_STEP;
	static const size_t
		DEFAULT_DIM = 3,
		MIN_DIM = 2,
		MAX_DIM = 20;

	size_t degree[2];
	static const size_t
		DEFAULT_DEGREE = 2,
	    MIN_DEGREE = 1,
		MAX_DEGREE = 7;

	enum Pos : int { START, END };
	
	bool clampKnots[2][2] = { {1, 1}, {1, 1} };
	std::vector<float> knots[2];
	cp_t controlPoints;
	
public:
	NURBS(size_t dimU, size_t dimV);
	inline NURBS(size_t dimUV=DEFAULT_DIM) : NURBS(dimUV, dimUV) {}

	inline size_t index2uv(size_t i, Dim d)
	{ return (d == U) ? i % dim[U] : i / dim[U]; }
	inline size_t uv2index(size_t u, size_t v)
	{ return v * dim[U] + u; }

	glm::vec3 calculateCenter();

	inline static constexpr Dim reverseDim(Dim dim) { return (dim == U) ? V : U; }
	void setDim(Dim d, size_t value);
	void removeDim(Dim d, size_t layer);
	void insertDim(Dim d, size_t layer, std::vector<glm::vec3> newCP);
	inline void insertDim(Dim d, size_t layer) { insertDim(d, layer, interpolateCP(d, layer)); }
	inline void insertDim(Dim d)               { insertDim(d, dim[d]); }

	inline size_t getOrder(Dim d)     { return degree[d] + 1; }
	inline size_t getMaxDegree(Dim d) { return std::min<size_t>(MAX_DEGREE, dim[d] - 1); }
	void setDegree(Dim d, size_t value = DEFAULT_DEGREE);

	void setKnots(Dim d);

	std::vector<glm::vec3> interpolateCP(Dim d, size_t layer);

	void output();
};