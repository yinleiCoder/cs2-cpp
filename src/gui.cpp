#include "imgui.h"
#include "gui.h"

void make_menu() {
	ImGui::Begin("YinZhiXia");
	{
		ImGui::Checkbox("Bhop", &yzx::movement::isBhop);
		ImGui::Checkbox("Box", &yzx::visuals::isBoxEsp);
		ImGui::Checkbox("Bone", &yzx::visuals::isBoneEsp);
		ImGui::Checkbox("Recoil", &yzx::attack::isRecoil);
	}
	ImGui::End();
}