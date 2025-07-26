#pragma once 
#include "imgui.h"

namespace yzx {
	namespace visuals {
		inline bool isBoxEsp = false;// cpp 17+
		inline bool isBoneEsp = false;// cpp 17+
		inline bool isPlayerEsp = false;// cpp 17+
	}
	
	namespace movement {
		inline bool isBhop = false;// cpp 17+
	}

	namespace attack {
		inline bool isRecoil = false;
	}

	namespace style {
		inline ImColor boxColor{0, 0, 0, 255};
		inline ImColor boneColor{ 255, 255, 255, 255 };
		inline ImColor healthColor{ 0, 255, 0, 255 };
		inline ImColor playerNameColor{255, 255, 255, 255};
	}
}

void make_menu();