#include "MainFileSystem.h"
#include "../Dependencies/fmt/fmt/base.h"
#include "../Dependencies/fmt/fmt/color.h"

namespace fs = std::filesystem;

namespace FS {

    std::filesystem::path CFileSystem::base;
    std::filesystem::path CFileSystem::base_folder;
    std::filesystem::path CFileSystem::scripts_path;
    std::filesystem::path CFileSystem::logs_path;
    std::filesystem::path CFileSystem::settings_path;
    std::vector<std::unique_ptr<ScriptJS>> CFileSystem::scripts_array;
    std::vector<std::string> CFileSystem::setting_files_array;

    void CFileSystem::InitFileSystem() {
        LoadLocations();
        if (!fs::exists(base)) {
            fmt::print("The base folder is non-existent! Leaving...");
            return;
        }
        if (!fs::exists(base_folder)) {
            fmt::print("Base directory does not exist.. Generating...\n");
            fs::create_directory(base_folder);
            fmt::print("Creating scripts folder..\n");
            fs::create_directory(scripts_path);
            fmt::print("Creating logs folder...");
            fs::create_directory(logs_path);
        }
        if (!LoadScripts()) {
            fmt::print("Failed to load scripts.\n");
        }
    }

    bool CFileSystem::LoadScripts() {
        std::error_code ec;
        if (!fs::exists(scripts_path, ec))
            return false;

        scripts_array.clear();

        const fs::directory_options opts =
                fs::directory_options::skip_permission_denied |
                fs::directory_options::follow_directory_symlink;

        for (const auto &entry :
                fs::recursive_directory_iterator(scripts_path, opts, ec)) {
            if (ec || !entry.is_regular_file())
                continue;

            auto ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext != ".js")
                continue;

            const std::string stem = entry.path().stem().string();
            if (stem.empty())
                continue;

            scripts_array.emplace_back(
                    std::make_unique<ScriptJS>(stem, entry.path().string()));
        }

        fmt::print("Loaded {} JavaScript files\n", scripts_array.size());
        return true;
    }

    void CFileSystem::LoadLocations() {
#ifdef _WIN32
        base = std::string(getenv("USERPROFILE"));
#elif __linux__
        base = std::string(getenv("HOME"));
#endif

        base_folder = base / "Buddy";
        scripts_path = base_folder / "Scripts";
        logs_path = base_folder / "Logs";
    }

    std::vector<std::string> &CFileSystem::GetSettings() {
        return setting_files_array;
    }

    std::vector<std::unique_ptr<ScriptJS>> &CFileSystem::GetScripts() {
        return scripts_array;
    }

    std::filesystem::path CFileSystem::GetScriptFolderLocation() {
        return scripts_path;
    }

}