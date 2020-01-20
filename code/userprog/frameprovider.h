#include "copyright.h"
#include "system.h"
#include "bitmap.h"


class FrameProvider {
  public:
    
    FrameProvider(int size);
    ~FrameProvider();

    int GetEmptyFrame(bool randomness);
    void ReleaseFrame(int byte);
    int NumAvailFrame();

  private:
    BitMap *bitmapFrame;

};
