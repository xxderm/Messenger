#pragma once
#include <cstring>
#include <cstdint>

namespace Utils {

    class OutputMemory final {
    public:
        OutputMemory() = delete;

        OutputMemory(const OutputMemory& other) = delete;

        explicit OutputMemory(uint32_t packetSize) {
            mData = new char[packetSize];
            memcpy(mData, &packetSize, sizeof(packetSize));
            mStride = sizeof(packetSize);
        }

        void Write(void* data, uint32_t typeSize) noexcept {
            memcpy(mData + mStride, data, typeSize);
            mStride += typeSize;
        }

        char* GetData() const noexcept { return mData; }

        ~OutputMemory() {
            delete mData;
            mData = nullptr;
        }
    private:
        char* mData = nullptr;
        uint32_t mStride = 0;
    };

    class InputMemory final {
    public:
        InputMemory() = delete;

        InputMemory(const InputMemory& other) = delete;

        explicit InputMemory(char* data) : mData(data) {
            memcpy(&mPacketSize, data, sizeof(uint32_t));
            mStride = sizeof(uint32_t);
        }

        uint32_t GetPacketSize() const noexcept {
            return mPacketSize;
        }

        void Read(void* data, uint32_t typeSize) noexcept {
            memcpy(data, mData + mStride, typeSize);
            mStride += typeSize;
        }
    private:
        char* mData = nullptr;
        uint32_t mPacketSize = 0;
        uint32_t mStride = 0;
    };

}