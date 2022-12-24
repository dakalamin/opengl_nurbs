#include "Core/Nurbs.h"

#include <iostream>
#include <stdexcept>

const float NURBS::DEFAULT_STEP = 1.0f;
const char* NURBS::dim_char[2] = { "U", "V" };


NURBS::NURBS(size_t dimU, size_t dimV)
{
	setDim(U, dimU); setDegree(U);
	setDim(V, dimV); setDegree(V);
	
	controlPoints = std::vector<glm::vec3>(dim[U] * dim[V]);
	for (int v = 0; v < dim[V]; v++)
	{
		for (int u = 0; u < dim[U]; u++)
		{
			controlPoints[dim[U]*v + u] =
			{
				(1.f-dim[U]) / 2.f + u,
				(1.f-dim[V]) / 2.f + v,
				0.f
			};
		}
	}
}


void NURBS::setKnots(Dim d)
{
	size_t knotCount = dim[d] + getOrder(d);
	knots[d].resize(knotCount);

	float uniform_delta = 1.f / (knotCount - (clampKnots[d][START] + clampKnots[d][END]) * degree[d]);
	float stash = 0.f;

	for (int i = 0; i < knotCount; i++)
	{
		bool clamp[2] = { clampKnots[d][START], clampKnots[d][END] };
		clamp[START] *= i < degree[d];
		clamp[END]   *= i >= knotCount - degree[d];

		stash += (clamp[START] || clamp[END]) ? 0.f : uniform_delta;
		knots[d][i] = stash;
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
			controlPoints.insert(controlPoints.begin() + dim[U]*i + layer, newCP[i]);
		break;
	case V:
		controlPoints.insert(controlPoints.begin() + dim[U]*layer, newCP.begin(), newCP.end());
		break;
	}
}

std::vector<glm::vec3> NURBS::interpolateCP(Dim d, size_t layer)
{
	std::vector<glm::vec3> cp(dim[reverseDim(d)], glm::vec3(0.f));
	size_t pos;

	for (int i = 0; i < dim[reverseDim(d)]; i++)
	{
		float step = 0.f;

		switch (d)
		{
		case U:
			pos = dim[U] * i + layer;

			if (layer > 0)
				cp[i] += controlPoints[pos - 1];
			else step -= DEFAULT_STEP;
			if (layer < dim[U])
				cp[i] += controlPoints[pos];
			else step += DEFAULT_STEP;

			cp[i].x += step; break;
		case V:
			pos = dim[U] * layer + i;

			if (layer > 0)
				cp[i] += controlPoints[pos - dim[U]];
			else step -= DEFAULT_STEP;
			if (layer < dim[V])
				cp[i] += controlPoints[pos];
			else step += DEFAULT_STEP;

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
			glm::vec3 cp = controlPoints[dim[U]*v + u];
			std::cout << cp.x << ' ' << cp.y << ' ' << cp.z << '\t';
		}
		std::cout << '\n';
	}
}