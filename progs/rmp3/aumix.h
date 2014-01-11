
#include <sys/soundcard.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SOUND_IOCTL(a,b,c)	ioctl(a,b,c)
#define MAXLEVEL 100		/* highest level permitted by OSS drivers */
extern int mixer_fd, devmask, recmask, recsrc, 
    stereodevs, mutestatem, current_dev;

extern const char *dev_label[SOUND_MIXER_NRDEVICES];

enum {
	ENOERROR,
	EOPENMIX,		/* trouble opening mixer device */
	EFINDDEVICE,		/* no device found */
	EREADDEV,		/* SOUND_MIXER_READ_DEVMASK */
	EREADRECMASK,		/* SOUND_MIXER_READ_RECMASK */
	EREADRECSRC,		/* SOUND_MIXER_READ_RECSRC */
	EREADSTEREO,		/* SOUND_MIXER_READ_STEREODEVS */
	EWRITERECSRC,		/* SOUND_MIXER_WRITE_RECSRC */
	EREADMIX,		/* MIXER_READ */
	EWRITEMIX,		/* MIXER_WRITE */
	ENOTOPEN,		/* mixer not open */
	EFILE			/* unable to open settings file */
};

void ErrorExitWarn(int error, int mode);
int MixerStatus(void);
int WriteRecSrc(void);
int ReadRecSrc(void);
int WriteLevel(int device, int leftright);
int ReadLevel(int device, int *leftright);
int InitializeMixer(char *device_filename);
void AdjustLevel(int dev, int incr, int setlevel);
int SetCurrentDev(const char *str);
/*
int main(int argc, char ** argv)
{
    if (argc < 2)
	return -1;
    InitializeMixer("/dev/mixer");
    if (*argv[1] == '+')
    {
	printf("Plus\n");
	AdjustLevel(4, 1, -1);
    }
    else if (*argv[1] == '-')
    {
	printf("Minus\n");
	AdjustLevel(4, -1, -1);
    }
    else
	printf("Nie znana opcja\n");
    return 0;
	
}
*/
