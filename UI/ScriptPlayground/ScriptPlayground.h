#pragma once
#include "./MATH/Vector2D.h"
#include "UI/INormalWindow.h"
#include <memory>
#include <string>

namespace GUI {
class CScriptPlayground : public IWindow {
public:
  CScriptPlayground() = default;
  void Draw() override;

  void SetWindowSize(const MATH::Vector2D<int> &size) override;
  MATH::Vector2D<int> GetWindowSize() override;
  void SetWindowPos(const MATH::Vector2D<int> &pos) override;
  MATH::Vector2D<int> GetWindowPos() override;

  void ResizeWindowScaled(MATH::Vector2D<int> &newwindowsize) override {
    MATH::Vector2D<int> despos = GetDesiredPos();
    SetWindowPos(despos);
  }

  void SetVisible(bool visible) { isVisible = visible; }
  bool IsVisible() const { return isVisible; }

  static bool isVisible;

private:
  MATH::Vector2D<int> windowSize = MATH::Vector2D<int>(600, 400);
  MATH::Vector2D<int> windowPos = MATH::Vector2D<int>(100, 100);

  std::string scriptContent = "";
  std::string outputContent = "";
  bool autoExecute = false;

  bool showEditor = false;
  std::string newFileName;
  std::string editorBuf;
  bool isNewFile;
  std::string filenamebackup;
  void DrawEditor();

public:
  MATH::Vector2D<int> GetDesiredPos() { return MATH::Vector2D<int>{150, 150}; }
  MATH::Vector2D<int> GetDesiredSize() { return MATH::Vector2D<int>{600, 400}; }

  void SetScriptContent(const std::string &content) { scriptContent = content; }
  const std::string &GetScriptContent() const { return scriptContent; }
  void ClearOutput() { outputContent.clear(); }
};
}
