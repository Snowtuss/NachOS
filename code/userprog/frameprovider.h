#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"
#include <strings.h>        

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


#endif // FRAMEPROVIDER_H