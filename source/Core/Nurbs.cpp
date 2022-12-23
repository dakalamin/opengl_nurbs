#include "Core/Nurbs.h"

#include <iostream>
#include <stdexcept>


const float NURBS::DEFAULT_STEP = 1.0f;


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


void NURBS::setKnots(Dim d, bool clampStart, bool clampEnd)
{
	size_t knotCount = dim[d] + getOrder(d);
	knots[d].resize(knotCount);

	float uniform_delta = 1.f / (knotCount - (clampStart + clampEnd) * degree[d]);
	float stash = 0.f;

	for (int i = 0; i < knotCount; i++)
	{
		if ( !((clampStart && i < degree[d]) || (clampEnd && i >= knotCount-degree[d])) )
			stash += uniform_delta;

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

void NURBS::insertDim(Dim d, size_t place, std::vector<glm::vec3> newCP)
{
	if (newCP.size() != dim[reverseDim(d)])
		throw std::invalid_argument
		("NURBS::insertDim: newCP size doesn't equal to the number of control points in the other dimension.");
	if (place < 0 || place > dim[d])
		throw std::invalid_argument
		("NURBS::insertDim: place is out of range.");

	setDim(d, dim[d] + 1);

	controlPoints.reserve(dim[U] * dim[V]);
	switch (d)
	{
	case U:
		for (int i = 0; i < dim[V]; i++)
			controlPoints.insert(controlPoints.begin() + dim[U]*i + place, newCP[i]);
		break;
	case V:
		controlPoints.insert(controlPoints.begin() + dim[U]*place, newCP.begin(), newCP.end());
		break;
	}
}
void NURBS::insertDim(Dim d, size_t place)
{
	std::vector<glm::vec3> newCP(dim[reverseDim(d)], glm::vec3(0.f));
	size_t pos;

	for (int i = 0; i < dim[reverseDim(d)]; i++)
	{
		float step = 0.f;
		
		switch (d)
		{
		case U:
			pos = dim[U]*i + place;
			
			if (place > 0)
				newCP[i] += controlPoints[pos - 1];
			else step -= DEFAULT_STEP;
			if (place < dim[U])
				newCP[i] += controlPoints[pos];
			else step += DEFAULT_STEP;

			newCP[i].x += step; break;
		case V:
			pos = dim[U]*place + i;

			if (place > 0)
				newCP[i] += controlPoints[pos - dim[U]];
			else step -= DEFAULT_STEP;
			if (place < dim[V])
				newCP[i] += controlPoints[pos];
			else step += DEFAULT_STEP;

			newCP[i].y += step; break;
		}
		
		if (!step) newCP[i] /= 2.f;
	}
	insertDim(d, place, newCP);
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