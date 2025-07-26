#include <Windows.h>
#include "../cs2dumper/offsets.hpp"
#include "../cs2dumper/client_dll.hpp"
#include "esp.h"
#include "bhop.h"
#include "gui.h"

void bHop(CUserCMD* pcmd) {
	if (!yzx::movement::isBhop) return;

    const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
	// 当前玩家（我）
	auto localController = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
	auto localHpawn = *reinterpret_cast<std::uint32_t*>(localController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
	if (localHpawn == 0xFFFFFFFF) {
		return;
	};
	auto localPawn = GetBaseEntityFromHandle(localHpawn, client);
	if (!localPawn) {
		return;
	}

	// 连跳
	short mfFlags = *reinterpret_cast<short*>(localPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags);
	if (mfFlags & (1 << 0)) {// 玩家在地面上
		pcmd->nButtons.nValue &= ~IN_JUMP;
	}
}