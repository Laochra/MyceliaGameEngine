#version 460

#define brightnessConstant (vec3(0.299, 0.587, 0.114))
#define fixedThreshold (0.0833)
#define relativeThreshold (0.166)
#define blendStrength (0.75)
#define edgePasses (3)

uniform sampler2D AliasedTexture;

in vec2 FragTexCoords;

out vec4 FragColour;

struct FragNeighbourhood
{
	float C, N, E, S, W;
	float NE, NW, SE, SW;
	
	float lumaMin, lumaMax, lumaRange;
	
	vec2 texelSize;
};
struct Edge
{
	bool isHorizontal;
	float stepDir;
	float lumaGradient;
	float otherLuma;
};

vec3 GetRGB(float xOffset = 0.0, float yOffset = 0.0)
{
	return texture(AliasedTexture, FragTexCoords + vec2(xOffset, yOffset)).rgb;
}
float GetLuma(vec3 rgb)
{
	return dot(rgb, brightnessConstant);
}

bool CanSkipFXAA (FragNeighbourhood frags)
{
	return frags.lumaRange < max(fixedThreshold, relativeThreshold * frags.lumaMax);
}

float GetSubpixelBlendFactor(FragNeighbourhood frags)
{
	float lumaWeightedSum = 2 * (frags.N + frags.E + frags.S + frags.W);
	lumaWeightedSum += frags.NW + frags.NE + frags.SW + frags.SE;
	float lumaAvg = lumaWeightedSum * (1.0 / 12.0);
	
	float result = abs(lumaAvg - frags.C);
	result = clamp(result / frags.lumaRange, 0, 1);
	result = smoothstep(0, 1, result);
	return result * result * blendStrength;
}

bool IsHorizontalEdge(FragNeighbourhood frags)
{
	float h = 2.0 *
		abs(frags.N + frags.S   - 2.0 * frags.C) +
		abs(frags.NW + frags.SW - 2.0 * frags.W) +
		abs(frags.NE + frags.SE - 2.0 * frags.E);
	float v = 2.0 *
		abs(frags.W + frags.E   - 2.0 * frags.C) +
		abs(frags.NW + frags.NE - 2.0 * frags.N) +
		abs(frags.SW + frags.SE - 2.0 * frags.S);
	
	return h >= v;
}

float GetEdgeBlendFactor(FragNeighbourhood frags, Edge edge)
{
	vec2 edgeCoord = FragTexCoords;
	vec2 coordStep = vec2(0.0);
	
	if (edge.isHorizontal)
	{
		edgeCoord.y += 0.5 * edge.stepDir;
		coordStep.x = frags.texelSize.x;
	}
	else
	{
		edgeCoord.x += 0.5 * edge.stepDir;
		coordStep.y = frags.texelSize.y;
	}
	
	float edgeLuma = 0.5 * (frags.C + edge.otherLuma);
	float gradientThreshold = 0.25 * edge.lumaGradient;
	
	vec2 coordPos = edgeCoord;
	float lumaDeltaPos;
	bool atEndPos;
	for (int i = 0; i < edgePasses && !atEndPos; i++)
	{
		coordPos += coordStep;
		lumaDeltaPos = GetLuma(GetRGB(coordPos.x, coordPos.y) - edgeLuma);
		atEndPos = abs(lumaDeltaPos) >= gradientThreshold;
	}
	if (!atEndPos) coordPos += coordStep;
	
	vec2 coordNeg = edgeCoord;
	float lumaDeltaNeg;
	bool atEndNeg;
	for (int i = 0; i < edgePasses && !atEndNeg; i++)
	{
		coordNeg -= coordStep;
		lumaDeltaNeg = GetLuma(GetRGB(coordNeg.x, coordNeg.y) - edgeLuma);
		atEndNeg = abs(lumaDeltaNeg) >= gradientThreshold;
	}
	if (!atEndNeg) coordNeg -= coordStep;
	
	float distanceToEndPos, distanceToEndNeg;
	if (edge.isHorizontal)
	{
		distanceToEndPos = coordPos.x - FragTexCoords.x;
		distanceToEndNeg = FragTexCoords.x - coordNeg.x;
	}
	else
	{
		distanceToEndPos = coordPos.y - FragTexCoords.y;
		distanceToEndNeg = FragTexCoords.y - coordNeg.y;
	}
	
	float distanceToNearestEnd;
	bool deltaSign;
	if (distanceToEndPos <= distanceToEndNeg)
	{
		distanceToNearestEnd = distanceToEndPos;
		deltaSign = lumaDeltaPos >= 0;
	}
	else
	{
		distanceToNearestEnd = distanceToEndNeg;
		deltaSign = lumaDeltaNeg >= 0;
	}
	
	if (deltaSign == (frags.C - edgeLuma >= 0))
	{
		return 0.0;
	}
	else
	{
		return 0.5 - distanceToNearestEnd / (distanceToEndPos + distanceToEndNeg);
	}
}

Edge GetEdge(FragNeighbourhood frags)
{
	Edge edge;
	
	edge.isHorizontal = IsHorizontalEdge(frags);
	
	float lumaPos, lumaNeg;
	if (edge.isHorizontal)
	{
		edge.stepDir = frags.texelSize.y;
		lumaPos = frags.N;
		lumaNeg = frags.S;
	}
	else
	{
		edge.stepDir = frags.texelSize.x;
		lumaPos = frags.E;
		lumaNeg = frags.W;
	}
	float gradientPos = abs(lumaPos - frags.C);
	float gradientNeg = abs(lumaNeg - frags.C);
		
	if (gradientPos < gradientNeg)
	{
		edge.stepDir = -edge.stepDir;
		edge.lumaGradient = gradientNeg;
		edge.otherLuma = lumaNeg;
	}
	else
	{
		edge.lumaGradient = gradientPos;
		edge.otherLuma = lumaPos;
	}
	
	return edge;
}

void main() // Fragment
{
	FragNeighbourhood frags;
	
	frags.texelSize = 1.0 / textureSize(AliasedTexture, 0);
	
	frags.C = GetLuma(GetRGB());
	
	frags.N = GetLuma(GetRGB(-frags.texelSize.x, 0.0));
	frags.E = GetLuma(GetRGB(0.0, -frags.texelSize.y));
	frags.S = GetLuma(GetRGB( frags.texelSize.x, 0.0));
	frags.W = GetLuma(GetRGB(0.0,  frags.texelSize.y));
	
	frags.NE = GetLuma(GetRGB(-frags.texelSize.x, -frags.texelSize.y));
	frags.NW = GetLuma(GetRGB( frags.texelSize.x, -frags.texelSize.y));
	frags.SE = GetLuma(GetRGB(-frags.texelSize.x,  frags.texelSize.y));
	frags.SW = GetLuma(GetRGB( frags.texelSize.x,  frags.texelSize.y));
	
	
	frags.lumaMin = min( frags.C, min( min( frags.N, frags.E ), min( frags.S, frags.W ) ) );
	frags.lumaMax = max( frags.C, max( max( frags.N, frags.E ), max( frags.S, frags.W ) ) );
	frags.lumaRange = frags.lumaMax - frags.lumaMin;
	
	if (CanSkipFXAA(frags))
	{
		FragColour = vec4(GetRGB(), 1.0);
	}
	else
	{
		Edge edge = GetEdge(frags);
		
		float blendFactor = max(GetSubpixelBlendFactor(frags), GetEdgeBlendFactor(frags, edge));
		vec2 lookupCoord = FragTexCoords;
		if (edge.isHorizontal)
		{
			lookupCoord.y += blendFactor * edge.stepDir;
		}
		else
		{
			lookupCoord.x += blendFactor * edge.stepDir;
		}
		
		FragColour = vec4(GetRGB(lookupCoord.x * frags.texelSize.x, lookupCoord.y * frags.texelSize.y), 1.0);
	}
}
