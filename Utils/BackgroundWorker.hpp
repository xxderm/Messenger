#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include <mutex>
#include <memory>

namespace Utils {

    class BackgroundWorker final {
    public:
        BackgroundWorker() = default;

        void SetInterval(long long  ms) {
            mInterval = ms;
        }

        void OnTick(std::function<void()> fn) {
            mCallBack = fn;
        }

        void Start() {
            mRunning = true;
            this->Worker();
        }

        void Stop() {
            mRunning = false;
        }

        long long int GetElapsedSec() const { return mElapsedSec; }
    private:
        void Worker() {
            std::thread([this](){
                auto beginTime = std::chrono::steady_clock::now();
                while (mRunning) {
                    auto currentTime = std::chrono::steady_clock::now();
                    mElapsedSec = (mInterval / 1000) - std::chrono::duration_cast<std::chrono::seconds>(currentTime - beginTime).count();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - beginTime).count();
                    if (elapsed >= mInterval) {
                        mCallBack();
                        beginTime = std::chrono::steady_clock::now();
                    }
                }
            }).detach();
        }
    private:
        std::mutex mMutex{};
        long long int mElapsedSec{};
        bool mRunning = false;
        long long mInterval;
        std::function<void()> mCallBack;
    };

}