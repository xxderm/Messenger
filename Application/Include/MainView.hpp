#pragma once
#include "imgui.h"
#include <iostream>
#include <SDL.h>
#include "stb_image.h"
#include "SDL_opengl.h"
#include "Texture.hpp"
#include <vector>
#include <string>

namespace Application::View {

    class ClientController;

    class MainView final {
    public:
        void Initialize();

        void Render(SDL_Window* window, SDL_Renderer* renderer);

        void SetServerStatus(bool status);

        void SetStatusContent(std::string content);

        void SetRunningPtr(bool* ptr);
    private:
        void RenderStatusBar(SDL_Window* window, SDL_Renderer* renderer);

        void RenderLeftPanel(SDL_Window* window, SDL_Renderer* renderer);

        void RenderWndTitle(SDL_Window* window, SDL_Renderer* renderer);

        void RenderChatPanel(SDL_Window* window, SDL_Renderer* renderer);

        void RenderUserName(SDL_Window* window, SDL_Renderer* renderer);

        void RenderNewChannelForm(SDL_Window* window, SDL_Renderer* renderer);

        void RenderJoinChannelWindow(SDL_Window* window, SDL_Renderer* renderer);
    private:
        ImFont* mStatusBarFont;
        ImFont* mBaseFont;
        GLuint mStatusFailTexture{};
        GLuint mStatusSuccessTexture{};
        GLuint mCloseWndTexture{};
        GLuint mChatIconTexture{};
        GLuint mUserNameEditTexture{};
        bool mServerStatus = false;
        bool *mAppRunningPtr = nullptr;
        bool mEditUserName{};
        bool mNewChannelFormOpened{};
        bool mChannelJoinWnd{};
        std::string mStatusContent = "";
        std::string mUserName{};
    };

}