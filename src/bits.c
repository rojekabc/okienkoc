/*
 * Pole bitowe w organizacji:
 *  na bazie uchar (gdyz w kazdym systemie ma 1B)
 *  Najmlodszy bajt na indeksie o najmniejszym numerze
 *  Najstarszy bajt na indeksie o najwiekszym nuemrze
 *  Najmlodszy bit w bajcie w pozycji 0, czyli wartosc 1
 *  Najstarszy bit w bajcie w pozycji 7, czyli wartosc 128
 */

/*
 * Przy kazdej z funkcji uzytkownik musi sam dbac, aby nie nastapil blad SEGV !
 */

int goc_bitIs(char *pb, const int nr)
{
    return (pb[nr>>3]>>(nr&0x07)) & 1;
}

void goc_bitSet(char *pb, const int nr)
{
    pb[nr>>3] |= (1 << (nr&0x07));
}

void goc_bitClear(char *pb, const int nr)
{
    pb[nr>>3] &= ~(1 << (nr&0x07));
}

void goc_bitNot(char *pb, const int nr)
{
    register char s = 1 << (nr&0x07);
    register char *a = &pb[nr>>3];
    *a = (*a & ~s) | (~*a & s);
}

