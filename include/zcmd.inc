/*
	ZCMD - Lightweight command processor
*/

#if defined _zcmd_included
	#endinput
#endif
#define _zcmd_included

#define CMD:%1(%2) forward cmd_%1(%2); public cmd_%1(%2)
#define COMMAND:%1(%2) forward cmd_%1(%2); public cmd_%1(%2)

