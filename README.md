# Messenger
TCP/IP Messenger

## Лабораторная 3
### Форма авторизации
Файл Application/Source/MainView.cpp
```cpp
void MainView::RenderAuthForm(SDL_Window *window, SDL_Renderer *renderer) {
        ImGui::Begin("AuthWindow") ;
        if (ImGui::BeginTabBar("Auth")) {
            if (ImGui::BeginTabItem("Sign In")) {

                if (ImGui::BeginTable("##SignIn", 2)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Логин: ");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##inputLogin", mUserNameBuffer, 10);


                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Пароль: ");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##inputPass", mUserPassBuffer, 10);

                    ImGui::EndTable();
                }
                if (ImGui::Button("Подключится")) {
                    std::string login{}, pass{};
                    std::ifstream input("wtf");
                    input >> login;
                    input >> pass;
                    if (login == mUserNameBuffer && pass == mUserPassBuffer) {
                        if (mAuthCallBack)
                            mAuthCallBack(login);
                    }
                    input.close();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Sign Up")) {

                if (ImGui::BeginTable("##SignUp", 2)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Логин: ");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##inputLoginUp", mUserNameBuffer, 10);


                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Пароль: ");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##inputPassUp", mUserPassBuffer, 10);

                    ImGui::EndTable();
                }
                if (ImGui::Button("Отправить")) {
                    std::string login{}, pass{};
                    std::ofstream out("wtf");
                    out << login;
                    out << pass;
                    if (mAuthCallBack)
                        mAuthCallBack(login);
                    out.close();
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
```
Формы:
![alt text](https://github.com/xxderm/Messenger/blob/lab3_auth/Resources/authformsignup.png)
![alt text](https://github.com/xxderm/Messenger/blob/lab3_auth/Resources/authformsignin.png)