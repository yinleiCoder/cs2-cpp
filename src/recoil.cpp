#include <Windows.h>
#include "../cs2dumper/offsets.hpp"
#include "../cs2dumper/client_dll.hpp"
#include "vector.h"
#include "recoil.h"
#include "esp.h"
#include "gui.h"

void normalizePitch(float& pPitch) {
	pPitch = (pPitch < -89.0f) ? -89.0f : pPitch;
	pPitch = (pPitch > 89.0f) ? 89.0f : pPitch;
}

void normalizeYaw(float& pYaw) {
	while (pYaw > 180.f) pYaw -= 360.f;
	while (pYaw < -180.f) pYaw += 360.f;
}

void recoil() {
    const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));

    Vector3 oldAimpunch{};
	bool backInitliaze = false;

    while (yzx::attack::isRecoil) {
		// 当前玩家（我）
		auto localController = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
		auto localHpawn = *reinterpret_cast<std::uint32_t*>(localController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
		if (localHpawn == 0xFFFFFFFF) {
			continue;
		};
		auto localPawn = GetBaseEntityFromHandle(localHpawn, client);
		if (!localPawn) {
			continue;
		}
		auto localHealth = *reinterpret_cast<int*>(localPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		if (localHealth <= 0) continue;

		// 后坐力补偿
		auto localViewAngles = *reinterpret_cast<Vector3*>(client + cs2_dumper::offsets::client_dll::dwViewAngles);
		auto aimpunchAngles = *reinterpret_cast<Vector3*>(localPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchAngle);
		auto shotFired = *reinterpret_cast<int*>(localPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired);// 是否正在开枪
		if (shotFired) {
			if (!backInitliaze) {
				backInitliaze = true;
			}
			Vector3 newViewAngles{};
			newViewAngles.x = localViewAngles.x + oldAimpunch.x - (aimpunchAngles.x * 2.f);
			newViewAngles.y = localViewAngles.y + oldAimpunch.y - (aimpunchAngles.y * 2.f);
			normalizePitch(newViewAngles.x);
			normalizeYaw(newViewAngles.y);
			*reinterpret_cast<Vector3*>(client + cs2_dumper::offsets::client_dll::dwViewAngles) = newViewAngles;
			oldAimpunch.x = aimpunchAngles.x * 2.f;
			oldAimpunch.y = aimpunchAngles.y * 2.f;
		}
		else {
			if (backInitliaze) {
				Vector3 backInitAnlge{};
				backInitAnlge.x = localViewAngles.x + oldAimpunch.x;
				backInitAnlge.y = localViewAngles.y + oldAimpunch.y;
				*reinterpret_cast<Vector3*>(client + cs2_dumper::offsets::client_dll::dwViewAngles) = backInitAnlge;
				backInitliaze = !backInitliaze;
			}
			oldAimpunch = { 0.f, 0.f, 0.f };
		}
    }
}