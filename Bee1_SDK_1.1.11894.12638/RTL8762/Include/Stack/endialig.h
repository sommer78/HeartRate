
#if ! defined(__ENDIALIG_H)
#define __ENDIALIG_H


/* Get local WORD from external 2 BYTE, Big-Endian format STANDARD NETWORK BYTE ORDER */
#define BE_EXTRN2WORD(p) ((*((p)+1)) & 0xff) + ((*(p)) << 8)

/* Get local DWORD from external 4 BYTE, Big-Endian format STANDARD NETWORK BYTE ORDER */
#define BE_EXTRN2DWORD(p) ((unsigned long)(*((p)+3)) & 0xff) + ((unsigned long)(*((p)+2)) << 8) \
    + ((unsigned long)(*((p)+1)) << 16)  + ((unsigned long)(*((p)+0)) << 24)

/* PUT external 2 CHARS from local SHORT, Big-Endian format STANDARD NETWORK BYTE ORDER */
#define BE_WORD2EXTRN(p,w)                 \
    {*((p)+1) = (unsigned char)((w) & 0xff);         \
        *(p)     = /*lint -e(572,778)*/ (unsigned char)(((w)>>8) & 0xff);}

/* PUT external 4 BYTE from local DWORD, Big-Endian format STANDARD NETWORK BYTE ORDER */
#define BE_DWORD2EXTRN(p,w)                   \
    {*((p)+3) = (unsigned char)((w) & 0xff);          \
        *((p)+2) = /*lint -e(572,778)*/ (unsigned char)(((w)>>8) & 0xff);     \
        *((p)+1) = /*lint -e(572,778)*/ (unsigned char)(((w)>>16) & 0xff);    \
        *((p)+0) = /*lint -e(572,778)*/ (unsigned char)(((w)>>24) & 0xff);}

/* Get local WORD from external 2 BYTE, Little-Endian format */
#define LE_EXTRN2WORD(p) (((*(p)) & 0xff) + ((*((p)+1)) << 8))

/* Get local DWORD from external 4 BYTE, Little-Endian format  */
#define LE_EXTRN2DWORD(p) (((unsigned long)(*((p)+0)) & 0xff) + ((unsigned long)(*((p)+1)) << 8) \
                           + ((unsigned long)(*((p)+2)) << 16)  + ((unsigned long)(*((p)+3)) << 24))

/* PUT external 2 BYTE from local WORD, Little-Endian Format */
#define LE_WORD2EXTRN(p,w)                 \
    {*((unsigned char*)p)     = (unsigned char)((unsigned short)(w) & 0xff);      \
        *(((unsigned char*)p)+1) = /*lint -e(572,778)*/ (unsigned char)(((unsigned short)(w)>>8) & 0xff);}

/* PUT external 4 BYTE from local DWORD, Little endian Format */
#define LE_DWORD2EXTRN(p,w)                      \
    {*((unsigned char*)p)     = (unsigned char)((unsigned long)(w) & 0xff);          \
        *(((unsigned char*)p)+1) = (unsigned char)(((unsigned long)(w)>>8) & 0xff);     \
        *(((unsigned char*)p)+2) = (unsigned char)(((unsigned long)(w)>>16) & 0xff);    \
        *(((unsigned char*)p)+3) = (unsigned char)(((unsigned long)(w)>>24) & 0xff);}

#endif

