#pragma once
#include <filesystem>
#include <mutex>
#include <string>
#include <vector>

namespace FS {

    struct ScriptJS {
        std::string name;
        std::string fullpath;
        std::string output;
        std::mutex m; // for thread-safe output

        // Constructor needed for make_unique
        ScriptJS(std::string n, std::string p): name(std::move(n)), fullpath(std::move(p)) {}

        // Delete copy operations (mutex is non-copyable)
        ScriptJS(const ScriptJS &) = delete;
        ScriptJS &operator=(const ScriptJS &) = delete;

        // Allow moves
        ScriptJS(ScriptJS &&) = default;
        ScriptJS &operator=(ScriptJS &&) = default;
    };

    class CFileSystem {
        public:
            CFileSystem() = delete;
            static void InitFileSystem();
            static bool LoadScripts();
            static void LoadLocations();
            static void LoadSavedFiles();
            static std::vector<std::string> &GetSettings();
            static std::vector<std::unique_ptr<FS::ScriptJS>> & GetScripts();
            static std::filesystem::path GetScriptFolderLocation();

        private:
            static std::filesystem::path base_folder;
            static std::filesystem::path base;
            static std::filesystem::path scripts_path;
            static std::filesystem::path logs_path;
            static std::filesystem::path settings_path;
            static std::vector<std::unique_ptr<ScriptJS>> scripts_array;
            static std::vector<std::string> setting_files_array;
    };
}
