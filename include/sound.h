#ifndef _SCOSOUND_H_
#define _SCOSOUND_H_

#include "oggplayer.h"
#include <fcntl.h>

extern MODPlay mod_track;

void initSound();
void playDefaultLevelMusic();
void playTitleScreenMusic();
void playLevelCleared();
void playOggMusic();
void playClick();
void playError();
void playBloop();
int loadCustomSongs();
bool checkOggExt(char* s1);
void shuffle(int len);
void swap(int a, int b);

#endif
