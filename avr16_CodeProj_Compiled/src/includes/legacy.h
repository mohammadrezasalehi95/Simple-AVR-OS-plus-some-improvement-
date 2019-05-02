/* GCC Legay compiler support for old projects */


/* AVR Compatibility with WinAVR latest versions */

#define PRG_RDB(x) pgm_read_byte(x)

#define sbi(p,b) (p) |= (1<<(b))
#define cbi(p,b) (p) &= ~(1<<(b))

#define outp(d,p) (p) = d
#define inp(p) (p)

#define BV(x) _BV(x)

