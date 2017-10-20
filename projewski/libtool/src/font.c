#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <stdio.h>
#include <unistd.h>

void goc_fontSet(char *bufor, const char *font, const int n)
/* Wpisz w bufor opis fontu w pozycje n */
{
  memcpy(bufor + (n<<5), font, 16);
//  bufor[(n<<5)+
}

void goc_fontsSet(const char *bufor)
/* Ustaw fonty, aby byly zgodne z zawartoscia bufora */
{
  int fd;
  fd = open("/dev/tty", O_RDONLY);
  ioctl(fd, PIO_FONT, bufor);
  close(fd);
}

void goc_fontsSave(const char *nazwa, const char *bufor)
/* Zapisz do pliku bufor z ustawionymi fontami */
{
    FILE *plik;
    plik = fopen(nazwa, "w");
    if (!plik) {
	printf("Blad otwarcia pliku %s", nazwa);
	return;
    }
    fwrite(bufor, sizeof(char), 8192, plik);
    fclose(plik);
}

void goc_fontsLoad(const char *nazwa, char *bufor)
/* Zaladuj w bufor fonty ze wskazanego pliku */
{
    int fd;
    fd = open(nazwa, O_RDONLY);
    read(fd, bufor, 8192);
    close(fd);
}

void goc_getfonts(char *bufor)
/* Wczytaj fonty aktualnie ustawione do bufora */
{
  int fd;
  fd = open("/dev/tty", O_RDONLY);
  ioctl(fd, GIO_FONT, bufor);
  close(fd);
}
