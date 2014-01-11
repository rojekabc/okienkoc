#ifdef _DEBUG
#	include <mpatrol.h>
#endif // ifdef _DEBUG
#include <stdlib.h>
#include "aumix.h"

int mixer_fd = -1, devmask = 0, recmask = 0, recsrc = 0, 
    stereodevs = 0, mutestate = 0, current_dev = 0;


const char *dev_label[SOUND_MIXER_NRDEVICES] =
{
	"Volume  ",
	"Bass    ",
	"Treble  ",
	"Synth   ",
	"PCM     ",
	"Speaker ",
	"Line    ",
	"Mic     ",
	"CD      ",
	"Mix     ",
	"PCM2    ",
	"Record  ",
	"Input   ",
	"Output  ",
	"Line1   ",
	"Line2   ",
	"Line3   ",
	"Digital1",
	"Digital2",
	"Digital3",
	"PhoneIn ",
	"PhoneOut",
	"Video   ",
	"Radio   ",
	"Monitor "
};

int SetCurrentDev(const char *str)
{
    int i;
    for (i = 0; i<SOUND_MIXER_NRDEVICES; i++)
        if (((1<<i) & devmask) && !strncasecmp(dev_label[i], str, strlen(str)))
	{
	    current_dev = i;
	    return 0;
	}
    return -1;
}

void ErrorExitWarn(int error, int mode)
{
/* Print error messages.  If mode is "e", bail out. */
	char            string[80];
	const char     *errorlist[] =
	{
	    "aumix:  error opening mixer",
	    "aumix:  no device found",
	    "aumix:  SOUND_MIXER_READ_DEVMASK",
	    "aumix:  SOUND_MIXER_READ_RECMASK",
	    "aumix:  SOUND_MIXER_READ_RECSRC",
	    "aumix:  SOUND_MIXER_READ_STEREODEVS",
	    "aumix:  SOUND_MIXER_WRITE_RECSRC",
	    "aumix:  MIXER_READ",
	    "aumix:  MIXER_WRITE",
	    "aumix:  mixer not open",
	    "aumix:  unable to open settings file"
	};
	if (!error)
		return;
	if (error > 12) {
		sprintf(string, "aumix:  unknown error %i", error);
		fprintf(stderr, string);
	} else if (error > 0) {
		fprintf(stderr, errorlist[error - 1]);
	}
	fprintf(stderr, "\n");
	if (mode == 'e')	/* exit */
		exit(-1);
	else
		return;		/* warn */
}

int MixerStatus(void)
{
/* Get status of the mixer.

   Global:

   mixer_fd   = mixer file descriptor reference number

   Bit masks indicating:

   devmask    = valid devices
   recmask    = valid input devices
   recsrc     = devices currently selected for input
   stereodevs = stereo devices
 */

	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_DEVMASK, &devmask) == -1)
		return EREADDEV;
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1)
		return EREADRECMASK;
	ErrorExitWarn(ReadRecSrc(), 'e');
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs) == -1)
		return EREADSTEREO;
	return 0;
}
int WriteRecSrc(void)
{
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_WRITE_RECSRC, &recsrc) == -1)
		return EWRITERECSRC;
	return 0;
}

int ReadRecSrc(void)
{
	if (SOUND_IOCTL(mixer_fd, SOUND_MIXER_READ_RECSRC, &recsrc) == -1)
		return EREADRECSRC;
	return 0;
}

int WriteLevel(int device, int leftright)
{
	if (SOUND_IOCTL(mixer_fd, MIXER_WRITE(device), &leftright) == -1)
		return EWRITEMIX;
	return 0;
}

int ReadLevel(int device, int *leftright)
{
	if (SOUND_IOCTL(mixer_fd, MIXER_READ(device), leftright) == -1)
		return EREADMIX;
	return 0;
}

int InitializeMixer(char *device_filename)
{
	if ((mixer_fd = open(device_filename, O_RDWR)) < 0)
		return EOPENMIX;
	ErrorExitWarn(MixerStatus(), 'n');
	if (!devmask)
		return EFINDDEVICE;
	return 0;
}

void AdjustLevel(int dev, int incr, int setlevel)
/*
 *  dev: device to adjust
 *  incr: signed percentage to increase (decrease) level, ignored unless
 *      setlevel = -1
 *  setlevel: level to set directly, or -1 to increment
 */
{
	int  balset, max, left, right, temp;
	if (!((1 << dev) & devmask) || (dev > SOUND_MIXER_NRDEVICES - 1) || (dev < 0))
		return;
	ErrorExitWarn(ReadLevel(dev, &temp), 'n');
	left = temp & 0x7F;
	right = (temp >> 8) & 0x7F;
	max = (left > right) ? left : right;
	if (max) {
		balset = (left > right) ? (MAXLEVEL / 2) * right / max : MAXLEVEL - ((MAXLEVEL / 2) * left / max);
	} else {
		balset = (MAXLEVEL / 2);
	}
	max += incr;
	if (balset > (MAXLEVEL / 2 - 1)) {
		left = max * (MAXLEVEL - balset) / (MAXLEVEL / 2);
		right = max;
	} else {
		right = max * balset / (MAXLEVEL / 2);
		left = max;
	}
	left = (setlevel > -1) ? setlevel : left;
	right = (setlevel > -1) ? setlevel : right;
	left = (left > MAXLEVEL) ? MAXLEVEL : left;
	right = (right > MAXLEVEL) ? MAXLEVEL : right;
	left = (left < 0) ? 0 : left;
	right = (right < 0) ? 0 : right;
	temp = (right << 8) | left;
	ErrorExitWarn(WriteLevel(dev, temp), 'n');
}
