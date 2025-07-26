#include "imgui.h"
#include "gui.h"

void make_menu() {
	ImGui::Begin("YinZhiXia");
	{
		ImGui::Checkbox("Box Esp", &yzx::visuals::isBoxEsp);
	}
	ImGui::End();
}