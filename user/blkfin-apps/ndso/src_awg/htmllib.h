/* htmllib.h */

#ifndef _HTMLLIB_H
#define _HTMLLIB_H

/* function prototypes */
void htmlHeader (char *title);
void htmlBody ();
void htmlFooter ();
void addTitleElement (char *title);
void htmlHeaderText (char *title);
void htmlHeaderNocache (char *title);

#endif /* !_HTMLLIB_H */
