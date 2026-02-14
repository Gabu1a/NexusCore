#pragma once
#include <quickjs.h>

// forward-declare your own types that the callbacks use
namespace FS {
    struct ScriptJS;
}

namespace SCR {
    // g_script_class_id is defined in scr_bindings.cpp, only declared here
    extern JSClassID g_script_class_id;

    // one-off class registration
    void register_class(JSRuntime *);

    // individual native callbacks
    JSValue js_console_log(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue js_imgui_window(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue js_http_get(JSContext *, JSValueConst, int, JSValueConst *);

    // helper that wires every global function you want to export
    void register_globals(JSContext *);
}