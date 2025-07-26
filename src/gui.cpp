#include "imgui.h"
#include "gui.h"

void make_menu() {
	ImGui::Begin("YinZhiXia");
	{
		ImGui::Checkbox("Box", &yzx::visuals::isBoxEsp);
		ImGui::Checkbox("Bone", &yzx::visuals::isBoneEsp);
	}
	ImGui::End();
}