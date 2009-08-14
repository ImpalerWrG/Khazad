/*
 * Author: Silas Dunsmore aka 0x517A5D							vim:ts=4:sw=4
 *
 * Released under the MIT X11 license; feel free to use some or all of this
 * code, as long as you include the copyright and license statement (below)
 * in all copies of the source code.  In fact, I truly encourage reuse.
 *
 * If you do use large portions of this code, I suggest but do not require
 * that you keep this code in a seperate file (such as this hexsearch.c file)
 * so that it is clear that the terms of the license do not also apply to
 * your code.
 *
 * Should you make fundamental changes, or bugfixes, to this code, I would
 * appreciate it if you would give me a copy of your changes.
 *
 *
 * Be advised that I use several advanced idioms of the C language:
 * macro expansion, stringification, and variable argument functions.
 * You do not need to understand them.  Usage should be obvious.
 *
 *
 * Lots of logging output is sent to OutputDebugString().
 * The Sysinternals' DebugView program is very useful in monitering this.
 *
 *
 * Copyright (C) 2007-2009 Silas Dunsmore
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * See also:	http://www.opensource.org/licenses/mit-license.php
 * and			http://en.wikipedia.org/wiki/MIT_License
 */

#include <stdarg.h>						// va_list and friends
#include <stdio.h>						// vsnprintf()
#include <stdlib.h>						// atexit()
#define WINVER 0x0500					// OpenThread(), PSAPI, Toolhelp32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>


#include "hexsearch.h"


// exported globals
HANDLE df_h_process, df_h_thread;
DWORD df_pid, df_main_win_tid;
DWORD here[HERE_ARRAY_SIZE];
DWORD target[HERE_ARRAY_SIZE];
char *errormessage;
DWORD df_timestamp;
DWORD df_memory_base, df_memory_start, df_memory_end;
BOOL is_sdl_app;


// local globals
static BOOL change_page_permissions;
static BOOL suspended;
static BYTE *copy_of_df_memory;
static int nexthere;
static int nexttarget;
static DWORD searchmemstart, searchmemend;
static DWORD df_code_start, df_code_end;
static DWORD df_data_start, df_data_end;
static DWORD df_bss_start, df_bss_end;


#define dump(x) d_printf("%-32s == %08X\n", #x, (x))
#define dumps(x) d_printf("%-32s == '%s'\n", #x, (x))
#define zero(x) memset(&x, '\0', sizeof(x))


// ============================================================================
// send info to KERNEL32:OutputDebugString(), useful for non-console programs.
// you can watch this with SysInternals' DebugView.
// http://www.microsoft.com/technet/sysinternals/Miscellaneous/DebugView.mspx
//
void d_printf(const char *format, ...) {
	va_list va;
	char debugstring[4096];				// debug strings can be up to 4K.

	va_start(va, format);
	_vsnprintf(debugstring, sizeof(debugstring) - 2, format, va);
	va_end(va);

	OutputDebugString(debugstring);
}


// ============================================================================
BOOL isvalidaddress(DWORD address) {
	MEMORY_BASIC_INFORMATION mbi;
	return (VirtualQueryEx(df_h_process, (void *)address, &mbi, sizeof(mbi))
			== sizeof(mbi) && mbi.State == MEM_COMMIT);
}


// ----------------------------------------------------------------------------
BOOL peekarb(DWORD address, OUT void *data, DWORD len) {
	BOOL ok = FALSE;
	DWORD succ = 0;
	DWORD OldProtect;
	MEMORY_BASIC_INFORMATION mbi = {0, 0, 0, 0, 0, 0, 0};
		
	errormessage = "";

	// do {} while(0) is a construct that lets you abort a piece of code
	// in the middle without using gotos.

	do {

		if((succ = VirtualQueryEx(df_h_process, (BYTE *)address + len - 1, 
					&mbi, sizeof(mbi))) != sizeof(mbi))
		{
			dump(address + len - 1);
			dump(succ);
			errormessage = "peekarb(): VirtualQueryEx() on end address failed";
			break;
		}
		if(mbi.State != MEM_COMMIT) {
			dump(mbi.State);
			errormessage = "peekarb(): VirtualQueryEx() says end address "
					"is not MEM_COMMIT";
			break;
		}

		if((succ = VirtualQueryEx(df_h_process, (void *)address, &mbi,
						sizeof(mbi))) != sizeof(mbi))
		{
			dump(address);
			dump(succ);
			errormessage ="peekarb(): VirtualQueryEx() on start address failed";
			break;
		}
		if(mbi.State != MEM_COMMIT) {
			dump(mbi.State);
			errormessage = "peekarb(): VirtualQueryEx() says start address is "
					"not MEM_COMMIT";
			break;
		}

		if (change_page_permissions) {
			if(!VirtualProtectEx(df_h_process, mbi.AllocationBase,
						mbi.RegionSize, PAGE_READWRITE, &OldProtect))
			{
				errormessage = "peekarb(): VirtualProtectEx() failed";
				break;
			}
		}

		if(!ReadProcessMemory(df_h_process, (void *)address, data, len, &succ))
		{
			errormessage = "peekarb(): ReadProcessMemory() failed";

			// note that we do NOT break here, as we want to restore the
			// page protection.
		}
		else if(len != succ) {
			errormessage = "peekarb(): ReadProcessMemory() returned "
				   "partial read";
		}
		else ok = TRUE;

		if (change_page_permissions) {
			if (!VirtualProtectEx(df_h_process, mbi.AllocationBase,
						mbi.RegionSize, OldProtect, &OldProtect))
			{
				errormessage = "peekarb(): undo VirtualProtectEx() failed";
				break;
			}
		}

	} while(0);

	if (errormessage != NULL && strlen(errormessage) != 0) {
		d_printf("%s\n", errormessage);
		dump(address);
		//dump(len);
		//dump(succ);
		//dump(mbi.AllocationBase);
	}

	return(ok);
}


// ----------------------------------------------------------------------------
BYTE peekb(DWORD address) {
	BYTE data;
	return(peekarb(address, &data, sizeof(data)) ? data : 0);
	// pop quiz: why don't we set errormessage?
}


// ----------------------------------------------------------------------------
WORD peekw(DWORD address) {
	WORD data;
	return(peekarb(address, &data, sizeof(data)) ? data : 0);
}


// ----------------------------------------------------------------------------
DWORD peekd(DWORD address) {
	DWORD data;
	return(peekarb(address, &data, sizeof(data)) ? data : 0);
}


// ----------------------------------------------------------------------------
char *peekstr(DWORD address, OUT char *data, DWORD maxlen) {
	BYTE c;
	int i = 0;

	data[0] = '\0';
	if (!isvalidaddress(address)) return(data);

	while(--maxlen && (c = peekb(address++)) >= ' ' && c <= '~') {
		if (!isvalidaddress(address)) return(data);
		data[i++] = c;
		data[i] = '\0';
	}

	return(data);						// for convenience
}


// ----------------------------------------------------------------------------
char *peekwstr(DWORD address, OUT char *data, DWORD maxlen) {
	BYTE c;
	int i = 0;

	data[0] = '\0';
	if (!isvalidaddress(address)) return(data);

	while(--maxlen && (c = peekb(address++)) >= ' ' && c <= '~'
			&& peekb(address++) == 0) {
		if (!isvalidaddress(address)) return(data);
		data[i++] = c;
		data[i] = '\0';
	}

	return(data);						// for convenience
}


// ----------------------------------------------------------------------------
BOOL pokearb(DWORD address, const void *data, DWORD len) {
	BOOL ok = FALSE;
	DWORD succ = 0;
	DWORD OldProtect;
	MEMORY_BASIC_INFORMATION mbi;

	errormessage = "";

	do {

		if (!isvalidaddress(address)) {
			errormessage = "pokearb() failed: invalid address";
			break;
		}

		if (!isvalidaddress(address + len - 1)) {
			errormessage = "pokearb() failed: invalid end address";
			break;
		}

		if (change_page_permissions) {
			if (VirtualQueryEx(df_h_process, (void *)address, &mbi, sizeof(mbi))
					!= sizeof(mbi))
			{
				errormessage = "pokearb(): VirtualQueryEx() failed";
				break;
			}

			if(!VirtualProtectEx(df_h_process, mbi.AllocationBase,
						mbi.RegionSize, PAGE_READWRITE, &OldProtect))
			{
				errormessage = "pokearb(): VirtualProtectEx() failed";
				break;
			}
		}

		if (!WriteProcessMemory(df_h_process, (void *)address, data, len,
					&succ))
		{
			errormessage = "pokearb(): WriteProcessMemory() failed";

			// note that we do NOT break here, as we want to restore the
			// page protection.
		}
		else if (len != succ) {
			errormessage = "pokearb(): WriteProcessMemory() did partial write";
		}
		else ok = TRUE;

		if (change_page_permissions) {
			if (!VirtualProtectEx(df_h_process, mbi.AllocationBase,
						mbi.RegionSize, OldProtect, &OldProtect))
			{
				errormessage = "pokearb(): undo VirtualProtectEx() failed";
				break;
			}
		}
	} while(0);

	if (errormessage != NULL && strlen(errormessage) != 0) {
		d_printf("%s @%08X\n", errormessage, address);
		//dump(len);
		//dump(succ);
		//dump(mbi.AllocationBase);
	}
	
	return(ok);
}


// ----------------------------------------------------------------------------
BOOL pokeb(DWORD address, BYTE data) {
	return(pokearb(address, &data, sizeof(data)));
}


// ----------------------------------------------------------------------------
BOOL pokew(DWORD address, WORD data) {
	return(pokearb(address, &data, sizeof(data)));
}


// ----------------------------------------------------------------------------
BOOL poked(DWORD address, DWORD data) {
	return(pokearb(address, &data, sizeof(data)));
}


// ----------------------------------------------------------------------------
BOOL pokestr(DWORD address, const char *data) {
// can't include a "\x00" in the string, obviously.
	return(pokearb(address, data, strlen(data)));
}


// ----------------------------------------------------------------------------
// helper function for hexsearch.  recursive, with backtracking.
// returns TRUE iff the given memory offset matches the given pattern.
//
// implementation note: this would be cleaner without the token1 parameter,
// but hexsearch2() needs its own token1, so must pass it on to here.
//
// implementation note: return_va and return_p pass the va_list and p pointer
// back out of a recursion iff the recursion successfully matched.
// they are not currently necessary.  they are included for future-proofing.
static BOOL hexsearch_match(BYTE *memstart, DWORD token1, va_list va) {
#define		SKIPME	0xFEDCBA98			// token for internal use only.

	static DWORD recursion_level = 0;
	static va_list return_va;
	static BYTE *return_p;

	DWORD token = 0x4DECADE5, lo, hi;
	BYTE *p = memstart;
	BOOL success = FALSE, lookedahead;


	if (token1 == SKIPME) {
		lookedahead = FALSE;
	}
	else {
		lookedahead = TRUE;
		token = token1;
	}

	// loop actions:
	//		if the current token is the EOL token, succeed.
	//		if a token and any associated data match memory, continue.
	//		if it did not match, fail.
	//		if we see an unknown token, fail.
	while(1) {
		// if a previous pass looked ahead, token is already set.
		if (!lookedahead) {
			token = va_arg(va, unsigned int);
		}
		lookedahead = FALSE;

		// exact-match a byte, advance.
		if (token <= 0xFF) {
			if (token != *p++) break;
		}

		// the remaining tokens (the metas) ought to be a switch.
		// but that would make it hard to break out of the while(1) loop.

		// if we hit an EOL, the match succeeded.
		else if (token == EOL) { success = TRUE; break; }

		// match any byte, advance 1.
		else if (token == ANYBYTE) p++;

		// return the address of the next matching byte instead of the
		// address of the start of the pattern.  don't advance.
		else if (token == HERE) { 
			here[nexthere++] = (DWORD)p;		// needs postprocessing.
		}

		// accept either of the next two parameters as a match. advance 1.
		else if (token == EITHER) {

			// reuse lo, hi vars as byte1 and byte2.
			if ((lo = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("EITHER: not followed by a legal token "
						"(byte1): %08X\n", lo);
				break;
			}
			if ((hi = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("EITHER: not followed by a legal token "
						"(byte2): %08X\n", hi);
				break;
			}
			if (!(*p == lo || *p == hi)) break;
			p++;
		}

		// set low value for range comparison.  don't advance.  DEPRECATED.
		else if (token == RANGE_LO) {
			if ((lo = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("RANGE_LO: not followed by a legal token: %08X\n", lo);
				break;
			}

			// Q: peek here to ensure next token is RANGE_HI ?
		}

		// set high value for range comparison, and do comparison.  advance 1.
		// DEPRECATED.
		else if (token == RANGE_HI) {
			if ((hi = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("RANGE_HI: not followed by a legal token: %08X\n", hi);
				break;
			}
			if (*p < lo || *p > hi) break;
			p++;
		}

		// do a byte-size range comparison.  advance 1.
		else if (token == BYTERANGE) {
			if ((lo = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("BYTERANGE: not followed by a legal token "
						"(lo): %08X\n", lo);
				break;
			}

			if ((hi = va_arg(va, unsigned int)) > 0xFF) {
				d_printf("BYTERANGE: not followed by a legal token "
						"(hi): %08X\n", hi);
				break;
			}
			if (*p < lo || *p > hi) break;
			p++;
		}

		// do a dword-size range comparison.  advance 4.
		else if (token == DWORDRANGE) {
			lo = va_arg(va, unsigned int);

			hi = va_arg(va, unsigned int);
			if (*(DWORD *)p < lo || *(DWORD *)p > hi) break;
			p += 4;
		}

		// this is the fun one.  this is where we recurse.
		// advance by however many tokens the recursive call consumed.
		else if (token == SKIP_UP_TO) {
			DWORD len;
			int savenexthere = nexthere;
			int savenexttarget = nexttarget;
			BOOL ok;

			len = va_arg(va, unsigned int) + 1;

			while (len) {

				// I think it's not technically legal to copy va_lists;
				// but it should work on any stack-based machine, which is
				// everything except old Crays and weird dataflow processors.

				recursion_level++;
				ok = hexsearch_match(p, SKIPME, va);
				recursion_level--;
				if (ok) {
					success = TRUE;
					va = return_va;
					p = return_p;
					break;
				}
				else {
					nexthere = savenexthere;
					nexttarget = savenexttarget;
				}

				p++;
				len--;
			}

			break;						// no match within nn bytes, abort.
		}

		else if (token == DWORD_) {		// exact-match a dword.  advance 4.
			DWORD d = va_arg(va, unsigned int);
			if (*(DWORD *)p != d) break;
			p += 4;
		}

		else if (token == ANYDWORD) {	// match any dword.  advance 4.
			p += 4;
		}

		else if (token == ADDRESS) {	// match any legal address in
										// program text.  advance 4.
			if (*(DWORD *)p < df_code_start) break;
			if (*(DWORD *)p > df_bss_end) break;
			p += 4;
		}

		else if (token == CALL) {		// match a near call.  advance 5.
			if (*p++ != 0xE8) break;
			target[nexttarget++] = *(DWORD *)p + (DWORD)p + 4;
			p += 4;
		}

		else if (token == JUMP) {		// match a short or near jump.  
										// advance 2 or 5.
			if (*p == 0xEB) {
				target[nexttarget++] = *(signed char *)(p+1) + (DWORD)p + 1;
				p += 2;
				continue;
			}

			if (*p++ != 0xE9) break;

			target[nexttarget++] = *(DWORD *)p + (DWORD)p + 4;
			p += 4;
		}

		else if (token == JZ) {			// match a JZ instruction.
										// advance 2 or 6.
			if (*p == 0x74) {
				target[nexttarget++] = *(signed char *)(p+1) + (DWORD)p + 2;
				p += 2;
				continue;
			}
			else if (*p == 0x0F && *(p+1) == 0x84
					// also restrict distance to < 64K.
					&& abs((int)(*(DWORD *)(p+2))) < 0x10000
			) {
				target[nexttarget++] = *(DWORD *)(p+2) + (DWORD)p + 6;
				p += 6;
				continue;
			}
			else break;
		}

		else if (token == JNZ) {		// match a JNZ instruction.
										// advance 2 or 6.
			if (*p == 0x75) {
				target[nexttarget++] = *(signed char *)(p+1) + (DWORD)p + 2;
				p += 2;
				continue;
			}
			else if (*p == 0x0F && *(p+1) == 0x85
					// also restrict distance to < 64K.
					&& abs((int)(*(DWORD *)(p+2))) < 0x10000
			) {
				target[nexttarget++] = *(DWORD *)(p+2) + (DWORD)p + 6;
				p += 6;
				continue;
			}
			else break;
		}

		else if (token == JCC) {		// match any conditional jump.
										// advance 2 or 6.
			if (*p >= 0x70 && *p <= 0x7F) {
				target[nexttarget++] = *(signed char *)(p+1) + (DWORD)p + 2;
				p += 2;
				continue;
			}
			else if (*p == 0x0F && *(p+1) >= 0x80 && *(p+1) <= 0x8F
					// also restrict distance to < 64K.
					&& abs((int)(*(DWORD *)(p+2))) < 0x10000
			) {
				target[nexttarget++] = *(DWORD *)(p+2) + (DWORD)p + 6;
				p += 6;
				continue;
			}
			else break;
		}

		else {							// unknown token, abort
			d_printf("unknown token: %08X\n", token);
			dump(p);
			dump(recursion_level);
			break;
		}	// end of huge if-else

	}	// end of while(1)

	if (success && recursion_level > 0) {
		return_va = va;
		return_p = p;
	}

	return(success);
#undef SKIPME
}


// ============================================================================
// The name has changed because the API HAS CHANGED!
//
// Now instead of returning HERE, it always returns the start of the match.
//
// However, there is an array, here[], that is filled with the locations of
//		the HERE tokens.
//		(Starting at 1.  here[0] is a copy of the start of the match.)
//
// Also, the target[] array, starting at 1, is filled with the target
// 		addresses of all CALL, JMP, JZ, JNZ, and JCC tokens.
//
// Finally, you no longer pass it a search length.  Instead, each set of
// 		search terms must end with an EOL.
//
//
//
// Okay, I admit this one is complicated.  Treat it as a black box.
//
// Search Dwarf Fortress's code and initialized data segments for a pattern.
//		(Does not search stack, heap, or thread-local memory.)
//
// Parameters: any number of search tokens, all unsigned ints.
//		The last token must be EOL.
//
//			0x00 - 0xFF: Match this byte.
//			EOL: End-of-list.  The match succeeds when this token is reached.
//			ANYBYTE: Match any byte.
//			DWORD_: Followed by a dword.  Exact-match the dword.
//				Equivalent to 4 match-this-byte tokens.
//			ANYDWORD: Match any dword.  Equivalant to 4 ANYBYTEs.
//			HERE: Put the current address into the here[] array.
//			SKIP_UP_TO, nn: Allow up to nn bytes between the previous match
//				and the next match.  The next token must be a match-this-byte
//				token.  There is sweet, sweet backtracking.
//			EITHER: Accept either of the next two tokens as a match.
//				Both must be match-this-byte tokens.
//			RANGE_LO, nn: Set low byte for a range comparison.  DEPRECATED.
//			RANGE_HI, nn: Set high byte for a range comparison, and do the
//				comparison.  Should immediately follow a RANGE_LO.  DEPRECATED.
//			BYTERANGE, nn, mm: followed by two bytes, the low and high limits
//				of the range.  This is the new way to do ranges.
//			DWORDRANGE, nnnnnnnn, mmmmmmmm: works like BYTERANGE.
//			ADDRESS: Accept any legal address in the program's text.
//				DOES NOT accept pointers into heap space and such.
//			CALL: Match a near call instruction to a reasonable address.
//			JUMP: Match a short or near unconditional jump to a reasonable
//				address.
//			JZ: Match a short or long jz (jump if zero) instruction.
//			JNZ: Match a short or long jnz (jump if not zero) instruction.
//			JCC: Match any short or long conditional jump instruction.
//			More tokens can easily be added.
//
// Returns the offset in Dwarf Fortress of the first match of the pattern.
//		Also sets global variables here[] and target[].
//
// Note: starting a pattern with ANYBYTE, ANYDWORD, SKIP_UP_TO, or EOL
//			is explicitly illegal.
//
//
// implementation detail: search() uses a cached copy of the memory, so
// poke()s and patch()s will not be reflected in the searches.
//
//
// TODO: should be a way to repeat a search in case we want the second or
//			eleventh occurance.
//
// TODO: allow EITHER to accept arbitrary (sets of) tokens.
//			(How would that work?  Have two sub-patterns, each terminating
//			with EOL?)
//
DWORD hexsearch2(DWORD token1, ...) {
	DWORD size;
	BYTE *currentoffset;
	BOOL foundit = FALSE;
	int i;
	va_list va;

	for (i = 0; i < HERE_ARRAY_SIZE; i++) here[i] = 0;
	for (i = 0; i < HERE_ARRAY_SIZE; i++) target[i] = 0;

	if (token1 == ANYBYTE
			|| token1 == ANYDWORD 
			|| token1 == SKIP_UP_TO
			|| token1 == EOL)  return 0;


	currentoffset = copy_of_df_memory;
	currentoffset += (searchmemstart - df_memory_start);
	size = searchmemend - searchmemstart;

	// loop actions:
	//		if currentoffset overruns the copy of df memory, fail.
	//		if memchr(currentoffset) can't find a match, fail.
	//		if hexsearch_match() return TRUE, succeed.
	//		else advance by memchr (if possible) or by 1 byte.
	while (1) {
		// for speed, if we start with a raw byte, use a builtin function
		// to skip ahead to the next actual occurance.
		if (token1 <= 0xFF) {
			currentoffset = (BYTE *)memchr(currentoffset, token1,
					size - (currentoffset - copy_of_df_memory));
			if (currentoffset == NULL) break;
		}

		nexthere = 1;
		nexttarget = 1;

		va_start(va, token1);
		foundit = hexsearch_match(currentoffset, token1, va);
		va_end(va);

		if (foundit) break;

		currentoffset++;
		if ((DWORD)(currentoffset - copy_of_df_memory - 
					(searchmemstart - df_memory_start)) >= size)  break;
	}

	if (!foundit) { 
		DWORD token;

		// kludge to debugprint the parameters of this failed search.
		d_printf("search failed!  hexsearch2(%X", token1);
		i = 0;
		va_start(va, token1);
		do {
			d_printf(",%X", token = va_arg(va, DWORD));
		} while (EOL != token && i++ < 32);
		va_end(va);
		if (i >= 32) d_printf(",...");
		d_printf(")\n");

		// maintainance -- didn't find a match, so zero the match arrays.
		nexthere = 0;
		for (i = 0; i < HERE_ARRAY_SIZE; i++) here[i] = 0;
		nexttarget = 0;
		for (i = 0; i < HERE_ARRAY_SIZE; i++) target[i] = 0;

		return 0;
	}

	here[0] = (DWORD)currentoffset;

	// maintainance -- correct the match arrays to point into DF memory.
	for (i = 0; i < HERE_ARRAY_SIZE; i++) {
		if (i < nexthere && here[i] != 0)
			here[i] += df_memory_start - (DWORD)copy_of_df_memory;
		else here[i] = 0;
	}
	for (i = 0; i < HERE_ARRAY_SIZE; i++) {
		if (i < nexttarget && target[i] != 0)
			target[i] += df_memory_start - (DWORD)copy_of_df_memory;
		else target[i] = 0;
	}

	return df_memory_start + (currentoffset - copy_of_df_memory);
}


// ----------------------------------------------------------------------------
void set_hexsearch2_limits(DWORD start, DWORD end) {
	if (end < start) end = start;

	// TODO: would it be clearer code if these were precorrected to point
	// into copy_of_df_memory instead of the offsets inside DF?
	searchmemstart = start >= df_memory_start && start < df_memory_end ?
			start : df_memory_start;
	searchmemend = end >= df_memory_start && end <= df_memory_end ? 
			end : df_memory_end;
}


// ----------------------------------------------------------------------------
void set_hexsearch2_limits_to_code(void) {
	set_hexsearch2_limits(df_code_start, df_code_end);
}


// ----------------------------------------------------------------------------
void set_hexsearch2_limits_to_data(void) {
	set_hexsearch2_limits(df_data_start, df_data_end);
}


// ============================================================================
// helper function for patch and verify.  does the actual work.
static BOOL vpatchengine(BOOL mode, DWORD address, va_list va) {
	// mode: TRUE == patch, FALSE == verify
	DWORD next = address, token, data;
	BYTE b;								// b is asm instruction byte to use.
	BOOL ok = FALSE;

	while(1) {
		token = va_arg(va, unsigned int);

		if (token == EOL) { ok = TRUE; break; }

		if (token == DWORD_) {
			data = va_arg(va, unsigned int);
			if (mode ? !poked(next, data) : data != peekd(next)) break;
			next += 4;
			continue;
		}

		if (token == CALL || token == JUMP) {
			b = (token == CALL ? 0xE8 : 0xE9);
			data = va_arg(va, unsigned int) - (next + 5);
			if (mode ? !pokeb(next, b) : b != peekb(next)) break;
			// do NOT merge the next++ into the previous if statement.
			next++;
			if (mode ? !poked(next, data) : data != peekd(next)) break;
			next += 4;
			continue;
		}

		if (token == JZ || token == JNZ) {
			b = (token == JZ ? 0x84 : 0x85);
			data = va_arg(va, unsigned int) - (next + 6);
			if (mode ? !pokeb(next, 0x0F) : 0x0F != peekb(next)) break;
			next++;
			if (mode ? !pokeb(next, b) : b != peekb(next)) break;
			next++;
			if (mode ? !poked(next, data) : data != peekd(next)) break;
			next += 4;
			continue;
		}

		if (token <= 0xFF) {
			if (mode ? !pokeb(next, token) : token != peekb(next)) break;
			next++;
			continue;
		}

		d_printf("vpatchengine: unsupported token: %08X\n", token);
		break;							// unsupported token
	}

	return(ok);
}


// ----------------------------------------------------------------------------
// patch2() and verify2() support a modified subset of hex_search2() tokens:
//	The names changed because the API HAS CHANGED!
//
// 			0x00 - 0xFF: poke the byte.
// 			EOL: end-of-list.  terminate when this token is reached.
// 			DWORD_: Followed by a DWORD, not a byte.  Poke the DWORD.
//			CALL: given an _address_; pokes near call with the proper _delta_.
//			JUMP: given an _address_; pokes near jump with the proper _delta_.
//			JZ: given an _address_; assembles a near (not short) jz & delta.
//			JNZ: given an _address_; assembles a near jnz & delta.
//
//	Particularly note that, unlike hex_search(), CALL, JUMP, JZ, and JNZ
//		are followed by a dword-sized target address.
//
//	Note that patch2() does its own verify2(), so you don't have to.
//
//	TODO: doing so many individual pokes and peeks is slow.  Consider
//			building a copy, then doing a pokearb/peekarb.

// TODO: is verify2() useful enough to maintain?

// Make an offset in Dwarf Fortress have certain bytes.
BOOL patch2(DWORD address, ...) {
	va_list va;
	BOOL ok;

	va_start(va, address);
	ok = vpatchengine(TRUE, address, va);
	va_end(va);

	va_start(va, address);
	if (ok) ok = vpatchengine(FALSE, address, va);
	va_end(va);
	return(ok);
}


// ----------------------------------------------------------------------------
// Check that an offset in Dwarf Fortress has certain bytes.
//
// See patch() documentation.
//
// TODO is this even useful?
BOOL verify2(DWORD address, ...) {
	BOOL ok;
	va_list va;
	va_start(va, address);
	ok = vpatchengine(FALSE, address, va);
	va_end(va);
	return(ok);
}


// ============================================================================
// This routine (only) is adapted from Hans Dietrich's XMemString.cpp
// which he released into the public domain.  Downloaded from
// <http://file.cscoding.com/file/35/17689/XMemString_demo.zip>
///////////////////////////////////////////////////////////////////////////////
//
// memmem()
//
// Purpose:     Find a byte sequence within a memory buffer
//
// Parameters:  buf               - pointer to buffer
//              buf_len           - size of buffer in bytes
//              byte_sequence     - byte sequence to look for
//              byte_sequence_len - size of byte sequence in bytes
//
// Returns:     void * - if successful, returns a pointer to the first
//                       occurrence of byte_sequence in buf;  otherwise,
//                       returns NULL
//
// Notes;       Characters in byte_sequence and characters in buf will be
//              compared "as is", with no case conversion.
// 
// ----------------------------------------------------------------------------
// TODO: as the search space is huge, and all bytes of the pattern are known
// and fixed, a skip-list type algorithm such as Boyer-Moore could be used.
// On the other hand, the patterns are likely to be short, and runtime is not
// much of an issue for a tool that runs once per game launch.  Also, this
// implementation uses memchr which is likely optimized.
// ----------------------------------------------------------------------------
DWORD df_memmem(const void *pattern, DWORD patternlen) {
	DWORD buf_len = searchmemend - searchmemstart;
	BYTE *bf = (BYTE *)copy_of_df_memory + (searchmemstart - df_memory_start);
	BYTE *p = bf;

	while (patternlen <= (buf_len - (p - bf))) {
		if ((p = (BYTE *)memchr(p, *(BYTE *)pattern,
						buf_len - (p - bf))) != NULL)
		{
			if ((memcmp(p, pattern, patternlen)) == 0)
				return (DWORD)(p - copy_of_df_memory) + df_memory_start;
			else p++;
		}
		else break;
	}
	return 0;
}


// ----------------------------------------------------------------------------
DWORD df_memstr(const char *substr) {
	DWORD loc;
	BYTE temp[256];

	// first look for a complete string using the heuristic that the byte
	// before the string is 00.
	temp[0] = '\0';
	strncpy(temp+1, substr, sizeof(temp)-1);

	loc = df_memmem(temp, 1 + strlen(substr)+1);
	if (loc) return (loc+1);

	return (df_memmem(substr, strlen(substr)+1));
}


// ----------------------------------------------------------------------------
static DWORD replace_refs(DWORD oldloc, DWORD newloc) {
	DWORD temp_searchmemstart = searchmemstart;

	DWORD num_replaced = 0;
	DWORD aref;
	BYTE temp[8];

	temp[0] = 0x68;						// push	constant
	*(DWORD *)(&temp[1]) = oldloc;

	while ((aref = df_memmem(temp, 5)) != 0) {
		//dump(aref);
		poked(aref+1, newloc);
		num_replaced++;
		set_hexsearch2_limits(aref+2, searchmemend);
	}
	set_hexsearch2_limits(temp_searchmemstart, searchmemend);
	return num_replaced;
}


// ----------------------------------------------------------------------------
BOOL replacearb(DWORD oldloc, const BYTE *newdata, DWORD newlen) {
	DWORD num_replaced;
	static DWORD injectedpage = 0, injectednext = 0;

	if (!injectedpage) {
		injectedpage = (DWORD)VirtualAllocEx(df_h_process, NULL, 0x10000,
				MEM_COMMIT, PAGE_READWRITE);
		injectednext = injectedpage;
	}

	if ( !pokearb(injectednext, newdata, newlen)
			|| !pokeb(injectednext+newlen, 0x00))
	{
		d_printf("replacearb(): poke into injected memory failed\n");
		return(FALSE);
	}

	injectednext += newlen + 1;			// data + trailing 00 byte

	num_replaced = replace_refs(oldloc, injectednext);

	return (num_replaced != 0);
}


// ----------------------------------------------------------------------------
BOOL replacestr(const char *oldstr, const char *newstr) {
	DWORD temp_searchmemstart = searchmemstart;
	DWORD temp_searchmemend   = searchmemend;
	DWORD oldloc;

	set_hexsearch2_limits_to_data();
	oldloc = df_memstr(oldstr);

	set_hexsearch2_limits(temp_searchmemstart, temp_searchmemend);
	if (oldloc)
		return(replacearb(oldloc, newstr, strlen(newstr)+1));
	return(FALSE);
}


// ============================================================================
void cleanup(void) {

	if (copy_of_df_memory != NULL) {
		GlobalFree(copy_of_df_memory);  copy_of_df_memory = NULL;
	}

	if (suspended) {
		if ((DWORD)-1 == ResumeThread(df_h_thread)) {
			// do what?  apologise?
		}
		else suspended = FALSE;
	}
#if 0
	// do this incase earlier runs crashed without executing the atexit handler.
	// TODO: trap exceptions?
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
	ResumeThread(df_h_thread);
#endif

	if (df_h_thread != NULL) {
		CloseHandle(df_h_thread);  df_h_thread = NULL;
	}
	if (df_h_process != NULL) {
		CloseHandle(df_h_process);  df_h_process = NULL;
	}
}


// ----------------------------------------------------------------------------
// Get the base address (the location in memory where the PE header is
// located) of the exe file of the given process.
//
// Three ways are used: PSAPI, Toolhelp32, and a fallback constant which
// should never actually be used.
//
static DWORD get_executable_base(DWORD pid) {
	DWORD base_from_PSAPI = 0, base_from_Toolhelp32 = 0;
	HANDLE h_process;
	HMODULE hmod = NULL;
	DWORD junk;

	h_process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
			FALSE, pid);
	// Here I assume that the first module in the list is the executable's
	// module.  Therefore I pass a list that can only hold one item.
	// Foolish but simple.  Requires PSAPI.DLL, therefore Windows2000 and up.
	if (h_process && 
			EnumProcessModules(h_process, &hmod, 1 * sizeof(HMODULE), &junk))
	{
		base_from_PSAPI = (DWORD)hmod;
	}
	else d_printf("EnumProcessModules() failed.\n");
	CloseHandle(h_process);
	dump(base_from_PSAPI);


	// Toolhelp32 method, also feels like a kludge, but it is done right.
	// Also requires Windows2000 and up.
	// This is closely based on code contributed by Jifodus.  Thanks!
	do {
		HANDLE hSnapshot;
		BOOL ok;

		PROCESSENTRY32 process;
		MODULEENTRY32 module;

		zero(process);
		zero(module);


		hSnapshot= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0/*ignored*/);
		if (hSnapshot == INVALID_HANDLE_VALUE) {
			d_printf("CreateToolhelp32Snapshot(Processes) failed.\n");
			break;
		}

		// Walk through the process list until we find the DF PID.
		process.dwSize = sizeof(PROCESSENTRY32);
		ok = Process32First(hSnapshot, &process);

		while (ok && process.th32ProcessID != pid) {
			//d_printf("Process: %d \"%.*s\"\n", process.th32ProcessID,
			//		MAX_PATH, process.szExeFile);
			ok = Process32Next(hSnapshot, &process);
		}
		CloseHandle(hSnapshot);  hSnapshot = NULL;

		if (!ok || /*canthappen*/ process.th32ProcessID != pid) {
			d_printf("Failed to find the target process ID in the "
				"Toolhelp32 Process32 list.\n");
			break;
		}

		// Interesting.  Never seen the .* modifier before.  
		// TODO: investigate if it is MSVCRT only.
		//d_printf("Target Process: %d \"%.*s\"\n", process.th32ProcessID,
		//		MAX_PATH, process.szExeFile);


		// Now find the module, by name.
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		if (hSnapshot == INVALID_HANDLE_VALUE) {
			d_printf("CreateToolhelp32Snapshot(Modules) failed.\n");
			break;
		}

		module.dwSize = sizeof(MODULEENTRY32);
		ok = Module32First(hSnapshot, &module);

		while (ok) {
			//d_printf("Module: %08X \"%.*s\" \"%.*s\"\n", 
			//		(DWORD)(SIZE_T)module.modBaseAddr, 
			//		MAX_MODULE_NAME32 + 1, module.szModule, 
			//		MAX_PATH, module.szExePath);

			if (!stricmp(module.szModule, process.szExeFile)) { break; }

			ok = Module32Next(hSnapshot, &module);
		}
		CloseHandle(hSnapshot);  hSnapshot = NULL;

		if (!ok || /*canthappen*/ stricmp(module.szModule, process.szExeFile)) {
			d_printf("Module32List(snapshot) Couldn't find the target "
				"module in the snapshot.\n");
			break;
		}
		//d_printf("Target Module: %08X \"%.*s\" \"%.*s\"\n",
		//		(DWORD)(SIZE_T)module.modBaseAddr,
		//		MAX_MODULE_NAME32 + 1, module.szModule,
		//		MAX_PATH, module.szExePath);

		base_from_Toolhelp32 = (DWORD)module.modBaseAddr;

	} while (0);

	dump(base_from_Toolhelp32);

	if (base_from_PSAPI && base_from_Toolhelp32) {
		if (base_from_PSAPI != base_from_Toolhelp32) {
			d_printf("Problem!  base_from_PSAPI != base_from_Toolhelp32: "
					"%08X != %08X\n", base_from_PSAPI, base_from_Toolhelp32);
			d_printf("Trusting value in base_from_Toolhelp32.\n");
			// Q: is this worth a warning MessageBox?
		}
		return base_from_Toolhelp32;
	}
	else if (base_from_PSAPI) { return base_from_PSAPI; }
	else if (base_from_Toolhelp32) { return base_from_Toolhelp32; }
	// if nothing worked, we will simply assume that the target executable
	// has not been rebased and is at the default location for executables.
	return 0x400000;
}


// ----------------------------------------------------------------------------
BOOL open_dwarf_fortress(void) {
	HANDLE df_main_wh;
	char *state;
	IMAGE_NT_HEADERS32 pe_header;
	IMAGE_SECTION_HEADER sections[16];

	change_page_permissions = FALSE;

	atexit(cleanup);

	errormessage = "";

	state = "calling FindWindow() OpenGL\n";
	d_printf(state);
	df_main_wh = FindWindow("OpenGL", "Dwarf Fortress");
	if (df_main_wh == 0) {
		state = ("not found -- trying FindWindow() SDL_app\n");
		d_printf(state);
		df_main_wh = FindWindow("SDL_app", "Dwarf Fortress");
		if (df_main_wh) is_sdl_app = TRUE;
	}
	dump(df_main_wh);
	if (df_main_wh == 0) {
		errormessage =
				"FindWindow(Dwarf Fortress) failed.\nIs the game running?";
		return FALSE;
	}

	state = "calling GetWindowThreadProcessId()\n";
	d_printf(state);
	df_main_win_tid = GetWindowThreadProcessId(df_main_wh, &df_pid);
	dump(df_pid);
	dump(df_main_win_tid);

	if (df_main_win_tid == 0) {
		errormessage =
				"GetWindowThreadProcessId(Dwarf Fortress) failed.\n"
				"That should not have happened!";
		return FALSE;
	}

	state = "calling OpenProcess()\n";
	df_h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, df_pid);
	dump(df_h_process);
	if (df_h_process == NULL) {
		errormessage = "OpenProcess(Dwarf Fortress) failed.\n"
				"Are you the same user that started the game?";
		return(FALSE);
	}

	df_memory_base = get_executable_base(df_pid);

	state = "calling OpenThread()\n";
	df_h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, df_main_win_tid);
	dump(df_h_thread);
	if (df_h_thread == NULL) {
		errormessage = "OpenThread(Dwarf Fortress) failed.";
		return(FALSE);
	}

	// Q: enum and suspend all threads?
	// A: not necessary, DF is single-threaded.
	state = "calling SuspendThread()\n";
	if ((DWORD)-1 == SuspendThread(df_h_thread)) {
		errormessage = "SuspendThread(Dwarf Fortress) failed.";
		return(FALSE);
	}
	suspended = TRUE;

	peekarb(df_memory_base + peekd(df_memory_base+0x3C)                            , &pe_header, sizeof(pe_header));
	peekarb(df_memory_base + peekd(df_memory_base+0x3C)+ sizeof(IMAGE_NT_HEADERS32), &sections , sizeof(sections) );


	df_timestamp = pe_header.FileHeader.TimeDateStamp;
	dump(df_timestamp);

	df_memory_start = df_memory_base + pe_header.OptionalHeader.BaseOfCode;
	df_memory_end = df_memory_base + sections[2].VirtualAddress +
			sections[2].SizeOfRawData;
	dump(df_memory_start);
	dump(df_memory_end);

	df_code_start = df_memory_base + sections[0].VirtualAddress;
	df_code_end = df_code_start + sections[0].Misc.VirtualSize;
	df_data_start = df_memory_base + sections[1].VirtualAddress;
	df_data_end = df_data_start + sections[1].Misc.VirtualSize;
	df_bss_start = df_memory_base + sections[2].VirtualAddress;
	df_bss_end = df_bss_start + sections[2].Misc.VirtualSize;
	dump(df_code_start);
	dump(df_code_end);
	dump(df_data_start);
	dump(df_data_end);
	dump(df_bss_start);
	dump(df_bss_end);


/*	parsed header of .40d9
0		IMAGE_DOS_HEADER
3C		DWORD	e_lfanew							00000118
118	0	DWORD	Signature							"PE\x00\x00"
11C 4	IMAGE_FILE_HEADER FileHeader
11C 4		WORD	Machine							014C
11E	6		WORD	NumberOfSections				0005
120 8		DWORD	TimeDateStamp					4967C2E0
124	C		DWORD	PointerToSymbolTable			00000000
128	10		DWORD	NumberOfSymbols					00000000
12C	14		WORD	SizeOfOptionalHeader			00E0
12E	16		WORD	Characteristics					0102
130	18	IMAGE_OPTIONAL_HEADER OptionalHeader
130	18		WORD	Magic 							010B
132 1A		BYTE	MajorLinkerVersion				01
133	1B		BYTE	MinorLinkerVersion				09
134	1C		DWORD	SizeOfCode						004E1800	**
138	20		DWORD	SizeOfInitializedData			000FDE00	**
13C	24		DWORD	SizeOfUninitializedData			00000000
140	28		DWORD	AddressOfEntryPoint				004BFFA1
144	2C		DWORD	BaseOfCode						00001000
148	30		DWORD	BaseOfData						004E3000	**
14C	34		DWORD	ImageBase						00400000
150	38		DWORD	SectionAlignment				00001000
154	3C		DWORD	FileAlignment					00000200
158	40		WORD	MajorOperatingSystemVersion		0005
15A 42		WORD	MinorOperatingSystemVersion		0000
15C	44		WORD	MajorImageVersion				0000
15E	46		WORD	MinorImageVersion				0000
160	48		WORD	MajorSubsystemVersion			0005
162	4A		WORD	WORD MinorSubsystemVersion		0000
164	4C		DWORD	Win32VersionValue				00000000
168	50		DWORD	SizeOfImage						01374000	**
16C	54		DWORD	SizeOfHeaders					00000400
170	58		DWORD	CheckSum						005E3B91
174	5C		WORD	Subsystem						0002
176	5E		WORD	DllCharacteristics				8040
178	60		DWORD	SizeOfStackReserve				00100000
17C	64		DWORD	SizeOfStackCommit				00001000
180	68		DWORD	SizeOfHeapReserve				00100000
184	6C		DWORD	SizeOfHeapCommit				00001000
188	70		DWORD	LoaderFlags						00000000
18C	74		DWORD	NumberOfRvaAndSizes				00000010
190		IMAGE_DATA_DIRECTORY[10]
190	78		DWORD	VirtualAddress					005654D0
194	7C		DWORD	Size							0000004A
198	80		DWORD	VirtualAddress					005627E4
19C	84		DWORD	Size							000000DC
1A0	88		DWORD	VirtualAddress					012FC000
1A4	8C		DWORD	Size							000003AC
1A8	90		DWORD	VirtualAddress					00000000
1AC	94		DWORD	Size							00000000
1B0	98		DWORD	VirtualAddress					00000000
1B4	9C		DWORD	Size							00000000
1B8	A0		DWORD	VirtualAddress					012FD000
1BC	A4		DWORD	Size							00061F24
1C0	A8		DWORD	VirtualAddress					00000000
1C4	AC		DWORD	Size							00000000
1C8	B0		DWORD	VirtualAddress					00000000
1CC	B4		DWORD	Size							00000000
1D0	B8		DWORD	VirtualAddress					00000000
1D4	BC		DWORD	Size							00000000
1D8	C0		DWORD	VirtualAddress					00000000
1DC	C4		DWORD	Size							00000000
1E0	C8		DWORD	VirtualAddress					0053F0D8
1E4	CC		DWORD	Size							00000040
1E8	D0		DWORD	VirtualAddress					00000000
1EC	D4		DWORD	Size							00000000
1F0	D8		DWORD	VirtualAddress					004E3000
1F4	DC		DWORD	Size							00000464
1F8	E0		DWORD	VirtualAddress					00000000
1FC	E4		DWORD	Size							00000000
200	E8		DWORD	VirtualAddress					00000000
204	EC		DWORD	Size							00000000
208	F0		DWORD	VirtualAddress					00000000
20C	F4		DWORD	Size							00000000
210	F8	IMAGE_SECTION_HEADER
210	F8		BYTE[8]	Name							".text"
218	100		DWORD	Misc.VirtualSize				004E16B5	**
21C	104		DWORD	VirtualAddress					00001000	**
220 108		DWORD	SizeOfRawData					004E1800
224	10C		DWORD	PointerToRawData				00000400
228	110		DWORD	PointerToRelocations			00000000
22C	114		DWORD	PointerToLinenumbers			00000000
230	118		WORD	NumberOfRelocations				0000
232	11A		WORD	NumberOfLinenumbers				0000
234	11C		DWORD	Characteristics					60000020
238	120		IMAGE_SECTION_HEADER
238	120		BYTE[8]	Name							".rdata"
240	128		DWORD	Misc.VirtualSize				0008251A	**
244	12C		DWORD	VirtualAddress					004E3000	**
248 130		DWORD	SizeOfRawData					00082600
24C	134		DWORD	PointerToRawData				004E1C00
250	138		DWORD	PointerToRelocations			00000000
254	13C		DWORD	PointerToLinenumbers			00000000
258	140		WORD	NumberOfRelocations				0000
25A	142		WORD	NumberOfLinenumbers				0000
25C	144		DWORD	Characteristics					40000040
260	148		IMAGE_SECTION_HEADER
260	148		BYTE[8]	Name							".data"
268	150		DWORD	Misc.VirtualSize				00D953C4
26C	154		DWORD	VirtualAddress					00566000
270 158		DWORD	SizeOfRawData					00005000
274	15C		DWORD	PointerToRawData				00564200
278	160		DWORD	PointerToRelocations			00000000
27C	164		DWORD	PointerToLinenumbers			00000000
280	168		WORD	NumberOfRelocations				0000
282	16A		WORD	NumberOfLinenumbers				0000
284	16C		DWORD	Characteristics					C0000040
288	170		IMAGE_SECTION_HEADER
288	170		BYTE[8]	Name							".rsrc"
290	178		DWORD	Misc.VirtualSize				000003AC
294	17C		DWORD	VirtualAddress					012FC000
298 180		DWORD	SizeOfRawData					00000400
29C	184		DWORD	PointerToRawData				00569200
2A0	188		DWORD	PointerToRelocations			00000000
2A4	18C		DWORD	PointerToLinenumbers			00000000
2A8	190		WORD	NumberOfRelocations				0000
2AA	192		WORD	NumberOfLinenumbers				0000
2AC	194		DWORD	Characteristics					40000040
2B0	198		IMAGE_SECTION_HEADER
2B0	198		BYTE[8]	Name							".reloc"
2B8	1A0		DWORD	Misc.VirtualSize				0007620A
2BC	1A4		DWORD	VirtualAddress					012FD000
2C0 1A8		DWORD	SizeOfRawData					00076400
2C4	1AC		DWORD	PointerToRawData				00569600
2C8	1B0		DWORD	PointerToRelocations			00000000
2CC	1B4		DWORD	PointerToLinenumbers			00000000
2D0	1B8		WORD	NumberOfRelocations				0000
2D2	1BA		WORD	NumberOfLinenumbers				0000
2D4	1BC		DWORD	Characteristics					42000040
*/



	state = "calling GlobalAlloc(huge)\n";
	d_printf(state);
	if (NULL == (copy_of_df_memory = 
			(BYTE *)GlobalAlloc(GPTR, df_memory_end - df_memory_start + 0x100)))
	{
		errormessage = "GlobalAlloc() of copy_of_df_memory failed";
		return FALSE;
	}
	
	state = "copying memory\n";
	d_printf(state);
	if (!peekarb(df_memory_start, copy_of_df_memory,
				df_memory_end-df_memory_start)) {
		d_printf("peekarb(entire program) for search()'s use failed\n");
		return FALSE;
	}

	set_hexsearch2_limits(0, 0);

	return(TRUE);
}


// not (normally) necessary because it is done at exit time by atexit().
// however you can call this to resume DF before putting up a dialog box.
void close_dwarf_fortress(void) {
	cleanup();
}


