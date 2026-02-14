#include "./Scripting.h"
#include "../Dependencies/fmt/fmt/core.h"
#include "../Dependencies/quickjs/quickjs.h"
#include "../NETWORKING/CNetworking.h"
#include "FS/MainFileSystem.h"
#include "FunctionBindings.h"
#include "ImGuiBindings.h"
#include <cstring>
#include <fstream>
#include <sstream>

namespace SCR {

    // Static storage
    std::vector<JSThreadInfo> CScripting::threads;

    // Public API
    void CScripting::RunScriptAsync(FS::ScriptJS *script) {
        JSThreadInfo info;
        info.script_name = script->name;
        info.running = true;

        info.thread =
                std::async(std::launch::async, [script] { RunScriptJob(script); });

        threads.emplace_back(std::move(info));
    }

    void CScripting::PollThreads() {
        for (auto &t : threads) {
            if (t.running && t.thread.wait_for(std::chrono::seconds(0)) ==
                             std::future_status::ready) {
                t.thread.get();
                t.running = false;
            }
        }
    }

    // Job executed in background
    void CScripting::RunScriptJob(FS::ScriptJS *script) {
        JSRuntime *rt = JS_NewRuntime();
        if (!rt) {
            fmt::print("QuickJS: cannot create runtime\n");
            return;
        }

        SCR::register_class(rt);
        JSContext *ctx = JS_NewContext(rt);
        if (!ctx) {
            fmt::print("QuickJS: cannot create context\n");
            JS_FreeRuntime(rt);
            return;
        }

        std::ifstream in(script->fullpath);
        if (!in) {
            fmt::print("Cannot open script {}\n", script->fullpath);
            JS_FreeContext(ctx);
            JS_FreeRuntime(rt);
            return;
        }

        const std::string src{std::istreambuf_iterator<char>(in), {}};

        // Wire console.log that carries the pointer in this
        JSValue global = JS_GetGlobalObject(ctx);

        JSValue console = JS_NewObjectClass(ctx, SCR::g_script_class_id);
        JS_SetOpaque(console, script);

        JS_SetPropertyStr(ctx, console, "log",
                        JS_NewCFunction(ctx, SCR::js_console_log, "log", 0));

        JS_SetPropertyStr(ctx, global, "console", console);

        JS_SetPropertyStr(ctx, global, "http_get",
                        JS_NewCFunction(ctx, SCR::js_http_get, "http_get", 1));

        SCR::install_ui_object(ctx);

        JS_FreeValue(ctx, global);

        JSValue res = JS_Eval(ctx, src.c_str(), src.size(), script->name.c_str(),
                            JS_EVAL_TYPE_GLOBAL);

        if (JS_IsException(res)) {
            JSValue exc = JS_GetException(ctx);
            const char *msg = JS_ToCString(ctx, exc);
        {
            std::lock_guard<std::mutex> lg(script->m); // LOCK here too
            script->output += "[JS exception] ";
            script->output += (msg ? msg : "(unable to stringify exception)");
            script->output += '\n';
        }

        JS_FreeCString(ctx, msg);
        JS_FreeValue(ctx, exc);
        } else {
            JS_FreeValue(ctx, res);
        }

        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
    }
}
