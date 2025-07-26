#include <Windows.h>
#include <optional>
#include <cmath>
#include <string>
#include "../cs2dumper/offsets.hpp"
#include "../cs2dumper/client_dll.hpp"
#include "esp.h"
#include "gui.h"

void DrawCenteredCircle()
{
	// 1. 获取主视口和中心点
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 center(viewport->GetCenter());

	// 2. 获取绘制列表（这里使用前景绘制列表，可覆盖在其他元素上）
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// 3. 绘制参数配置
	const float radius = 50.0f;                // 圆半径
	const ImU32 color = IM_COL32(255, 0, 0, 255); // RGBA红色
	const int num_segments = 64;                // 圆的分段数（越高越平滑）
	const float thickness = 2.0f;               // 边框粗细

	// 4. 绘制圆形
	draw_list->AddCircle(center, radius, color, num_segments, thickness);
}

void DrawCenteredTextWithBackground(const char* text,
	const ImVec4& text_color,
	const ImVec4& bg_color,
	float padding = 5.0f)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 center = viewport->GetCenter();
	ImVec2 text_size = ImGui::CalcTextSize(text);

	ImVec2 text_pos(
		center.x - text_size.x * 0.5f,
		center.y - text_size.y * 0.5f
	);

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	// 绘制背景矩形
	ImVec2 rect_min(text_pos.x - padding, text_pos.y - padding);
	ImVec2 rect_max(
		text_pos.x + text_size.x + padding,
		text_pos.y + text_size.y + padding
	);

	draw_list->AddRectFilled(rect_min, rect_max, ImColor(bg_color), 4.0f);

	// 绘制文本
	draw_list->AddText(text_pos, ImColor(text_color), text);
}


uintptr_t GetBaseEntity(int index, uintptr_t client) {
	auto entityListBase = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwEntityList);
	if (entityListBase == 0) return 0;
	
	auto entityList = *reinterpret_cast<std::uintptr_t*>(entityListBase + 0x8 * (index >> 9) + 16);
	if (entityList == 0) return 0;

	return *reinterpret_cast<std::uintptr_t*>(entityList + (0x78 * (index & 0x1FF)));
}

uintptr_t GetBaseEntityFromHandle(uint32_t uHandle, uintptr_t client) {
	auto entityListBase = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwEntityList);
	if (entityListBase == 0) return 0;

	const int nIndex = uHandle & 0x7FFF;// 社区服的头64个entity是玩家，普通服是头32个

	auto entityList = *reinterpret_cast<std::uintptr_t*>(entityListBase + 8 * (nIndex >> 9) + 16);
	if (!entityList) return 0;

	return *reinterpret_cast<std::uintptr_t*>(entityList + (0x78 * (nIndex & 0x1FF)));
}

std::optional<Vector3> GetEyePos(uintptr_t addr) noexcept {
	auto* origin = reinterpret_cast<Vector3*>(addr + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);
	auto* viewOffset = reinterpret_cast<Vector3*>(addr + cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_vecViewOffset);

	Vector3 localEye = *origin + *viewOffset;
	if (!std::isfinite(localEye.x) || !std::isfinite(localEye.y) || !std::isfinite(localEye.z)) return std::nullopt;
	if (localEye.Length() < 0.1f) return std::nullopt;

	return localEye;
}

bool WorldToScreen(Vector3 world, Vector3& screen, float* matrix, const int winWidth, const int winHeight) {
	float matrix2[4][4];

	memcpy(matrix2, matrix, 16 * sizeof(float));

	const float mX{ winWidth * 1.f / 2 };
	const float mY{ winHeight * 1.f / 2 };

	const float w{
		matrix2[3][0] * world.x +
		matrix2[3][1] * world.y +
		matrix2[3][2] * world.z +
		matrix2[3][3]
	};

	if (w < 0.65f) return false;

	const float x{
		matrix2[0][0] * world.x +
		matrix2[0][1] * world.y +
		matrix2[0][2] * world.z +
		matrix2[0][3]
	};

	const float y{
		matrix2[1][0] * world.x +
		matrix2[1][1] * world.y +
		matrix2[1][2] * world.z +
		matrix2[1][3]
	};

	screen.x = (mX + mX * x / w);
	screen.y = (mY - mY * y / w);
	screen.z = 0;

	return true;
}

Vector3 GetBone(uintptr_t addr, int32_t index) {
	int32_t d = 32 * index;
	uintptr_t address{};
	address = *reinterpret_cast<std::uintptr_t*>(addr + cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	if (!address) return Vector3();

	address = *reinterpret_cast<std::uintptr_t*>(address + cs2_dumper::schemas::client_dll::CSkeletonInstance::m_modelState + 0x80);
	if (!address) return Vector3();

	return *reinterpret_cast<Vector3*>(address + d);// 得到当前骨骼结点的坐标
}

void drawBoneLine(std::vector<Vector3> bones, ImColor color, float* matrix) {
	Vector3 points;// 骨骼节点坐标转换为屏幕坐标
	std::vector<Vector3> drawList{};
	for (int i = 0; i < bones.size(); ++i) {
		if (!WorldToScreen(bones[i], points, matrix, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN))) continue;
		drawList.push_back(points);
	}

	for (int i = 1; i < drawList.size(); ++i) {
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(drawList[i].x, drawList[i].y), ImVec2(drawList[i-1].x, drawList[i-1].y), color);
	}
}

void drawBone(uintptr_t pawn, ImColor boneColor, float* matrix){
	boneDrawList.clear();
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::head));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::neck_0));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::spine_2));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::pelvis));
	drawBoneLine(boneDrawList, boneColor, matrix);

	boneDrawList.clear();
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::neck_0));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::arm_upper_l));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::arm_lower_l));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::hand_l));
	drawBoneLine(boneDrawList, boneColor, matrix);

	boneDrawList.clear();
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::neck_0));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::arm_upper_r));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::arm_lower_r));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::hand_r));
	drawBoneLine(boneDrawList, boneColor, matrix);

	boneDrawList.clear();
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::pelvis));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::leg_upper_l));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::leg_lower_l));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::ankle_l));
	drawBoneLine(boneDrawList, boneColor, matrix);

	boneDrawList.clear();
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::pelvis));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::leg_upper_r));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::leg_lower_r));
	boneDrawList.push_back(GetBone(pawn, Bone::BoneIndex::ankle_r));
	drawBoneLine(boneDrawList, boneColor, matrix);
}

void make_esp() {
	const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
	auto localController = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
	auto localHpawn = *reinterpret_cast<std::uint32_t*>(localController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
	if (localHpawn == 0xFFFFFFFF) {
		return;
	};

	auto localPawn = GetBaseEntityFromHandle(localHpawn, client);
	if (!localPawn) {
		return;
	}

	auto localTeam = *reinterpret_cast<int*>(localPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);

	// 世界矩阵
	auto matrix = reinterpret_cast<float*>(client + cs2_dumper::offsets::client_dll::dwViewMatrix);

	for (int i = 0; i < 64; i++) {// CS2社区服的头64个entity是玩家，普通服是头32个
		auto playerController = GetBaseEntity(i, client);
		if (!playerController) continue;

		auto playerHpawn = *reinterpret_cast<std::uint32_t*>(playerController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
		if (playerHpawn == 0xFFFFFFFF) continue;

		auto playerPawn = GetBaseEntityFromHandle(playerHpawn, client);
		if (!playerPawn) continue;

		auto playerTeam = *reinterpret_cast<int*>(playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		if (localTeam == playerTeam) {// 队友
			continue;
		}

		auto playerHealth = *reinterpret_cast<int*>(playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		if (playerHealth <= 0) continue;// 敌人死亡
		auto playerOrigin = *reinterpret_cast<Vector3*>(playerPawn + cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin);// 敌人脚
		auto playerEyesPos = GetEyePos(playerPawn);
		if (!playerEyesPos.has_value()) continue;

		auto playerEyes = playerEyesPos.value();

		static const int widthDS = GetSystemMetrics(SM_CXSCREEN);
		static const int heightDS = GetSystemMetrics(SM_CYSCREEN);

		Vector3 head2D{};
		Vector3 foot2D{};

		if (!WorldToScreen(playerOrigin, foot2D, matrix, widthDS, heightDS)) continue;
		if (!WorldToScreen(playerEyes, head2D, matrix, widthDS, heightDS)) continue;

		const float height{ ::abs(head2D.y - foot2D.y) * 1.25f };
		const float width{ height / 2.f };
		const float x = head2D.x - (width / 2.f);
		const float y = head2D.y - (width / 2.5f);

		if (yzx::visuals::isBoxEsp) {
			//DrawCenteredCircle();
			//DrawCenteredTextWithBackground(std::to_string(widthDS).append(",").append(std::to_string(heightDS)).append(",").append(std::to_string(foot2D.y)).append(",").append(std::to_string(head2D.y)).c_str(), ImColor(0, 255, 0, 255), ImColor(255, 255, 255, 255));
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x+width, y+height),ImColor(255,0,0,255), 5.f, 0, 2.f);
		}

		if (yzx::visuals::isBoneEsp) {
			drawBone(playerPawn, ImColor(255, 255, 255, 255), matrix);
		}
	}

}