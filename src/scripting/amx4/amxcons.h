#ifndef AMXCONS_H_INCLUDED
#define AMXCONS_H_INCLUDED
#ifdef  __cplusplus
extern  "C" {
#endif
#define MAX_FORMATSTR   256

typedef struct tagFMTINFO {
  const cell *params;
  int numparams;
  int skip;     /* number of characters to skip from the beginning */
  int length;   /* number of characters to print */
  /* helper functions */
  int (*f_putstr)(void *dest, const char *);
  int (*f_putchar)(void *dest, char);
  void *user;   /* user data */
} AMX_FMTINFO;

int amx_printstring(AMX *amx,cell *cstr, AMX_FMTINFO *info);

#ifdef  __cplusplus
}
#endif
#endif /* AMXCONS_H_INCLUDED */
