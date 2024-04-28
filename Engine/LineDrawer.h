#pragma once

#include "MathIncludes.h"
#include "GeneralMacros.h"

#include "Shader.h"

#include <vector>
using std::vector;

typedef unsigned int uint;

struct Colour
{
	float r, g, b, a;

	Colour(float red, float green, float blue, float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha) {}
};

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
	void SetColour(Colour colour) noexcept;
	/// <summary>Uses current default colour. See overload to specify a colour.</summary> <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void Add(vec3 start, vec3 end, float lifetime = FLT_MIN) noexcept;
	/// <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void Add(vec3 start, vec3 end, Colour colour, float lifetime = FLT_MIN) noexcept;
	/// <summary>Adds 3 lines displaying XYZ directions coloured RGB.</summary> <param name="lifetime">= How many seconds the transform lines will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddXYZLines(float lifetime) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB. <para>Uses current default colour for grid. See overload to specify a colour.</para></summary> <param name="size">= How many units to span from the specified centre.</param> <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddGrid(vec3 centre, float size, float lifetime = FLT_MIN) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB.</summary> <param name="size">= How many units to span from the specified centre.</param> <param name="lifetime">= How many seconds the line will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddGrid(vec3 centre, float size, Colour colour, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a cuboid at the specified centre. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="size">= XYZ size lengths of the cuboid</param> <param name="lifetime">= How many seconds the cuboid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCuboid(vec3 centre, vec3 size, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a cuboid at the specified centre.</summary> <param name="size">= XYZ size lengths of the cuboid</param> <param name="lifetime">= How many seconds the cuboid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCuboid(vec3 centre, vec3 size, Colour colour, float lifetime = FLT_MIN) noexcept;

protected:
	vector<vec3> positions;
	vector<Colour> colours;
	vector<float> lifetimes;
	
	Colour currentColour = Colour(1.0f, 1.0f, 1.0f);

	uint vao;
	uint positionBuffer;
	uint colourBuffer;

	ShaderProgram linesProgram;

	bool initialised = false;

	void Initialise() noexcept;
};