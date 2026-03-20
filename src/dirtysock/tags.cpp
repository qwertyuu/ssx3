#include "common.h"

// External C library functions
extern "C" {
    uint strlen(const char *s);
    void *memcpy(void *dst, const void *src, uint n);
    void *memmove(void *dst, const void *src, uint n);
    int memcmp(const void *s1, const void *s2, uint n);
    void *memset(void *s, int c, uint n);
    char *strcpy(char *dst, const char *src);
    char *strcat(char *dst, const char *src);
    int sprintf(char *buf, const char *fmt, ...);
}

// Internal tag functions (forward declarations)
extern "C" {
    char *cDirtysock_tag__TagFieldSetupAppend(char *message, char *buffer, char *key);
    int cDirtysock_tag__TagFieldSetupTerm(char *buffer, int buflen, char *tagfield);
    char *cDirtysock_tag_TagFieldFind(char *buffer, char *name);
    int cDirtysock_tag_TagFieldDelete(char *buffer, char *name);
    int cDirtysock_tag_TagFieldDupl(char *dst, int dstlen, char *src);
    int cDirtysock_tag_TagFieldSetNumber(char *buffer, int buflen, char *name, int value);
    int cDirtysock_tag_TagFieldSetString(char *buffer, int buflen, char *name, char *value);
    int cDirtysock_tag_TagFieldSetEpoch(char *buffer, int buflen, char *name, int epoch);
    uint cDirtysock_tag_TagFieldGetTime(char *value, uint defval);
}

// External dirtysock time functions
extern "C" {
    uint func_003EB090(int *timedata);     // time struct to epoch
    void *func_003EAEB8(void *buf, int epoch); // epoch to time struct
    char *func_003EAE68(char *str, void *out); // parse number from string
    int func_003EF088(void);               // get current epoch time
}

// Global field separator (typically '\n')
extern char bGp00001980;

// Character tables
extern char DAT_004962c0[];  // hex decode table
extern char DAT_004963c0[];  // case-insensitive comparison table
extern char DAT_004964c0[];  // case-insensitive mapping table (tolower)
extern char DAT_004965c0[];  // flag character string "@ABCDEFGHIJKLMNOPQRSTUVWXYZ0123"
extern uint DAT_004965e0[];  // flag bitmask table

// Hex encode/decode lookup table
extern char s_hex_encode_table[];  // "0000...ffff...0123456789abcdef..."

// String constants
extern char DAT_004a4a78[];  // empty key name for func_003EB8B8
extern char DAT_004A4A80[];  // default suffix string for func_003EBD70
extern char DAT_004a4a88[];  // default crypt key
extern char DAT_004a4a90[];  // empty string ""
extern char s_epoch_fmt[];   // "%04d-%02d-%02dT%02d:%02d:%02d"

//100%
//https://decomp.me/scratch/fZ4tu
INCLUDE_ASM("dirtysock/tags", cDirtysock_tag__TagFieldSetupAppend__FPcN20);
#ifdef SKIP_ASM
char* cDirtysock_tag__TagFieldSetupAppend(char* message, char* buffer, char* key)
{
    if (key == (char*)0x0) {
        *message = 0;
    }
    else {
        while (*key != 0x0) {
            *buffer = *key;
            key = key + 1;
            buffer = buffer + 1;
        }
        *buffer = '=';
        buffer = buffer + 1;
    }
    return buffer;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag__TagFieldSetupTerm);
#ifdef SKIP_ASM
int cDirtysock_tag__TagFieldSetupTerm(char *buffer, int buflen, char *tagfield)
{
    char firstchar;
    char *insert;
    char *scan;
    char *end;
    int taglen;
    int diff;

    if (buffer == (char *)0x0) {
        return -1;
    }
    if (tagfield == (char *)0x0) {
        return -1;
    }
    firstchar = *tagfield;
    if ((unsigned char)firstchar < 0x21) {
        return -1;
    }

    scan = buffer;
    insert = scan;

    if (firstchar == 0x7e) {
        // Append mode: find end of buffer
        while (*scan != 0) {
            scan = scan + 1;
        }
        if (scan != buffer && (unsigned char)scan[-1] > 0x1f &&
            (unsigned char)scan[-1] != (unsigned char)bGp00001980 &&
            scan < buffer + buflen - 1) {
            *scan = bGp00001980;
            scan = scan + 1;
            *scan = 0;
        }
        firstchar = *tagfield;
        insert = scan;
    }
    else {
        // Search for existing field
        while (*scan != 0) {
            if ((unsigned char)*scan < 0x21) {
                scan = scan + 1;
            }
            else {
                if (DAT_004964c0[(unsigned char)*scan] == 1) goto found_insert;

                char scanmap = DAT_004964c0[(unsigned char)*scan];
                uint scanval = (uint)(unsigned char)scanmap;
                char tagmap = DAT_004964c0[(unsigned char)firstchar];
                uint tagval = (uint)(unsigned char)tagmap;
                int match;

                if (scanmap == tagmap) {
                    match = (uint)(unsigned char)scanmap + (uint)(unsigned char)tagmap;
                    if ((unsigned char)scanmap > 1) {
                        match = 1;
                        char *tp = tagfield;
                        while (1) {
                            tp = tp + 1;
                            tagval = (uint)(unsigned char)DAT_004964c0[(unsigned char)*tp];
                            scanmap = DAT_004964c0[(unsigned char)scan[match]];
                            scanval = (uint)(unsigned char)scanmap;
                            if (scanmap != DAT_004964c0[(unsigned char)*tp] || (unsigned char)scanmap < 2) break;
                            match = match + 1;
                        }
                        goto compute_match;
                    }
                }
                else {
compute_match:
                    match = scanval + tagval;
                }

                if (match == 2) {
                    // Found matching field - skip to value
                    insert = scan;
                    while ((unsigned char)*insert > 0x1f) {
                        insert = insert + 1;
                    }
                    if (*insert != 0) {
                        insert = insert + 1;
                    }
                    goto found_insert;
                }

                // Skip this field
                do {
                    scan = scan + 1;
                } while ((unsigned char)*scan > 0x1f);
            }
        }

        // Not found, append at end
        if (scan != buffer) {
            if ((unsigned char)scan[-1] < 0x20) {
                firstchar = *tagfield;
                goto found_insert;
            }
            if ((unsigned char)scan[-1] != (unsigned char)bGp00001980) {
                if (buffer + buflen - 1 <= scan) {
                    firstchar = *tagfield;
                    goto found_insert;
                }
                *scan = bGp00001980;
                scan = scan + 1;
                *scan = 0;
            }
        }
        firstchar = *tagfield;
        insert = scan;
    }

found_insert:
    taglen = 0;
    if ((signed char)firstchar > 0x1f) {
        for (taglen = 1; (signed char)tagfield[taglen] > 0x1f; taglen = taglen + 1) {
        }
    }
    int newlen = taglen + 1;
    diff = newlen - ((int)insert - (int)scan);

    // Find end of remaining data
    end = insert;
    while (*end != 0) {
        end = end + 1;
    }
    end = end + 1;

    if (diff > 0 && (int)(buffer + buflen - (int)end) < diff) {
        return -1;
    }
    if (diff == 0 && memcmp(scan, tagfield, newlen) == 0) {
        return 0;
    }
    if (diff > 0) {
        memmove(insert + diff, insert, (int)end - (int)insert);
    }
    if (diff < 0) {
        memmove(scan, (char *)((int)scan - diff), (int)end - ((int)scan - diff));
    }
    memcpy(scan, tagfield, newlen);
    scan = scan + newlen;
    if (*scan == 0 && bGp00001980 != 10) {
        scan[-1] = 0;
        return taglen;
    }
    scan[-1] = bGp00001980;
    return taglen;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EB578);
#ifdef SKIP_ASM
// Sets the field separator character, returns old value
char func_003EB578(char newSep)
{
    char old = bGp00001980;
    bGp00001980 = newSep;
    return old;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EB588);
#ifdef SKIP_ASM
// Normalizes tag field separators in a buffer
void func_003EB588(char *buffer, char sep)
{
    char *write;
    char *read;
    char *orig;
    char ch;
    char next;

    sep = (unsigned char)sep;
    orig = buffer;
    write = buffer;
    read = buffer;
    ch = (unsigned char)*read;
    if (ch == 0) goto after_first_pass;

    // First pass: copy chars, replace control chars with separator
    while (1) {
        if ((unsigned char)ch >= 0x20) {
            // printable char: copy it
            *write = ch;
            read++;
            write++;
            ch = (unsigned char)*read;
        } else {
            // control char: write separator and skip control sequence
            *write = sep;
            write++;

            // skip remaining control chars
            while (1) {
                next = (unsigned char)*read;
                if ((unsigned char)(next - 1) >= 0x1F) break;
                read++;
            }

            // check for field delimiter (= or :)
            next = (unsigned char)*read;
            if (next == '=') {
                ch = (unsigned char)*(read + 1);
            } else if (next == ':') {
                ch = (unsigned char)*(read + 1);
            } else {
                goto check_loop;
            }

            // if next after delimiter is control/null, we're at end
            if ((unsigned char)ch < 0x20) goto after_first_pass;
            ch = (unsigned char)*read;
            goto check_loop;
        }
    check_loop:
        if (ch == 0) break;
    }

after_first_pass:
    // Fix trailing separator
    if (write != orig) {
        if ((unsigned char)*(write - 1) == sep) {
            if (sep == '\n') {
                // keep newline separator
                *(write - 1) = '\n';
            } else if ((unsigned char)*read != 0) {
                *(write - 1) = '\n';
            } else {
                *(write - 1) = 0;
            }
        }
    }

    ch = (unsigned char)*read;

    // Second pass: remaining data
    if (ch == 0) {
        *write = 0;
        return;
    }

    while (1) {
        if ((unsigned char)ch >= 0x20) {
            *write = ch;
            read++;
            write++;
            ch = (unsigned char)*read;
        } else {
            *write = '\n';
            sep = (unsigned char)*read;
            read++;
            ch = (unsigned char)*read;
            write++;

            if ((unsigned char)(ch - 1) < 0x1F) {
                if (ch != sep) {
                    read++;
                    while (1) {
                        ch = (unsigned char)*read;
                        if ((unsigned char)(ch - 1) >= 0x1F) break;
                        if (ch == sep) break;
                        read++;
                    }
                }
            }
        }
        if (ch == 0) break;
    }
    *write = 0;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldFind);
#ifdef SKIP_ASM
char *cDirtysock_tag_TagFieldFind(char *buffer, char *name)
{
    char *nameend;
    char *scan;
    char *check;
    char *namep;

    if (buffer == (char *)0x0 || *buffer == 0 || name == (char *)0x0 || *name == 0) {
        return (char *)0x0;
    }

    // Find end of name
    nameend = name;
    do {
        nameend = nameend + 1;
    } while (nameend[0] != 0);
    nameend = nameend - 1; // last char of name

    scan = buffer;
    while (*scan != 0) {
        if (*scan == '=' || *scan == ':') {
            char nextch = scan[1];
            // Check if value separator with empty value
            if ((unsigned char)nextch < 0x20 && (unsigned char)scan[-1] < 0x21) {
                return (char *)0x0;
            }
            check = scan - 1;
            if (buffer <= check && DAT_004963c0[(unsigned char)scan[-1]] == DAT_004963c0[(unsigned char)*nameend]) {
                char *valuep;
                if (nextch != 0x20) {
                    valuep = scan + 1;
                } else {
                    valuep = scan + 2;
                }
                namep = nameend;
                do {
                    if (namep == name) {
                        if (check == buffer || (unsigned char)check[-1] < 0x21) {
                            return valuep;
                        }
                    }
                    check = check - 1;
                    namep = namep - 1;
                } while (buffer <= check && DAT_004963c0[(unsigned char)*check] == DAT_004963c0[(unsigned char)*namep]);
            }
            scan = scan + 1;
        }
        else {
            scan = scan + 1;
        }
    }
    return (char *)0x0;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EB840);
#ifdef SKIP_ASM
// TagFieldFind with optional prefix
char *func_003EB840(char *buffer, char *prefix, char *name)
{
    char temp[256];

    if (prefix == 0) {
        return cDirtysock_tag_TagFieldFind(buffer, name);
    }
    else if (name == 0) {
        return cDirtysock_tag_TagFieldFind(buffer, prefix);
    }
    else {
        strcpy(temp, prefix);
        strcat(temp, name);
        return cDirtysock_tag_TagFieldFind(buffer, temp);
    }
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EB8B8);
#ifdef SKIP_ASM
// Find first value field in buffer
char *func_003EB8B8(char *buffer)
{
    char *value;

    value = cDirtysock_tag_TagFieldFind(buffer, DAT_004a4a78);
    if (value != (char *)0x0) {
        while ((unsigned char)*value - 1 < 0x20) {
            value = value + 1;
        }
        if (*value == 0) {
            value = (char *)0x0;
        }
    }
    return value;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldDelete);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldDelete(char *buffer)
{
    char *value;
    char *start;

    value = cDirtysock_tag_TagFieldFind(buffer);
    start = value;
    if (value == (char *)0x0) {
        return -1;
    }

    // Find start of key
    while (1) {
        if (start == buffer) break;
        if ((unsigned char)start[-1] < 0x21) break;
        start = start - 1;
    }

    // Skip value
    while ((unsigned char)*value > 0x1f) {
        value = value + 1;
    }
    // Skip separator
    while ((unsigned char)*value - 1 < 0x1f) {
        value = value + 1;
    }

    // Move remaining data
    if (*value == 0) {
        *start = 0;
    }
    else {
        do {
            *start = *value;
            value = value + 1;
            start = start + 1;
        } while (*value != 0);
        *start = 0;
    }
    return 0;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EB9E8);
#ifdef SKIP_ASM
// Rename a tag field key
int func_003EB9E8(char *buffer, int buflen, char *oldname, char *newname)
{
    char *value;
    char *keystart;
    char *keyend;
    char *scan;
    int newnamelen;
    int diff;

    value = cDirtysock_tag_TagFieldFind(buffer, oldname);
    if (value == (char *)0x0) {
        return -1;
    }
    if (value[-1] == ' ') {
        value = value - 1;
    }
    keyend = value - 1;
    keystart = keyend;
    while (keystart != buffer && (unsigned char)keystart[-1] > 0x20) {
        keystart = keystart - 1;
    }

    newnamelen = strlen(newname);
    diff = newnamelen - ((int)keyend - (int)keystart);

    if (diff < 0) {
        if (*keyend == '\0') {
            *keyend = '\0';
        }
        else {
            keyend[diff] = *keyend;
            while (*keyend != '\0') {
                keyend[diff] = *keyend;
            }
            *keyend = '\0';
        }
    }
    if (diff > 0) {
        scan = keyend;
        if (*keyend != '\0') {
            for (; *value != '\0'; value = value + 1) {
                scan = value;
            }
        }
        if ((int)scan - (int)buffer < buflen - diff) {
            return -1;
        }
        if (scan == keyend) {
            goto copy_name;
        }
        do {
            char ch = *scan;
            char *dst = scan + diff;
            scan = scan - 1;
            *dst = ch;
        } while (scan != keyend);
    }
copy_name:
    while (*newname != '\0') {
        *keystart = *newname;
        newname = newname + 1;
        keystart = keystart + 1;
    }
    return 0;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldDupl);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldDupl(char *dst, int dstlen, char *src)
{
    int remaining = dstlen;
    for (; remaining > 1 && *src != '\0'; src = src + 1) {
        *dst = *src;
        remaining = remaining - 1;
        dst = dst + 1;
    }
    if (remaining > 0) {
        *dst = '\0';
    }
    return dstlen - remaining;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EBBA8);
#ifdef SKIP_ASM
// Extract the Nth value from a comma/separator-delimited tag field value
int func_003EBBA8(char *src, char *dst, int dstlen, int index)
{
    int written = 0;
    char *scan;
    char *end;
    char ch;
    char next_ch;

    if (dstlen <= 0) {
        *dst = 0;
    }
    if (src == (char *)0x0) {
        return 0;
    }
    ch = (unsigned char)*src;
    if (ch == 0) {
        return 0;
    }
    scan = src;

    // Skip to the Nth field (skip 'index' delimiters: '=' or ':')
    while (1) {
        ch = (unsigned char)*scan;
        if (ch == 0) goto done;
        if (ch == '=') {
            next_ch = (unsigned char)*(scan + 1);
            if ((unsigned char)next_ch < 0x20) {
                // At end of field
                goto extract;
            }
            scan++;
            continue;
        }
        if (ch == ':') {
            next_ch = (unsigned char)*(scan + 1);
            if ((unsigned char)next_ch < 0x20) {
                goto extract;
            }
            scan++;
            continue;
        }
        scan++;
    }

extract:
    // Found field, now check if trailing whitespace
    end = scan;
    if (end != src) {
        while (end != src) {
            if ((unsigned char)*(end - 1) >= 0x21) break;
            end--;
        }
    }

    // Copy value chars to dst within dstlen
    if (end != scan) {
        char *p = end;
        while (1) {
            *(dst + written) = *p;
            written++;
            p++;
            if (p == scan) break;
            if (written + 1 >= dstlen) break;
        }
    }

done:
    *(dst + written) = 0;
    return written;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EBCB0);
#ifdef SKIP_ASM
// Append a key=value entry to a tag field buffer; returns 0 on success, -1 if no space
int func_003EBCB0(char *buffer, int buflen, char *value)
{
    char *end;
    char *limit;
    char ch;

    end = buffer;

    // Find end of existing content
    while (1) {
        ch = (unsigned char)*end;
        if (ch == 0) break;
        if (ch == '=') {
            if ((unsigned char)*(end + 1) < 0x20) break;
            end++;
            continue;
        }
        if (ch == ':') {
            if ((unsigned char)*(end + 1) < 0x20) break;
            end++;
            continue;
        }
        end++;
    }

    // Null terminate at end
    *end = 0;

    if (value == (char *)0x0) {
        return 0;
    }

    // Check space: need at least 5 bytes (=\nvalue\0)
    limit = buffer + buflen - 1;
    if ((int)(limit - end) < 5) {
        return -1;
    }

    // Write '=\n' then copy value
    *end = '=';
    end++;
    *end = '\n';
    end++;

    ch = *value;
    if (ch != 0) {
        while (end != limit) {
            *end = (unsigned char)*value;
            value++;
            end++;
            ch = *value;
            if (ch == 0) break;
        }
    }
    *end = 0;
    return 0;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EBD70);
#ifdef SKIP_ASM
// Pad/format a tag field value to a minimum width, appending default suffix
int func_003EBD70(char *buffer, int buflen)
{
    int len;
    char ch;
    char sep;
    char *p;

    len = 0;
    ch = (unsigned char)*buffer;
    if (ch != 0) {
        len = 1;
        while ((unsigned char)*(buffer + len) != 0) {
            len++;
        }
    }

    if (len <= 0) {
        goto pad;
    }

    // Check if trailing char is whitespace
    if ((unsigned char)*(buffer + len - 1) < 0x20) {
        goto pad_check;
    }

    // Check if trailing char is the field separator
    sep = bGp00001980;
    if ((unsigned char)*(buffer + len - 1) == (unsigned char)sep) {
        goto done;
    }

    // Ensure we have space for separator + padding
    if (len >= buflen - 1) {
        goto pad_check;
    }

    // Append separator
    sep = (unsigned char)bGp00001980;
    *(buffer + len) = sep;
    len++;
    *(buffer + len) = 0;

done:
    // Check if len + 4 fits in buflen
pad_check:
    if (len + 4 >= buflen) {
        return -1;
    }

pad:
    // Append default suffix from D_004A4A80
    p = DAT_004A4A80;
    ch = *p;
    if (ch != 0) {
        while (1) {
            *(buffer + len) = (unsigned char)*p;
            p++;
            len++;
            ch = *p;
            if (ch == 0) break;
        }
    }
    *(buffer + len) = 0;
    return -1;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetNumber);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetNumber(char *buffer, int buflen, char *name, int value)
{
    char tagbuf[288];
    char *p;

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    if (value < 0) {
        value = -value;
        *p = '-';
        p = p + 1;
    }
    char *end = p + 0x1f;
    p[0x1f] = '\0';
    char *digit = end;
    for (; value > 0; value = value / 10) {
        digit = digit - 1;
        *digit = (char)(value % 10) + '0';
    }
    if (*digit == '\0') {
        digit = digit - 1;
        *digit = '0';
    }
    while (*digit != '\0') {
        *p = *digit;
        digit = digit + 1;
        p = p + 1;
    }
    *p = '\0';
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetFlags);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetFlags(char *buffer, int buflen, char *name, uint flags)
{
    char tagbuf[288];
    char *p;

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    char *flagchars = DAT_004965c0;
    char ch = DAT_004965c0[0];
    if (flags != 0) {
        while (ch != '\0') {
            uint bit = flags & 1;
            flags = (int)flags >> 1;
            if (bit != 0) {
                *p = *flagchars;
                p = p + 1;
            }
            flagchars = flagchars + 1;
            if (flags == 0) break;
            ch = *flagchars;
        }
    }
    *p = '\0';
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetAddress);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetAddress(char *buffer, int buflen, char *name, uint addr)
{
    char tagbuf[288];
    unsigned char octets[4];
    char *p;
    int i;

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    octets[3] = (unsigned char)(addr);
    octets[2] = (unsigned char)(addr >> 8);
    octets[1] = (unsigned char)(addr >> 16);
    octets[0] = (unsigned char)(addr >> 24);
    for (i = 0; i < 4; i++) {
        unsigned char val = octets[i];
        if (i > 0) {
            *p = '.';
            p = p + 1;
        }
        if (val > 9) {
            uint uval = (uint)val;
            if (val >= 100) {
                uint rem = uval % 100;
                *p = val / 100 + 0x30;
                p = p + 1;
                val = (unsigned char)(rem % 10);
                uval = rem;
            } else {
                val = val % 10;
            }
            *p = (char)(uval / 10) + '0';
            p = p + 1;
        }
        *p = val + 0x30;
        p = p + 1;
    }
    *p = '\0';
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

//23.48%
//https://decomp.me/scratch/KjMXX
INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetFourCC);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetFourCC(char *buffer, int buflen, char *name, uint fourcc)
{
    char tagbuf[288];
    char *p;

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    for (; fourcc != 0; fourcc = fourcc << 8) {
        if (fourcc > 0x20ffffff) {
            *p = (char)(fourcc >> 24);
            p = p + 1;
        }
    }
    *p = 0;
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetString);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldSetString(char *buffer, int buflen, char *name, char *value)
{
    char tagbuf[2560];
    char *p;
    int needquote;
    char *scan;
    unsigned char ch;

    if (value == (char *)0x0) {
        cDirtysock_tag_TagFieldDelete(buffer);
        return 0;
    }

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);

    // Check if value contains spaces (needs quoting)
    needquote = 0;
    scan = value;
    if (*value == 0) {
        if (needquote == 0x20) goto add_quote;
        goto start_encode;
    }
    if (*value != 0x20) {
        do {
            scan = scan + 1;
            if (*scan == 0) {
                ch = *scan;
                if (needquote == 0x20) goto add_quote;
                goto start_encode;
            }
        } while (*scan != 0x20);
    }
    needquote = *scan;
    if (needquote == 0x20) {
add_quote:
        *p = '"';
        p = p + 1;
    }

start_encode:
    ch = (unsigned char)*value;
    while (1) {
        if (ch == 0) {
            if (needquote == 0x20) {
                *p = '"';
                p[1] = 0;
            } else {
                *p = 0;
            }
            return cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
        }
        if (ch - 0x20 < 0x5f) {
            if (ch == '=' || ch == '"' || ch == ':') {
                *p = '%';
                p[1] = s_hex_encode_table[(unsigned char)*value];
                p = p + 2;
                *p = s_hex_encode_table[(unsigned char)*value + 0x100];
            } else if (ch == '%') {
                *p = '%';
                p[1] = s_hex_encode_table[(unsigned char)*value];
                p = p + 2;
                *p = s_hex_encode_table[(unsigned char)*value + 0x100];
            } else {
                *p = ch;
            }
        } else {
            *p = '%';
            p[1] = s_hex_encode_table[(unsigned char)*value];
            p = p + 2;
            *p = s_hex_encode_table[(unsigned char)*value + 0x100];
        }
        value = value + 1;
        ch = (unsigned char)*value;
        p = p + 1;
    }
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetBinary);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetBinary(char *buffer, int buflen, char *name, unsigned char *data, int datalen)
{
    char tagbuf[4352];
    char *p;

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    *p = '$';
    p = p + 1;
    for (; datalen > 0; datalen = datalen - 1) {
        *p = s_hex_encode_table[*data];
        unsigned char val = *data;
        data = data + 1;
        p[1] = s_hex_encode_table[val + 0x100];
        p = p + 2;
    }
    *p = '\0';
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetStructure);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetStructure(char *buffer, int buflen, char *name, uint *data,
                                          int datalen, char *format)
{
    char tagbuf[2560];
    char *p;
    uint *dataend;
    uint value;
    char fmtch;
    int hexlen;
    uint *strptr;

    value = 0;
    if (datalen < 0) {
        dataend = data + 0x4000;
    } else {
        dataend = (uint *)((char *)data + datalen);
    }

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    fmtch = *format;

    while (1) {
        if (fmtch == 0) {
            // Trim trailing commas
            if (p[-1] == ',') {
                do {
                    char *prev = p - 1;
                    p = prev;
                } while (p[-1] == ',');
                *p = 0;
            } else {
                *p = 0;
            }
            cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
            return;
        }

        uint charval;
        if (fmtch == '#') {
            // Skip comment until '='
            format = format + 1;
            do {
                fmtch = *format;
                if (fmtch == 0) {
                    charval = (uint)(unsigned char)*format;
                    goto parse_num;
                }
                format = format + 1;
            } while (fmtch != '=');
            charval = (uint)(unsigned char)*format;
        } else {
            charval = (uint)(unsigned char)*format;
        }

parse_num:
        hexlen = 0;
        char *fmtp = format;
        int count = 0;
        while (charval - 0x30 < 10) {
            fmtp = fmtp + 1;
            count = count * 10 + (charval & 0xf);
            charval = (uint)(unsigned char)*fmtp;
        }

        int numdigits = 0;
        fmtch = *fmtp;
        strptr = (uint *)0x0;

        if (*fmtp == 'a') {
            if (count == 0) {
                data = (uint *)((char *)data + 1);
            } else {
                data = (uint *)((char *)data + count);
            }
        }
        if (fmtch == 'b') {
            value = (uint)(unsigned char)*(char *)data;
            numdigits = 2;
            data = (uint *)((char *)data + 1);
        }
        if (fmtch == 'w') {
            value = (uint)(unsigned short)*(short *)data;
            numdigits = 4;
            data = (uint *)((char *)data + 2);
        }
        if (fmtch == 'l') {
            value = *data;
            numdigits = 8;
            data = data + 1;
        }

        uint *nextdata = data;
        if (fmtch == 's') {
            nextdata = (uint *)((char *)data + count);
            strptr = data;
        }

        if (dataend < nextdata) {
            // Past end, terminate
            if (p[-1] == ',') {
                do {
                    char *prev = p - 1;
                    p = prev;
                } while (p[-1] == ',');
                *p = 0;
            } else {
                *p = 0;
            }
            cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
            return;
        }

        char *outp = p;
        if (numdigits != 0) {
            if (value > 0xffff0000) {
                value = -value;
                *p = '-';
                p = p + 1;
            }
            outp = p + numdigits;
            *outp = 0;
            char *dp = outp;
            for (; value != 0; value = value >> 4) {
                dp = dp - 1;
                *dp = s_hex_encode_table[(value & 0xf) + 0x100];
            }
            if (p != dp) {
                while (*dp != 0) {
                    *p = *dp;
                    dp = dp + 1;
                    p = p + 1;
                }
                *p = ',';
                outp = p + 1;
            }
        }

        if (strptr != (uint *)0x0) {
            unsigned char sch = *(unsigned char *)strptr;
            while (sch != 0) {
                uint sv = *(unsigned char *)strptr;
                uint sval = (uint)(unsigned char)sv;
                strptr = (uint *)((char *)strptr + 1);
                if (DAT_004964c0[sval] == '\0') {
                    *outp = '%';
                    outp[1] = s_hex_encode_table[sval];
                    outp = outp + 2;
                    *outp = s_hex_encode_table[sval + 0x100];
                } else {
                    *outp = (char)sv;
                }
                outp = outp + 1;
                sch = *(unsigned char *)strptr;
            }
            *outp = ',';
            outp = outp + 1;
        }

        format = fmtp + 1;
        if (fmtp[1] == '*') {
            format = fmtp;
        }
        fmtch = *format;
        data = nextdata;
        p = outp;
    }
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetCrypt);
#ifdef SKIP_ASM
void cDirtysock_tag_TagFieldSetCrypt(char *buffer, int buflen, char *name, unsigned char *key,
                                      unsigned char *data)
{
    char tagbuf[4352];
    char *p;
    uint hash;
    unsigned char *keyp;

    if (key == (unsigned char *)0x0 || *key == 0) {
        key = (unsigned char *)DAT_004a4a88;
    }

    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    hash = 0;
    *p = '$';
    p = p + 1;

    keyp = key;
    while (*data != 0) {
        hash = *data ^ hash;
        unsigned char kch = *keyp;
        keyp = keyp + 1;
        data = data + 1;
        if (*keyp == 0) {
            keyp = key;
        }
        hash = ((uint)kch ^ (hash << 3 | hash >> 5)) & 0xff;
        *p = s_hex_encode_table[hash];
        p[1] = s_hex_encode_table[hash + 0x100];
        p = p + 2;
    }
    *p = '\0';
    cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldSetEpoch);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldSetEpoch(char *buffer, int buflen, char *name, int epoch)
{
    char tagbuf[288];
    char timebuf[48];
    char *p;

    if (epoch == 0) {
        epoch = func_003EF088();
    }
    void *timedata = func_003EAEB8(timebuf, epoch);
    if (timedata == 0) {
        return -1;
    }
    p = cDirtysock_tag__TagFieldSetupAppend(buffer, tagbuf, name);
    int *td = (int *)timedata;
    sprintf(p, s_epoch_fmt, td[5] + 0x76c, td[4] + 1, td[3], td[2], td[1], td[0]);
    return cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, tagbuf);
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EC8B8);
#ifdef SKIP_ASM
// Set epoch from individual date/time components
void func_003EC8B8(char *buffer, int buflen, char *name, int year, int month,
                    int day, int hour, int minute, int second, int offset)
{
    int timedata[9];

    memset(timedata, 0, 0x24);
    timedata[5] = year - 0x76c;
    timedata[8] = -1;
    timedata[4] = month - 1;
    timedata[0] = second;
    timedata[1] = minute;
    timedata[2] = hour;
    timedata[3] = day;
    int epoch = func_003EB090(timedata);
    int result = 0;
    if (epoch != 0) {
        result = epoch - offset;
    }
    cDirtysock_tag_TagFieldSetEpoch(buffer, buflen, name, result);
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EC9A0);
#ifdef SKIP_ASM
// Set epoch from packed date/time values
void func_003EC9A0(char *buffer, int buflen, char *name, uint date, uint time, int offset)
{
    int timedata[9];

    memset(timedata, 0, 0x24);
    timedata[4] = ((int)date >> 8 & 0xff) - 1;
    timedata[5] = (date >> 16) - 0x76c;
    timedata[2] = (int)time >> 16 & 0xff;
    timedata[1] = (int)time >> 8 & 0xff;
    timedata[3] = date & 0xff;
    timedata[0] = time & 0xff;
    timedata[8] = -1;
    int epoch = func_003EB090(timedata);
    int result = 0;
    if (epoch != 0) {
        result = epoch - offset;
    }
    cDirtysock_tag_TagFieldSetEpoch(buffer, buflen, name, result);
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003ECA88);
#ifdef SKIP_ASM
// Get raw tag field value (including key name)
int func_003ECA88(char *buffer, int buflen, char *name, char *srcbuf)
{
    char *value;

    value = cDirtysock_tag_TagFieldFind(srcbuf, name);
    if (value == 0) {
        return 0;
    }
    // Back up to start of key
    while (value != srcbuf && (unsigned char)value[-1] > 0x1f) {
        value = value - 1;
    }
    return cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, value);
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003ECB00);
#ifdef SKIP_ASM
// Merge/copy multiple tag fields from source to destination
int func_003ECB00(char *buffer, int buflen, char *src)
{
    int count = 0;

    while (1) {
        while (1) {
            unsigned char ch = (unsigned char)*src;
            if (ch == 0) {
                return count;
            }
            if (ch > 0x20) break;
            src = src + 1;
        }
        if (*src == '=') break;
        if (*src == ':') {
            return count;
        }
        int result = cDirtysock_tag__TagFieldSetupTerm(buffer, buflen, src);
        count = count + (uint)(result > 0);
        while ((unsigned char)*src > 0x1f) {
            src = src + 1;
        }
    }
    return count;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003ECBD8);
#ifdef SKIP_ASM
// Format tag fields with printf-style format string
int func_003ECBD8(char *output, int outlen, char *format, ...)
{
    // This function uses varargs stored in stack
    // Simplified representation - actual implementation reads varargs from stack
    char *fmt = format;
    char *out = output;
    int remaining = outlen;
    unsigned char ch = *fmt;

    while (1) {
        if (ch == 0 || remaining < 2) {
            if (remaining > 0) {
                *out = 0;
            }
            return outlen - remaining;
        }

        char *next = fmt + 1;
        if (ch < 0x21) {
            remaining = remaining - 1;
            *out = bGp00001980;
            out = out + 1;
            if (*next != 0) {
                if (*next > ' ') {
                    ch = *next;
                    fmt = next;
                    continue;
                }
                next = fmt + 2;
                while (*next != 0) {
                    if (*next > ' ') {
                        ch = *next;
                        fmt = next;
                        goto continue_loop;
                    }
                    next = next + 1;
                }
            }
            ch = *next;
            fmt = next;
            continue;
        }

        if (ch != '%') {
            *out = ch;
            remaining = remaining - 1;
            out = out + 1;
            ch = *next;
            fmt = next;
            continue;
        }

        // Format specifier
        int len = 0;
        if (*next == 0) {
            ch = *next;
            fmt = next;
            continue;
        }

        unsigned char spec = *next;
        next = fmt + 2;

        if (out[-1] == '=') {
            out = out - 1;
            remaining = remaining + 1;
        }
        *out = 0;

        // Handle format specifiers: %s, %d, %a, %f, %e, %r
        // (varargs handling simplified - actual impl reads from stack pointer)
        ch = *next;
        fmt = next;
continue_loop:
        ;
    }
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003ECE38);
#ifdef SKIP_ASM
// Find and extract a named field value from tag string; copies to output buffer
char *func_003ECE38(char *tagstr, char *output, int outlen, char *defval)
{
    char *scan;
    char *src;
    int copied;
    char ch;

    scan = tagstr;

    // Find '=' or ':' delimiter (skip printable chars)
    while (1) {
        ch = (unsigned char)*scan;
        if (ch == 0) goto notfound;
        if (ch == '=') {
            ch = (unsigned char)*(scan + 1);
            if ((unsigned char)ch < 0x20) break;
            scan++;
            continue;
        }
        if (ch == ':') {
            ch = (unsigned char)*(scan + 1);
            if ((unsigned char)ch < 0x20) break;
            scan++;
            continue;
        }
        scan++;
    }

    // Found delimiter, skip past it
    scan += 2;
    if (output == (char *)0x0) {
        return scan;
    }

    // Copy value to output
    outlen--;
    if (outlen <= 0) goto done_copy;
    copied = 0;
    ch = (unsigned char)*scan;
    if (ch == 0) goto done_copy;

    src = scan;
    while (1) {
        *(output + copied) = (unsigned char)*src;
        copied++;
        src++;
        if (copied >= outlen) break;
        ch = (unsigned char)*src;
        if (ch == 0) break;
    }

done_copy:
    *(output + copied) = 0;
    return scan;

notfound:
    // Field not found: use default value
    if (output == (char *)0x0) {
        return (char *)0x0;
    }
    if (defval == (char *)0x0) {
        outlen--;
        if (outlen <= 0) goto done_def;
        copied = 0;
    defloop:
        ch = *defval;
        if (ch == 0) goto done_def;
        while (1) {
            *(output + copied) = (unsigned char)*defval;
            copied++;
            defval++;
            if (copied >= outlen) break;
            ch = *defval;
            if (ch == 0) break;
        }
    done_def:
        *(output + copied) = 0;
    }
    return (char *)0x0;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetNumber);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldGetNumber(char *value, int defval)
{
    int sign;
    int result;

    if (value == (char *)0x0) {
        return defval;
    }
    sign = 1;
    if (*value == '+') {
        value = value + 1;
    } else if (*value == '-') {
        sign = -1;
        value = value + 1;
    }

    result = 0;
    while ((unsigned char)*value - 0x30 < 10) {
        result = result * 10 + (*value & 0xf);
        value = value + 1;
    }
    return sign * result;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetFlags);
#ifdef SKIP_ASM
uint cDirtysock_tag_TagFieldGetFlags(char *value, uint defval)
{
    if (value != (char *)0x0) {
        char ch = *value;
        defval = 0;
        while (DAT_004965e0[(unsigned char)ch] != 0) {
            defval = defval | DAT_004965e0[(unsigned char)ch];
            value = value + 1;
            ch = *value;
        }
    }
    return defval;
}
#endif

//56.50%
//https://decomp.me/scratch/98Kir
INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetAddr);
#ifdef SKIP_ASM
uint cDirtysock_tag_TagFieldGetAddr(unsigned char *value, uint defval)
{
    uint result = 0;

    if (value == (unsigned char *)0x0) {
        return defval;
    }
    while (1) {
        unsigned char ch = *value;
        if (ch - 0x30 < 10) {
            result = (result & 0xffffff00) | ((result & 0xff) * 10 + (ch & 0xf));
        } else {
            if (ch != '.') {
                return result;
            }
            result = result << 8;
        }
        value = value + 1;
    }
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetFourCC);
#ifdef SKIP_ASM
uint cDirtysock_tag_TagFieldGetFourCC(char *value, uint defval)
{
    uint result;

    if (value != (char *)0x0) {
        char ch = *value;
        if (ch > ' ' && ch < 0x7f) {
            result = 0x20202020;
            while (1) {
                result = (result << 8) | (uint)(unsigned char)ch;
                value = value + 1;
                if ((int)result > 0x20fffffe) {
                    return (uint)result;
                }
                ch = *value;
                if (ch < '!') {
                    return (uint)result;
                }
                if (ch > '~') {
                    return (uint)result;
                }
            }
        }
    }
    return defval;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003ED0F8);
#ifdef SKIP_ASM
// Get string value with delimiter and field index support
int func_003ED0F8(char *value, char *output, int outlen, char *defval, int fieldidx, uint delim)
{
    uint quotech;
    uint ch;
    int count;

    if (value != (char *)0x0) {
        quotech = 0;
        if (*value == '"' || *value == '\'') {
            quotech = (uint)(unsigned char)*value;
            value = value + 1;
        }

        // Skip to requested field
        if (fieldidx > 0) {
            ch = (uint)(unsigned char)*value;
            while (ch != quotech) {
                if (ch < 0x20) goto not_found;
                if (ch == delim) break;
                value = value + 1;
                ch = (uint)(unsigned char)*value;
            }
            value = value + 1;
            if ((unsigned char)*value == delim) {
                fieldidx = fieldidx - 1;
                if (fieldidx < 1) goto found;
                ch = (uint)(unsigned char)*value;
                goto skip_more; // Continue skipping
            }
            value = (char *)0x0;
        }

found:
        if (value != (char *)0x0) {
            if (output == (char *)0x0) {
                // Count mode - count length of value
                ch = (uint)(unsigned char)*value;
                count = 0;
                if (ch == quotech || ch < 0x20) {
                    return 0;
                }
                while (1) {
                    if (ch == delim) {
                        return count;
                    }
                    if (ch == '%') {
                        if ((unsigned char)value[1] < 0x20) {
                            value = value + 1;
                        } else {
                            if ((unsigned char)value[2] > 0x1f) {
                                value = value + 2;
                            }
                            value = value + 1;
                        }
                    } else {
                        value = value + 1;
                    }
                    ch = (uint)(unsigned char)*value;
                    count = count + 1;
                    if (ch == quotech || ch < 0x20) {
                        return count;
                    }
                }
            }

            count = 1;
            if (outlen < 1) {
                return -1;
            }
            if (outlen > 1) {
                ch = (uint)(unsigned char)*value;
                if (ch != quotech && ch >= 0x20) {
                    while (ch != delim) {
                        if (ch != '%') {
                            *output = (char)*value;
                            value = value + 1;
                        } else {
                            if (value[1] == '%') {
                                *output = '%';
                                value = value + 2;
                            } else {
                                if ((unsigned char)value[1] < 0x20) {
                                    *output = *value;
                                    value = value + 1;
                                } else {
                                    if ((unsigned char)value[2] < 0x20) {
                                        *output = *value;
                                        value = value + 1;
                                    } else {
                                        char hi = value[1];
                                        char lo = value[2];
                                        value = value + 3;
                                        *output = s_hex_encode_table[(unsigned char)hi + 0x200] | DAT_004962c0[(unsigned char)lo];
                                    }
                                }
                            }
                        }
                        output = output + 1;
                        count = count + 1;
                        if (outlen <= count) {
                            *output = 0;
                            goto done;
                        }
                        ch = (uint)(unsigned char)*value;
                        if (ch == quotech || ch < 0x20) break;
                    }
                }
            }
            *output = 0;
            goto done;
        }
    }

not_found:
    if (defval == (char *)0x0) {
        return -1;
    }
    count = 1;
    if (output == (char *)0x0) {
        count = strlen(defval);
        return count;
    }
    for (; count < outlen; count = count + 1) {
        if (*defval == 0) {
            *output = 0;
            goto done;
        }
        *output = *defval;
        defval = defval + 1;
        output = output + 1;
    }
    *output = 0;
done:
    return count - 1;

skip_more:
    // This is part of the field skip loop above, simplified
    return -1;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetString);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldGetString(char *value, char *output, int outlen, char *defval)
{
    unsigned char quotech;
    unsigned char ch;
    int count;

    if (value == (char *)0x0) {
        if (defval == (char *)0x0) {
            return -1;
        }
        count = 1;
        if (output == (char *)0x0) {
            count = strlen(defval);
            return count;
        }
        for (; count < outlen; count = count + 1) {
            if (*defval == 0) {
                *output = 0;
                goto done;
            }
            *output = *defval;
            defval = defval + 1;
            output = output + 1;
        }
        *output = 0;
    } else {
        quotech = 0;
        if (*value == '"' || *value == '\'') {
            quotech = (unsigned char)*value;
            value = value + 1;
        }

        if (output == (char *)0x0) {
            // Count mode
            ch = (unsigned char)*value;
            count = 0;
            while (1) {
                if (ch == quotech || ch < 0x20) {
                    return count;
                }
                if (ch == '%') {
                    if ((unsigned char)value[1] < 0x20) {
                        value = value + 1;
                    } else {
                        if ((unsigned char)value[2] > 0x1f) {
                            value = value + 2;
                        }
                        value = value + 1;
                    }
                } else {
                    value = value + 1;
                }
                ch = (unsigned char)*value;
                count = count + 1;
            }
        }

        count = 1;
        if (outlen < 1) {
            return -1;
        }
        if (outlen > 1) {
            ch = (unsigned char)*value;
            if (ch != quotech && ch >= 0x20) {
                do {
                    if (ch != '%') {
                        ch = (unsigned char)*value;
                        *output = *value;
                        value = value + 1;
                    } else {
                        if ((unsigned char)value[1] == '%') {
                            *output = '%';
                            value = value + 2;
                        } else {
                            if ((unsigned char)value[1] < 0x20) {
                                *output = *value;
                                value = value + 1;
                            } else {
                                if ((unsigned char)value[2] < 0x20) {
                                    *output = *value;
                                    value = value + 1;
                                } else {
                                    unsigned char hi = (unsigned char)value[1];
                                    unsigned char lo = (unsigned char)value[2];
                                    value = value + 3;
                                    *output = s_hex_encode_table[hi + 0x200] | DAT_004962c0[lo];
                                }
                            }
                        }
                    }
                    output = output + 1;
                    count = count + 1;
                    if (outlen <= count) {
                        *output = 0;
                        goto done;
                    }
                    ch = (unsigned char)*value;
                } while (ch != quotech && ch >= 0x20);
            }
        }
        *output = 0;
    }
done:
    return count - 1;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetBinary);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldGetBinary(char *value, unsigned char *output, int outlen)
{
    int count;

    if (value == 0 || *value != '$') {
        return -1;
    }

    count = 0;
    if (output == (unsigned char *)0x0) {
        // Count mode
        unsigned char ch = (unsigned char)value[1];
        unsigned char *scan = (unsigned char *)(value + 1);
        while (ch > 0x2f) {
            unsigned char *next = scan + 1;
            scan = scan + 2;
            if (*next < 0x30) break;
            count = count + 1;
            ch = *scan;
        }
        return count;
    }

    if (outlen > 0) {
        count = 0;
        if (outlen > 0 && (unsigned char)value[1] > 0x2f && (unsigned char)value[2] > 0x2f) {
            unsigned char hi = (unsigned char)value[1];
            unsigned char *scan = (unsigned char *)(value + 1);
            while (1) {
                count = count + 1;
                *output = s_hex_encode_table[hi + 0x200] | DAT_004962c0[scan[1]];
                output = output + 1;
                unsigned char *next = scan + 2;
                if (outlen <= count || *next < 0x30 || scan[3] < 0x30) break;
                hi = *next;
                scan = next;
            }
        }
        return count;
    }
    return -1;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetStructure);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldGetStructure(char *value, uint *output, int outlen, char *format)
{
    uint *outend;
    uint *outptr;
    uint charval;
    int count;
    char *fmtp;
    unsigned char fmtch;

    if (outlen < 0) {
        outend = (uint *)((char *)output + 0xffff);
    } else {
        outend = (uint *)((char *)output + outlen);
    }

    if (value == (char *)0x0) {
        value = DAT_004a4a90;
    }

    fmtch = *format;
    if (output == (uint *)0x0) {
        // Use local buffer
        uint localbuf[272];
        output = localbuf;
    }
    outptr = output;

    while (fmtch != 0) {
        if (fmtch == '#') {
            // Skip comment in format
            format = format + 1;
            do {
                fmtch = *format;
                if (fmtch == 0) {
                    charval = (uint)(unsigned char)*format;
                    goto parse_size;
                }
                format = format + 1;
            } while (fmtch != '=');
            charval = (uint)(unsigned char)*format;
        } else {
            charval = (uint)(unsigned char)*format;
        }

parse_size:
        count = 0;
        fmtp = (char *)format;
        while (charval - 0x30 < 10) {
            fmtp = fmtp + 1;
            count = count * 10 + (charval & 0xf);
            charval = (uint)(unsigned char)*fmtp;
        }

        fmtch = *fmtp;
        if (*fmtp == 'a') {
            if (count == 0) {
                outptr = (uint *)((char *)outptr + 1);
            } else {
                outptr = (uint *)((char *)outptr + count);
            }
        }

        int hexdigits;
        if (fmtch == 'b') {
            hexdigits = 2;
        } else {
            hexdigits = 0;
        }
        if (fmtch == 'w') {
            hexdigits = 4;
        }
        if (fmtch == 'l') {
            hexdigits = 8;
        }

        if (hexdigits == 0) {
            fmtch = *fmtp;
        } else {
            // Parse hex value
            char sign = *value;
            uint val = 0;
            if (sign == '-') {
                value = value + 1;
            }
            char hexch = *value;
            while (hexch > '/') {
                value = value + 1;
                hexdigits = hexdigits - 1;
                val = val << 4 | (uint)(unsigned char)DAT_004962c0[(unsigned char)hexch];
                if (hexdigits < 1) goto store_val;
                hexch = *value;
            }
            if (hexdigits > 0 && *value == ',') {
                value = value + 1;
            }
store_val:
            if (sign == '-') {
                val = -val;
            }
            if (*fmtp == 'b') {
                *(char *)outptr = (char)val;
                outptr = (uint *)((char *)outptr + 1);
            }
            fmtch = *fmtp;
            if (fmtch == 'w') {
                *(short *)outptr = (short)val;
                outptr = (uint *)((char *)outptr + 2);
                fmtch = *fmtp;
            }
            if (fmtch == 'l') {
                *outptr = val;
                outptr = outptr + 1;
                fmtch = *fmtp;
            }
        }

        // Handle string fields
        if (fmtch == 's' && count > 0) {
            int si = 0;
            while (1) {
                char sch = *value;
                if ((sch < '0' && sch != '%') || count <= si + 1) break;
                if (sch == '%') {
                    char *hi = value + 1;
                    char *lo = value + 2;
                    value = value + 3;
                    *(char *)((char *)outptr + si) =
                        s_hex_encode_table[(unsigned char)*hi + 0x200] | DAT_004962c0[(unsigned char)*lo];
                    si = si + 1;
                } else {
                    value = value + 1;
                    *(char *)((char *)outptr + si) = sch;
                    si = si + 1;
                }
            }
            for (; si < count; si = si + 1) {
                *(char *)((char *)outptr + si) = 0;
            }
            outptr = (uint *)((char *)outptr + si);
            if (*value == ',') {
                value = value + 1;
            }
        }

        if (outend <= outptr) break;

        format = fmtp + 1;
        if (fmtp[1] == '*') {
            format = fmtp;
        }
        fmtch = *format;
    }
    return (int)outptr - (int)output;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetCrypt);
#ifdef SKIP_ASM
int cDirtysock_tag_TagFieldGetCrypt(char *value, unsigned char *output, int outlen,
                                     unsigned char *defval, unsigned char *key)
{
    unsigned char *keyp;
    unsigned char hash;

    if (value == 0) {
        return -1;
    }
    if (*value != '$') {
        // Not encrypted, treat as regular string
        return cDirtysock_tag_TagFieldGetString(value, (char *)output, outlen, (char *)defval);
    }

    if (key == (unsigned char *)0x0 || *key == 0) {
        key = (unsigned char *)DAT_004a4a88;
    }

    int count = 0;
    if (output == (unsigned char *)0x0) {
        // Count mode
        unsigned char ch = (unsigned char)value[1];
        unsigned char *scan = (unsigned char *)(value + 1);
        while (ch >= 0x30) {
            if (scan[1] < 0x30) break;
            scan = scan + 2;
            count = count + 1;
            ch = *scan;
        }
        return count;
    }

    count = 1;
    if (outlen < 1) {
        return -1;
    }

    hash = 0;
    if (outlen > 1) {
        if ((unsigned char)value[1] < 0x30 || (unsigned char)value[2] < 0x30) {
            *output = 0;
            goto done;
        }

        unsigned char hi = (unsigned char)value[1];
        keyp = key;
        unsigned char *scan = (unsigned char *)(value + 1);
        count = 1;
        while (1) {
            count = count + 1;
            unsigned char *knext = keyp + 1;
            unsigned char decoded = (s_hex_encode_table[hi + 0x200] | DAT_004962c0[scan[1]]) ^ *keyp;
            keyp = knext;
            if (*knext == 0) {
                keyp = key;
            }
            *output = (decoded << 5 | decoded >> 3) ^ hash;
            output = output + 1;
            unsigned char *next = scan + 2;
            hash = DAT_004962c0[scan[1]] | s_hex_encode_table[(unsigned char)*scan + 0x200];
            if (outlen <= count) break;
            if (*next < 0x30 || scan[3] < 0x30) {
                *output = 0;
                goto done;
            }
            hi = *next;
            scan = next;
        }
    }
    *output = 0;
done:
    return count - 1;
}
#endif

INCLUDE_ASM("dirtysock/tags", cDirtysock_tag_TagFieldGetTime);
#ifdef SKIP_ASM
uint cDirtysock_tag_TagFieldGetTime(char *value, uint defval)
{
    uint result = 0;

    if (value != 0) {
        char *scan = (char *)value;
        if (*scan == '$') {
            // Hex epoch format
            scan = scan + 1;
            unsigned char ch = *scan;
            while (ch > '/') {
                scan = scan + 1;
                result = result << 4 | (uint)(unsigned char)DAT_004962c0[(unsigned char)ch];
                ch = *scan;
            }
        }
        else if ((unsigned char)*scan - 0x30 < 10) {
            // Text date format: YYYY-MM-DDThh:mm:ss
            uint localbuf[4];
            char *p = (char *)func_003EAE68(value, localbuf);
            if (*p > ' ' && (unsigned char)*scan - 0x30 < 10) {
                int timedata[9];
                int year_arr[3];
                memset(timedata, 0, 0x24);
                timedata[8] = -1;

                char *p2 = (char *)func_003EAE68(value, &year_arr);
                p = p2 + 1;
                if ((unsigned char)(*p2 - 0x2d) > 1) {
                    p = p2;
                }
                p2 = (char *)func_003EAE68(p, &timedata[4]);
                p = p2 + 1;
                if ((unsigned char)(*p2 - 0x2d) > 1) {
                    p = p2;
                }
                p2 = (char *)func_003EAE68(p, &timedata[3]);
                p = p2 + 1;
                if (*p2 != ' ') {
                    p = p2;
                }
                p2 = (char *)func_003EAE68(p, &timedata[2]);
                p = p2 + 1;
                if (*p2 != ':') {
                    p = p2;
                }
                p2 = (char *)func_003EAE68(p, &timedata[1]);
                p = p2 + 1;
                if (*p2 != ':') {
                    p = p2;
                }
                func_003EAE68(p, timedata);

                if ((uint)(year_arr[0] - 0x7b2) > 0x81 || timedata[4] < 1 ||
                    timedata[4] > 12 || timedata[3] < 1 || timedata[3] > 0x1f) {
                    year_arr[0] = 0;
                }
                if (timedata[2] > 0x17 || timedata[1] < 0 || timedata[1] > 0x3b ||
                    timedata[0] < 0 || timedata[0] > 0x3d) {
                    year_arr[0] = 0;
                }
                if (year_arr[0] != 0) {
                    year_arr[0] = year_arr[0] - 0x76c;
                    timedata[4] = timedata[4] - 1;
                    result = func_003EB090(timedata);
                }
            }
        }
    }

    if (result == 0) {
        result = defval;
        if (defval == 0) {
            result = func_003EF088();
        }
    }
    return result;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EDD70);
#ifdef SKIP_ASM
// Get time components from epoch value
int func_003EDD70(char *value, int *year, int *month, int *day, int *hour, int *minute,
                   int *second, int offset)
{
    uint epoch;
    char timebuf[48];

    epoch = cDirtysock_tag_TagFieldGetTime(value, 1);
    if (epoch < 2) {
        return -1;
    }
    void *timedata = func_003EAEB8(timebuf, (int)epoch + offset);
    if (timedata == 0) {
        return -1;
    }
    int *td = (int *)timedata;
    if (year != 0) {
        *year = td[5] + 0x76c;
    }
    if (month != 0) {
        *month = td[4] + 1;
    }
    if (day != 0) {
        *day = td[3];
    }
    if (hour != 0) {
        *hour = td[2];
    }
    if (minute != 0) {
        *minute = td[1];
    }
    if (second != 0) {
        *second = td[0];
    }
    return 0;
}
#endif

INCLUDE_ASM("dirtysock/tags", func_003EDE80);
#ifdef SKIP_ASM
// Get packed date/time from epoch value
int func_003EDE80(char *value, uint *dateout, uint *timeout, int offset)
{
    uint epoch;
    char timebuf[48];

    epoch = cDirtysock_tag_TagFieldGetTime(value, 1);
    if (epoch < 2) {
        return -1;
    }
    void *timedata = func_003EAEB8(timebuf, (int)epoch + offset);
    if (timedata == 0) {
        return -1;
    }
    uint *td = (uint *)timedata;
    if (dateout != 0) {
        *dateout = (td[5] + 0x76c) * 0x10000 | (td[4] + 1) * 0x100 | td[3];
    }
    if (timeout != 0) {
        *timeout = td[2] << 16 | td[1] << 8 | td[0];
    }
    return 0;
}
#endif
