#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "screen.h"
#include "conflog.h"
#include "mystr.h"
#include "system.h"

static void goc_systemDefaultSignal(int signum)
{
	GOC_MSG_CATCHSIGNAL( msg, signum );
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msg);
	return;
}

// Twarde zatrzymanie systemu. Zadbaj g³ównie o przywrócenie terminala
// do stanu przedzawa³owego. Resztê mo¿na sobie odpu¶ciæ na dalszy plan.
static void goc_systemBreakDown(int signum)
{
	GOC_FERROR(goc_stringAddInt( goc_stringCopy( NULL,
		"Abnormal system break down - truning off. SIGNAL CODE: "), signum ) );
	// TODO: More abnormal system finish - don't use memory
	GOC_MSG_FINISH( msg, 0 );
	goc_systemSendMsg(GOC_HANDLER_SYSTEM, msg);
	return;
}

void static retCheck(int res)
{
	if ( res == 0 )
		GOC_DEBUG("Signal catched");
	else
		GOC_DEBUG("Problem");
}

void goc_systemCatchSignals()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = &goc_systemDefaultSignal;
	retCheck(sigaction(SIGINT, &sa, NULL)); // Interrupt from keyboard
	retCheck(sigaction(SIGQUIT, &sa, NULL)); // Quit from keyboard
	retCheck(sigaction(SIGABRT, &sa, NULL)); // Abort signal from abort
	// SIGKILL & SIGSTOP cannot be catched :<
	//retCheck(sigaction(SIGKILL, &sa, NULL)); // Kill signal
	retCheck(sigaction(SIGUSR1, &sa, NULL)); // User defined 1
	retCheck(sigaction(SIGUSR2, &sa, NULL)); // User defined 2
	retCheck(sigaction(SIGTERM, &sa, NULL)); // Termination signall
	retCheck(sigaction(SIGCHLD, &sa, NULL)); // Child stopped or terminated
	retCheck(sigaction(SIGWINCH, &sa, NULL)); // Window resize terminal
	sa.sa_handler = &goc_systemBreakDown;
	retCheck(sigaction(SIGHUP, &sa, NULL)); // Hangup detected
	retCheck(sigaction(SIGILL, &sa, NULL)); // Illegal instruction
	// retCheck(sigaction(SIGTRAP, &sa, NULL)); // Trace/Breakpoint trap
	retCheck(sigaction(SIGIOT, &sa, NULL));  // IOT Trap. As SIGABRT
	retCheck(sigaction(SIGFPE, &sa, NULL)); // Floating point exception
	retCheck(sigaction(SIGSEGV, &sa, NULL)); // Invalid memory reference
	retCheck(sigaction(SIGPIPE, &sa, NULL)); // Broken pipe
	//retCheck(sigaction(SIGCONT, &sa, NULL)); // Continue if stopped
	//retCheck(sigaction(SIGSTOP, &sa, NULL)); // Stop process
	//retCheck(sigaction(SIGTSTP, &sa, NULL)); // Stop typed at tty
	//retCheck(sigaction(SIGTTIN, &sa, NULL)); // TTy input for background process
	//retCheck(sigaction(SIGTTOU, &sa, NULL)); // TTy output for background process
}
