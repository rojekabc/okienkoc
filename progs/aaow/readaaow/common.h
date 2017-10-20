
// Mamy strukture StBuild, ktora posiada na poczatku nazwe onst char* a nastepnie
// typowa strukture goc dla StValuePoint. Dzieki temu mozemy utrzymujac jedna strukture
// poslugiwac sie nia po obu stronach. Pomyslec o lepszym rozwiazaniu.
#define TOGOC(variable) (GOC_StValuePoint*)(((char*)variable)+sizeof(const char*))
#define FROMGOC(variable) (((char*)variable)-sizeof(const char*))

#define FLAG_GREEN 0x10
#define FLAG_RED 0x20

#define MAPHEIGHT 20
#define MAPWIDTH 20

extern GOC_HANDLER maska;
extern GOC_HANDLER terrain;
extern GOC_HANDLER build;
extern GOC_HANDLER front;
extern GOC_HANDLER labelDesc;

void createCommonObjects();
