#include "bitmap.h"
#include <strings.h>	
#include "frameprovider.h"
#include "system.h"


static Semaphore *semFrame = new Semaphore("semFrame",1);

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
	semFrame->P();
	if((bitmapFrame->NumClear())<=0){
		semFrame->V();
		return -1;
	}
	//Vesrion aléatoire
//	RandomInit(Time());
//	int tmp=Random()%(BitmapSize-1);
//	while(phyMemBitmap->Test(tmp)){
//		tmp=Random()%(BitmapSize-1);
//	}
//	phyMemBitmap->Mark(tmp);
	//Version non aléatoire
	int tmp = bitmapFrame->Find();
	semFrame->V();
	bzero(&machine->mainMemory[tmp*PageSize], PageSize);
	return tmp;
}

void FrameProvider::ReleaseFrame(int byte)
{
	bitmapFrame->Clear(byte);
}

int FrameProvider::NumAvailFrame()
{
	return bitmapFrame->NumClear();
}

