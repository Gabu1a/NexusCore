#pragma once
#include "Dependencies/miniaudio/miniaudio.h"
#include <string>

namespace AUDIO {
    class AudioPlayer {
    public:
        static AudioPlayer *GetInstance();

        void Init();

        void Shutdown();

        void Play(const std::string &filePath);

        void PlayFromURL(const std::string &url);

        // 0.0 = muted, 1.0 = full volume
        void SetMasterVolume(float volume);

        void StopAllSounds();

    private:
        AudioPlayer() = default;
        ~AudioPlayer() = default;
        AudioPlayer(const AudioPlayer &) = delete;
        AudioPlayer &operator=(const AudioPlayer &) = delete;

        ma_engine engine;
        bool isInitialized = false;

        static AudioPlayer *instance;
    };
}