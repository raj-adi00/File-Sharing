#pragma once
#include<chrono>
#include<cstddef>

using namespace std;

class ProgressTracker{
    public:
      ProgressTracker(size_t totalBytes,size_t initialOffset);
      void update(size_t bytesTransferred);
      void render();
    private:
      size_t total;
      size_t transferred;
      size_t offset;

      chrono::steady_clock::time_point start;
};