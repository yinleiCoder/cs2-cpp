# CS2游戏外挂-内部注入（C++）

采用cpp与cmake构建的游戏内部注入外挂软件

## 软件功能

- 方框透视
- 骨骼透视
- 自瞄
- 枪支后坐力补偿

## 使用方法

- VS生成解决方案DLL
- 注入器[ExtremeInjector](https://github.com/master131/ExtremeInjector)注入DLL到游戏
- 

## 第三方库

- [ImGui](https://github.com/ocornut/imgui)：CS2游戏使用Directx11，获取游戏原生的IDXGISwapChain并注入ImGui
- [MinHook](https://github.com/TsudaKageyu/minhook)：VS编译MinHook
- [cs2 Dumper](https://github.com/a2x/cs2-dumper): 《反恐精英2》的外部偏移量/接口转储工具


## 本地构建

- 配置项目为：`Release x64`，CS2为`x64`游戏
- 项目构建：`CMake`
- 启用CS2开发者控制台，常见指令如下：
	- `sv_cheats 1`
	- `bot_stop 1`
	- `bind n noclip`
	- `bind f1 bot_place`
	- `mp_autoteambalance 0`
	- `mp_limitteams 10`
	- `bot_add_ct`

## 捐赠支持