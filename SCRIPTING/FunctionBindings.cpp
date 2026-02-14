#include "./FunctionBindings.h"
#include "../FS/MainFileSystem.h"      // FS::ScriptJS
#include "../NETWORKING/CNetworking.h" // Curl::Get

#include "ImGuiBindings.h"
#include <imgui.h>
#include <mutex>
#include <string>

namespace SCR {

    JSClassID g_script_class_id = 0;

    void register_class(JSRuntime *rt) {
        if (g_script_class_id == 0)
            JS_NewClassID(&g_script_class_id);

        JSClassDef def{};
        def.class_name = "Script";
        JS_NewClass(rt, g_script_class_id, &def);
    }

    JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc,
                           JSValueConst *argv) {
        auto *script = static_cast<FS::ScriptJS *>(
                JS_GetOpaque2(ctx, this_val, g_script_class_id));

        if (!script)
            return JS_ThrowTypeError(ctx, "invalid this");

        for (int i = 0; i < argc; ++i) {
            const char *s = JS_ToCString(ctx, argv[i]);

            if (s) {
                std::lock_guard<std::mutex> lg(script->m);
                script->output += s;

                if (i + 1 < argc)
                    script->output += ' ';

                JS_FreeCString(ctx, s);
            }
        }

        std::lock_guard<std::mutex> lg(script->m);
        script->output += '\n';

        return JS_UNDEFINED;
    }

    JSValue js_http_get(JSContext *ctx, JSValueConst, int argc,
                        JSValueConst *argv) {
        if (argc < 1 || !JS_IsString(argv[0]))
            return JS_ThrowTypeError(ctx, "url string expected");

        size_t n;
        const char *url_c = JS_ToCStringLen(ctx, &n, argv[0]);
        std::string url(url_c, n);
        JS_FreeCString(ctx, url_c);

        try {
            std::string body = Curl::Get(url);
            return JS_NewStringLen(ctx, body.c_str(), body.size());
        } catch (const std::exception &e) {
            return JS_ThrowInternalError(ctx, "%s", e.what());
        }
    }

    // One helper to install every global symbol
    void register_globals(JSContext *ctx) {
        JSValue global = JS_GetGlobalObject(ctx);

        JS_SetPropertyStr(ctx, global, "console", // console object
                            JS_NewObjectClass(ctx, g_script_class_id));

        JSValue console = JS_GetPropertyStr(ctx, global, "console");
        JS_SetPropertyStr(ctx, console, "log",
                            JS_NewCFunction(ctx, js_console_log, "log", 0));
        JS_FreeValue(ctx, console);

        JS_SetPropertyStr(ctx, global, "imgui_window",
                            JS_NewCFunction(ctx, js_imgui_window, "imgui_window", 3));

        JS_SetPropertyStr(ctx, global, "http_get",
                            JS_NewCFunction(ctx, js_http_get, "http_get", 1));
        JS_SetPropertyStr(ctx, global, "create_window",
                            JS_NewCFunction(ctx, js_create_window, "create_window", 2));
        JS_FreeValue(ctx, global);
    }
}
