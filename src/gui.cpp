#include "imgui.h"
#include "gui.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void make_menu() {
	ImGui::Begin("YinZhiXia");
	ImGui::Text("ImGui Library: (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Configuration"))
    {
        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::TreeNode("Functions"))
        {
            ImGui::Checkbox("Bhop", &yzx::movement::isBhop);
            ImGui::SameLine(); HelpMarker("bhop hook createmove");
            ImGui::Checkbox("Box", &yzx::visuals::isBoxEsp);
            ImGui::SameLine(); HelpMarker("basic box esp");
            ImGui::Checkbox("Bone", &yzx::visuals::isBoneEsp);
            ImGui::SameLine(); HelpMarker("basic bone esp");
            ImGui::Checkbox("Player Information", &yzx::visuals::isPlayerEsp);
            ImGui::SameLine(); HelpMarker("player information esp");
            ImGui::Checkbox("Recoil", &yzx::attack::isRecoil);
            ImGui::SameLine(); HelpMarker("gun recoil, and it may not work");

            ImGui::TreePop();
            ImGui::Spacing();
        }
        if (ImGui::TreeNode("Style"))
        {
            ImGui::ColorEdit4("Box Color", &yzx::style::boxColor.Value.x);
            ImGui::ColorEdit4("Bone Color", &yzx::style::boneColor.Value.x);
            ImGui::ColorEdit4("Player Health Color", &yzx::style::healthColor.Value.x);
            ImGui::ColorEdit4("Player Name Color", &yzx::style::playerNameColor.Value.x);

            ImGui::TreePop();
            ImGui::Spacing();
        }
	}
    if (ImGui::CollapsingHeader("Help"))
    {
        ImGui::SeparatorText("Software Brief");
        ImGui::TextWrapped("This is an internal DLL injection game plugin, designed solely for educational and communication purposes");
        ImGui::SeparatorText("Github HomePage:");
        ImGui::TextLinkOpenURL("https://github.com/yinleiCoder/cs2-cpp");
        ImGui::SeparatorText("My Website:");
        ImGui::TextLinkOpenURL("http://yinleilei.com/");
    }

	ImGui::End();
}