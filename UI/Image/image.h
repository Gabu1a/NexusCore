#ifndef _C_IMAGE
#define _C_IMAGE
#include "../../Dependencies/ImGui/imgui.h"
#include "../../Dependencies/stb/stb_image.h"
#include "../../NETWORKING/CNetworking.h"
#include "fmt/base.h"
#include <GL/gl.h>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
struct CImage {
public:
  CImage(const std::string &path) : path(path) {}
  CImage(const std::string &path, bool is_url)
      : path(path), is_url_source(is_url) {}
  CImage(const unsigned char *buf, size_t len) {
    // Detect GIF header
    bool gif =
        len >= 6 && (!memcmp(buf, "GIF87a", 6) || !memcmp(buf, "GIF89a", 6));

    if (gif) {
      isGif = true;
      data = stbi_load_gif_from_memory(buf, len, &delays, &width, &height,
                                       &frameCount, &channel, 4);
      channel = 4;
    } else {
      data = stbi_load_from_memory(buf, (int)len, &width, &height, &channel, 4);
      if (data) {
        backup.assign(data, data + width * height * 4);
        keepBackup = true;
      }
    }

    if (data)
      CreateTexture();
    else
      fmt::print("embedded decode failed\n");
  }

  ~CImage() {
    if (data) {
      stbi_image_free(data);
      data = nullptr;
    }
    if (delays) {
      stbi_image_free(delays);
      delays = nullptr;
    }
  }

  std::size_t ComputeHash() const {
    std::ostringstream oss;
    oss << width << height << channel;
    for (std::size_t i = 0; i < width * height * channel; i++) {
      oss << static_cast<int>(data[i]);
    }
    std::string str = oss.str();
    return std::hash<std::string>{}(str);
  }

  void LoadImage() {
    // Check if it's a GIF
    if (path.size() >= 4 && (path.substr(path.size() - 4) == ".gif" ||
                             path.substr(path.size() - 4) == ".GIF")) {
      LoadGIF();
    } else {
      data = stbi_load(path.c_str(), &width, &height, &channel, 4);
      if (data) {
        backup.assign(data, data + width * height * 4); // save pristine copy
        keepBackup = true;
        CreateTexture();
      } else {
        fmt::print("failed to load {}\n", path);
      }
    }
  }

  void Recolour(ImVec4 from, ImVec4 to, float tol = 0.05f) {
    if (!keepBackup)
      return; // we only support static RGBA

    const uint8_t fr = uint8_t(from.x * 255.f);
    const uint8_t fg = uint8_t(from.y * 255.f);
    const uint8_t fb = uint8_t(from.z * 255.f);

    const uint8_t tr = uint8_t(to.x * 255.f);
    const uint8_t tg = uint8_t(to.y * 255.f);
    const uint8_t tb = uint8_t(to.z * 255.f);

    const int thr = int(tol * 255.f);

    std::memcpy(data, backup.data(), backup.size());

    uint8_t *p = data;
    for (int i = 0; i < width * height; ++i, p += 4) {
      if (std::abs(int(p[0]) - fr) <= thr && std::abs(int(p[1]) - fg) <= thr &&
          std::abs(int(p[2]) - fb) <= thr) {
        p[0] = tr;
        p[1] = tg;
        p[2] = tb;
      }
    }

    // Re-upload to the already created texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
                    GL_UNSIGNED_BYTE, data);
  }

  void LoadImageFromURL() {
    try {
      std::string img = Curl::Get(path);

      // Check if it's a GIF by looking at header
      if (img.size() >= 6 && (memcmp(img.data(), "GIF87a", 6) == 0 ||
                              memcmp(img.data(), "GIF89a", 6) == 0)) {
        LoadGIFFromMemory((const unsigned char *)img.data(), img.size());
      } else {
        data = stbi_load_from_memory((const unsigned char *)img.data(),
                                     (int)img.size(), &width, &height, &channel,
                                     4);
        if (data) {
          backup.assign(data, data + width * height * 4);
          keepBackup = true;
          CreateTexture();
        } else {
          fmt::print("decode failed for {}\n", path);
        }
      }
    } catch (const std::exception &e) {
      fmt::print("HTTP error: {}\n", e.what());
    }
  }

  void UpdateAnimation(float deltaTime) {
    if (!isGif || frameCount <= 1 || !delays)
      return;

    elapsedTime += deltaTime * 1000.0f;

    if (elapsedTime >= delays[currentFrame]) {
      elapsedTime = 0.0f;
      currentFrame = (currentFrame + 1) % frameCount;
      UpdateCurrentFrame();
    }
  }

  void *GetDataRaw() {
    return ImageLoaded ? (void *)(intptr_t)texture : nullptr;
  }

  float GetWidth() { return this->width; }
  float GetHeight() { return this->height; }

  bool IsAnimated() const { return isGif && frameCount > 1; }
  bool ImageLoaded = false;

private:
  void LoadGIF() {
    FILE *file = fopen(path.c_str(), "rb");
    if (!file) {
      fmt::print("failed to open {}\n", path);
      return;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<unsigned char> buffer(size);
    fread(buffer.data(), 1, size, file);
    fclose(file);

    LoadGIFFromMemory(buffer.data(), size);
  }

  void LoadGIFFromMemory(const unsigned char *buffer, size_t size) {
    isGif = true;

    data = stbi_load_gif_from_memory(buffer, size, &delays, &width, &height,
                                     &frameCount, &channel, 4);

    if (data) {
      channel = 4;
      currentFrame = 0;
      elapsedTime = 0.0f;
      CreateTexture();
    } else {
      fmt::print("failed to load GIF {}\n", path);
      isGif = false;
    }
  }

  void CreateTexture() {
    if (!data)
      return;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, GetCurrentFrameData());

    ImageLoaded = true;
  }

  void UpdateCurrentFrame() {
    if (!ImageLoaded || !isGif)
      return;

    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum fmt = (channel == 4) ? GL_RGBA : GL_RGB;
    unsigned char *frameData = GetCurrentFrameData();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, fmt,
                    GL_UNSIGNED_BYTE, frameData);
  }

  unsigned char *GetCurrentFrameData() {
    if (!data || !isGif || frameCount <= 1) {
      return data;
    }

    size_t frameSize = width * height * channel;
    return data + (currentFrame * frameSize);
  }

  std::string path;
  int width = -1;
  int height = -1;
  int channel = -1;
  unsigned char *data = nullptr;
  GLuint texture = -1;
  std::mutex mutex;
  std::condition_variable cv;
  bool LoadingStarted = false;
  bool is_url_source = false;

  bool isGif = false;
  int *delays = nullptr;
  int frameCount = 1;
  int currentFrame = 0;
  float elapsedTime = 0.0f;

  // Recoloring members
  std::vector<uint8_t> backup;
  bool keepBackup = false;
};

class CImageLoader {
public:
  static void CreateThreadPool(const std::size_t &);
  static void AddImage(CImage *image);

private:
  // static ThreadPool _threadPool;
};
#endif
