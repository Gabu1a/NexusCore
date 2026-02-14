#define MINIAUDIO_IMPLEMENTATION
#include "Audio.h"
#include "NETWORKING/CNetworking.h"
#include <iostream>
#include <thread>
#include <vector>

namespace AUDIO {
    AudioPlayer *AudioPlayer::instance = nullptr;

    struct SoundWithDecoder {
        ma_sound *sound;
        ma_decoder *decoder;
    };

    void sound_cleanup_callback(void *pUserData, ma_sound *pSound) {
        SoundWithDecoder *soundData = (SoundWithDecoder *)pUserData;
        if (soundData != nullptr) {
            ma_sound_uninit(soundData->sound);
            ma_decoder_uninit(soundData->decoder);
            delete soundData->sound;
            delete soundData->decoder;
            delete soundData;
        }
    }

    AudioPlayer *AudioPlayer::GetInstance() {
        if (instance == nullptr) {
            instance = new AudioPlayer();
        }
        return instance;
    }

    void AudioPlayer::Init() {
        if (isInitialized) {
            return;
        }

        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio engine." << std::endl;
            return;
        }
        isInitialized = true;
    }

    void AudioPlayer::Shutdown() {
        if (!isInitialized) {
            return;
        }
        ma_engine_uninit(&engine);
        isInitialized = false;
    }

    void AudioPlayer::Play(const std::string &filePath) {
        if (!isInitialized)
            return;
        ma_engine_play_sound(&engine, filePath.c_str(), NULL);
    }

    void AudioPlayer::PlayFromURL(const std::string &url) {
        if (!isInitialized)
            return;

        // Launch a new thread to download and play the sound asynchronously
        // This prevents the main UI thread from freezing during download
        std::thread([this, url]() {
            try {
                auto downloadResult = Curl::Get(url);
                if (downloadResult.empty()) {
                    std::cerr << "Download failed or returned empty data for: " << url
                              << std::endl;
                    return;
                }

                // Safety limit
                const size_t MAX_AUDIO_SIZE = 50 * 1024 * 1024; // 50MB
                if (downloadResult.size() > MAX_AUDIO_SIZE) {
                    std::cerr << "Audio file too large (" << downloadResult.size()
                              << " bytes) for: " << url << std::endl;
                    return;
                }

                std::vector<char> audioData(downloadResult.begin(), downloadResult.end());

                ma_decoder *decoder = new ma_decoder;
                ma_result result = ma_decoder_init_memory(
                        audioData.data(), audioData.size(), NULL, decoder);
                if (result != MA_SUCCESS) {
                    std::cerr << "Failed to decode audio from URL: " << url << std::endl;
                    delete decoder;
                    return;
                }

                // Create sound on heap
                ma_sound *sound = new ma_sound;
                result = ma_sound_init_from_data_source(&engine, decoder, 0, NULL, sound);

                if (result != MA_SUCCESS) {
                    std::cerr << "Failed to init sound from decoded URL data: " << url
                              << std::endl;
                    ma_decoder_uninit(decoder);
                    delete decoder;
                    delete sound;
                    return;
                }

                SoundWithDecoder *soundData = new SoundWithDecoder{sound, decoder};

                ma_sound_set_end_callback(sound, sound_cleanup_callback, soundData);

                ma_sound_start(sound);

            } catch (const std::exception &e) {
                std::cerr << "Audio download/play failed for URL " << url << ": "
                          << e.what() << std::endl;
            }
        }).detach(); // Detach the thread to let it run in the background
    }

    void AudioPlayer::SetMasterVolume(float volume) {
        if (!isInitialized)
            return;
        // Volume between 0.0 and 1.0
        float clampedVolume = std::max(0.0f, std::min(1.0f, volume));
        ma_engine_set_volume(&engine, clampedVolume);
    }

    void AudioPlayer::StopAllSounds() {
        if (!isInitialized)
            return;
        ma_engine_stop(&engine);
    }
}
