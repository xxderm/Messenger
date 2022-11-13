#include "Connection.hpp"
#include "Packet.hpp"
#include "BackgroundWorker.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include "SDL_opengl.h"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Messenger",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    auto context = SDL_GL_CreateContext(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    SDL_GL_SetSwapInterval(1);

    bool run = 1;
    SDL_Event event;
    while (run) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                run = 0;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();

    /*
    Application::Model::TcpCoonection connection;
    connection.Init(5000);
    connection.Connect();
    Utils::BackgroundWorker worker;
    worker.SetInterval(5000);
    worker.OnTick([&](void){

        Utils::ConnectPacket pack1("User" );
        auto pack1Data = pack1.Data();
        auto pack1Size = pack1.OutputDataSize();
        connection.Send(pack1Data, pack1Size);

        Utils::PacketRequest request(Utils::Signal::GUESTS);
        auto requestData = request.Data();
        auto requestSize = request.OutputDataSize();
        connection.Send(requestData, requestSize);


    });
    worker.Start();
    while (true) {
        connection.Receive();
    }*/
    system("pause");
    return 0;
}