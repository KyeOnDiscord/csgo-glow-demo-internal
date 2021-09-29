#pragma once

struct GlowStruct
{
	char pad_0000[8]; //0x0000
	float Red; //0x0008
	float Green; //0x000C
	float Blue; //0x0010
	float Alpha; //0x0014
	char pad_0018[16]; //0x0018
	bool renderOccluded; //0x0028
	bool renderUnoccluded; //0x0029
	bool m_bFullBloomRender;
	char      cJunk3[5];
	int32_t i32GlowStyle; // 0 = NormalGlow, 1 = MatrixGlow. 2= LegitGlow, 3 = Blinking LegitGlow
};