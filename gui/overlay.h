#pragma once
#include <Windows.h>
#include <dwmapi.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <string>
#include <vector>

#include "menu.h"

#pragma comment(lib,"d3d9")
#pragma comment(lib,"d3dx9")
#pragma comment(lib,"dwmapi.lib")

namespace hack {
  namespace gui {
		class Overlay
		{
		public:
			Overlay(uint32_t target_window_process_id, const std::string& window_class_name, const std::string& window_name);

			Menu& GetMenu();

			void InvalidateObjects();
			void CreateObjectsDeviceLost();

			void Loop();

			void DrawString(int x, int y, int color, std::string text);

			inline IDirect3DDevice9Ex* GetDevice() { return this->device_; }
			inline D3DPRESENT_PARAMETERS* GetParameters() { return &this->params_; }

		private:
			HWND target_hwnd_ = nullptr;
			HWND hwnd_ = nullptr;

			std::string window_name_ = "";
			std::string window_class_name_ = "";

			int screen_width_ = 0;
			int screen_height_ = 0;
			int screen_top_ = 0;
			int screen_right_ = 0;
			int screen_bottom_ = 0;
			int screen_left_ = 0;

			int overlay_width_ = 0;
			int overlay_height_ = 0;

			WNDCLASSEX window_class_ = { NULL };
			MARGINS margins_ = { -1 };
			D3DPRESENT_PARAMETERS params_ = { NULL };
			IDirect3D9Ex* direct3d9_ = nullptr;
			IDirect3DDevice9Ex* device_ = nullptr;
			MSG message_ = { NULL };
			LPD3DXFONT font_ = nullptr;

			Menu menu_;

			bool Init(uint32_t target_window_process_id);
			bool SetupWindow();
			bool SetupDirectX();

			void CreateObjects();

			void Update();
			void Render();
		};
  }
}

extern hack::gui::Overlay* overlay;
