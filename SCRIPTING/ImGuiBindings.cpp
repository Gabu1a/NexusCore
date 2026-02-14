#include "ImGuiBindings.h"
#include "../UI/GuiTaskQueue.h"
#include "../UI/Image/image.h"
#include "FunctionBindings.h"
#include "UI/Renderer.h"

static inline ImDrawList *GetDL() { return ImGui::GetWindowDrawList(); }

static inline bool js_to_double(JSContext *c, JSValueConst v, double &out) {
  return JS_ToFloat64(c, &out, v);
}

static inline bool js_to_int(JSContext *c, JSValueConst v, int &out) {
  return JS_ToInt32(c, &out, v);
}

#define ARG_CHECK(cond, ...)                                                   \
  if (!(cond))                                                                 \
  return JS_ThrowTypeError(ctx, __VA_ARGS__)

namespace SCR {

static std::unordered_map<int, std::shared_ptr<CImage>> g_image_cache;
static int g_next_image_id = 1;

JSValue ui_add_line(JSContext *ctx, JSValueConst, int argc,
                    JSValueConst *argv) {
  ARG_CHECK(argc >= 5, "add_line: need x1,y1,x2,y2,color[,thickness]");
  double x1, y1, x2, y2, th = 1.0;
  int col;
  if (js_to_double(ctx, argv[0], x1) || js_to_double(ctx, argv[1], y1) ||
      js_to_double(ctx, argv[2], x2) || js_to_double(ctx, argv[3], y2) ||
      js_to_int(ctx, argv[4], col))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, argv[5], th);

  GetDL()->AddLine(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2),
                   (ImU32)col, (float)th);
  return JS_UNDEFINED;
}

JSValue ui_add_rect(JSContext *ctx, JSValueConst, int argc,
                    JSValueConst *argv) {
  ARG_CHECK(argc >= 5, "add_rect: need x1,y1,x2,y2,col[,rounding,flags,thick]");
  double x1, y1, x2, y2, rnd = 0, th = 1;
  int col, flags = 0;
  if (js_to_double(ctx, argv[0], x1) || js_to_double(ctx, argv[1], y1) ||
      js_to_double(ctx, argv[2], x2) || js_to_double(ctx, argv[3], y2) ||
      js_to_int(ctx, argv[4], col))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, argv[5], rnd);
  if (argc > 6)
    js_to_int(ctx, argv[6], flags);
  if (argc > 7)
    js_to_double(ctx, argv[7], th);

  GetDL()->AddRect(ImVec2((float)x1, (float)y1), ImVec2((float)x2, (float)y2),
                   (ImU32)col, (float)rnd, flags, (float)th);
  return JS_UNDEFINED;
}

JSValue ui_add_rect_filled(JSContext *ctx, JSValueConst, int argc,
                           JSValueConst *argv) {
  ARG_CHECK(argc >= 5,
            "add_rect_filled: need x1,y1,x2,y2,col[,rounding,flags]");
  double x1, y1, x2, y2, rnd = 0;
  int col, flags = 0;
  if (js_to_double(ctx, argv[0], x1) || js_to_double(ctx, argv[1], y1) ||
      js_to_double(ctx, argv[2], x2) || js_to_double(ctx, argv[3], y2) ||
      js_to_int(ctx, argv[4], col))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, argv[5], rnd);
  if (argc > 6)
    js_to_int(ctx, argv[6], flags);

  GetDL()->AddRectFilled(ImVec2((float)x1, (float)y1),
                         ImVec2((float)x2, (float)y2), (ImU32)col, (float)rnd,
                         flags);
  return JS_UNDEFINED;
}

JSValue ui_add_circle(JSContext *ctx, JSValueConst, int argc,
                      JSValueConst *argv) {
  ARG_CHECK(argc >= 4, "add_circle: need cx,cy,r,col[,segments,thick]");
  double cx, cy, r, th = 1;
  int col, segs = 0;
  if (js_to_double(ctx, argv[0], cx) || js_to_double(ctx, argv[1], cy) ||
      js_to_double(ctx, argv[2], r) || js_to_int(ctx, argv[3], col))
    return JS_EXCEPTION;
  if (argc > 4)
    js_to_int(ctx, argv[4], segs);
  if (argc > 5)
    js_to_double(ctx, argv[5], th);

  GetDL()->AddCircle(ImVec2((float)cx, (float)cy), (float)r, (ImU32)col, segs,
                     (float)th);
  return JS_UNDEFINED;
}

JSValue ui_add_rect_filled_multi_color(JSContext *ctx, JSValueConst, int argc,
                                       JSValueConst *v) {
  ARG_CHECK(argc == 8,
            "add_rect_filled_multi_color: need x1,y1,x2,y2,4 colors");
  double x1, y1, x2, y2;
  int cul, cur, cbr, cbl;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_int(ctx, v[4], cul) || js_to_int(ctx, v[5], cur) ||
      js_to_int(ctx, v[6], cbr) || js_to_int(ctx, v[7], cbl))
    return JS_EXCEPTION;
  GetDL()->AddRectFilledMultiColor(
      {(float)x1, (float)y1}, {(float)x2, (float)y2}, (ImU32)cul, (ImU32)cur,
      (ImU32)cbr, (ImU32)cbl); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddQuad / AddQuadFilled --------------------------------*/
JSValue ui_add_quad(JSContext *ctx, JSValueConst, int argc, JSValueConst *v) {
  ARG_CHECK(argc >= 9, "add_quad: x1,y1,x2,y2,x3,y3,x4,y4,col[,thick]");
  double x1, y1, x2, y2, x3, y3, x4, y4, th = 1;
  int col;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_double(ctx, v[6], x4) || js_to_double(ctx, v[7], y4) ||
      js_to_int(ctx, v[8], col))
    return JS_EXCEPTION;
  if (argc > 9)
    js_to_double(ctx, v[9], th);
  GetDL()->AddQuad({(float)x1, (float)y1}, {(float)x2, (float)y2},
                   {(float)x3, (float)y3}, {(float)x4, (float)y4}, (ImU32)col,
                   (float)th); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_quad_filled(JSContext *ctx, JSValueConst, int argc,
                           JSValueConst *v) {
  ARG_CHECK(argc == 9, "add_quad_filled: x1,y1,x2,y2,x3,y3,x4,y4,col");
  double x1, y1, x2, y2, x3, y3, x4, y4;
  int col;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_double(ctx, v[6], x4) || js_to_double(ctx, v[7], y4) ||
      js_to_int(ctx, v[8], col))
    return JS_EXCEPTION;
  GetDL()->AddQuadFilled({(float)x1, (float)y1}, {(float)x2, (float)y2},
                         {(float)x3, (float)y3}, {(float)x4, (float)y4},
                         (ImU32)col); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddTriangle / AddTriangleFilled ------------------------*/
JSValue ui_add_triangle(JSContext *ctx, JSValueConst, int argc,
                        JSValueConst *v) {
  ARG_CHECK(argc >= 7, "add_triangle: x1,y1,x2,y2,x3,y3,col[,thick]");
  double x1, y1, x2, y2, x3, y3, th = 1;
  int col;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_int(ctx, v[6], col))
    return JS_EXCEPTION;
  if (argc > 7)
    js_to_double(ctx, v[7], th);
  GetDL()->AddTriangle({(float)x1, (float)y1}, {(float)x2, (float)y2},
                       {(float)x3, (float)y3}, (ImU32)col, (float)th); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_triangle_filled(JSContext *ctx, JSValueConst, int argc,
                               JSValueConst *v) {
  ARG_CHECK(argc == 7, "add_triangle_filled: x1,y1,x2,y2,x3,y3,col");
  double x1, y1, x2, y2, x3, y3;
  int col;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_int(ctx, v[6], col))
    return JS_EXCEPTION;
  GetDL()->AddTriangleFilled({(float)x1, (float)y1}, {(float)x2, (float)y2},
                             {(float)x3, (float)y3}, (ImU32)col); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddNgon / AddNgonFilled --------------------------------*/
JSValue ui_add_ngon(JSContext *ctx, JSValueConst, int argc, JSValueConst *v) {
  ARG_CHECK(argc >= 5, "add_ngon: cx,cy,r,col,segments[,thick]");
  double cx, cy, r, th = 1;
  int col, segs;
  if (js_to_double(ctx, v[0], cx) || js_to_double(ctx, v[1], cy) ||
      js_to_double(ctx, v[2], r) || js_to_int(ctx, v[3], col) ||
      js_to_int(ctx, v[4], segs))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, v[5], th);
  GetDL()->AddNgon({(float)cx, (float)cy}, (float)r, (ImU32)col, segs,
                   (float)th); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_ngon_filled(JSContext *ctx, JSValueConst, int argc,
                           JSValueConst *v) {
  ARG_CHECK(argc == 5, "add_ngon_filled: cx,cy,r,col,segments");
  double cx, cy, r;
  int col, segs;
  if (js_to_double(ctx, v[0], cx) || js_to_double(ctx, v[1], cy) ||
      js_to_double(ctx, v[2], r) || js_to_int(ctx, v[3], col) ||
      js_to_int(ctx, v[4], segs))
    return JS_EXCEPTION;
  GetDL()->AddNgonFilled({(float)cx, (float)cy}, (float)r, (ImU32)col,
                         segs); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddEllipse / AddEllipseFilled --------------------------*/
JSValue ui_add_ellipse(JSContext *ctx, JSValueConst, int argc,
                       JSValueConst *v) {
  ARG_CHECK(argc >= 6, "add_ellipse: cx,cy,rx,ry,col[,rot,segments,thick]");
  double cx, cy, rx, ry, rot = 0, th = 1;
  int col, segs = 0;
  if (js_to_double(ctx, v[0], cx) || js_to_double(ctx, v[1], cy) ||
      js_to_double(ctx, v[2], rx) || js_to_double(ctx, v[3], ry) ||
      js_to_int(ctx, v[4], col))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, v[5], rot);
  if (argc > 6)
    js_to_int(ctx, v[6], segs);
  if (argc > 7)
    js_to_double(ctx, v[7], th);
  GetDL()->AddEllipse({(float)cx, (float)cy}, {(float)rx, (float)ry},
                      (ImU32)col, (float)rot, segs, (float)th); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_ellipse_filled(JSContext *ctx, JSValueConst, int argc,
                              JSValueConst *v) {
  ARG_CHECK(argc >= 6, "add_ellipse_filled: cx,cy,rx,ry,col[,rot,segments]");
  double cx, cy, rx, ry, rot = 0;
  int col, segs = 0;
  if (js_to_double(ctx, v[0], cx) || js_to_double(ctx, v[1], cy) ||
      js_to_double(ctx, v[2], rx) || js_to_double(ctx, v[3], ry) ||
      js_to_int(ctx, v[4], col))
    return JS_EXCEPTION;
  if (argc > 5)
    js_to_double(ctx, v[5], rot);
  if (argc > 6)
    js_to_int(ctx, v[6], segs);
  GetDL()->AddEllipseFilled({(float)cx, (float)cy}, {(float)rx, (float)ry},
                            (ImU32)col, (float)rot, segs); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddBezierCubic / AddBezierQuadratic --------------------*/
JSValue ui_add_bezier_cubic(JSContext *ctx, JSValueConst, int argc,
                            JSValueConst *v) {
  ARG_CHECK(argc >= 11, "add_bezier_cubic: p1..p4(col,thick[,segments])");
  double x1, y1, x2, y2, x3, y3, x4, y4, th;
  int col, segs = 0;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_double(ctx, v[6], x4) || js_to_double(ctx, v[7], y4) ||
      js_to_int(ctx, v[8], col) || js_to_double(ctx, v[9], th))
    return JS_EXCEPTION;
  if (argc > 10)
    js_to_int(ctx, v[10], segs);
  GetDL()->AddBezierCubic({(float)x1, (float)y1}, {(float)x2, (float)y2},
                          {(float)x3, (float)y3}, {(float)x4, (float)y4},
                          (ImU32)col, (float)th, segs); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_bezier_quadratic(JSContext *ctx, JSValueConst, int argc,
                                JSValueConst *v) {
  ARG_CHECK(argc >= 8, "add_bezier_quadratic: p1..p3,col,thick[,segments]");
  double x1, y1, x2, y2, x3, y3, th;
  int col, segs = 0;
  if (js_to_double(ctx, v[0], x1) || js_to_double(ctx, v[1], y1) ||
      js_to_double(ctx, v[2], x2) || js_to_double(ctx, v[3], y2) ||
      js_to_double(ctx, v[4], x3) || js_to_double(ctx, v[5], y3) ||
      js_to_int(ctx, v[6], col) || js_to_double(ctx, v[7], th))
    return JS_EXCEPTION;
  if (argc > 8)
    js_to_int(ctx, v[8], segs);
  GetDL()->AddBezierQuadratic({(float)x1, (float)y1}, {(float)x2, (float)y2},
                              {(float)x3, (float)y3}, (ImU32)col, (float)th,
                              segs); /* [1] */
  return JS_UNDEFINED;
}

/*---------------- AddImageQuad / AddImageRounded -------------------------*/
JSValue ui_add_image_quad(JSContext *ctx, JSValueConst, int argc,
                          JSValueConst *v) {
  ARG_CHECK(argc >= 9,
            "add_image_quad: tex,x1,y1,x2,y2,x3,y3,x4,y4[,uv*8,col]");
  int64_t tex;
  double x1, y1, x2, y2, x3, y3, x4, y4;
  if (JS_ToInt64(ctx, &tex, v[0]) || js_to_double(ctx, v[1], x1) ||
      js_to_double(ctx, v[2], y1) || js_to_double(ctx, v[3], x2) ||
      js_to_double(ctx, v[4], y2) || js_to_double(ctx, v[5], x3) ||
      js_to_double(ctx, v[6], y3) || js_to_double(ctx, v[7], x4) ||
      js_to_double(ctx, v[8], y4))
    return JS_EXCEPTION;
  /* uv + color defaults */
  double u1 = 0, v1u = 0, u2 = 1, v2u = 0, u3 = 1, v3u = 1, u4 = 0, v4u = 1;
  int col = 0xFFFFFFFF;
  int idx = 9;
  if (argc > idx)
    js_to_double(ctx, v[idx++], u1);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v1u);
  if (argc > idx)
    js_to_double(ctx, v[idx++], u2);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v2u);
  if (argc > idx)
    js_to_double(ctx, v[idx++], u3);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v3u);
  if (argc > idx)
    js_to_double(ctx, v[idx++], u4);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v4u);
  if (argc > idx)
    js_to_int(ctx, v[idx++], col);

  GetDL()->AddImageQuad((ImTextureID)tex, {(float)x1, (float)y1},
                        {(float)x2, (float)y2}, {(float)x3, (float)y3},
                        {(float)x4, (float)y4}, {(float)u1, (float)v1u},
                        {(float)u2, (float)v2u}, {(float)u3, (float)v3u},
                        {(float)u4, (float)v4u}, (ImU32)col); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_image_rounded(JSContext *ctx, JSValueConst, int argc,
                             JSValueConst *v) {
  ARG_CHECK(argc >= 7,
            "add_image_rounded: tex,x1,y1,x2,y2,col,rounding[,flags,uv*4]");
  int64_t tex;
  double x1, y1, x2, y2, rounding;
  int col, flags = 0;
  if (JS_ToInt64(ctx, &tex, v[0]) || js_to_double(ctx, v[1], x1) ||
      js_to_double(ctx, v[2], y1) || js_to_double(ctx, v[3], x2) ||
      js_to_double(ctx, v[4], y2) || js_to_int(ctx, v[5], col) ||
      js_to_double(ctx, v[6], rounding))
    return JS_EXCEPTION;
  int idx = 7;
  if (argc > idx)
    js_to_int(ctx, v[idx++], flags);
  double u1 = 0, v1u = 0, u2 = 1, v2u = 0;
  if (argc > idx)
    js_to_double(ctx, v[idx++], u1);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v1u);
  if (argc > idx)
    js_to_double(ctx, v[idx++], u2);
  if (argc > idx)
    js_to_double(ctx, v[idx++], v2u);

  GetDL()->AddImageRounded((ImTextureID)tex, {(float)x1, (float)y1},
                           {(float)x2, (float)y2}, {(float)u1, (float)v1u},
                           {(float)u2, (float)v2u}, (ImU32)col, (float)rounding,
                           flags); /* [1] */
  return JS_UNDEFINED;
}

JSValue ui_add_circle_filled(JSContext *ctx, JSValueConst, int argc,
                             JSValueConst *argv) {
  ARG_CHECK(argc >= 4, "add_circle_filled: need cx,cy,r,col[,segments]");
  double cx, cy, r;
  int col, segs = 0;
  if (js_to_double(ctx, argv[0], cx) || js_to_double(ctx, argv[1], cy) ||
      js_to_double(ctx, argv[2], r) || js_to_int(ctx, argv[3], col))
    return JS_EXCEPTION;
  if (argc > 4)
    js_to_int(ctx, argv[4], segs);

  GetDL()->AddCircleFilled(ImVec2((float)cx, (float)cy), (float)r, (ImU32)col,
                           segs);
  return JS_UNDEFINED;
}

JSValue ui_add_text(JSContext *ctx, JSValueConst, int argc,
                    JSValueConst *argv) {
  ARG_CHECK(argc >= 4, "add_text: need x,y,col,text");
  double x, y;
  int col;
  const char *txt = JS_ToCString(ctx, argv[3]);
  if (!txt)
    return JS_EXCEPTION;
  if (js_to_double(ctx, argv[0], x) || js_to_double(ctx, argv[1], y) ||
      js_to_int(ctx, argv[2], col)) {
    JS_FreeCString(ctx, txt);
    return JS_EXCEPTION;
  }

  GetDL()->AddText(ImVec2((float)x, (float)y), (ImU32)col, txt);
  JS_FreeCString(ctx, txt);
  return JS_UNDEFINED;
}

JSValue ui_add_image(JSContext *ctx, JSValueConst, int argc,
                     JSValueConst *argv) {
  ARG_CHECK(argc >= 5,
            "add_image: texture,x1,y1,x2,y2,[uv0x,uv0y,uv1x,uv1y,col]");
  int64_t tex;
  double x1, y1, x2, y2;
  if (JS_ToInt64(ctx, &tex, argv[0]) || js_to_double(ctx, argv[1], x1) ||
      js_to_double(ctx, argv[2], y1) || js_to_double(ctx, argv[3], x2) ||
      js_to_double(ctx, argv[4], y2))
    return JS_EXCEPTION;

  double uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 1;
  int col = 0xFFFFFFFF;
  if (argc > 5)
    js_to_double(ctx, argv[5], uv0x);
  if (argc > 6)
    js_to_double(ctx, argv[6], uv0y);
  if (argc > 7)
    js_to_double(ctx, argv[7], uv1x);
  if (argc > 8)
    js_to_double(ctx, argv[8], uv1y);
  if (argc > 9)
    js_to_int(ctx, argv[9], col);

  GetDL()->AddImage((ImTextureID)tex, ImVec2((float)x1, (float)y1),
                    ImVec2((float)x2, (float)y2),
                    ImVec2((float)uv0x, (float)uv0y),
                    ImVec2((float)uv1x, (float)uv1y), (ImU32)col);
  return JS_UNDEFINED;
}

JSValue js_imgui_window(JSContext *ctx, JSValueConst, int argc,
                        JSValueConst *argv) {
  if (argc < 1 || !JS_IsString(argv[0]))
    return JS_ThrowTypeError(ctx, "imgui_window: title string expected");

  size_t len;
  const char *title = JS_ToCStringLen(ctx, &len, argv[0]);

  if (argc >= 3 && JS_IsNumber(argv[1]) && JS_IsNumber(argv[2])) {
    double w, h;
    JS_ToFloat64(ctx, &w, argv[1]);
    JS_ToFloat64(ctx, &h, argv[2]);
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h),
                             ImGuiCond_FirstUseEver);
  }
  ImGui::Begin(title);
  ImGui::End();

  JS_FreeCString(ctx, title);
  return JS_UNDEFINED;
}
JSValue ui_text(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "string expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  ImGui::TextUnformatted(s, s + len);
  JS_FreeCString(c, s);
  return JS_UNDEFINED;
}

JSValue js_create_window(JSContext *ctx, JSValueConst /*this_val*/, int argc,
                         JSValueConst *argv) {
  /* -------------------------------------------------------------- */
  /* 1. Argument check                                              */
  /* -------------------------------------------------------------- */
  if (argc < 2 || !JS_IsString(argv[0]) || !JS_IsFunction(ctx, argv[1]))
    return JS_ThrowTypeError(ctx, "create_window(titleString, drawCallback)");

  /* -------------------------------------------------------------- */
  /* 2. Copy the title string                                       */
  /* -------------------------------------------------------------- */
  size_t len;
  const char *c_title = JS_ToCStringLen(ctx, &len, argv[0]);
  std::string title(c_title, len);
  JS_FreeCString(ctx, c_title);

  /* -------------------------------------------------------------- */
  /* 3. Serialize the JS callback into source code                  */
  /*    (we cannot move JSValue across threads or runtimes)         */
  /* -------------------------------------------------------------- */
  JSValue src_val = JS_ToString(ctx, argv[1]); // fn -> string
  const char *c_src = JS_ToCString(ctx, src_val);
  std::string fn_source(c_src);
  JS_FreeCString(ctx, c_src);
  JS_FreeValue(ctx, src_val); // done with it

  /* -------------------------------------------------------------- */
  /* 4. Post a task to the GUI thread                               */
  /* -------------------------------------------------------------- */
  g_guiTasks.push([title = std::move(title),
                   fn_source = std::move(fn_source)]() mutable {
    /* 4.1 Create QuickJS runtime _on the GUI thread_             */
    JSRuntime *gui_rt = JS_NewRuntime();
    SCR::register_class(gui_rt);
    JSContext *gui_ctx = JS_NewContext(gui_rt);
    SCR::install_ui_object(gui_ctx); // ui.text, etc.

    /* 4.2 Evaluate the callback source in this GUI context       */
    JSValue draw_cb = JS_Eval(gui_ctx, fn_source.c_str(), fn_source.size(),
                              "<draw_cb>", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(draw_cb)) { // broken script?
      JSValue exc = JS_GetException(gui_ctx);
      const char *msg = JS_ToCString(gui_ctx, exc);
      JS_FreeCString(gui_ctx, msg);
      JS_FreeValue(gui_ctx, exc);
      JS_FreeContext(gui_ctx);
      JS_FreeRuntime(gui_rt);
      return; // abort window
    }

    /* 4.3 Create the C++ ImGui window wrapper                    */
    auto win = std::make_shared<SCR::JSImGuiWindow>(title, gui_ctx, draw_cb);

    /* 4.4 Hand it to the renderer (GUI thread)                   */
    GUI::Renderer::renderer->PushWindow(std::move(win));
  });

  return JS_UNDEFINED; // worker thread returns immediately
}
/* Button ------------------------------------------------------------ */
JSValue ui_button(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "label expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  bool pressed = ImGui::Button(s);
  JS_FreeCString(c, s);
  return JS_NewBool(c, pressed);
}

/* Frame counter ----------------------------------------------------- */
JSValue ui_frame(JSContext *c, JSValueConst, int, JSValueConst *) {
  return JS_NewInt64(c, ImGui::GetFrameCount());
}

/* ------------------------------------------------------------------ */
/* Global object "ui" ------------------------------------------------ */
void install_ui_object(JSContext *ctx) {
  JSValue ui = JS_NewObject(ctx);
  JS_SetPropertyStr(
      ctx, ui, "create_window",
      JS_NewCFunction(ctx, SCR::js_create_window, "create_window", 2));

  // Existing functions
  JS_SetPropertyStr(ctx, ui, "text", JS_NewCFunction(ctx, ui_text, "text", 1));
  JS_SetPropertyStr(ctx, ui, "button",
                    JS_NewCFunction(ctx, ui_button, "button", 1));
  JS_SetPropertyStr(ctx, ui, "frame",
                    JS_NewCFunction(ctx, ui_frame, "frame", 0));

  // Text functions
  JS_SetPropertyStr(ctx, ui, "text_colored",
                    JS_NewCFunction(ctx, ui_text_colored, "text_colored", 4));
  JS_SetPropertyStr(ctx, ui, "text_wrapped",
                    JS_NewCFunction(ctx, ui_text_wrapped, "text_wrapped", 1));
  JS_SetPropertyStr(ctx, ui, "bullet_text",
                    JS_NewCFunction(ctx, ui_bullet_text, "bullet_text", 1));

  // Button functions
  JS_SetPropertyStr(ctx, ui, "small_button",
                    JS_NewCFunction(ctx, ui_small_button, "small_button", 1));
  JS_SetPropertyStr(
      ctx, ui, "invisible_button",
      JS_NewCFunction(ctx, ui_invisible_button, "invisible_button", 3));

  // Input functions
  JS_SetPropertyStr(ctx, ui, "input_text",
                    JS_NewCFunction(ctx, ui_input_text, "input_text", 2));
  JS_SetPropertyStr(ctx, ui, "input_int",
                    JS_NewCFunction(ctx, ui_input_int, "input_int", 2));
  JS_SetPropertyStr(ctx, ui, "input_float",
                    JS_NewCFunction(ctx, ui_input_float, "input_float", 2));

  // Slider functions
  JS_SetPropertyStr(ctx, ui, "slider_int",
                    JS_NewCFunction(ctx, ui_slider_int, "slider_int", 4));
  JS_SetPropertyStr(ctx, ui, "slider_float",
                    JS_NewCFunction(ctx, ui_slider_float, "slider_float", 4));

  // Checkbox and radio
  JS_SetPropertyStr(ctx, ui, "checkbox",
                    JS_NewCFunction(ctx, ui_checkbox, "checkbox", 2));
  JS_SetPropertyStr(ctx, ui, "radio_button",
                    JS_NewCFunction(ctx, ui_radio_button, "radio_button", 2));

  // Layout functions
  JS_SetPropertyStr(ctx, ui, "separator",
                    JS_NewCFunction(ctx, ui_separator, "separator", 0));
  JS_SetPropertyStr(ctx, ui, "same_line",
                    JS_NewCFunction(ctx, ui_same_line, "same_line", 1));
  JS_SetPropertyStr(ctx, ui, "new_line",
                    JS_NewCFunction(ctx, ui_new_line, "new_line", 0));
  JS_SetPropertyStr(ctx, ui, "spacing",
                    JS_NewCFunction(ctx, ui_spacing, "spacing", 0));
  JS_SetPropertyStr(ctx, ui, "dummy",
                    JS_NewCFunction(ctx, ui_dummy, "dummy", 2));

  // Tree and collapsing
  JS_SetPropertyStr(ctx, ui, "tree_node",
                    JS_NewCFunction(ctx, ui_tree_node, "tree_node", 1));
  JS_SetPropertyStr(ctx, ui, "tree_pop",
                    JS_NewCFunction(ctx, ui_tree_pop, "tree_pop", 0));
  JS_SetPropertyStr(
      ctx, ui, "collapsing_header",
      JS_NewCFunction(ctx, ui_collapsing_header, "collapsing_header", 1));

  // Progress bar
  JS_SetPropertyStr(ctx, ui, "progress_bar",
                    JS_NewCFunction(ctx, ui_progress_bar, "progress_bar", 3));

  // Image functions (your existing ones)
  JS_SetPropertyStr(ctx, ui, "load_image",
                    JS_NewCFunction(ctx, ui_load_image, "load_image", 1));
  JS_SetPropertyStr(ctx, ui, "image",
                    JS_NewCFunction(ctx, ui_image, "image", 3));
  JS_SetPropertyStr(
      ctx, ui, "is_key_pressed",
      JS_NewCFunction(ctx, SCR::ui_is_key_pressed, "is_key_pressed", 2));
  JS_SetPropertyStr(
      ctx, ui, "is_window_focused",
      JS_NewCFunction(ctx, SCR::ui_is_window_focused, "is_window_focused", 0));

  JS_SetPropertyStr(ctx, ui, "KEY_LEFT", JS_NewInt32(ctx, KEY_LEFT));
  JS_SetPropertyStr(ctx, ui, "KEY_RIGHT", JS_NewInt32(ctx, KEY_RIGHT));
  JS_SetPropertyStr(ctx, ui, "KEY_UP", JS_NewInt32(ctx, KEY_UP));
  JS_SetPropertyStr(ctx, ui, "KEY_DOWN", JS_NewInt32(ctx, KEY_DOWN));

  JS_SetPropertyStr(ctx, ui, "add_line",
                    JS_NewCFunction(ctx, ui_add_line, "add_line", 6));

  JS_SetPropertyStr(ctx, ui, "add_rect",
                    JS_NewCFunction(ctx, ui_add_rect, "add_rect", 8));
  JS_SetPropertyStr(
      ctx, ui, "add_rect_filled",
      JS_NewCFunction(ctx, ui_add_rect_filled, "add_rect_filled", 7));
  JS_SetPropertyStr(ctx, ui, "add_rect_filled_multi_color",
                    JS_NewCFunction(ctx, ui_add_rect_filled_multi_color,
                                    "add_rect_filled_multi_color", 8));

  JS_SetPropertyStr(ctx, ui, "add_circle",
                    JS_NewCFunction(ctx, ui_add_circle, "add_circle", 6));
  JS_SetPropertyStr(
      ctx, ui, "add_circle_filled",
      JS_NewCFunction(ctx, ui_add_circle_filled, "add_circle_filled", 5));

  JS_SetPropertyStr(ctx, ui, "add_ellipse",
                    JS_NewCFunction(ctx, ui_add_ellipse, "add_ellipse", 8));
  JS_SetPropertyStr(
      ctx, ui, "add_ellipse_filled",
      JS_NewCFunction(ctx, ui_add_ellipse_filled, "add_ellipse_filled", 7));

  JS_SetPropertyStr(ctx, ui, "add_ngon",
                    JS_NewCFunction(ctx, ui_add_ngon, "add_ngon", 6));
  JS_SetPropertyStr(
      ctx, ui, "add_ngon_filled",
      JS_NewCFunction(ctx, ui_add_ngon_filled, "add_ngon_filled", 5));

  JS_SetPropertyStr(ctx, ui, "add_quad",
                    JS_NewCFunction(ctx, ui_add_quad, "add_quad", 10));
  JS_SetPropertyStr(
      ctx, ui, "add_quad_filled",
      JS_NewCFunction(ctx, ui_add_quad_filled, "add_quad_filled", 9));

  JS_SetPropertyStr(ctx, ui, "add_triangle",
                    JS_NewCFunction(ctx, ui_add_triangle, "add_triangle", 8));
  JS_SetPropertyStr(
      ctx, ui, "add_triangle_filled",
      JS_NewCFunction(ctx, ui_add_triangle_filled, "add_triangle_filled", 7));

  JS_SetPropertyStr(
      ctx, ui, "add_bezier_cubic",
      JS_NewCFunction(ctx, ui_add_bezier_cubic, "add_bezier_cubic", 11));
  JS_SetPropertyStr(
      ctx, ui, "add_bezier_quadratic",
      JS_NewCFunction(ctx, ui_add_bezier_quadratic, "add_bezier_quadratic", 9));

  JS_SetPropertyStr(ctx, ui, "add_image",
                    JS_NewCFunction(ctx, ui_add_image, "add_image", 10));
  JS_SetPropertyStr(
      ctx, ui, "add_image_quad",
      JS_NewCFunction(ctx, ui_add_image_quad, "add_image_quad", 18));
  JS_SetPropertyStr(
      ctx, ui, "add_image_rounded",
      JS_NewCFunction(ctx, ui_add_image_rounded, "add_image_rounded", 11));
  JS_SetPropertyStr(ctx, ui, "get_cursor_screen_pos",
                    JS_NewCFunction(ctx, ui_get_cursor_screen_pos,
                                    "get_cursor_screen_pos", 0));

  /* Convenience name to avoid clashing with ui.text */
  JS_SetPropertyStr(ctx, ui, "add_text_dl",
                    JS_NewCFunction(ctx, ui_add_text, "add_text_dl", 4));
  JSValue global = JS_GetGlobalObject(ctx);
  JS_SetPropertyStr(ctx, global, "ui", ui);
  JS_FreeValue(ctx, global);
}

JSValue ui_get_cursor_screen_pos(JSContext *ctx, JSValueConst /*this_val*/,
                                 int /*argc*/, JSValueConst * /*argv*/) {
  ImVec2 p = ImGui::GetCursorScreenPos(); // <- built-in ImGui call
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "x", JS_NewFloat64(ctx, p.x));
  JS_SetPropertyStr(ctx, obj, "y", JS_NewFloat64(ctx, p.y));
  return obj;
}

JSValue ui_is_key_pressed(JSContext *c, JSValueConst, int argc,
                          JSValueConst *argv) {
  if (argc < 1 || !JS_IsNumber(argv[0]))
    return JS_ThrowTypeError(c, "key code expected");

  int key;
  JS_ToInt32(c, &key, argv[0]);
  bool repeat = false;
  if (argc >= 2)
    repeat = JS_ToBool(c, argv[1]);

  bool pressed = ImGui::IsKeyPressed((ImGuiKey)key, repeat);
  return JS_NewBool(c, pressed);
}

JSValue ui_is_window_focused(JSContext *c, JSValueConst, int, JSValueConst *) {
  bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
  return JS_NewBool(c, focused);
}

JSValue ui_load_image(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "path string expected");

  size_t len;
  const char *path_c = JS_ToCStringLen(c, &len, v[0]);
  std::string path(path_c, len);
  JS_FreeCString(c, path_c);

  bool is_url = (path.find("http://") == 0 || path.find("https://") == 0);
  fmt::print("[JS] Loading image: {} (is_url: {})\n", path, is_url);

  auto image = std::make_shared<CImage>(path, is_url);

  if (is_url) {
    fmt::print("[JS] Calling LoadImageFromURL...\n");
    image->LoadImageFromURL();
  } else {
    fmt::print("[JS] Calling LoadImage...\n");
    image->LoadImage();
  }

  fmt::print("[JS] After loading - ImageLoaded: {}\n", image->ImageLoaded);

  // Check if loading succeeded
  if (!image->ImageLoaded) {
    fmt::print("[JS] Loading failed for: {}\n", path);
    return JS_ThrowInternalError(c, "Failed to load image: %s", path.c_str());
  }

  // Store in cache and return handle
  int handle = g_next_image_id++;
  g_image_cache[handle] = image;

  fmt::print("[JS] Successfully loaded image, handle: {}\n", handle);
  return JS_NewInt32(c, handle);
}

JSValue ui_image(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsNumber(v[0]))
    return JS_ThrowTypeError(c, "image handle (number) expected");

  int handle;
  JS_ToInt32(c, &handle, v[0]);

  // Find the image
  auto it = g_image_cache.find(handle);
  if (it == g_image_cache.end())
    return JS_ThrowReferenceError(c, "invalid image handle: %d", handle);

  auto &image = it->second;

  // Get optional width/height
  float width = 100.0f, height = 100.0f;
  if (argc >= 3) {
    double w, h;
    JS_ToFloat64(c, &w, v[1]);
    JS_ToFloat64(c, &h, v[2]);
    width = static_cast<float>(w);
    height = static_cast<float>(h);
  }

  // Render with ImGui (image is already loaded)
  ImGui::Image(image->GetDataRaw(), ImVec2(width, height));
  return JS_UNDEFINED;
}

JSValue ui_text_colored(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 4 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "text_colored(text, r, g, b) expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);

  double r, g, b;
  JS_ToFloat64(c, &r, v[1]);
  JS_ToFloat64(c, &g, v[2]);
  JS_ToFloat64(c, &b, v[3]);

  ImGui::TextColored(ImVec4((float)r, (float)g, (float)b, 1.0f), "%s", s);
  JS_FreeCString(c, s);
  return JS_UNDEFINED;
}

JSValue ui_text_wrapped(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "string expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  ImGui::TextWrapped("%s", s);
  JS_FreeCString(c, s);
  return JS_UNDEFINED;
}

JSValue ui_bullet_text(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "string expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  ImGui::BulletText("%s", s);
  JS_FreeCString(c, s);
  return JS_UNDEFINED;
}

/* ------------------------------------------------------------------ */
/* Button functions ------------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_small_button(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "label expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  bool pressed = ImGui::SmallButton(s);
  JS_FreeCString(c, s);
  return JS_NewBool(c, pressed);
}

JSValue ui_invisible_button(JSContext *c, JSValueConst, int argc,
                            JSValueConst *v) {
  if (argc < 3 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "invisible_button(id, width, height) expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);

  double w, h;
  JS_ToFloat64(c, &w, v[1]);
  JS_ToFloat64(c, &h, v[2]);

  bool pressed = ImGui::InvisibleButton(s, ImVec2((float)w, (float)h));
  JS_FreeCString(c, s);
  return JS_NewBool(c, pressed);
}

/* ------------------------------------------------------------------ */
/* Input functions -------------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_input_text(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsString(v[0]) || !JS_IsString(v[1]))
    return JS_ThrowTypeError(c, "input_text(label, text) expected");

  size_t label_len, text_len;
  const char *label = JS_ToCStringLen(c, &label_len, v[0]);
  const char *text = JS_ToCStringLen(c, &text_len, v[1]);

  char buffer[256];
  strncpy(buffer, text, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  bool changed = ImGui::InputText(label, buffer, sizeof(buffer));

  JS_FreeCString(c, label);
  JS_FreeCString(c, text);

  if (changed) {
    return JS_NewString(c, buffer);
  }
  return JS_NewString(c, text);
}

JSValue ui_input_int(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsString(v[0]) || !JS_IsNumber(v[1]))
    return JS_ThrowTypeError(c, "input_int(label, value) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);

  int value;
  JS_ToInt32(c, &value, v[1]);

  bool changed = ImGui::InputInt(label, &value);
  JS_FreeCString(c, label);

  return JS_NewInt32(c, value);
}

JSValue ui_input_float(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsString(v[0]) || !JS_IsNumber(v[1]))
    return JS_ThrowTypeError(c, "input_float(label, value) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);

  double d_value;
  JS_ToFloat64(c, &d_value, v[1]);
  float value = (float)d_value;

  bool changed = ImGui::InputFloat(label, &value);
  JS_FreeCString(c, label);

  return JS_NewFloat64(c, (double)value);
}

/* ------------------------------------------------------------------ */
/* Slider functions ------------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_slider_int(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 4 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "slider_int(label, value, min, max) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);

  int value, min_val, max_val;
  JS_ToInt32(c, &value, v[1]);
  JS_ToInt32(c, &min_val, v[2]);
  JS_ToInt32(c, &max_val, v[3]);

  bool changed = ImGui::SliderInt(label, &value, min_val, max_val);
  JS_FreeCString(c, label);

  return JS_NewInt32(c, value);
}

JSValue ui_slider_float(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 4 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c,
                             "slider_float(label, value, min, max) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);

  double d_value, d_min, d_max;
  JS_ToFloat64(c, &d_value, v[1]);
  JS_ToFloat64(c, &d_min, v[2]);
  JS_ToFloat64(c, &d_max, v[3]);

  float value = (float)d_value;
  float min_val = (float)d_min;
  float max_val = (float)d_max;

  bool changed = ImGui::SliderFloat(label, &value, min_val, max_val);
  JS_FreeCString(c, label);

  return JS_NewFloat64(c, (double)value);
}

/* ------------------------------------------------------------------ */
/* Checkbox and radio ----------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_checkbox(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsString(v[0]) || !JS_IsBool(v[1]))
    return JS_ThrowTypeError(c, "checkbox(label, checked) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);
  bool checked = JS_ToBool(c, v[1]);

  bool changed = ImGui::Checkbox(label, &checked);
  JS_FreeCString(c, label);

  return JS_NewBool(c, checked);
}

JSValue ui_radio_button(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsString(v[0]) || !JS_IsBool(v[1]))
    return JS_ThrowTypeError(c, "radio_button(label, active) expected");

  size_t len;
  const char *label = JS_ToCStringLen(c, &len, v[0]);
  bool active = JS_ToBool(c, v[1]);

  bool clicked = ImGui::RadioButton(label, active);
  JS_FreeCString(c, label);

  return JS_NewBool(c, clicked);
}

/* ------------------------------------------------------------------ */
/* Layout functions ------------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_separator(JSContext *c, JSValueConst, int, JSValueConst *) {
  ImGui::Separator();
  return JS_UNDEFINED;
}

JSValue ui_same_line(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc >= 1 && JS_IsNumber(v[0])) {
    double offset;
    JS_ToFloat64(c, &offset, v[0]);
    ImGui::SameLine((float)offset);
  } else {
    ImGui::SameLine();
  }
  return JS_UNDEFINED;
}

JSValue ui_new_line(JSContext *c, JSValueConst, int, JSValueConst *) {
  ImGui::NewLine();
  return JS_UNDEFINED;
}

JSValue ui_spacing(JSContext *c, JSValueConst, int, JSValueConst *) {
  ImGui::Spacing();
  return JS_UNDEFINED;
}

JSValue ui_dummy(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 2 || !JS_IsNumber(v[0]) || !JS_IsNumber(v[1]))
    return JS_ThrowTypeError(c, "dummy(width, height) expected");

  double w, h;
  JS_ToFloat64(c, &w, v[0]);
  JS_ToFloat64(c, &h, v[1]);

  ImGui::Dummy(ImVec2((float)w, (float)h));
  return JS_UNDEFINED;
}

/* ------------------------------------------------------------------ */
/* Tree and collapsing ---------------------------------------------- */
/* ------------------------------------------------------------------ */
JSValue ui_tree_node(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "string expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  bool open = ImGui::TreeNode(s);
  JS_FreeCString(c, s);
  return JS_NewBool(c, open);
}

JSValue ui_tree_pop(JSContext *c, JSValueConst, int, JSValueConst *) {
  ImGui::TreePop();
  return JS_UNDEFINED;
}

JSValue ui_collapsing_header(JSContext *c, JSValueConst, int argc,
                             JSValueConst *v) {
  if (argc < 1 || !JS_IsString(v[0]))
    return JS_ThrowTypeError(c, "string expected");

  size_t len;
  const char *s = JS_ToCStringLen(c, &len, v[0]);
  bool open = ImGui::CollapsingHeader(s);
  JS_FreeCString(c, s);
  return JS_NewBool(c, open);
}

/* ------------------------------------------------------------------ */
/* Progress bar ------------------------------------------------------ */
/* ------------------------------------------------------------------ */
JSValue ui_progress_bar(JSContext *c, JSValueConst, int argc, JSValueConst *v) {
  if (argc < 1 || !JS_IsNumber(v[0]))
    return JS_ThrowTypeError(c, "progress_bar(fraction) expected");

  double fraction;
  JS_ToFloat64(c, &fraction, v[0]);

  if (argc >= 3 && JS_IsNumber(v[1]) && JS_IsNumber(v[2])) {
    double w, h;
    JS_ToFloat64(c, &w, v[1]);
    JS_ToFloat64(c, &h, v[2]);
    ImGui::ProgressBar((float)fraction, ImVec2((float)w, (float)h));
  } else {
    ImGui::ProgressBar((float)fraction);
  }

  return JS_UNDEFINED;
}

} // namespace SCR
