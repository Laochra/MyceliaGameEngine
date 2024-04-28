#pragma once

#include "MathIncludes.h"
#include "GeneralMacros.h"

#include "Shader.h"

#include <vector>
using std::vector;

typedef unsigned int uint;

// Only use after glad and glfw have been initialised.
class LineDrawer
{
public:
	LineDrawer() noexcept = default;
	~LineDrawer() noexcept;
	uncopyable(LineDrawer)
	immovable(LineDrawer)

	// Clear lines with expired lifetimes and draw the rest.
	void Draw() noexcept;
	/// <param name="colour:">The colour to be used for lines when none is specified.</param>
	void SetColour(vec3 colour) noexcept;
	/// <summary>Uses current default colour. See overload to specify a colour.</summary>
	/// <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void Add(vec3 start, vec3 end, float lifetime = FLT_MIN) noexcept;
	/// <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void Add(vec3 start, vec3 end, vec3 colour, float lifetime = FLT_MIN) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB. Uses current default colour for grid. See overload to specify a colour.</summary>
	/// <param name="size">= How many units to span from the specified centre.</param>
	/// <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddWorldGrid(float size, vec3 centre, float lifetime = FLT_MIN) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB.</summary>
	/// <param name="size">= How many units to span from the specified centre.</param>
	/// <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddWorldGrid(float size, vec3 centre, vec3 colour, float lifetime = FLT_MIN) noexcept;

protected:
	vector<vec3> positions;
	vector<vec3> colours;
	vector<float> lifetimes;
	
	vec3 currentColour = vec3(1.0f, 1.0f, 1.0f);

	uint vao;
	uint positionBuffer;
	uint colourBuffer;

	ShaderProgram linesProgram;

	bool initialised = false;

	void Initialise() noexcept;
};