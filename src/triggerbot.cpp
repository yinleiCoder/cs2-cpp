#include <Windows.h>
#include "../cs2dumper/offsets.hpp"
#include "../cs2dumper/client_dll.hpp"
#include "../cs2dumper/buttons.hpp"
#include "triggerbot.h"
#include "esp.h"

// 单独创建线程运行，通用TriggerBot，非AutoWall
void triggerBot() {
	const auto client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
	
	while (true) {
		auto localController = *reinterpret_cast<std::uintptr_t*>(client + cs2_dumper::offsets::client_dll::dwLocalPlayerController);
		auto localHpawn = *reinterpret_cast<std::uint32_t*>(localController + cs2_dumper::schemas::client_dll::CBasePlayerController::m_hPawn);
		if (localHpawn == 0xFFFFFFFF) continue;

		auto localPawn = GetBaseEntityFromHandle(localHpawn, client);
		if (!localPawn) continue;

		auto localTeam = *reinterpret_cast<int*>(localPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		auto localHealth = *reinterpret_cast<int*>(localPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		if (localHealth <= 0) continue;

		// 通用Triggerbot，看当前准星瞄准的敌人并射击，不用考虑掩体判断
		auto crosshairEntityHandle = *reinterpret_cast<std::uint32_t*>(localPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_iIDEntIndex);
		if (crosshairEntityHandle == 0 || crosshairEntityHandle == 0xFFFFFFFF) continue;
		auto playerPawn = GetBaseEntityFromHandle(crosshairEntityHandle, client);// 当前准星瞄准的敌人
		if (!playerPawn) continue;
		auto playerTeam = *reinterpret_cast<int*>(playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum);
		if (localTeam == playerTeam) continue;
		auto playerHealth = *reinterpret_cast<int*>(playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth);
		auto playerMaxHealth = *reinterpret_cast<int*>(playerPawn + cs2_dumper::schemas::client_dll::C_BaseEntity::m_iMaxHealth);
		if (localHealth <= 0 || playerMaxHealth != 100) continue;

		// 按下扳机
		if ((GetAsyncKeyState(VK_XBUTTON1) & 0x8000)) {
			auto forceAttack = reinterpret_cast<int*>(client + cs2_dumper::buttons::attack);//attack2是开镜
			Sleep(25);
			// 开火
			*forceAttack = 65537;
			Sleep(10);
			*forceAttack = 256;
		}
	}

}