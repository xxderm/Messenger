#pragma once
#include "imgui.h"
#include <iostream>
#include <SDL.h>
#include "stb_image.h"
#include "SDL_opengl.h"
#include "Texture.hpp"
#include <vector>
#include <string>
#include "Entities.hpp"
#include <functional>
#include <string>

namespace Application::View {

    class MainView final {
    public:
        void Initialize();

        void Render(SDL_Window* window, SDL_Renderer* renderer);

        void SetServerStatus(bool status);

        void SetStatusContent(std::string content);

        void SetRunningPtr(bool* ptr);

        void UpdateGuestsModel(const Model::Guests& guests);

        void UpdateMessagesModel(const Model::Messages& messages);

        void UpdateChannelsModel(const Model::Channels& channels);

        void UpdateUserName(std::shared_ptr<std::string> username);

        void OnUserNameUpdate(std::function<void()> fn);

        void OnChannelCreate(std::function<void(std::string, uint32_t, bool access)> fn);

        void OnJoinChannel(std::function<void(uint32_t, std::string)> function);

        void SetUid(std::shared_ptr<uint32_t> uid);
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
        GLuint mSendMessageTexture{};
        bool mServerStatus = false;
        bool *mAppRunningPtr = nullptr;
        bool mEditUserName{};
        bool mNewChannelFormOpened{};
        bool mChannelJoinWnd{};
        std::string mStatusContent = "";
        std::shared_ptr<std::string> mUserName{};
        std::shared_ptr<uint32_t> mUid{};
        Model::Guests mGuests;
        Model::Messages mMessages;
        Model::Channels mChannels;
        std::function<void()> mOnUserNameUpdateCallBack;
        std::function<void(std::string, uint32_t, bool access)> mChannelCreateCallBack;
        std::function<void(uint32_t, std::string)> mJoinChannelCallBack;
        int mChannelPlaces = 0;
        char mChannelNameBuffer[10]{'\0'};
        const char* mItemChannelAccess[2] = { "открытый", "закрытый" };
        int mSelectedChannelId = -1;
    };

}