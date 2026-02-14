#pragma once
#include "FS/MainFileSystem.h"
#include <future>
#include <string>
#include <vector>

namespace SCR {

    struct JSThreadInfo {
        std::string script_name;
        std::future<void> thread;
        bool running = false;
    };

    class CScripting {
        public:
            static void RunScriptAsync(FS::ScriptJS *script);

            static void PollThreads();

        private:
            static void RunScriptJob(FS::ScriptJS *script);

            static std::vector<JSThreadInfo> threads;
    };
}
