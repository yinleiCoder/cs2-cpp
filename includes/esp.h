#pragma once
#include <vector>
#include "vector.h"
#include "imgui.h"

uintptr_t GetBaseEntity(int index, uintptr_t client);
uintptr_t GetBaseEntityFromHandle(uint32_t uHandle, uintptr_t client);

void make_esp();

namespace Bone {
	enum BoneIndex {
		head = 6,
		neck_0 = 5,
		spine_1 = 4,
		spine_2 = 2,
		pelvis = 0,
		arm_upper_l = 8,
		arm_lower_l = 9,
		hand_l = 10,
		arm_upper_r = 13,
		arm_lower_r = 14,
		hand_r = 15,
		leg_upper_l = 22,
		leg_lower_l = 23,
		ankle_l = 24,
		leg_upper_r = 25,
		leg_lower_r = 26,
		ankle_r = 27,
	};
}

inline std::vector<Vector3> boneDrawList{};

// 获取每个骨骼的坐标
Vector3 GetBone(uintptr_t addr, int32_t index);

// 骨骼绘制
void drawBone(uintptr_t pawn, ImColor boneColor, float* matrix);

// 骨骼间连接
void drawBoneLine(std::vector<Vector3> bones, ImColor color, float* matrix);

