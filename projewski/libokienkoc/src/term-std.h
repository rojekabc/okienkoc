#ifndef _TERM_LINUX_H_
#	define _TERM_LINUX_H_
#	include "iterm.h"

struct GOC_Terminal* goc_termStdAlloc(void(* event)(GOC_EVENTTYPE, GOC_EVENTDATA));

#endif
