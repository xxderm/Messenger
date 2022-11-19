#include "Packet.hpp"
#include <queue>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include "SDL_opengl.h"

namespace DedicatedHost::View {

    class PacketView final {
    public:
        PacketView() = default;

        void Initialize();

        void Render(bool &running);

        void AddOutPacket(Utils::Signal signal);

        void AddInPacket(Utils::Signal signal);
    private:
        std::string DetectSignalName(Utils::Signal signal);
    private:
        std::vector<std::string> mOutputPackets;
        std::vector<std::string> mInputPackets;
        SDL_Renderer* mRenderer{};
        SDL_GLContext mContext{};
        SDL_Window* mWindow{};
        SDL_Event mEvent{};
    };

}