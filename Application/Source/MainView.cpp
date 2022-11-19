#include <vector>
#include "MainView.hpp"

namespace Application::View {

    void MainView::Render(SDL_Window* window, SDL_Renderer* renderer) {
        this->RenderLeftPanel(window, renderer);
        this->RenderChatPanel(window, renderer);
        this->RenderWndTitle(window, renderer);
        this->RenderUserName(window, renderer);
        this->RenderNewChannelForm(window, renderer);
        this->RenderJoinChannelWindow(window, renderer);
        this->RenderStatusBar(window, renderer);
    }

    void MainView::RenderStatusBar(SDL_Window* window, SDL_Renderer* renderer) {
        ImGui::PushFont(mStatusBarFont);
        int winWidth{};
        int winHeight{};
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        ImVec2 statusBarSize = {static_cast<float>(winWidth), 20.};
        ImVec2 statusBarPos = { 0., static_cast<float>(winHeight - 30) };
        ImGui::SetNextWindowPos(statusBarPos);
        ImGui::SetNextWindowSize(statusBarSize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(.17, 0.18, 0.24, 1));
        ImGui::Begin("#StatusBar", 0,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoNav);
        ImGui::Text("Статус сервера:");
        GLuint tex = mStatusFailTexture;
        if (mServerStatus)
            tex = mStatusSuccessTexture;
        ImGui::SameLine(0);
        ImGui::Image((ImTextureID)tex, ImVec2(15,15));
        ImGui::SameLine(0);
        ImGui::Text(mStatusContent.c_str());
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    void MainView::Initialize() {
        srand(time(0));

        auto& io = ImGui::GetIO();
        mStatusBarFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14, NULL,
                                                      io.Fonts->GetGlyphRangesCyrillic());
        mBaseFont =  io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 18, NULL,
                                                  io.Fonts->GetGlyphRangesCyrillic());

        int w{}, h{};
        LoadTextureFromFile("../../Resources/fail.png", &mStatusFailTexture, &w, &h);
        LoadTextureFromFile("../../Resources/success.png", &mStatusSuccessTexture, &w, &h);
        LoadTextureFromFile("../../Resources/close.png", &mCloseWndTexture, &w, &h);
        LoadTextureFromFile("../../Resources/chat icon.png", &mChatIconTexture, &w, &h);
        LoadTextureFromFile("../../Resources/edit.png", &mUserNameEditTexture, &w, &h);
        LoadTextureFromFile("../../Resources/send.png", &mSendMessageTexture, &w, &h);
    }

    void MainView::SetServerStatus(bool status) {
        this->mServerStatus = status;
    }

    void MainView::SetStatusContent(std::string content) {
        this->mStatusContent = content;
    }

    void MainView::RenderLeftPanel(SDL_Window* window, SDL_Renderer* renderer) {
        ImGui::PushFont(mBaseFont);

        std::vector<std::string> guests = {
                "GuestName#1",
                "GuestName#2",
                "GuestName#3",
                "GuestName#4",
                "GuestName#5",
                "GuestName#6",
                "GuestName#7",
                "GuestName#8",
                "GuestName#9"
        };

        int winWidth{};
        int winHeight{};
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        ImVec2 leftPanelSize = { 250, static_cast<float>(winHeight - 55) };
        ImVec2 leftPanelPos = { 0, 25 };
        ImGui::SetNextWindowPos(leftPanelPos);
        ImGui::SetNextWindowSize(leftPanelSize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04, 0.047, 0.1, 1.0));
        ImGui::Begin("##LeftPanelWindow", 0,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoNav);
        ImGui::GetStyle().TabRounding = 0.f;
        ImGui::BeginTabBar("##LeftPanelTabs");
        if (ImGui::BeginTabItem("Каналы")) {
            if (ImGui::BeginTable("##ChannelsGrid", 1)) {
                for (int i = 0; i < mChannels.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                           IM_COL32(33, 53, 122, 255));
                    auto str = "Title: " + mChannels[i].Title + "\nPlaces: "
                            + std::to_string(mChannels[i].Places) + "\n" + "Access: " + (mChannels[i].Access ? "Открытый" : "Закрытый");
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4);
                    if (ImGui::Selectable(str.c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                        mChannelJoinWnd = true;
                        mSelectedChannelId = i;
                    }
                    ImGui::TableNextRow();
                }
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("New channel"))
                    mNewChannelFormOpened = true;
                ImGui::TableNextRow();
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Сообщения")) {
            if (ImGui::BeginTable("##MessageWithUser", 1)) {
                for (int i = 0; i < guests.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                           IM_COL32(33, 53, 122, 255));
                    auto str = "Общение с " + guests[i];
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4);
                    ImGui::Selectable(str.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::TableNextRow();
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Люди")) {
            if (ImGui::BeginTable("##GuestsList", 1)) {
                for (int i = 0; i < mGuests.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                           IM_COL32(33, 53, 122, 255));
                    auto str = mGuests[i].Name + "#" + std::to_string(mGuests[i].Id);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4);
                    ImGui::Selectable(str.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                    ImGui::TableNextRow();
                }
                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();
        ImGui::PopStyleColor();

        ImGui::PopFont();
    }

    void MainView::RenderWndTitle(SDL_Window* window, SDL_Renderer* renderer) {
        ImGui::PushFont(mStatusBarFont);
        int winWidth{};
        int winHeight{};
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        ImVec2 wndTitleSize = { static_cast<float>(winWidth), 20. };
        ImVec2 wndTitlePos = { 0., 0. };
        ImGui::SetNextWindowPos(wndTitlePos);
        ImGui::SetNextWindowSize(wndTitleSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {10, 10 });
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.34, 0.34, 0.34, 1.0));
        ImGui::Begin("##WndTitleBar", 0,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove
                     | ImGuiWindowFlags_NoNav);
        auto currentPosY = ImGui::GetCursorPosY();
        ImGui::SetCursorPosY(currentPosY - 5.);
        ImGui::Text("Мессенджер");
        ImGui::SameLine();
        ImGui::SetCursorPosX(winWidth - 20);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));
        if (ImGui::ImageButton((ImTextureID)mCloseWndTexture,
                ImVec2(15, 15), ImVec2(0, 0), ImVec2(1, 1), 0) )
            *mAppRunningPtr = false;
        ImGui::PopStyleColor();
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopFont();
    }

    void MainView::SetRunningPtr(bool *ptr) {
        this->mAppRunningPtr = ptr;
    }

    void MainView::RenderChatPanel(SDL_Window *window, SDL_Renderer *renderer) {

        std::vector<std::string> usrNames = {
                "usr1",
                "usr0",
                "usr1",
                "usr1",
                "usr1",
                "usr0",
                "usr1",
                "usr1",
                "usr1",
                "usr0",
                "usr1",
                "usr1",
                "usr1",
                "usr0",
                "usr1",
                "usr1",
                "usr1",
                "usr0",
                "usr1",
                "usr1"
        };

        std::vector<uint32_t> usrMessage = {
                1,
                0,
                1,
                1,
                1,
                0,
                1,
                1,
                1,
                0,
                1,
                1,
                1,
                0,
                1,
                1,
                1,
                0,
                1,
                1
        };

        std::vector<std::string> messages = {
            "HelloFrom1 Test text",
            "HelloFrom0 Test Text from 0 test text from 0",
            "HelloFrom1 test test text test test from",
            "HelloFrom1 test test small text test",
            "HelloFrom1 Test text",
            "HelloFrom0 Test Text from 0 test text from 0",
            "HelloFrom1 test test text test test from",
            "HelloFrom1 test test small text test",
            "HelloFrom1 Test text",
            "HelloFrom0 Test Text from 0 test text from 0",
            "HelloFrom1 test test text test test from",
            "HelloFrom1 test test small text test",
            "HelloFrom1 Test text",
            "HelloFrom0 Test Text from 0 test text from 0",
            "HelloFrom1 test test text test test from",
            "HelloFrom1 test test small text test",
            "HelloFrom1 Test text",
            "HelloFrom0 Test Text from 0 test text from 0",
            "HelloFrom1 test test text test test from",
            "HelloFrom1 test test small text test"
        };


        ImGui::PushFont(mBaseFont);

        int winWidth{};
        int winHeight{};
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        ImVec2 chatPanelSize = { static_cast<float>(winWidth - 250), static_cast<float>(winHeight - 90) };
        ImVec2 chatPanelPos = { 250, 60 };
        ImGui::SetNextWindowPos(chatPanelPos);
        ImGui::SetNextWindowSize(chatPanelSize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04, 0.047, 0.1, 1.0));
        ImGui::Begin("##ChatPanel",0 , ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
        if (messages.empty()) {
            ImGui::SetCursorPos(ImVec2((chatPanelSize.x / 2.) - 50, (chatPanelSize.y / 2) - 37.5));
            ImGui::Image((ImTextureID) mChatIconTexture, ImVec2(100, 75));
        }
        if (!mChannels.empty() || !mMessages.empty()) {
            ImGui::BeginTabBar("##ChatChannelsTab");
            for (auto &channel : mChannels) {
                bool connected = false;
                for (auto &usr : channel.Users) {
                    if (usr.Id == *mUid.get()) {
                        connected = true;
                        break;
                    }
                }
                if (connected) {
                    if (ImGui::BeginTabItem(channel.Title.c_str())) {
                        if (ImGui::BeginTable("##ChatGrid", 2)) {
                            for (int i = 0; i < messages.size(); ++i) {
                                ImGui::TableNextRow();
                                int column = 1;
                                if (usrMessage[i] == *mUid.get())
                                    column = 0;
                                ImGui::TableSetColumnIndex(column);
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_::ImGuiTableBgTarget_CellBg,
                                                       IM_COL32(38, 42, 103, 255));
                                auto usrNameText = " " + usrNames[i] + ":";
                                ImGui::Text(usrNameText.c_str());
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4);
                                ImGui::TextWrapped(messages[i].c_str());
                                ImGui::TableNextRow();
                                ImGui::TableNextRow();
                            }
                            ImGui::EndTable();
                            char buff[50];
                            ImGui::SetCursorPosY(static_cast<float>(winHeight - 90) - 30);
                            ImGui::PushItemWidth(static_cast<float>(winWidth - 250) - 50);
                            ImGui::InputText("##ChatTextInput", buff, 50);
                            ImGui::PopItemWidth();
                            ImGui::SameLine();
                            if (ImGui::ImageButton(
                                    (ImTextureID)mSendMessageTexture, ImVec2(24, 24),
                                    ImVec2(0,0), ImVec2(1, 1), 0
                                    )) {
                                // TODO:
                            }
                        }
                        ImGui::EndTabItem();
                    }
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
        ImGui::PopStyleColor();

        ImGui::PopFont();
    }

    void MainView::RenderUserName(SDL_Window *window, SDL_Renderer *renderer) {
        ImGui::PushFont(mBaseFont);
        int winWidth{};
        int winHeight{};
        SDL_GetWindowSize(window, &winWidth, &winHeight);
        ImGui::SetNextWindowSize(ImVec2(winWidth - 10, 10));
        ImGui::SetNextWindowPos(ImVec2(250, 28));
        ImGui::Begin("##UserNameBar", 0, ImGuiWindowFlags_NoDecoration |
                                         ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);
        if (!mEditUserName) {
            auto userName = *mUserName.get() +  "#" + std::to_string(*mUid.get());
            ImGui::Selectable(mUserName->c_str(), &mEditUserName, 0, ImVec2(270, 20));
        }
        if (mEditUserName) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
            ImGui::InputText("##Input", &mUserName->data()[0], 30);
            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
            if (ImGui::ImageButton((ImTextureID)mUserNameEditTexture, ImVec2(16, 16))) {
                mEditUserName = false;
                mOnUserNameUpdateCallBack();
            }
        }
        ImGui::End();

        ImGui::PopFont();
    }

    void MainView::RenderNewChannelForm(SDL_Window *window, SDL_Renderer *renderer) {
        ImGui::PushFont(mBaseFont);

        static const char* current_item = NULL;
        if (mNewChannelFormOpened) {
            ImGui::Begin("Новый канал", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (ImGui::BeginTable("##NewChannelGrid", 2)) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Название: ");

                ImGui::TableSetColumnIndex(1);
                ImGui::InputText("##inputChannelTitle", mChannelNameBuffer, 10);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Доступ: ");

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginCombo("##inputAccess", "открытый")) {
                    for (int n = 0; n < IM_ARRAYSIZE(mItemChannelAccess); n++) {
                        bool is_selected = (current_item ==
                                mItemChannelAccess[n]);
                        if (ImGui::Selectable(mItemChannelAccess[n], is_selected))
                            current_item = mItemChannelAccess[n];
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Мест: ");

                ImGui::TableSetColumnIndex(1);
                ImGui::InputInt("##inputPlaces", &mChannelPlaces);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("Отмена")) {
                    mNewChannelFormOpened = false;
                }

                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("Создать")) {
                    mChannelCreateCallBack(mChannelNameBuffer, mChannelPlaces, true);
                    std::cout << "TITLE:" << mChannelNameBuffer << std::endl;
                    std::cout << "id:" << *mUid.get() << std::endl;
                    mJoinChannelCallBack(*mUid.get(), mChannelNameBuffer);
                    mNewChannelFormOpened = false;
                }

                ImGui::EndTable();
            }
            ImGui::End();
        }

        ImGui::PopFont();
    }

    void MainView::RenderJoinChannelWindow(SDL_Window *window, SDL_Renderer *renderer) {
        if (mChannelJoinWnd) {
            ImGui::PushFont(mBaseFont);

            ImGui::Begin("Подключиться к каналу", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            if (ImGui::BeginTable("##JoinChannelGrid", 2)) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Название: ");

                ImGui::TableSetColumnIndex(1);
                ImGui::Text(mChannels[mSelectedChannelId].Title.c_str());

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Доступ: ");

                ImGui::TableSetColumnIndex(1);
                ImGui::Text(mChannels[mSelectedChannelId].Access ? "Открытый" : "Закрытый");

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Мест: ");

                ImGui::TableSetColumnIndex(1);
                ImGui::Text(std::to_string(mChannels[mSelectedChannelId].Places).c_str());

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (ImGui::Button("Отмена")) {
                    mChannelJoinWnd = false;
                    mSelectedChannelId = -1;
                }

                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("Подключиться")) {
                    mJoinChannelCallBack(*mUid.get(), mChannels[mSelectedChannelId].Title);
                    mChannelJoinWnd = false;
                    mSelectedChannelId = -1;
                }

                ImGui::EndTable();
            }
            ImGui::End();

            ImGui::PopFont();
        }
    }

    void MainView::UpdateGuestsModel(const Model::Guests& guests) {
        this->mGuests = guests;
    }

    void MainView::UpdateMessagesModel(const Model::Messages &messages) {
        this->mMessages = messages;
    }

    void MainView::UpdateChannelsModel(const Model::Channels &channels) {
        this->mChannels = channels;
    }

    void MainView::UpdateUserName(std::shared_ptr<std::string> username) {
        mUserName = username;
    }

    void MainView::OnUserNameUpdate(std::function<void()> fn) {
        mOnUserNameUpdateCallBack = fn;
    }

    void MainView::SetUid(std::shared_ptr<uint32_t> uid) {
        this->mUid = uid;
    }

    void MainView::OnChannelCreate(std::function<void(std::string, uint32_t, bool)> fn) {
        this->mChannelCreateCallBack = fn;
    }

    void MainView::OnJoinChannel(std::function<void(uint32_t, std::string)> function) {
        mJoinChannelCallBack = function;
    }

}