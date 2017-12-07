
# Where to install binary and manpage on "make install":
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
SHAREDIR=$(PREFIX)/share/rmp3
OBJECTS=mixer.o common-mixer.o rmp3.o

# Check properties file
include Makefile.properties

ifeq ($(PLATFORM),linux)
	CC := gcc
	LDFLAGS := -lpthread -lprojewski-tools -lm -lavutil
	CFLAGS := -DI386_ASSEM -DPENTIUM_OPT -DREAL_IS_FLOAT -DLINUX -DOSS -DTERM_CONTROL -Wall -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math
	CFLAGS += -I/usr/local/include/projewski
endif

ifeq ($(DEBUG),true)
	CFLAGS += -g -O0
else
	CFLAGS += -O3
endif

ifeq ($(OSS_MIXER_ENABLE),true)
	CFLAGS += -DOSS_MIXER_ENABLE
	OBJECTS += oss-mixer.o
endif

ifeq ($(ALSA_MIXER_ENABLE),true)
	CFLAGS += -DALSA_MIXER_ENABLE
	OBJECTS += alsa-mixer.o
	LDFLAGS += -lasound
endif

ifeq ($(OUTPUT_LIBRARY),libao)
	CFLAGS += -DHAS_LIBAO
	LDFLAGS += -lao
	OBJECTS += libao-player.o
else ifeq ($(OUTPUT_LIBRARY),alsa)
	CFLAGS += -DHAS_ALSA
	LDFLAGS += -lasound
	OBJECTS += alsa-player.o
endif

ifeq ($(DECODE_LIBRARY),libsox)
	CFLAGS += -DHAS_LIBSOX
	LDFLAGS += -lsox
	OBJECTS += libsox-decoder.o
endif

ifeq ($(FILEINFO_LIBRARY),ffmpeg)
	CFLAGS += -DHAS_FFMPEG
	LDFLAGS += -lavutil -lavformat
	OBJECTS += ffmpeg-finfo.o
endif

ifeq ($(CONTROL_LIBRARY),okienko)
	LDFLAGS += -lprojewski-okienkoc
	OBJECTS += okienko-control.o
endif

ifeq ($(PLAYLIST_LIBRARY),default)
	OBJECTS += default-playlist.o
endif

all: prepare rmp3

prepare:
	rm -rf ERRORS

rmp3: Makefile $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ 2>> ERRORS

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 2>> ERRORS

clean:
	rm -f *.o *core *~ rmp3 .*.un~ ERRORS

install:
	strip rmp3
	install -d $(BINDIR)
	install -m 755 rmp3 $(BINDIR)
	install -d $(SHAREDIR)
	install -m 644 rmp3.logo $(SHAREDIR)
