#include "Core/Nurbs.h"

#include <iostream>
#include <stdexcept>

const float NURBS::DEFAULT_STEP = 1.0f;
const char* NURBS::dim_char[2]  = { "U", "V" };


NURBS::NURBS(size_t dimU, size_t dimV)
{
	setDim(U, dimU); setDegree(U);
	setDim(V, dimV); setDegree(V);
	
	controlPoints = std::vector<glm::vec3>(dim[U] * dim[V]);
	for (size_t i = 0; i < controlPoints.size(); i++)
		controlPoints[i] = glm::vec3
		(
			(1.f - dim[U]) / 2.f + index2uv(i, U),
			(1.f - dim[V]) / 2.f + index2uv(i, V),
			0.f
		);
}

glm::vec3 NURBS::calculateCenter()
{
	glm::vec3 min, max;
	min = max = controlPoints[0];

	for (auto& cp : controlPoints)
	{
		for (int i = 0; i < 3; i++)
		{
			if (cp[i] < min[i]) min[i] = cp[i];
			if (cp[i] > max[i]) max[i] = cp[i];
		}
	}

	return (max + min) / 2.f;
}


void NURBS::setKnots(Dim d)
{
	size_t knotCount = dim[d] + degree[d];
	knots[d].resize(knotCount+1);

	float uniform_delta = 1.f / (knotCount - (clampKnots[d][START]+clampKnots[d][END]) * degree[d]);
	float stash = 0.f;

	for (int i = 0; i < knots[d].size(); i++)
	{
		bool clamp = (clampKnots[d][START] && i <  degree[d]) ||
		             (clampKnots[d][END]   && i >= knotCount - degree[d]);

		knots[d][i] = stash;
		stash += (clamp) ? 0.f : uniform_delta;
	}
}

void NURBS::setDegree(Dim d, size_t value)
{
	degree[d] = std::clamp(value, MIN_DEGREE, getMaxDegree(d));
	setKnots(d);
}


void NURBS::setDim(Dim d, size_t value)
{
	if (value < MIN_DIM || value > MAX_DIM)
		throw std::invalid_argument
		("NURBS::setDim: Invalid dimension value");

	dim[d] = value;
	setDegree(d, degree[d]);
}

void NURBS::removeDim(Dim d, size_t layer)
{
	if (layer < 0 || layer > dim[d])
		throw std::invalid_argument
		("NURBS::removeDim: place is out of range.");

	setDim(d, dim[d] - 1);
	switch (d)
	{
	case U:
		for (int i = 0; i < dim[V]; i++)
			controlPoints.erase(controlPoints.begin() + uv2index(layer, i));
		break;
	case V:
		cp_t::iterator start = controlPoints.begin() + uv2index(0, layer);
		controlPoints.erase(start, start+dim[U]);
		break;
	}
}

void NURBS::insertDim(Dim d, size_t layer, std::vector<glm::vec3> newCP)
{
	if (newCP.size() != dim[reverseDim(d)])
		throw std::invalid_argument
		("NURBS::insertDim: newCP size doesn't equal to the number of control points in the other dimension.");
	if (layer < 0 || layer > dim[d])
		throw std::invalid_argument
		("NURBS::insertDim: place is out of range.");

	setDim(d, dim[d] + 1);

	controlPoints.reserve(dim[U] * dim[V]);
	switch (d)
	{
	case U:
		for (int i = 0; i < dim[V]; i++)
			controlPoints.emplace(controlPoints.begin() + uv2index(layer, i), newCP[i]);
		break;
	case V:
		controlPoints.insert(controlPoints.begin() + uv2index(0, layer),
			newCP.begin(), newCP.end());
		break;
	}
}

std::vector<glm::vec3> NURBS::interpolateCP(Dim d, size_t layer)
{
	std::vector<glm::vec3> cp(dim[reverseDim(d)], glm::vec3(0.f));
	for (size_t i = 0; i < cp.size(); i++)
	{
		float step = 0.f;
		switch (d)
		{
		case U:
			if (layer > 0)      cp[i] += controlPoints[uv2index(layer-1, i)];
			else                step  -= DEFAULT_STEP;
			if (layer < dim[U]) cp[i] += controlPoints[uv2index(layer, i)];
			else                step  += DEFAULT_STEP;

			cp[i].x += step; break;
		case V:
			if (layer > 0)      cp[i] += controlPoints[uv2index(i, layer-1)];
			else                step  -= DEFAULT_STEP;
			if (layer < dim[V]) cp[i] += controlPoints[uv2index(i, layer)];
			else                step  += DEFAULT_STEP;

			cp[i].y += step; break;
		}
		if (!step) cp[i] /= 2.f;
	}
	return cp;
}


void NURBS::output()
{
	for (int v = 0; v < dim[V]; v++)
	{
		for (int u = 0; u < dim[U]; u++)
		{
			glm::vec3 cp = controlPoints[uv2index(u, v)];
			std::cout << cp.x << ' ' << cp.y << ' ' << cp.z << '\t';
		}
		std::cout << '\n';
	}
}