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
		lifetimes[i + 1] -= Time::delta;
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
	linesProgram.BindUniform("PVMatrix", mainCamera->GetPVMatrix());

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
	for (int x = -size; x <= size; x++)
	{
		a = centre + vec3(x, 0, -size);
		b = centre + vec3(x, 0, size);
		Add(a, b, colour, lifetime);
	}
	for (int z = -size; z <= size; z++)
	{
		a = centre + vec3(-size, 0, z);
		b = centre + vec3(size, 0, z);
		Add(a, b, colour, lifetime);
	}
}

void LineDrawer::AddCuboid(vec3 centre, vec3 size, float lifetime) noexcept
{
	AddCuboid(size, centre, currentColour, lifetime);
}

void LineDrawer::AddCuboid(vec3 centre, vec3 size, Colour colour, float lifetime) noexcept
{
	vec3 halfSize = size * 0.5f;
	
	#define PointAAA vec3(centre.x - halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z)
	#define PointAAB vec3(centre.x - halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z)
	#define PointABA vec3(centre.x - halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z)
	#define PointABB vec3(centre.x - halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z)
	#define PointBAA vec3(centre.x + halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z)
	#define PointBAB vec3(centre.x + halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z)
	#define PointBBA vec3(centre.x + halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z)
	#define PointBBB vec3(centre.x + halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z)
	
	Add(PointAAB, PointABB, colour, lifetime);
	Add(PointABB, PointBBB, colour, lifetime);
	Add(PointBBB, PointBAB, colour, lifetime);
	Add(PointBAB, PointAAB, colour, lifetime);

	Add(PointAAA, PointABA, colour, lifetime);
	Add(PointABA, PointBBA, colour, lifetime);
	Add(PointBBA, PointBAA, colour, lifetime);
	Add(PointBAA, PointAAA, colour, lifetime);

	Add(PointAAB, PointAAA, colour, lifetime);
	Add(PointABB, PointABA, colour, lifetime);
	Add(PointBBB, PointBBA, colour, lifetime);
	Add(PointBAB, PointBAA, colour, lifetime);

	#undef PointAAA
	#undef PointAAB
	#undef PointABA
	#undef PointABB
	#undef PointBAA
	#undef PointBAB
	#undef PointBBA
	#undef PointBBB
}

void LineDrawer::AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, float lifetime) noexcept
{
	AddCone(point, direction, range, baseRadius, baseSides, currentColour, lifetime);
}

void LineDrawer::AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, Colour colour, float lifetime) noexcept
{
	const vec3 baseCentre = point + (direction * range);
	Add(point, baseCentre, colour, lifetime);

	const float phi = 2.0f * glm::pi<float>() / baseSides;
	const float theta = std::atan(baseRadius / range);

	// TODO: Wierd issue with X being flipped. Setting the up vector to -1 seems to be fixing it but not sure why?
	const mat4 coneRotation = glm::lookAt(point, baseCentre, { 0, -1, 0 });

	vec3 previousPoint, firstPoint;
	mat4 newRotation;
	vec3 newDirection, newPoint;
	for (int i = 0; i < baseSides; i++)
	{
		newRotation = glm::rotate(coneRotation, phi * i, { 0, 0, 1 });
		newRotation = glm::rotate(newRotation, theta, { 1, 0, 0 });

		newDirection = glm::normalize(-newRotation[2]);
		newPoint = newDirection * sqrt(range * range + baseRadius * baseRadius);

		Add(point, newPoint, colour, lifetime);
		//Add(baseCentre, newPoint, colour, lifetime);

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

void LineDrawer::Initialise() noexcept
{
	initialised = true;

	glLineWidth(3.0f);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &colourBuffer);

	linesProgram.LoadShader(VertexStage, "Engine\\DefaultAssets\\Lines.vert");
	linesProgram.LoadShader(FragmentStage, "Engine\\DefaultAssets\\Lines.frag");
	linesProgram.Link();
}
