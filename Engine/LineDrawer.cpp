#include "LineDrawer.h"

#include "TimeManager.h"

#include "glad.h"

#include "Camera.h"

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colours.size(), colours.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

	// Bind Program and View Projection Matrix
	linesProgram.Bind();
	linesProgram.BindUniform("PVMatrix", mainCamera->GetPVMatrix());

	// Draw the Lines!
	glDrawArrays(GL_LINES, 0, (int)positions.size());


	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineDrawer::SetColour(vec3 colour) noexcept
{
	currentColour = colour;
}

void LineDrawer::Add(vec3 start, vec3 end, float lifetime) noexcept
{
	Add(start, end, currentColour, lifetime);
}
void LineDrawer::Add(vec3 start, vec3 end, vec3 colour, float lifetime) noexcept
{
	positions.push_back(start);
	colours.push_back(colour);
	lifetimes.push_back(lifetime);

	positions.push_back(end);
	colours.push_back(colour);
	lifetimes.push_back(lifetime);
}

void LineDrawer::AddWorldGrid(float size, vec3 centre, float lifetime) noexcept
{
	AddWorldGrid(size, centre, currentColour, lifetime);
}
void LineDrawer::AddWorldGrid(float size, vec3 centre, vec3 colour, float lifetime) noexcept
{
	Add(vec3(0), vec3(1, 0, 0), vec3(1, 0, 0), lifetime);
	Add(vec3(0), vec3(0, 1, 0), vec3(0, 1, 0), lifetime);
	Add(vec3(0), vec3(0, 0, 1), vec3(0, 0, 1), lifetime);

	vec3 a, b;
	for (int x = centre.x - size; x <= centre.x + size; x++)
	{
		if (x != 0)
		{
			a = centre + vec3(x, 0, -size);
			b = centre + vec3(x, 0, size);
			Add(a, b, colour, lifetime);
			continue;
		}
		
		a = centre + vec3(0, 0, -size);
		b = centre;
		Add(a, b, colour, lifetime);

		a = centre + vec3(0, 0, 1);
		b = centre + vec3(0, 0, size);
		Add(a, b, colour, lifetime);
	}
	for (int z = centre.z - size; z <= centre.z + size; z++)
	{
		if (z != 0)
		{
			a = centre + vec3(-size, 0, z);
			b = centre + vec3(size, 0, z);
			Add(a, b, colour, lifetime);
			continue;
		}

		a = centre + vec3(-size, 0, 0);
		b = centre;
		Add(a, b, colour, lifetime);

		a = centre + vec3(1, 0, 0);
		b = centre + vec3(size, 0, 0);
		Add(a, b, colour, lifetime);
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
