#include "HexCameraData.h"

vec2 HexCameraData::offsetDirection = glm::normalize(vec2(6.0f, 5.5f));

float HexCameraData::minZoom = 4.0f;
float HexCameraData::maxZoom = 12.0f;
float HexCameraData::zoomSpeed = 6.0f;
float HexCameraData::startZoom = 8.0f;

float HexCameraData::currentZoom = 8.0f;
