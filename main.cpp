#include "./AUDIO/Audio.h"
#include "./FS/MainFileSystem.h"
#include "Dependencies/fmt/fmt/core.h"
#include "UI/Renderer.h"
int main() {
    FS::CFileSystem::InitFileSystem();
    AUDIO::AudioPlayer::GetInstance()->Init();
    GUI::Renderer::Get()->Initialize();
    AUDIO::AudioPlayer::GetInstance()->Shutdown();
    return 0;
}
