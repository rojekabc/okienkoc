#ifndef _GOC_MYSTR_H_
#define _GOC_MYSTR_H_

extern const char const* GOC_ENDSTR;

char *goc_stringCopy(char *dst, const char *src);
char *goc_stringMultiCopyInner(char *dst, ...);
#define goc_stringMultiCopy(dst, src...) goc_stringMultiCopyInner(dst, src, GOC_ENDSTR)
char *goc_stringSet(char *dst, char *src);
char *goc_stringAdd(char *dst, const char *src);
char *goc_stringMultiAddInner(char *dst, ...);
#define goc_stringMultiAdd(dst, src...) goc_stringMultiAddInner(dst, src, GOC_ENDSTR)
char *goc_stringAddLen(char *dst, const char *src, int len);
char *goc_stringAddInt(char *dst, int val);
char *goc_stringAddHex(char *dst, int val);
char *goc_stringInsertAtPos(char *dst, unsigned int pos, const char *src);
char *goc_stringInsertAtPnt(char *dst, char *p, const char *src);
char *goc_stringInsertCharAt(char *dst, unsigned int pos, char src);
char *goc_stringDelAtPos(char *dst, unsigned int pos);
char *goc_stringDelAtPnt(char *dst, char *p);
char *goc_stringFree(char *str);
int goc_stringEquals(const char *str1, const char *str2);
#define goc_stringOrEquals(str1, strings...) goc_stringOrEqualsInner(str1, strings, GOC_ENDSTR)
int goc_stringOrEqualsInner(const char* str1, ...);
char *goc_stringReplaceStr(char *src, const char *find, const char *replace);
char *stringReplaceFirstStr(char *src, const char *find, const char *replace);
char *goc_stringTrim(char *src);
char *goc_stringTrimList(char *src, const char *remchars);
char *goc_stringToUpper(char *src);
char *goc_stringToLower(char *src);
int goc_stringStartsWith(const char *str1, const char *str2);
int goc_stringEndsWith(const char *str1, const char *str2);
int goc_stringStartsWithCase(const char *str1, const char *str2);
int goc_stringEndsWithCase(const char *str1, const char *str2);
int goc_stringEqualsCase(const char *str1, const char *str2);
int goc_stringLength(const char* str);

#endif /* ifndef _GOC_MYSTR_H_*/
