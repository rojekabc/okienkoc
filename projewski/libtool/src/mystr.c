#define _GNU_SOURCE

#include "global-inc.h"
#include "mystr.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

const char *GOC_ENDSTR = "";

int string_equals(const char *_str1_, const char *_str2_) {
    return (_str1_ == _str2_) || (_str1_ && _str2_ && (strcmp(_str1_, _str2_) == 0));
}

int string_equalsIgnoreCase(const char *_str1_, const char *_str2_) {
    return (_str1_ == _str2_) || (_str1_ && _str2_ && (strcasecmp(_str1_, _str2_) == 0));
}

int string_startsWith(const char *_str1_, const char *_str2_) {
    return (_str1_ == _str2_) || (_str1_ && _str2_ && (strncmp(_str1_, _str2_, strlen(_str2_)) == 0));
}

char *goc_stringAssign(char *str, char *src) {
    if (str) {
        free(str);
    }
    return src;
}

char *goc_stringCopy(char *dst, const char *src) {
    if (dst) {
        free(dst);
    }
    return src ? strdup(src) : NULL;
}

char *goc_stringMultiCopyInner(char *dst, ...) {
    va_list ap;
    const char *src = NULL;
    va_start(ap, dst);
    if (dst) {
        free(dst);
        dst = NULL;
    }
    while ((src = va_arg(ap, const char*)) != GOC_ENDSTR) {
        if (src)
            dst = goc_stringAddLen(dst, src, strlen(src));
    }
    va_end(ap);
    return dst;
}

char *goc_stringAddLen(char *dst, const char *src, int len) {
    if (!src)
        return dst;

    if (dst) {
        dst = realloc(dst, strlen(dst) + len + 1);
        strncat(dst, src, len);
    } else {
        dst = strndup(src, len);
    }
    return dst;
}

char *goc_stringAdd(char *dst, const char *src) {
    if (!src)
        return dst;
    else
        return goc_stringAddLen(dst, src, strlen(src));
}

/**
 * Adding strings until get GOC_ENDSTR
 */
char *goc_stringMultiAddInner(char *dst, ...) {
    va_list ap;
    const char *src = NULL;
    va_start(ap, dst);
    while ((src = va_arg(ap, const char*)) != GOC_ENDSTR) {
        if (src)
            dst = goc_stringAddLen(dst, src, strlen(src));
    }
    va_end(ap);
    return dst;
}

char *goc_stringInsertAtPos(char *dst, unsigned int pos, const char *src) {
    char *tmp = 0;

    if (!dst)
        return goc_stringAdd(dst, src);
    if (!src)
        return dst;

    tmp = (char *) malloc(strlen(dst) + strlen(src) + 1);
    strncpy(tmp, dst, pos);
    strcpy(tmp + pos, src);
    strcpy(tmp + strlen(src) + pos, dst + pos);
    free(dst);
    dst = tmp;
    return dst;
}

char *goc_stringInsertAtPnt(char *dst, char *p, const char *src) {
    int dif;
    if (!dst)
        return dst;
    dif = p - dst;
    if (dif < 0)
        return 0;
    return goc_stringInsertAtPos(dst, dif, src);
}

char *goc_stringDelAtPos(char *dst, unsigned int pos) {
    char *tmp = NULL;
    if (!dst)
        return dst;
    if (pos > strlen(dst))
        return dst;
    tmp = (char *) malloc(strlen(dst));
    strncpy(tmp, dst, pos);
    strcpy(tmp + pos, dst + pos + 1);
    free(dst);
    dst = tmp;
    return dst;
}

char *goc_stringDelAtPnt(char *dst, char *p) {
    int dif;
    if (!dst)
        return 0;
    dif = p - dst;
    if (dif < 0)
        return 0;
    return goc_stringDelAtPos(dst, dif);
}

char *goc_stringInsertCharAt(char *dst, unsigned int pos, char src) {
    char *tmp = 0;

    if (!dst) {
        dst = malloc(2);
        dst[0] = src;
        dst[1] = 0;
        return dst;
    }

    tmp = (char *) malloc(strlen(dst) + 2);
    memset(tmp, 0, strlen(dst) + 2);
    strncpy(tmp, dst, pos);
    tmp[pos] = src;
    tmp[pos + 1] = 0;
    strcat(tmp, dst + pos);
    free(dst);
    dst = tmp;
    return dst;
}

char *goc_stringFree(char *str) {
    if (str) {
        free(str);
    }
    return NULL;
}

int goc_stringOrEqualsInner(const char *str1, ...) {
    va_list ap;
    const char *src = NULL;
    va_start(ap, str1);
    while ((src = va_arg(ap, const char*)) != GOC_ENDSTR) {
        if (string_equals(str1, src))
            return 1;
    }
    va_end(ap);
    return 0;
}

// wykonuje zamiane okreslonego ciagu znakow (find) na inny (replace)
// we wszystkich miejscach przeszukiwanego ciagu (src)
// Zwraca nowo utworzony ciag znakow. Ciag wejsciowy (src) jest usuwany
// Podanie gdziekolwiek na wejsciu NULL nie powoduje zadnych zmian
char *goc_stringReplaceStr(char *src, const char *find, const char *replace) {
    char *tmp = src;
    char *last = tmp;
    char *ret = NULL;
    if ((!src) || (!find) || (!replace))
        return src;
    while ((tmp = strstr(tmp, find))) {
        *tmp = 0;
        ret = goc_stringAdd(ret, last);
        ret = goc_stringAdd(ret, replace);
        tmp += strlen(find);
        last = tmp;
    }
    ret = goc_stringAdd(ret, last);
    src = goc_stringFree(src);
    return ret;
}

// wykonuje zamiane okreslonego ciagu znakow (find) na inny (replace)
// w pierszym miejscu przeszukiwanego ciagu (src)
// Zwraca nowo utworzony ciag znakow. Ciag wejsciowy (src) jest usuwany
// Podanie gdziekolwiek na wejsciu NULL nie powoduje zadnych zmian
char *stringReplaceFirstStr(char *src, const char *find, const char *replace) {
    char *tmp = NULL;
    char *ret = NULL;
    if ((!src) || (!find) || (!replace))
        return src;
    tmp = strstr(src, find);
    if (tmp) {
        *tmp = 0;
        ret = goc_stringAdd(ret, src);
        ret = goc_stringAdd(ret, replace);
        ret = goc_stringAdd(ret, tmp + strlen(find));
        src = goc_stringFree(src);
    } else {
        ret = src;
    }
    return ret;
}

// usuwa z poczatku i konca ciagu znakow znaki z podanego zestawu znakow
char *goc_stringTrimList(char *src, const char *remchars) {
    char *b = src;
    char *e = NULL;

    if ((src == NULL) || (remchars == NULL))
        return src;
    e = src + strlen(src) - 1;
    // usuwanie od poczatku niepozadanych znakow
    while ((*b) && (strchr(remchars, *b)))
        b++;
    // usuwanie od konca
    while ((e >= b) && (strchr(remchars, *e)))
        e--;
    if (b <= e) {
        e++;
        *e = 0;
        b = strdup(b);
    } else {
        b = NULL; // THINK: Moze lepiej pusty ciag, niz NULL
    }
    free(src);
    return b;
}

// usuwa z poczatku i konca znaki biale, konca linii, powrotu karetki
char *goc_stringTrim(char *src) {
    return goc_stringTrimList(src, "\t \n\r");
}

// Dodanie reprezentacji liczbowej w postaci ciagu znakow
char *goc_stringAddInt(char *dst, int val) {
    char *buf = malloc(32);
    memset(buf, 0, 32);
    snprintf(buf, 32, "%d", val);
    dst = goc_stringAdd(dst, buf);
    free(buf);
    return dst;
}

// Dodanie reprezentacji liczbowej w postaci ciagu znakow
char *goc_stringAddHex(char *dst, int val) {
    char *buf = malloc(32);
    memset(buf, 0, 32);
    sprintf(buf, "%x", val);
    dst = goc_stringAdd(dst, buf);
    free(buf);
    return dst;
}

char *goc_stringToUpper(char *src) {
    char *tmp = src;
    if (!src)
        return src;
    while (*tmp) {
        *tmp = toupper(*tmp);
        tmp++;
    }
    return src;
}

char *goc_stringToLower(char *src) {
    char *tmp = src;
    if (!src)
        return src;
    while (*tmp) {
        *tmp = tolower(*tmp);
        tmp++;
    }
    return src;
}

// sprawdzenie czy ciag znak�w str1 ko�czy si� na str2
int goc_stringEndsWith(const char *str1, const char *str2) {
    int len;
    // Oba NULL, lub ten sam adres
    if (str1 == str2)
        return 1;
    // Tylko jeden jest NULL
    if ((!str1) || (!str2))
        return 0;
    len = strlen(str1) - strlen(str2);
    if (len < 0)
        return 0;
    if (strcmp(str1 + len, str2) == 0)
        return 1;
    return 0;
}

// sprawdzenie czy ciag znak�w str1 ko�czy si� na str2
// Sprawdzanie bez r�nicy na wielko�� liter
int goc_stringEndsWithCase(const char *str1, const char *str2) {
    int len;
    // Oba NULL, lub ten sam adres
    if (str1 == str2)
        return 1;
    // Tylko jeden jest NULL
    if ((!str1) || (!str2))
        return 0;
    len = strlen(str1) - strlen(str2);
    if (len < 0)
        return 0;
    if (strcasecmp(str1 + len, str2) == 0)
        return 1;
    return 0;
}

// sprawdzenie czy ci�g znak�w str1 rozpoczyna si� od str2
// Sprawdzanie bez r�nicy na wielko�� liter
int goc_stringStartsWithCase(const char *str1, const char *str2) {
    // Oba NULL, lub ten sam adres
    if (str1 == str2)
        return 1;
    // Tylko jeden jest NULL
    if ((!str1) || (!str2))
        return 0;
    if (strcasecmp(str1, str2) == 0)
        return 1;
    return 0;
}


