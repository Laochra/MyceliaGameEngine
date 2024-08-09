#include "LineDrawer.h"

#include "TimeManager.h"

#include "glad.h"

#include "Camera.h"

#include "Debug.h"

LineDrawer::~LineDrawer() noexcept
{
	if (initialised)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &colourBuffer);
	}
}

void LineDrawer::Draw() noexcept
{
	if (!initialised) Initialise();

	glLineWidth((float)lineWidth);

	int i = 0;
	while (i < lifetimes.size())
	{
		if (lifetimes[i] == -FLT_MAX)
		{
			i += 2;
			continue;
		}

		if (lifetimes[i] <= 0.0f)
		{
			positions.erase(positions.begin() + i, positions.begin() + i + 1);
			colours.erase(colours.begin() + i, colours.begin() + i + 1);
			lifetimes.erase(lifetimes.begin() + i, lifetimes.begin() + i + 1);
			continue;
		}

		lifetimes[i] -= Time::delta;
		int nextI = i + 1;
		lifetimes[nextI] -= Time::delta;
		i += 2;
		continue;
	}

	// Bind Vertex Array Object
	glBindVertexArray(vao);

	// Upload Positions and Colours
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colour) * colours.size(), colours.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Colour), 0);

	// Bind Program and View Projection Matrix
	linesProgram.Bind();
	linesProgram.BindUniform("PVMatrix", AppInfo::ActiveCamera()->GetPVMatrix());

	// Draw the Lines!
	glDrawArrays(GL_LINES, 0, (int)positions.size());


	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineDrawer::SetColour(Colour colour) noexcept
{
	currentColour = colour;
}

void LineDrawer::Add(vec3 start, vec3 end, float lifetime) noexcept
{
	Add(start, end, currentColour, lifetime);
}
void LineDrawer::Add(vec3 start, vec3 end, Colour colour, float lifetime) noexcept
{
	positions.push_back(start);
	colours.push_back(colour);
	lifetimes.push_back(lifetime);

	positions.push_back(end);
	colours.push_back(colour);
	lifetimes.push_back(lifetime);
}

void LineDrawer::AddXYZLines(float lifetime) noexcept
{
	Add(vec3(0), vec3(1, 0, 0), Colour(1, 0, 0), lifetime);
	Add(vec3(0), vec3(0, 1, 0), Colour(0, 1, 0), lifetime);
	Add(vec3(0), vec3(0, 0, 1), Colour(0, 0, 1), lifetime);
}

void LineDrawer::AddGrid(vec3 centre, float size, float lifetime) noexcept
{
	AddGrid(centre, size, currentColour, lifetime);
}
void LineDrawer::AddGrid(vec3 centre, float size, Colour colour, float lifetime) noexcept
{
	vec3 a, b;
	for (int x = (int)-size; x <= (int)size; x++)
	{
		a = centre + vec3(x, 0, -size);
		b = centre + vec3(x, 0, size);
		Add(a, b, colour, lifetime);
	}
	for (int z = (int)-size; z <= (int)size; z++)
	{
		a = centre + vec3(-size, 0, z);
		b = centre + vec3(size, 0, z);
		Add(a, b, colour, lifetime);
	}
}

void LineDrawer::AddCuboid(vec3 centre, vec3 size, float lifetime) noexcept
{
	AddCuboid(centre, size, currentColour, lifetime);
}

void LineDrawer::AddCuboid(vec3 centre, vec3 size, Colour colour, float lifetime) noexcept
{
	vec3 halfSize = size * 0.5f;
	
	vec3 vNNN(centre.x - halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z);
	vec3 vNPN(centre.x - halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z);
	vec3 vPNN(centre.x + halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z);
	vec3 vPPN(centre.x + halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z);

	vec3 vNNP(centre.x - halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z);
	vec3 vNPP(centre.x - halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z);
	vec3 vPNP(centre.x + halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z);
	vec3 vPPP(centre.x + halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z);
	
	// Near Quad
	Add(vNNN, vNPN, colour, lifetime);
	Add(vNPN, vPPN, colour, lifetime);
	Add(vPPN, vPNN, colour, lifetime);
	Add(vPNN, vNNN, colour, lifetime);

	// Far Quad
	Add(vNNP, vNPP, colour, lifetime);
	Add(vNPP, vPPP, colour, lifetime);
	Add(vPPP, vPNP, colour, lifetime);
	Add(vPNP, vNNP, colour, lifetime);

	// Near-Far Lines
	Add(vNNP, vNNN, colour, lifetime);
	Add(vNPP, vNPN, colour, lifetime);
	Add(vPPP, vPPN, colour, lifetime);
	Add(vPNP, vPNN, colour, lifetime);
}

void LineDrawer::AddSphere(vec3 centre, float radius, int ringSides, float lifetime) noexcept
{
	AddSphere(centre, radius, ringSides, currentColour, lifetime);
}

void LineDrawer::AddSphere(vec3 centre, float radius, int ringSides, Colour colour, float lifetime) noexcept
{
	const float phi = 2.0f * glm::pi<float>() / ringSides;

	vec3 previousPoint{}, firstPoint{};
	mat4 newRotation{};
	vec3 newDirection{}, newPoint{};

	vec3 currentAxis = vec3(1, 0, 0);

	for (int axisNum = 0; axisNum < 3; axisNum++)
	{
		for (int i = 0; i < ringSides; i++)
		{
			newRotation = glm::rotate((float)i * phi, vec3(1, 0, 0));
			newRotation = glm::rotate(newRotation, glm::radians(90.0f), currentAxis);
			newDirection = (vec3)glm::normalize(glm::inverse(newRotation)[2]);
			newPoint = centre + newDirection * radius;

			if (i > 0)
			{
				Add(previousPoint, newPoint, colour, lifetime);
				if (i == ringSides - 1) { Add(newPoint, firstPoint, colour, lifetime); }
			}
			else { firstPoint = newPoint; }
			previousPoint = newPoint;
		}

		currentAxis = vec3(currentAxis.z, currentAxis.x, currentAxis.y);
	}
}

void LineDrawer::AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, float lifetime) noexcept
{
	AddCone(point, direction, range, baseRadius, baseSides, currentColour, lifetime);
}

void LineDrawer::AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, Colour colour, float lifetime) noexcept
{
	const vec3 baseCentre = point + (direction * range);

	const float phi = 2.0f * glm::pi<float>() / baseSides;
	const float theta = std::atan(baseRadius / range);

	const mat4 coneRotation = glm::inverse(glm::lookAt(point, baseCentre, { 0, 1, 0 }));

	vec3 previousPoint{}, firstPoint{};
	mat4 newRotation{};
	vec3 newDirection{}, newPoint{};
	for (int i = 0; i < baseSides; i++)
	{
		newRotation = glm::rotate(coneRotation, phi * i, { 0, 0, 1 });
		newRotation = glm::rotate(newRotation, theta, { 1, 0, 0 });

		newDirection = glm::normalize(-newRotation[2]);
		newPoint = point + newDirection * sqrt(range * range + baseRadius * baseRadius);

		Add(point, newPoint, colour, lifetime);

		if (i > 0)
		{
			Add(previousPoint, newPoint, colour, lifetime);
			if (i == baseSides - 1)
			{
				Add(newPoint, firstPoint, colour, lifetime);
			}
		}
		else
		{
			firstPoint = newPoint;
		}
		previousPoint = newPoint;
	}
}

void LineDrawer::AddConicalFrustum(vec3 point, vec3 direction, float innerRange, float outerRange, float angle, int baseSides, float lifetime) noexcept
{
	AddConicalFrustum(point, direction, innerRange, outerRange, angle, baseSides, currentColour, lifetime);
}

void LineDrawer::AddConicalFrustum(vec3 point, vec3 direction, float innerRange, float outerRange, float angle, int baseSides, Colour colour, float lifetime) noexcept
{
	const float innerBaseRadius = innerRange * tan(angle);
	const float outerBaseRadius = outerRange * tan(angle);

	const float phi = 2.0f * glm::pi<float>() / baseSides;
	const float theta = angle;

	const mat4 coneRotation = glm::lookAt(point, point + direction, { 0, 1, 0 });

	vec3 previousInnerPoint{}, firstInnerPoint{};
	vec3 previousOuterPoint{}, firstOuterPoint{};
	for (int i = 0; i < baseSides; i++)
	{
		mat4 newRotation = glm::rotate(coneRotation, phi * i, { 0, 0, 1 });
		newRotation = glm::rotate(newRotation, theta, { 1, 0, 0 });

		vec3 newDirection = glm::normalize(-newRotation[2]);
		vec3 newInnerPoint = point + newDirection * sqrt(sqr(innerRange) + sqr(innerBaseRadius));
		vec3 newOuterPoint = point + newDirection * sqrt(sqr(outerRange) + sqr(outerBaseRadius));

		Add(newInnerPoint, newOuterPoint, colour, lifetime);

		if (i > 0)
		{
			Add(previousInnerPoint, newInnerPoint, colour, lifetime);
			Add(previousOuterPoint, newOuterPoint, colour, lifetime);
			if (i == baseSides - 1)
			{
				Add(newInnerPoint, firstInnerPoint, colour, lifetime);
				Add(newOuterPoint, firstOuterPoint, colour, lifetime);
			}
		}
		else
		{
			firstInnerPoint = newInnerPoint;
			firstOuterPoint = newOuterPoint;
		}
		previousInnerPoint = newInnerPoint;
		previousOuterPoint = newOuterPoint;
	}
}

void LineDrawer::AddFrustum(mat4 pvMatrix, float lifetime) noexcept
{
	AddFrustum(pvMatrix, currentColour, lifetime);
}
void LineDrawer::AddFrustum(mat4 pvMatrix, Colour colour, float lifetime) noexcept
{
	mat4 toWorld = glm::inverse(pvMatrix);

	vec4 vNNN(toWorld * vec4(-1.0f, -1.0f, -1.0f, 1.0f));	vNNN /= vNNN.w;
	vec4 vNPN(toWorld * vec4(-1.0f, 1.0f, -1.0f, 1.0f));	vNPN /= vNPN.w;
	vec4 vPPN(toWorld * vec4(1.0f, 1.0f, -1.0f, 1.0f));	vPPN /= vPPN.w;
	vec4 vPNN(toWorld * vec4(1.0f, -1.0f, -1.0f, 1.0f));	vPNN /= vPNN.w;

	vec4 vNNP(toWorld * vec4(-1.0f, -1.0f, 1.0f, 1.0f));	vNNP /= vNNP.w;
	vec4 vNPP(toWorld * vec4(-1.0f, 1.0f, 1.0f, 1.0f));	vNPP /= vNPP.w;
	vec4 vPPP(toWorld * vec4(1.0f, 1.0f, 1.0f, 1.0f));		vPPP /= vPPP.w;
	vec4 vPNP(toWorld * vec4(1.0f, -1.0f, 1.0f, 1.0f));	vPNP /= vPNP.w;

	// Near Quad
	Add(vNNN, vNPN, colour, lifetime);
	Add(vNPN, vPPN, colour, lifetime);
	Add(vPPN, vPNN, colour, lifetime);
	Add(vPNN, vNNN, colour, lifetime);

	// Far Quad
	Add(vNNP, vNPP, colour, lifetime);
	Add(vNPP, vPPP, colour, lifetime);
	Add(vPPP, vPNP, colour, lifetime);
	Add(vPNP, vNNP, colour, lifetime);

	// Near-Far Lines
	Add(vNNN, vNNP, colour, lifetime);
	Add(vNPN, vNPP, colour, lifetime);
	Add(vPPN, vPPP, colour, lifetime);
	Add(vPNN, vPNP, colour, lifetime);
}

void LineDrawer::Initialise() noexcept
{
	initialised = true;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &colourBuffer);

	linesProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\Lines.vert");
	linesProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\Lines.frag");
	linesProgram.Link();
}
