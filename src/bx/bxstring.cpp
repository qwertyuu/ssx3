#include "common.h"

// cBXString layout: single pointer to char data
// Before char data: [-0xC] refcount, [-0x8] length, [-0x4] capacity
// Refcount < 0 means locked (forces deep copy)

extern char D_0048DCD4[];  // empty string singleton data
extern char* PTR_DAT_004a3e90;  // pointer to empty string singleton (&D_0048DCD4)

void* cMemMan_alloc(int size, const char* tag, uint flags, int align);
void cMemMan_free(void* ptr);
void operator_delete(void* ptr);
int strlen(const char*);
void* memcpy(void* dst, const void* src, int size);
void* memset(void* dst, int val, int size);
char* strchr(const char* s, int c);

// Forward declarations
void cBXString_Realloc(int* this_, int size);
void cBXString_Reset(int* this_);
void cBXString_InitFromCString(int* this_, int len, const char* src);
void func_00318120(int* this_);
int* cBXString_cBXString1(int* this_, int* other);
void cBXString__cBXString(int* this_, uint flags);
void func_00318540(int* this_, int len1, const char* src1, int len2, const char* src2);
void cBXString_ConcatImpl(int* this_, int len, const char* src);
int func_003189D0(int* this_, int minCap);
void func_003181E8(int* src, int* dst, int len, int offset, int extra);
int* func_003190A8(int* result, int* src, int count);

extern const char D_0048D850[];  // allocation tag string

// strchr-like functions
char* func_0041ACC0(const char*, int);
char* func_00416404(const char*, int);
char* func_0041AC40(const char*);
char* func_0041AD80(const char*, const char*);
char* func_0041AD10(const char*, const char*);
char* func_0041AB08(const char*, const char*);

// character class table
extern char PTR_DAT_00499C81[];

// Copy constructor
INCLUDE_ASM("bx/bxstring", cBXString_cBXString1);
#ifdef SKIP_ASM
int* cBXString_cBXString1(int* this_, int* other)
{
	int data = *other;
	if (*(int*)(data - 0xC) < 0) {
		*this_ = (int)PTR_DAT_004a3e90;
		cBXString_cBXString4(this_, data);
	}
	else {
		*this_ = data;
		*(int*)(data - 0xC) = *(int*)(data - 0xC) + 1;
	}
	return this_;
}
#endif

// Allocate new string buffer
INCLUDE_ASM("bx/bxstring", cBXString_Realloc);
#ifdef SKIP_ASM
void cBXString_Realloc(int* this_, int size)
{
	if (size == 0) {
		*this_ = (int)PTR_DAT_004a3e90;
	}
	else {
		int* buf = (int*)cMemMan_alloc(size + 0xD, D_0048D850, 0x20000000, 0);
		buf[0] = 1;
		*this_ = (int)(buf + 3);
		*(char*)(*this_ + size) = 0;
		buf[2] = size;
		buf[1] = size;
	}
}
#endif

// Release string buffer
INCLUDE_ASM("bx/bxstring", cBXString_Reset);
#ifdef SKIP_ASM
void cBXString_Reset(int* this_)
{
	char* data = (char*)*this_;
	if (data != D_0048DCD4) {
		int rc = *(int*)(data - 0xC);
		*(int*)(data - 0xC) = rc - 1;
		if (rc - 1 == 0 && *this_ != 0xC) {
			cMemMan_free((void*)(*this_ - 0xC));
		}
		*this_ = (int)PTR_DAT_004a3e90;
	}
}
#endif

// Make unique (copy-on-write)
INCLUDE_ASM("bx/bxstring", func_00318120);
#ifdef SKIP_ASM
void func_00318120(int* this_)
{
	int data = *this_;
	if (1 < *(int*)(data - 0xC)) {
		cBXString_Reset(this_);
		cBXString_Realloc(this_, *(int*)(data - 8));
		memcpy((void*)*this_, (void*)data, *(int*)(data - 8) + 1);
	}
}
#endif

// Ensure capacity
INCLUDE_ASM("bx/bxstring", cBXString_Resize);
#ifdef SKIP_ASM
void cBXString_Resize(int* this_, int capacity)
{
	if (1 < *(int*)(*this_ - 0xC) || *(int*)(*this_ - 4) < capacity) {
		cBXString_Reset(this_);
		cBXString_Realloc(this_, capacity);
	}
}
#endif

// Substring builder helper
INCLUDE_ASM("bx/bxstring", func_003181E8);
#ifdef SKIP_ASM
void func_003181E8(int* src, int* dst, int len, int offset, int extra)
{
	int total = len + extra;
	if (total == 0) {
		*dst = (int)PTR_DAT_004a3e90;
	}
	else {
		cBXString_Realloc(dst, total);
		memcpy((void*)*dst, (void*)(*src + offset), len);
	}
}
#endif

// Destructor
INCLUDE_ASM("bx/bxstring", cBXString__cBXString);
#ifdef SKIP_ASM
void cBXString__cBXString(int* this_, uint flags)
{
	char* data = (char*)*this_;
	if (data != D_0048DCD4) {
		int rc = *(int*)(data - 0xC);
		*(int*)(data - 0xC) = rc - 1;
		if (rc - 1 < 1 && *this_ != 0xC) {
			cMemMan_free((void*)(*this_ - 0xC));
		}
	}
	if ((flags & 1) != 0) {
		operator_delete(this_);
	}
}
#endif

// Constructor from C-string
INCLUDE_ASM("bx/bxstring", cBXString_cBXString2);
#ifdef SKIP_ASM
int* cBXString_cBXString2(int* this_, const char* str)
{
	int len;
	*this_ = (int)PTR_DAT_004a3e90;
	if (str == 0) {
		len = 0;
	}
	else {
		len = strlen(str);
	}
	if (len != 0) {
		cBXString_Realloc(this_, len);
		memcpy((void*)*this_, str, len);
	}
	return this_;
}
#endif

// Constructor from C-string with max length
INCLUDE_ASM("bx/bxstring", func_00318350);
#ifdef SKIP_ASM
int* func_00318350(int* this_, const char* str, uint maxLen)
{
	uint len;
	*this_ = (int)PTR_DAT_004a3e90;
	len = strlen(str);
	if (len <= maxLen) {
		maxLen = strlen(str);
	}
	if (maxLen != 0) {
		cBXString_Realloc(this_, maxLen);
		memcpy((void*)*this_, str, maxLen);
	}
	return this_;
}
#endif

// Assign from C-string with explicit length
INCLUDE_ASM("bx/bxstring", cBXString_InitFromCString);
#ifdef SKIP_ASM
void cBXString_InitFromCString(int* this_, int len, const char* src)
{
	cBXString_Resize(this_, len);
	if ((char*)*this_ != D_0048DCD4) {
		memcpy((void*)*this_, src, len);
		*(int*)(*this_ - 8) = len;
		*(char*)(*this_ + len) = 0;
	}
}
#endif

// Assignment operator (operator=)
INCLUDE_ASM("bx/bxstring", cBXString_operatorE);
#ifdef SKIP_ASM
int* cBXString_operatorE(int* this_, int* other)
{
	char* data = (char*)*this_;
	char* otherData = (char*)*other;
	int rc;

	if (data == otherData) {
		return this_;
	}
	if (*(int*)(data - 0xC) < 0) {
		if (data != D_0048DCD4) {
			rc = *other;
			goto deep_copy;
		}
		rc = *(int*)(otherData - 0xC);
	}
	else {
		rc = *(int*)(otherData - 0xC);
	}
	if (-1 < rc) {
		cBXString_Reset(this_);
		rc = *other;
		*this_ = rc;
		*(int*)(rc - 0xC) = *(int*)(rc - 0xC) + 1;
		return this_;
	}
	rc = *other;
deep_copy:
	cBXString_InitFromCString(this_, *(int*)(rc - 8), (const char*)rc);
	return this_;
}
#endif

// Assign from C-string (computes strlen)
INCLUDE_ASM("bx/bxstring", cBXString_cBXString4);
#ifdef SKIP_ASM
int* cBXString_cBXString4(int* this_, const char* str)
{
	int len;
	if (str == 0) {
		len = 0;
	}
	else {
		len = strlen(str);
	}
	cBXString_InitFromCString(this_, len, str);
	return this_;
}
#endif

// Concatenate two buffers into this
INCLUDE_ASM("bx/bxstring", func_00318540);
#ifdef SKIP_ASM
void func_00318540(int* this_, int len1, const char* src1, int len2, const char* src2)
{
	if (len1 + len2 != 0) {
		cBXString_Realloc(this_, len1 + len2);
		memcpy((void*)*this_, src1, len1);
		memcpy((void*)(*this_ + len1), src2, len2);
	}
}
#endif

// operator+ (string + string)
INCLUDE_ASM("bx/bxstring", func_003185C8);
#ifdef SKIP_ASM
int* func_003185C8(int* result, int* a, int* b)
{
	int tmp[1];
	tmp[0] = (int)PTR_DAT_004a3e90;
	func_00318540(tmp, *(int*)(*a - 8), (const char*)*a, *(int*)(*b - 8), (const char*)*b);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// operator+ (string + C-string)
INCLUDE_ASM("bx/bxstring", func_00318630);
#ifdef SKIP_ASM
int* func_00318630(int* result, int* a, const char* b)
{
	int data = *a;
	int tmp[1];
	int blen;
	int data2;
	tmp[0] = (int)PTR_DAT_004a3e90;
	if (b == 0) {
		blen = 0;
		data2 = *a;
	}
	else {
		blen = strlen(b);
		data2 = *a;
	}
	func_00318540(tmp, *(int*)(data - 8), (const char*)data2, blen, b);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// operator+ (C-string + string)
INCLUDE_ASM("bx/bxstring", func_003186D0);
#ifdef SKIP_ASM
int* func_003186D0(int* result, const char* a, int* b)
{
	int alen;
	int tmp[1];
	tmp[0] = (int)PTR_DAT_004a3e90;
	if (a == 0) {
		alen = 0;
	}
	else {
		alen = strlen(a);
	}
	func_00318540(tmp, alen, a, *(int*)(*b - 8), (const char*)*b);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// operator+ (string + char)
INCLUDE_ASM("bx/bxstring", func_00318760);
#ifdef SKIP_ASM
int* func_00318760(int* result, int* a, char ch)
{
	int tmp[1];
	char buf[16];
	tmp[0] = (int)PTR_DAT_004a3e90;
	buf[0] = ch;
	func_00318540(tmp, *(int*)(*a - 8), (const char*)*a, 1, buf);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// operator+ (char + string)
INCLUDE_ASM("bx/bxstring", func_003187C8);
#ifdef SKIP_ASM
int* func_003187C8(int* result, char ch, int* b)
{
	int tmp[1];
	char buf[16];
	tmp[0] = (int)PTR_DAT_004a3e90;
	buf[0] = ch;
	func_00318540(tmp, 1, buf, *(int*)(*b - 8), (const char*)*b);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// In-place append (operator+= impl)
INCLUDE_ASM("bx/bxstring", cBXString_ConcatImpl);
#ifdef SKIP_ASM
void cBXString_ConcatImpl(int* this_, int len, const char* src)
{
	int data = *this_;
	if (*(int*)(data - 0xC) < 2 && *(int*)(data - 8) + len <= *(int*)(data - 4)) {
		memcpy((void*)(data + *(int*)(data - 8)), src, len);
		*(int*)(*this_ - 8) = *(int*)(*this_ - 8) + len;
		*(char*)(*this_ + *(int*)(*this_ - 8)) = 0;
	}
	else {
		int tmp[4];
		cBXString_cBXString1(tmp, this_);
		data = *this_;
		cBXString_Reset(this_);
		func_00318540(this_, *(int*)(data - 8), (const char*)data, len, src);
		cBXString__cBXString(tmp, 2);
	}
}
#endif

// operator+= (C-string)
//94.09%
//https://decomp.me/scratch/4rHwX
INCLUDE_ASM("bx/bxstring", cBXString_Concat);
#ifdef SKIP_ASM
int* cBXString_Concat(int* this_, const char* str)
{
	int len;
	if (str == 0) {
		len = 0;
	}
	else {
		len = strlen(str);
	}
	cBXString_ConcatImpl(this_, len, str);
	return this_;
}
#endif

// operator+= (char)
INCLUDE_ASM("bx/bxstring", func_00318968);
#ifdef SKIP_ASM
int* func_00318968(int* this_, char ch)
{
	char buf[16];
	buf[0] = ch;
	cBXString_ConcatImpl(this_, 1, buf);
	return this_;
}
#endif

// operator+= (string)
INCLUDE_ASM("bx/bxstring", func_003189A0);
#ifdef SKIP_ASM
int* func_003189A0(int* this_, int* other)
{
	cBXString_ConcatImpl(this_, *(int*)(*other - 8), (const char*)*other);
	return this_;
}
#endif

// Reserve capacity
INCLUDE_ASM("bx/bxstring", func_003189D0);
#ifdef SKIP_ASM
int func_003189D0(int* this_, int minCap)
{
	int data;
	if (*(int*)(*this_ - 0xC) < 2 && minCap <= *(int*)(*this_ - 4)) {
		data = *this_;
	}
	else {
		int tmp[4];
		cBXString_cBXString1(tmp, this_);
		int len = *(int*)(tmp[0] - 8);
		cBXString_Reset(this_);
		if (minCap <= len) {
			minCap = len;
		}
		cBXString_Realloc(this_, minCap);
		memcpy((void*)*this_, (void*)tmp[0], len);
		*(int*)(*this_ - 8) = len;
		*(char*)(*this_ + len) = 0;
		cBXString__cBXString(tmp, 2);
		data = *this_;
	}
	return data;
}
#endif

// Set length
INCLUDE_ASM("bx/bxstring", func_00318A88);
#ifdef SKIP_ASM
void func_00318A88(int* this_, int len)
{
	func_00318120(this_);
	if (len == -1) {
		len = strlen((const char*)*this_);
	}
	if ((char*)*this_ != D_0048DCD4) {
		*(int*)(*this_ - 8) = len;
		*(char*)(*this_ + len) = 0;
	}
}
#endif

// Resize and set length
INCLUDE_ASM("bx/bxstring", func_00318AF8);
#ifdef SKIP_ASM
int func_00318AF8(int* this_, int len)
{
	func_003189D0(this_, len);
	*(int*)(*this_ - 8) = len;
	*(char*)(*this_ + len) = 0;
	return *this_;
}
#endif

// Shrink to fit
INCLUDE_ASM("bx/bxstring", func_00318B40);
#ifdef SKIP_ASM
void func_00318B40(int* this_)
{
	int data = *this_;
	if (*(int*)(data - 8) != *(int*)(data - 4)) {
		int tmp[4];
		cBXString_cBXString1(tmp, this_);
		int len = *(int*)(tmp[0] - 8);
		cBXString_Reset(this_);
		cBXString_Realloc(this_, len);
		memcpy((void*)*this_, (void*)tmp[0], len);
		cBXString__cBXString(tmp, 2);
	}
}
#endif

// Lock (set refcount to -1)
INCLUDE_ASM("bx/bxstring", func_00318BC0);
#ifdef SKIP_ASM
void func_00318BC0(int* this_)
{
	func_003189D0(this_, 0);
	*(int*)(*this_ - 0xC) = -1;
}
#endif

// Unlock (set refcount to 1)
INCLUDE_ASM("bx/bxstring", func_00318BF8);
#ifdef SKIP_ASM
void func_00318BF8(int* this_)
{
	int data = *this_;
	if (data != (int)D_0048DCD4) {
		*(int*)(data - 0xC) = 1;
	}
}
#endif

// Find char from offset (FindLastOf per symbol name)
INCLUDE_ASM("bx/bxstring", cBXString_FindLastOf);
#ifdef SKIP_ASM
//94.17%
//https://decomp.me/scratch/p8aRA
int cBXString_FindLastOf(int* this_, char ch, int offset)
{
	char* result = (char*)strchr((const char*)(*this_ + offset), ch);
	if (result) {
		return (int)result - *this_;
	}
	return -1;
}
#endif

// Find first digit
INCLUDE_ASM("bx/bxstring", func_00318C60);
#ifdef SKIP_ASM
int func_00318C60(int* this_)
{
	char* result = func_0041AC40((const char*)*this_);
	if (result) {
		return (int)result - *this_;
	}
	return -1;
}
#endif

// Find first occurrence of char
//100%
//https://decomp.me/scratch/ETxUq
INCLUDE_ASM("bx/bxstring", cBXString_FindFirstOf);
#ifdef SKIP_ASM
int cBXString_FindFirstOf(int* this_, char ch)
{
	char* result = func_0041ACC0((const char*)*this_, ch);
	if (result) {
		return (int)result - *this_;
	}
	return -1;
}
#endif

// Find substring from offset
INCLUDE_ASM("bx/bxstring", func_00318CE8);
#ifdef SKIP_ASM
int func_00318CE8(int* this_, const char* needle, int offset)
{
	char* result = func_0041AD80((const char*)(*this_ + offset), needle);
	if (result) {
		return (int)result - *this_;
	}
	return -1;
}
#endif

// ToUpper
INCLUDE_ASM("bx/bxstring", func_00318D28);
#ifdef SKIP_ASM
void func_00318D28(int* this_)
{
	func_00318120(this_);
	char* p = (char*)*this_;
	int len = *(int*)(p - 8);
	while (len = len - 1, -1 < len) {
		char c = *p;
		char upper = c - 0x20;
		if ((PTR_DAT_00499C81[(int)c] & 2) == 0) {
			upper = c;
		}
		*p = upper;
		p = p + 1;
	}
}
#endif

// ToLower
INCLUDE_ASM("bx/bxstring", func_00318D90);
#ifdef SKIP_ASM
void func_00318D90(int* this_)
{
	func_00318120(this_);
	char* p = (char*)*this_;
	int len = *(int*)(p - 8);
	while (len = len - 1, -1 < len) {
		char c = *p;
		char lower = c + 0x20;
		if ((PTR_DAT_00499C81[(int)c] & 1) == 0) {
			lower = c;
		}
		*p = lower;
		p = p + 1;
	}
}
#endif

// Replace all occurrences of a char
INCLUDE_ASM("bx/bxstring", func_00318DF8);
#ifdef SKIP_ASM
void func_00318DF8(int* this_, char find, char replace)
{
	func_00318120(this_);
	char* p = (char*)*this_;
	int len = *(int*)(p - 8);
	while (len = len - 1, -1 < len) {
		if (*p == find) {
			*p = replace;
		}
		p = p + 1;
	}
}
#endif

// SetAt (set char at index)
INCLUDE_ASM("bx/bxstring", func_00318E68);
#ifdef SKIP_ASM
void func_00318E68(int* this_, int index, char ch)
{
	func_00318120(this_);
	*(char*)(*this_ + index) = ch;
}
#endif

// Constructor with repeated char fill
INCLUDE_ASM("bx/bxstring", func_00318EB8);
#ifdef SKIP_ASM
int* func_00318EB8(int* this_, char ch, int count)
{
	*this_ = (int)PTR_DAT_004a3e90;
	if (0 < count) {
		cBXString_Realloc(this_, count);
		memset((void*)*this_, ch, count);
	}
	return this_;
}
#endif

// Assign from single char
INCLUDE_ASM("bx/bxstring", func_00318F20);
#ifdef SKIP_ASM
int* func_00318F20(int* this_, char ch)
{
	char buf[16];
	buf[0] = ch;
	cBXString_InitFromCString(this_, 1, buf);
	return this_;
}
#endif

// Substr wrapper (defaults)
INCLUDE_ASM("bx/bxstring", func_00318F58);
#ifdef SKIP_ASM
int* func_00318F58(int* this_)
{
	func_00318F90(this_);
	return this_;
}
#endif

// Substr(start, length)
INCLUDE_ASM("bx/bxstring", func_00318F90);
#ifdef SKIP_ASM
int* func_00318F90(int* result, int* src, int start, int length)
{
	int count = 0;
	if (-1 < length) {
		count = length;
	}
	if (start < 0) {
		start = 0;
	}
	int srcLen = *(int*)(*src - 8);
	if (srcLen < start + count) {
		count = srcLen - start;
	}
	int finalLen = 0;
	if (start <= srcLen) {
		finalLen = count;
	}
	int tmp[4];
	tmp[0] = (int)PTR_DAT_004a3e90;
	func_003181E8(src, tmp, finalLen, start, 0);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// Right(n) - extract last n chars
INCLUDE_ASM("bx/bxstring", func_00319028);
#ifdef SKIP_ASM
int* func_00319028(int* result, int* src, int count)
{
	if (count < 0) {
		count = 0;
	}
	else {
		int len = *(int*)(*src - 8);
		if (len < count) {
			count = len;
		}
	}
	int tmp[4];
	tmp[0] = (int)PTR_DAT_004a3e90;
	func_003181E8(src, tmp, count, *(int*)(*src - 8) - count, 0);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// Left(n) - extract first n chars
INCLUDE_ASM("bx/bxstring", cBXString_cBXString5);
#ifdef SKIP_ASM
int* func_003190A8(int* result, int* src, int count)
{
	if (count < 0) {
		count = 0;
	}
	else {
		int len = *(int*)(*src - 8);
		if (len < count) {
			count = len;
		}
	}
	int tmp[4];
	tmp[0] = (int)PTR_DAT_004a3e90;
	func_003181E8(src, tmp, count, 0, 0);
	cBXString_cBXString1(result, tmp);
	cBXString__cBXString(tmp, 2);
	return result;
}
#endif

// Left up to substring delimiter
INCLUDE_ASM("bx/bxstring", func_00319120);
#ifdef SKIP_ASM
int* func_00319120(int* result, int* src, const char* delim)
{
	int pos = (int)func_0041AD10((const char*)*src, delim);
	func_003190A8(result, src, pos);
	return result;
}
#endif

// Left up to char delimiter
INCLUDE_ASM("bx/bxstring", func_00319170);
#ifdef SKIP_ASM
int* func_00319170(int* result, int* src, const char* delim)
{
	int pos = (int)func_0041AB08((const char*)*src, delim);
	func_003190A8(result, src, pos);
	return result;
}
#endif
