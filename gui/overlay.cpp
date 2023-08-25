#include "overlay.h"

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include "../sdk/sdk.h"

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace hack {
  namespace gui {
    Overlay::Overlay(uint32_t target_window_process_id, const std::string& window_class_name, const std::string& window_name)
      : window_class_name_(window_class_name), window_name_(window_name), menu_(window_name, 30.f, 30.f)
    {
      if (!Init(target_window_process_id)) {
        printf("Failed to initialize overlay..\n");
      }
    }

    Menu& Overlay::GetMenu()
    {
      return this->menu_;
    }

    bool Overlay::Init(uint32_t target_window_process_id)
    {
      bool window_focus = false;
      while (!window_focus) {
        DWORD foreground_window_process_id = 0;
        GetWindowThreadProcessId(GetForegroundWindow(), &foreground_window_process_id);
        if (target_window_process_id == foreground_window_process_id) {
          this->target_hwnd_ = GetForegroundWindow();
          RECT TempRect;
          GetWindowRect(this->target_hwnd_, &TempRect);
          this->screen_width_ = TempRect.right - TempRect.left;
          this->screen_height_ = TempRect.bottom - TempRect.top;
          this->screen_left_ = TempRect.left;
          this->screen_right_ = TempRect.right;
          this->screen_bottom_ = TempRect.bottom;
          this->screen_top_ = TempRect.top;
          window_focus = true;
        }
      }
      return true;
    }

    bool Overlay::SetupWindow()
    {
      this->window_class_ = {
          sizeof(WNDCLASSEX), 0, WinProc, 0,0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, this->window_class_name_.c_str(), LoadIcon(nullptr, IDI_APPLICATION)
      };
      RegisterClassEx(&this->window_class_);
      if (this->target_hwnd_) {
        static RECT temp_rect = { 0 };
        static POINT temp_pt;
        GetClientRect(this->target_hwnd_, &temp_rect);
        ClientToScreen(this->target_hwnd_, &temp_pt);
        temp_rect.left = temp_pt.x;
        temp_rect.top = temp_pt.y;
        this->overlay_width_ = temp_rect.right;
        this->overlay_height_ = temp_rect.bottom;
        this->hwnd_ = CreateWindowEx(0, this->window_class_name_.c_str(), this->window_name_.c_str(), WS_POPUP | WS_VISIBLE, this->screen_left_, this->screen_top_, this->screen_width_, this->screen_height_, 0, 0, 0, 0);
        DwmExtendFrameIntoClientArea(this->hwnd_, &this->margins_);
        SetWindowLong(this->hwnd_, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
        ShowWindow(this->hwnd_, SW_SHOW);
        UpdateWindow(this->hwnd_);
        return true;
      }
      return false;
    }

    bool Overlay::SetupDirectX()
    {
      if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &this->direct3d9_))) {
        return false;
      }

      this->params_ = { 0 };
      this->params_.Windowed = TRUE;
      this->params_.SwapEffect = D3DSWAPEFFECT_DISCARD;
      this->params_.hDeviceWindow = this->hwnd_;
      this->params_.MultiSampleQuality = D3DMULTISAMPLE_NONE;
      this->params_.BackBufferFormat = D3DFMT_A8R8G8B8;
      this->params_.BackBufferWidth = this->screen_width_;
      this->params_.BackBufferHeight = this->screen_height_;
      this->params_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
      this->params_.EnableAutoDepthStencil = TRUE;
      this->params_.AutoDepthStencilFormat = D3DFMT_D16;
      this->params_.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
      this->params_.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

      if (FAILED(this->direct3d9_->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->hwnd_, D3DCREATE_HARDWARE_VERTEXPROCESSING, &this->params_, 0, &this->device_))) {
        this->direct3d9_->Release();
        return false;
      }

      ImGui::CreateContext();

      ImGuiIO& io = ImGui::GetIO();
      ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

      ImGui_ImplWin32_Init(this->hwnd_);
      ImGui_ImplDX9_Init(this->device_);

      this->direct3d9_->Release();

      return true;
    }

    void Overlay::Loop()
    {
      if (!this->SetupWindow()) {
        printf("Failed to setup window overlay..\n");
        return;
      }

      if (!this->SetupDirectX()) {
        printf("Failed to setup directx..\n");
        return;
      }

      this->CreateObjects();

      this->Update();
    }

    void Overlay::Update()
    {
      static RECT old_rect;

      ZeroMemory(&this->message_, sizeof(MSG));

      while (this->message_.message != WM_QUIT) {
        if (PeekMessage(&this->message_, this->hwnd_, 0, 0, PM_REMOVE)) {
          TranslateMessage(&this->message_);
          DispatchMessage(&this->message_);
        }

        HWND foreground_window = GetForegroundWindow();
        if (foreground_window == this->target_hwnd_) {
          HWND temp_process_hwnd = GetWindow(foreground_window, GW_HWNDPREV);
          SetWindowPos(this->hwnd_, temp_process_hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        }

        RECT temp_rect;
        POINT temp_pt;

        ZeroMemory(&temp_rect, sizeof(RECT));
        ZeroMemory(&temp_pt, sizeof(POINT));

        GetClientRect(this->target_hwnd_, &temp_rect);
        ClientToScreen(this->target_hwnd_, &temp_pt);

        temp_rect.left = temp_pt.x;
        temp_rect.top = temp_pt.y;

        ImGuiIO& io = ImGui::GetIO();
        io.ImeWindowHandle = this->target_hwnd_;

        POINT temp_pt_2;

        GetCursorPos(&temp_pt_2);

        io.MousePos.x = static_cast<float>(temp_pt_2.x - temp_pt.x);
        io.MousePos.y = static_cast<float>(temp_pt_2.y - temp_pt.y);

        if (GetAsyncKeyState(MK_LBUTTON)) {
          io.MouseDown[0] = true;
          io.MouseClicked[0] = true;
          io.MouseClickedPos[0].x = io.MousePos.x;
          io.MouseClickedPos[0].y = io.MousePos.y;
        }
        else {
          io.MouseDown[0] = false;
        }

        if (temp_rect.left != old_rect.left || temp_rect.right != old_rect.right || temp_rect.top != old_rect.top || temp_rect.bottom != old_rect.bottom) {
          old_rect = temp_rect;

          this->screen_width_ = temp_rect.right;
          this->screen_height_ = temp_rect.bottom;
          sdk::vars::gScreenWidth = this->screen_width_;
          sdk::vars::gScreenHeight = this->screen_height_;

          this->params_.BackBufferWidth = this->screen_width_;
          this->params_.BackBufferHeight = this->screen_height_;

          SetWindowPos(this->hwnd_, (HWND)0, temp_pt.x, temp_pt.y, this->screen_width_, this->screen_height_, SWP_NOREDRAW);

          this->device_->Reset(&this->params_);
        }

        this->Render();
      }

      ImGui_ImplDX9_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();

      if (this->device_) {
        this->device_->EndScene();
        this->device_->Release();
      }
      if (this->direct3d9_) {
        this->direct3d9_->Release();
      }
    }

    void Overlay::Render()
    {
      ImGui_ImplDX9_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();
      {
        // STYLING
        {
          ImGuiStyle* style = &ImGui::GetStyle();
          style->WindowPadding = ImVec2(15, 15);
          style->WindowRounding = 5.0f;
          style->FramePadding = ImVec2(5, 5);
          style->FrameRounding = 4.0f;
          style->ItemSpacing = ImVec2(12, 8);
          style->ItemInnerSpacing = ImVec2(8, 6);
          style->IndentSpacing = 25.0f;
          style->ScrollbarSize = 15.0f;
          style->ScrollbarRounding = 9.0f;
          style->GrabMinSize = 5.0f;
          style->GrabRounding = 3.0f;
          style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
          style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
          style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
          style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
          style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
          style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
          style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
          style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
          style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
          style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
          style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
          style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
          style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
          style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
          style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
          style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
          style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
          style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
          style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
          style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
          style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
          style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
          style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
          style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
          style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
          style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
          style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
          style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
          style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        }
        // END OF STYLING

        // WINDOW OVERLAY
        {
          /*for (int key_code : listen_keys) {

          }*/
          this->menu_.Tick();

          this->menu_.Draw();
          /*ImGui::SetNextWindowSize(ImVec2(675, 460));
          ImGui::Begin("JustForFun v0.0.1 | absence", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
          ImGui::Button("Test Button");
          ImGui::End();*/
        }
        // END OF WINDOW OVERLAY
      }
      ImGui::EndFrame();

      this->device_->SetRenderState(D3DRS_ZENABLE, false);
      this->device_->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
      this->device_->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

      this->device_->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

      if (this->device_->BeginScene() >= 0) {

        ImGui::Render();

        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        DrawString(200, 250, D3DCOLOR_ARGB(255, 78, 87, 0), "facilier is gay!!1");

        this->device_->EndScene();
      }

      HRESULT res = this->device_->Present(0, 0, 0, 0);
      if (res == D3DERR_DEVICELOST && this->device_->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        InvalidateObjects();

        this->device_->Reset(&this->params_);

        CreateObjectsDeviceLost();
        ImGui_ImplDX9_CreateDeviceObjects();
      }
    }

    void Overlay::DrawString(int x, int y, int color, std::string text)
    {
      RECT rect;
      SetRect(&rect, x, y, x, y);
      this->font_->DrawTextA(0, text.c_str(), -1, &rect, DT_RIGHT | DT_NOCLIP, color);
    }

    void Overlay::InvalidateObjects()
    {
      this->font_->OnLostDevice();
    }

    void Overlay::CreateObjectsDeviceLost()
    {
      this->font_->OnResetDevice();
    }

    void Overlay::CreateObjects()
    {
      D3DXCreateFontA(this->device_, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &this->font_);
    }
  }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam)) {
    return true;
  }
  switch (Message) {
  case WM_DESTROY:
  {
    auto device = overlay->GetDevice();
    if (device != 0) {
      device->EndScene();
      device->Release();
    }
    break;
  }
  case WM_SIZE:
  {
    auto device = overlay->GetDevice();
    if (device != 0 && wParam != SIZE_MINIMIZED) {
      ImGui_ImplDX9_InvalidateDeviceObjects();
      overlay->InvalidateObjects();
      auto pParams = overlay->GetParameters();
      pParams->BackBufferWidth = LOWORD(lParam);
      pParams->BackBufferHeight = HIWORD(lParam);
      HRESULT hr = device->Reset(pParams);
      if (hr == D3DERR_INVALIDCALL) {
        printf("reset failed \n");
      }
      overlay->CreateObjectsDeviceLost();
      ImGui_ImplDX9_CreateDeviceObjects();
    }
    break;
  }
  }
  return DefWindowProc(hWnd, Message, wParam, lParam);
}
