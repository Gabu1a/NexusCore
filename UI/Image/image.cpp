#include "UTILS/ThreadPool.h"
#define STB_IMAGE_IMPLEMENTATION
#include "image.h"

void CImageLoader::CreateThreadPool(const std::size_t &threadsnr) {
  // CImageLoader::_threadPool = ThreadPool{threadsnr};
}

void CImageLoader::AddImage(CImage *image) {
  // CImageLoader::_threadPool.Add(&image->LoadImage);
}
