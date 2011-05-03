/**
 * Separate source file just to set the status text of the mainframe in non-GUI classes.
 * Only reason this exists is to avoid the non-GUI classes to include the wx.h header.
 * Both because of decouplement and because it takes an annoyingly long time to compile.
 */

#ifndef STATUSTEXT_H
#define	STATUSTEXT_H

void setStatus(const char *newStatus, ...);

#endif	/* STATUSTEXT_H */

