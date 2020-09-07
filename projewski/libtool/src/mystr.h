#ifndef _GOC_MYSTR_H_
#define _GOC_MYSTR_H_

// direct assign the dynamic char* value
#define string_assign(_variable_, _value_) _variable_ = goc_stringAssign(_variable_, _value_)
// set the value of string variable by copying the value
#define string_set(_variable_, _value_) _variable_ = goc_stringCopy(_variable_, _value_)
// alloc memory for the string
#define string_alloc(_variable_, _size_) char *_variable_ = (char*)malloc(_size_); memset(_variable_, 0, _size_)
// free memory of the string
#define string_free(_variable_) _variable_ = goc_stringFree(_variable_)
// append the string
#define string_add(_variable_, _value_) _variable_ = goc_stringAdd(_variable_, _value_)
// append int value to the string
#define string_addInt(_variable_, _value_) _variable_ = goc_stringAddInt(_variable_, _value_)
// get the length of the string
#define string_length(_variable_) (_variable_ ? strlen(_variable_) : 0)

extern const char* GOC_ENDSTR;

// check, that two strings are equal
int string_equals(const char* _str1_, const char *_str2_);
// check, that two string are equal with case ignore
int string_equalsIgnoreCase(const char *_str1_, const char *_str2_);
// check, that sting starts with
int string_startsWith(const char *_str1_, const char *_str2_);

char* goc_stringAssign(char* str, char* src);
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
#define goc_stringOrEquals(str1, strings...) goc_stringOrEqualsInner(str1, strings, GOC_ENDSTR)
int goc_stringOrEqualsInner(const char* str1, ...);
char *goc_stringReplaceStr(char *src, const char *find, const char *replace);
char *stringReplaceFirstStr(char *src, const char *find, const char *replace);
char *goc_stringTrim(char *src);
char *goc_stringTrimList(char *src, const char *remchars);
char *goc_stringToUpper(char *src);
char *goc_stringToLower(char *src);
int goc_stringEndsWith(const char *str1, const char *str2);
int goc_stringStartsWithCase(const char *str1, const char *str2);
int goc_stringEndsWithCase(const char *str1, const char *str2);
int goc_stringEqualsCase(const char *str1, const char *str2);

#endif /* ifndef _GOC_MYSTR_H_*/
