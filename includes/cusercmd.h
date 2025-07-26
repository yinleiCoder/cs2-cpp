#pragma once
#include <cstdint>

enum CmdButtons : std::uint64_t {
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
};

struct CInButtonState {
public:
	char pad[0x8];
	std::uint64_t nValue; // 0x8
	std::uint64_t nValueChanged; // 0x10
	std::uint64_t nValueScroll; // 0x18
};

class CUserCMD {
public:
	char pad[0x58];
	CInButtonState nButtons; // 0x58
	char pad_2[0x20];
};