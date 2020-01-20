#include "frameprovider.h"

FrameProvider::FrameProvider(int size)
{
	bitmapFrame = new BitMap(size);
}

FrameProvider::~FrameProvider()
{
	delete bitmapFrame;
}

int FrameProvider::GetEmptyFrame(bool randomness)
{

	int tempFrame;
	tempFrame = bitmapFrame->Find();
	if(randomness){
		do
			tempFrame = rand() % ((int)(MemorySize/PageSize)) + 1;
		while(tempFrame > NumAvailFrame());
	}
	//bzero(tempFrame, sizeof(int));
	bzero (machine->mainMemory, MemorySize);
	bitmapFrame->Mark(tempFrame);
	//printf("\n==================TEMPFRAME = %d==================\n", tempFrame);
	return tempFrame;
}

void FrameProvider::ReleaseFrame(int byte)
{
	bitmapFrame->Clear(byte);
}

int FrameProvider::NumAvailFrame()
{
	return bitmapFrame->NumClear();
}
