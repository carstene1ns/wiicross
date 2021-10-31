#include "wiicross.h"
#include "sound.h"

#include "snd_titlescreen_mod.h"
#include "snd_bg0_mod.h"
#include "snd_bg1_mod.h"
#ifdef MAKE_WII
#include "snd_levelcleared_ogg.h"
#endif
#include "snd_levelcleared_raw.h"
#include "snd_click_raw.h"
#include "snd_error_raw.h"
#include "snd_bloop_raw.h"

MODPlay mod_track;
s_song song;

extern s_option options;

void VoiceCallBack(AESNDPB *pb, u32 state) {
	// Free the voice if end reached
	if (state == VOICE_STATE_STOPPED) AESND_FreeVoice(pb);
}

void initSound(){

	#ifdef IS_EMU
	return;
	#endif

	AESND_Init();
	MODPlay_Init(&mod_track);
	
	song.totalSongs = loadCustomSongs();
	shuffle(song.totalSongs);
}

void playDefaultLevelMusic(){

	#ifdef IS_EMU
	return;
	#endif
	AESND_Pause(false); // the sound loop is running now

	int rand;
	rand = getRandomNum(0, 1);
	
	if(rand == 0){
		if(MODPlay_SetMOD(&mod_track, snd_bg0_mod ) < 0 ){
			MODPlay_Unload(&mod_track);
		}
		else{
			MODPlay_SetVolume(&mod_track, 64,64);
			MODPlay_Start(&mod_track); // Play the MOD
		}
	}
	else{
		if(MODPlay_SetMOD(&mod_track, snd_bg1_mod ) < 0 ){
			MODPlay_Unload(&mod_track);
		}
		else{
			MODPlay_SetVolume(&mod_track, 64,64);
			MODPlay_Start(&mod_track); // Play the MOD
		}
	}
}

void playTitleScreenMusic(){

	#ifdef IS_EMU
	return;
	#endif
	
	#ifdef MAKE_WII
	if(options.musicType == MUSIC_CUSTOM){
		StopOgg();
	}
	#endif
	
	AESND_Pause(0); // the sound loop is running now

	if(MODPlay_SetMOD(&mod_track, snd_titlescreen_mod ) < 0 ){
		MODPlay_Unload(&mod_track);
	}
	else{
		MODPlay_SetVolume(&mod_track, 64,64);
		MODPlay_Start(&mod_track);
	}
}

void playLevelCleared(){

	#ifdef IS_EMU
	return;
	#endif

	#ifdef MAKE_WII
	PlayOggMem(snd_levelcleared_ogg, snd_levelcleared_ogg_size, 0, OGG_ONE_TIME);
	#else
	AESNDPB* this_voice = AESND_AllocateVoice(VoiceCallBack);
	AESND_PlayVoice(this_voice, VOICE_MONO8, snd_levelcleared_raw, snd_levelcleared_raw_size, 8000, 0, 0);
	#endif
}

void playOggMusic(){

	#ifdef IS_EMU
	return;
	#endif
	
	#ifdef MAKE_WII
	if(song.totalSongs == 0)
		return;
	
	char sng[200];	
	
	sprintf(sng, DIR_ROOT "res/music/%s", song.songsArray[getRandomNum(0, song.totalSongs-1)]);
	
	if(access(sng, F_OK) != 0){
		//breakpoint(strerror(errno), errno);
		//breakpoint(sng, 123);
		return;
	}

	PlayOggFile(sng, 0, OGG_ONE_TIME);
	#endif
}

int loadCustomSongs(){

	DIR* dir;
	struct dirent* st;
	dir = opendir(DIR_ROOT "res/music");
	
	int i = 0;
	
	if(dir == NULL){
		return -1;
	}
	
	while((st = readdir(dir)) != NULL) {
		if((strlen(st->d_name) > 2) && (st->d_type == DT_REG)) {
			if(checkOggExt(st->d_name)){
				strcpy(song.songsArray[i], st->d_name);
				i++;
			}
		}
	}
	
	closedir(dir);
	
	return i;
}

void playClick(){
	AESNDPB* this_voice = AESND_AllocateVoice(VoiceCallBack);
	AESND_PlayVoice(this_voice, VOICE_MONO8, snd_click_raw, snd_click_raw_size, 8000, 0, 0);
}

void playError(){
	AESNDPB* this_voice = AESND_AllocateVoice(VoiceCallBack);
	AESND_PlayVoice(this_voice, VOICE_MONO8, snd_error_raw, snd_error_raw_size, 8000, 0, 0);
}

void playBloop(){
	AESNDPB* this_voice = AESND_AllocateVoice(VoiceCallBack);
	AESND_PlayVoice(this_voice, VOICE_MONO8, snd_bloop_raw, snd_bloop_raw_size, 8000, 0, 0);
}

bool checkOggExt(char* s1){
    
    if((s1[0] != 'o' && s1[0] != 'O') || (s1[1] != 'g' && s1[1] != 'G') || (s1[2] != 'g' && s1[2] != 'G'))
      return true;
    
    return false;
}

void shuffle(int len){
	
	int i;
	
	for(i=0; i<len; i++){
		int a = getRandomNum(0, len-1);
        int b = getRandomNum(0, len-1);
		swap(a, b);
	}
}

void swap(int a, int b){

	char temp[256];
	
	sprintf(temp, song.songsArray[a]);
	sprintf(song.songsArray[a], song.songsArray[b]);
	sprintf(song.songsArray[b], temp);
}

bool checkMusicPlaying(){

	#ifndef IS_EMU
	#ifdef MAKE_WII
	if(options.musicType == MUSIC_CUSTOM)
		return (StatusOgg() == OGG_STATUS_RUNNING);
	else
	#endif
	if(options.musicType == MUSIC_ON)
		return (mod_track.playing == 1);
	#endif

	return false;
}

void stopMusic(int musicType){

	#ifdef IS_EMU
	return;
	#endif

	#ifdef MAKE_WII
	if(musicType == MUSIC_CUSTOM)
		StopOgg();
	else
	#endif
	if(musicType == MUSIC_ON)
		MODPlay_Stop(&mod_track);
}
