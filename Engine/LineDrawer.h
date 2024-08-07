#pragma once

#include "MathIncludes.h"
#include "GeneralMacros.h"

#include "Shader.h"

#include <vector>
using std::vector;

#include "Colour.h"

typedef unsigned int uint;

// Only use after glad and glfw have been initialised.
class LineDrawer
{
public:
	// Can range from 1-10
	uint lineWidth = 3;

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
	void AddXYZLines(float lifetime = FLT_MIN) noexcept;
	
	/// <summary>Adds a grid with XYZ coloured RGB. <para>Uses current default colour for grid. See overload to specify a colour.</para></summary> <param name="size">= How many units to span from the specified centre.</param> <param name="lifetime">= How many seconds the grid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddGrid(vec3 centre, float size, float lifetime = FLT_MIN) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB.</summary> <param name="size">= How many units to span from the specified centre.</param> <param name="lifetime">= How many seconds the grid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddGrid(vec3 centre, float size, Colour colour, float lifetime = FLT_MIN) noexcept;
	
	/// <summary> Adds a cuboid at the specified centre. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="size">= XYZ size lengths of the cuboid</param> <param name="lifetime">= How many seconds the cuboid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCuboid(vec3 centre, vec3 size, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a cuboid at the specified centre.</summary> <param name="size">= XYZ size lengths of the cuboid</param> <param name="lifetime">= How many seconds the cuboid will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCuboid(vec3 centre, vec3 size, Colour colour, float lifetime = FLT_MIN) noexcept;

	/// <summary> Adds a sphere at the specified centre with the specified range. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="ringSides">Amount of sides for each ring of the sphere.</param> <param name="lifetime">= How many seconds the sphere will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddSphere(vec3 centre, float radius, int ringSides, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a sphere at the specified centre with the specified range.</summary> <param name="ringSides">Amount of sides for each ring of the sphere.</param> <param name="lifetime">= How many seconds the sphere will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddSphere(vec3 centre, float radius, int ringSides, Colour colour, float lifetime = FLT_MIN) noexcept;

	/// <summary> Adds a cone originating from the specified point. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="direction">= The direction from the point to the base.</param> <param name="range">= The distance from the point to the base.</param> <param name="baseRadius">= The distance from the centre of the base to each of its points.</param> <param name="baseSides">= The amount of sides the base has.</param> <param name="lifetime">= How many seconds the cone will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a cone originating from the specified point.</summary> <param name="direction">= The direction from the point to the base.</param> <param name="range">= The distance from the point to the base.</param> <param name="baseRadius">= The distance from the centre of the base to each of its points.</param> <param name="baseSides">= The amount of sides the base has.</param> <param name="lifetime">= How many seconds the cone will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddCone(vec3 point, vec3 direction, float range, float baseRadius, int baseSides, Colour colour, float lifetime = FLT_MIN) noexcept;

	/// <summary> Adds a conical frustum originating from the inner range and extending to the outer range, using point as the origin of the angle. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="direction">= The direction from the point to the outer base.</param> <param name="innerRange">= The distance from the point to the inner base.</param> <param name="outerRange">= The distance from the point to the outer base.</param> <param name="angle">= The angle in radians from the direction to the outside of the conical frustum.</param> <param name="baseSides">= The amount of sides the bases have.</param> <param name="lifetime">= How many seconds the conical frustum will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddConicalFrustum(vec3 point, vec3 direction, float innerRange, float outerRange, float angle, int baseSides, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a conical frustum originating from the inner range and extending to the outer range, using point as the origin of the angle. </summary> <param name="direction">= The direction from the point to the outer base.</param> <param name="innerRange">= The distance from the point to the inner base.</param> <param name="outerRange">= The distance from the point to the outer base.</param> <param name="angle">= The angle in radians from the direction to the outside of the conical frustum.</param> <param name="baseSides">= The amount of sides the bases have.</param> <param name="lifetime">= How many seconds the conical frustum will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddConicalFrustum(vec3 point, vec3 direction, float innerRange, float outerRange, float angle, int baseSides, Colour colour, float lifetime = FLT_MIN) noexcept;

	/// <summary> Adds a frustum from a projection view matrix. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="lifetime">= How many seconds the conical frustum will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddFrustum(mat4 pvMatrix, float lifetime = FLT_MIN) noexcept;
	/// <summary> Adds a frustum from a projection view matrix. </summary> <param name="lifetime">= How many seconds the conical frustum will stay. FLT_MIN for single frame or -FLT_MAX for indefinite.</param>
	void AddFrustum(mat4 pvMatrix, Colour colour, float lifetime = FLT_MIN) noexcept;

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