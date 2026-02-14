#pragma once
#include "UI/IWindow.h"
#include <quickjs.h>
#include <string>

#define KEY_LEFT (int)ImGuiKey_LeftArrow
#define KEY_RIGHT (int)ImGuiKey_RightArrow
#define KEY_UP (int)ImGuiKey_UpArrow
#define KEY_DOWN (int)ImGuiKey_DownArrow

namespace SCR {
    class JSImGuiWindow : public GUI::IWindow {
    public:
        JSImGuiWindow(std::string title, JSContext *ctx, JSValue draw_cb)
                : title_(std::move(title)), ctx_(ctx), cb_(draw_cb), is_open_(true) {}

        ~JSImGuiWindow() override { JS_FreeValue(ctx_, cb_); }

        void Draw() override {
            if (!is_open_) return;

            // Pass the is_open_ flag to ImGui::Begin() to get the native close button
            if (!ImGui::Begin(title_.c_str(), &is_open_)) {
                ImGui::End();
                return;
            }

            // Get the global 'ui' object that was created by install_ui_object()
            JSValue global = JS_GetGlobalObject(ctx_);
            JSValue ui_obj = JS_GetPropertyStr(ctx_, global, "ui");

            // Pass it as the first argument to the callback
            JSValue args[] = {ui_obj};
            JSValue res = JS_Call(ctx_, cb_, JS_UNDEFINED, 1, args); // 1 arg

            if (JS_IsException(res)) {
                JSValue exc = JS_GetException(ctx_);
                const char *msg = JS_ToCString(ctx_, exc);
                ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "[JS exception] %s", msg);
                JS_FreeCString(ctx_, msg);
                JS_FreeValue(ctx_, exc);
            }

            // Clean up
            JS_FreeValue(ctx_, res);
            JS_FreeValue(ctx_, ui_obj);
            JS_FreeValue(ctx_, global);

            ImGui::End();
        }

        bool IsOpen() const { return is_open_; }

        void SetWindowSize(const MATH::Vector2D<int> &s) override { size_ = s; }
        MATH::Vector2D<int> GetWindowSize() override { return size_; }

        void SetWindowPos(const MATH::Vector2D<int> &p) override { pos_ = p; }
        MATH::Vector2D<int> GetWindowPos() override { return pos_; }

        void ResizeWindowScaled(MATH::Vector2D<int> &) override {} // no-op

    private:
        std::string title_;
        JSContext *ctx_;
        JSValue cb_;
        MATH::Vector2D<int> size_{400, 300};
        MATH::Vector2D<int> pos_{100, 100};
        bool is_open_;
    };

    JSValue ui_text(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_frame(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_load_image(JSContext *c, JSValueConst, int argc, JSValueConst *v);
    JSValue ui_image(JSContext *c, JSValueConst, int argc, JSValueConst *v);
    JSValue ui_get_cursor_screen_pos(JSContext *ctx, JSValueConst /*this_val*/,
                                     int /*argc*/, JSValueConst * /*argv*/);
    JSValue ui_text_colored(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_text_wrapped(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_bullet_text(JSContext *, JSValueConst, int, JSValueConst *);

    // Button functions
    JSValue ui_button(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_small_button(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_invisible_button(JSContext *, JSValueConst, int, JSValueConst *);

    // Input functions
    JSValue ui_input_text(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_input_int(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_input_float(JSContext *, JSValueConst, int, JSValueConst *);

    // Slider functions
    JSValue ui_slider_int(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_slider_float(JSContext *, JSValueConst, int, JSValueConst *);

    // Checkbox and radio
    JSValue ui_checkbox(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_radio_button(JSContext *, JSValueConst, int, JSValueConst *);

    // Layout functions
    JSValue ui_separator(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_same_line(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_new_line(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_spacing(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_dummy(JSContext *, JSValueConst, int, JSValueConst *);

    // Tree and collapsing
    JSValue ui_tree_node(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_tree_pop(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_collapsing_header(JSContext *, JSValueConst, int, JSValueConst *);

    // Combo and listbox
    JSValue ui_combo(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_listbox(JSContext *, JSValueConst, int, JSValueConst *);

    // Progress and other
    JSValue ui_progress_bar(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_frame(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue js_create_window(JSContext *ctx, JSValueConst /*this_val*/, int argc,
                             JSValueConst *argv);

    JSValue ui_is_key_pressed(JSContext *c, JSValueConst, int argc,
                              JSValueConst *argv);

    JSValue ui_is_window_focused(JSContext *c, JSValueConst, int, JSValueConst *);

    JSValue ui_add_line(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv);
    JSValue ui_add_rect(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv);
    JSValue ui_add_rect_filled(JSContext *ctx, JSValueConst this_val, int argc,
                               JSValueConst *argv);
    JSValue ui_add_circle(JSContext *ctx, JSValueConst this_val, int argc,
                          JSValueConst *argv);
    JSValue ui_add_circle_filled(JSContext *ctx, JSValueConst this_val, int argc,
                                 JSValueConst *argv);
    JSValue ui_add_text(JSContext *ctx, JSValueConst this_val, int argc,
                        JSValueConst *argv);
    JSValue ui_add_image(JSContext *ctx, JSValueConst this_val, int argc,
                         JSValueConst *argv);

    JSValue ui_add_rect_filled_multi_color(JSContext *, JSValueConst, int,
                                           JSValueConst *);

    JSValue ui_add_quad(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_quad_filled(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue ui_add_triangle(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_triangle_filled(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue ui_add_ngon(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_ngon_filled(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue ui_add_ellipse(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_ellipse_filled(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue ui_add_bezier_cubic(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_bezier_quadratic(JSContext *, JSValueConst, int, JSValueConst *);

    JSValue ui_add_image_quad(JSContext *, JSValueConst, int, JSValueConst *);
    JSValue ui_add_image_rounded(JSContext *, JSValueConst, int, JSValueConst *);

    void install_ui_object(JSContext *ctx);
}
