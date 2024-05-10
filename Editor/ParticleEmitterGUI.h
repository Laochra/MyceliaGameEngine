#pragma once

#include "GameObject3DGUI.h"

#include "ParticleEmitter.h"

class ParticleEmitterGUI : public GameObject3DGUI
{
public:
	using GameObject3DGUI::GameObject3DGUI;

	virtual void Draw() override;

protected:
	static void DrawParticleEmitterGUI(ParticleEmitter* particleEmitter);
};