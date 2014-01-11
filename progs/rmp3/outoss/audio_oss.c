
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define MOD_OUTPUT
#include "../mod_fun.h"

//#include "mpg123.h"

#ifndef AFMT_S16_NE
# ifdef OSS_BIG_ENDIAN
#  define AFMT_S16_NE AFMT_S16_BE
# else
#  define AFMT_S16_NE AFMT_S16_LE
# endif
#endif

#ifndef AFMT_U16_NE
# ifdef OSS_BIG_ENDIAN
#  define AFMT_U16_NE AFMT_U16_BE
# else
#  define AFMT_U16_NE AFMT_U16_LE
# endif
#endif

//extern int outburst;

#include <sys/ioctl.h>
#ifdef linux
#include <linux/soundcard.h>
#elif defined(__bsdi__)
#include <sys/soundcard.h>
#else
#include <machine/soundcard.h>
#endif
// UWAGA ! w programie Parametry s± ustawiane przed otworzeniem urz±dzenia
// Domy¶lnie powinno nastêpowaæ otworzenie z ustawieniem ustalonych parametrów
int filedevice = -1;
int format = -1, channels = -1, rate = -1;
#define SOUNDDEVICE "/dev/dsp"

int audio_rate_best_match()
{
  int ret,dsp_rate;

  if ( filedevice < 0 || rate < 0 )
	  return -1;
  dsp_rate = rate;
  ret = ioctl(filedevice, SNDCTL_DSP_SPEED,&dsp_rate);
  if(ret < 0)
    return ret;
  rate = dsp_rate;
  return 0;
}

int audio_set_rate()
{
  int ret = 0;

  if(rate >= 0) {
    ret = ioctl(filedevice, SNDCTL_DSP_SPEED,&rate);
    if ( ret == -1 )
	fprintf( stderr,
		"B³±d nadania parametru RATE %s %d\n",
		__FILE__, __LINE__ );
  }
  return ret;
}

int audio_set_channels()
{
  int chan = channels - 1;
  int ret;

  if(channels < 0)
    return 0;

  ret = ioctl(filedevice, SNDCTL_DSP_STEREO, &chan);
  if ( ret == -1 )
	fprintf( stderr,
		"B³±d nadania parametru CHANNELS %s %d\n",
		__FILE__, __LINE__ );
  if(chan != (channels-1)) {
    return -1;
  }
  return ret;
}

int audio_set_format()
{
  int sample_size,fmts;
  int sf,ret;

  if(format == -1)
    return 0;

  switch(format) {
    case AUDIO_FORMAT_SIGNED_16:
    default:
      fmts = AFMT_S16_NE;
      sample_size = 16;
      break;
    case AUDIO_FORMAT_UNSIGNED_8:
      fmts = AFMT_U8;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_SIGNED_8:
      fmts = AFMT_S8;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_ULAW_8:
      fmts = AFMT_MU_LAW;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_ALAW_8:
      fmts = AFMT_A_LAW;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_UNSIGNED_16:
      fmts = AFMT_U16_NE;
      break;
  }
#if 0
  if(ioctl(ai->fn, SNDCTL_DSP_SAMPLESIZE,&sample_size) < 0)
    return -1;
#endif
  sf = fmts;
  ret = ioctl(filedevice, SNDCTL_DSP_SETFMT, &fmts);
  if ( ret == -1 )
  {
		fprintf( stderr,
			"B³±d nadania parametru FORMAT %s %d\n",
			__FILE__, __LINE__);
  }
  if(sf != fmts) {
    return -1;
  }
  return ret;
}

/* !!! pamiêtaæ o zainicjowaniu parametrów !!! */
int outputInit()
{
	int ret;
	filedevice = open( SOUNDDEVICE, O_WRONLY );
	if ( filedevice == -1 )
		return -1;
	ret = ioctl( filedevice, SNDCTL_DSP_RESET, NULL );
	if(ret < 0)
	{
		fprintf(stderr,"Can't reset audio!\n");
		return -1;
	}
	if ( audio_set_format() < 0 )
		return -1;
	if ( audio_set_rate() < 0 )
		return -1;
	if ( audio_set_channels() < 0 )
		return -1;
	return 0;
}

void outputReopen()
{
	outputCleanup();
	outputInit();
}

/*
int audio_open(struct audio_info_struct *ai)
{
  if(!ai)
    return -1;

  if(!ai->device)
    ai->device = "/dev/dsp";

  ai->fn = open(ai->device,O_WRONLY);  

  if(ai->fn < 0)
  {
    fprintf(stderr,"Can't open %s!\n",ai->device);
    exit(1);
  }

  if(audio_reset_parameters(ai) < 0) {
    close(ai->fn);
    return -1;
  }

  outburst = 512;

  if(ai->gain >= 0) {
    int e,mask;
    e = ioctl(ai->fn , SOUND_MIXER_READ_DEVMASK ,&mask);
    if(e < 0) {
      fprintf(stderr,"audio/gain: Can't get audio device features list.\n");
    }
    else if(mask & SOUND_MASK_PCM) {
      int gain = (ai->gain<<8)|(ai->gain);
      e = ioctl(ai->fn, SOUND_MIXER_WRITE_PCM , &gain);
    }
    else if(!(mask & SOUND_MASK_VOLUME)) {
      fprintf(stderr,"audio/gain: setable Volume/PCM-Level not supported by your audio device: %#04x\n",mask);
    }
    else { 
      int gain = (ai->gain<<8)|(ai->gain);
      e = ioctl(ai->fn, SOUND_MIXER_WRITE_VOLUME , &gain);
    }
  }

  return ai->fn;
}

int audio_reset_parameters(struct audio_info_struct *ai)
{
  int ret;
  ret = ioctl(ai->fn,SNDCTL_DSP_RESET,NULL);
  if(ret < 0)
    fprintf(stderr,"Can't reset audio!\n");
  ret = audio_set_format(ai);
  ret = audio_set_channels(ai);
  ret = audio_set_rate(ai);

  return ret;
}

static int audio_get_parameters(struct audio_info_struct *ai)
{
	int c=-1;
	int r=-1;
	int f=-1;

	if(ioctl(ai->fn,SNDCTL_DSP_SPEED,&r) < 0)
		return -1;
	if(ioctl(ai->fn,SNDCTL_DSP_STEREO,&c) < 0)
		return -1;
	if(ioctl(ai->fn,SNDCTL_DSP_SETFMT,&f) < 0)
		return -1;

	ai->rate = r;
	ai->channels = c + 1;
	ai->format = f;

	return 0;
}


*/

int audio_get_formats()
{
  int fmt = 0;
  int r = rate;
  int c = channels;
  int i;

  static int fmts[] = { 
	AUDIO_FORMAT_ULAW_8 , AUDIO_FORMAT_SIGNED_16 ,
	AUDIO_FORMAT_UNSIGNED_8 , AUDIO_FORMAT_SIGNED_8 ,
	AUDIO_FORMAT_UNSIGNED_16 , AUDIO_FORMAT_ALAW_8 };
	
  for(i=0;i<6;i++) {
	format = fmts[i];
	if(audio_set_format() < 0) {
		continue;
        }
	channels = c;
	if(audio_set_channels() < 0) {
		continue;
	}
	rate = r;
	if(audio_rate_best_match() < 0) {
		continue;
	}
	if( (rate*100 > r*(100-AUDIO_RATE_TOLERANCE)) && (rate*100 < r*(100+AUDIO_RATE_TOLERANCE)) ) {
		fmt |= fmts[i];
	}
  }

  return fmt;
}
/*
int audio_play_samples(struct audio_info_struct *ai,unsigned char *buf,int len)
{
#ifdef PPC_ENDIAN
#define BYTE0(n) ((unsigned char)(n) & (0xFF))
#define BYTE1(n) BYTE0((unsigned int)(n) >> 8)
#define BYTE2(n) BYTE0((unsigned int)(n) >> 16)
#define BYTE3(n) BYTE0((unsigned int)(n) >> 24)
   {
     register int i;
     int swappedInt;
     int *intPtr;

     intPtr = (int *)buf;

     for (i = 0; i < len / sizeof(int); i++)
       {
         swappedInt = (BYTE0(*intPtr) << 24 |
                       BYTE1(*intPtr) << 16 |
                       BYTE2(*intPtr) <<  8 |
                       BYTE3(*intPtr)         );

         *intPtr = swappedInt;
         intPtr++;
       }
    }
#endif

  return write(ai->fn,buf,len);
}

int audio_close(struct audio_info_struct *ai)
{
  close (ai->fn);
  return 0;
}
*/

void outputCleanup()
{
	close( filedevice );
}

int outputPlaySample(const unsigned char *pBuf, unsigned int cBuf)
{
	return write( filedevice, pBuf, cBuf );
}

int outputGetParameter(int param)
{
	switch (param)
	{
		case OUTPARAM_RATE:
			return rate;
		case OUTPARAM_CHANNELS:
			return channels;
		case OUTPARAM_FORMAT:
			return format;
		case OUTPARAM_GETFORMATS:
			return audio_get_formats();
		default:
			fprintf(stderr, "Nierozpoznany parametr %s %d\n",
				__FILE__, __LINE__ );
			return;
	}
}

void outputSetParameter(int param, void *value)
{
	switch (param)
	{
		case OUTPARAM_RATE:
			rate = *((int*)value);
			break;
		case OUTPARAM_CHANNELS:
			channels = *((int*)value);
			break;
		case OUTPARAM_FORMAT:
			format = *((int*)value);
			break;
		case OUTPARAM_QUEUEFLUSH: // No action
			break;
		default:
			fprintf(stderr, "Nierozpoznany parametr %s %d\n",
				__FILE__, __LINE__ );
			return;
	}
}
