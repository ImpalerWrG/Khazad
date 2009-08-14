/*
 * Author: Silas Dunsmore aka 0x517A5D							vim:ts=4:sw=4
 *
 * A wrapper for Rick/Gibbed's map revealler utility.
 *
 *
 * This utility is expected to work for future versions of Dwarf Fortress.
 *
 * Should the program fail to work as expected, please report it at
 * http://www.dwarffortresswiki.net/index.php/User_talk:0x517A5D
 *
 * Lots of logging output is sent to OutputDebugString().
 * The Sysinternals' DebugView program is very useful in monitering this.
 * 
 *
 * Compile instructions:  I use Cygwin to compile, but target MinGW.
 * However this code should be fairly portable to other Windows compilers.
 * HOWEVER, the Structured Exception Handling code is likely fragile,
 *	especially the assembly code that installs and removes the handler.
 *
 * There is a Cygwin-compatible Makefile.  Just type "make".
 *
 *
 * Be advised that I use several advanced idioms of the C language:
 * macro expansion, stringification, and variable argument functions.
 * You do not need to understand them.  Usage should be obvious.
 *
 *
 * Copyright (C) 2008-2009 Silas Dunsmore
 *
 * Released under the MIT X11 license; feel free to use some or all of this
 * code, as long as you include the copyright and license statement (below)
 * in all copies of the source code.  In fact, I truly encourage reuse.
 * If you do use large portions of this code, I suggest but do not require
 * that you keep my code in a seperate file so that it is clear that the
 * terms of the license do not also apply to your code.
 *
 * I will not claim copyright (even under the MIT X11 license) on small
 * snippets from this file.  (Note the "substantial portions" clause in
 * the license.)  So feel free to use the search terms.
 *
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


#include <stdio.h>
#include <stdlib.h>
#define WINVER 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../hexsearch.h"


#define USE_SEH							// "structured exception handling",
										// undocumented spaghetti POS.
#ifdef USE_SEH
# include <mapi.h>
#endif


char myname[MAX_PATH];
DWORD mytimestamp;
char *myininame;
char inisectionname[16];
const char cr[]="Copyright (C) 2008-2009 Silas Dunsmore.  Open source; "
		"source is available under the MIT X11 license.";
const char report[]="Please report this at\n"
		"http://www.dwarffortresswiki.net/index.php/User_talk:0x517A5D";
char emailaddress[]="tjmbteAqtzcfs/dpn";
DWORD pe_header_timestamp;


#define dump(x) d_printf("%-32s == %08X\n", #x, (x));
#define dumps(x) d_printf("%-32s == '%s'\n", #x, (x));

#define zero(x) memset(&x, '\0', sizeof(x))


// .INI entries needed by Rick's reveal
DWORD dwarf_fortress, map_data, map_x_count, map_y_count, map_z_count;
unsigned int map_data_designation_offset;



// ============================================================================
void croak(const char *message, ...) {
	DWORD lasterr;
	va_list va;
	char expandedmessage[512];			// yes, I know, but if we're crashing,
										// we shouldn't call malloc or friends.
										// (probably shouldn't call printf...)
	close_dwarf_fortress();

	lasterr = GetLastError();

	va_start(va, message);
	_vsnprintf(expandedmessage, sizeof(expandedmessage), message, va);
	va_end(va);
	d_printf("%s\n", expandedmessage);	// %s stands for safety!
	MessageBox(NULL, expandedmessage, myname, MB_OK | MB_ICONERROR);
	exit(lasterr ? lasterr : 1);
}


// ============================================================================
#ifdef USE_SEH
// TODO: would SetUnhandledExceptionFilter be better?
int my_se_handler(
		struct _EXCEPTION_RECORD *ExceptionRecord,
		void * EstablisherFrame,
		struct _CONTEXT *ContextRecord,
		void * DispatcherContext)
{
	static volatile int recursing = 0;
	DWORD i;
	char expandedmessage[4096];

	// better safe than sorry
	if (recursing++) {
		OutputDebugString("Caught recursion in se_handler(), abort()ing\n");
		abort();
	}

	d_printf("entered se_handler()\n");

	EstablisherFrame = EstablisherFrame;		// shaddup the compiler warning
	DispatcherContext = DispatcherContext;		// unused parameter blah blah

	d_printf("attempting to close our hooks into Dwarf Fortress... \n");
	close_dwarf_fortress();
	d_printf("done\n");

	_snprintf(expandedmessage, sizeof(expandedmessage),
			"Something bad happened:\n"
			"Exception %08X occured at or around address %08X\n"
			"Cannot continue.  Cleaning up... done.  Exiting.\n\n"
			"May this program attempt to email information\n"
			"about this problem to the author?",
			ExceptionRecord->ExceptionCode,
			ContextRecord->Eip
	);
	if (MessageBox(NULL, expandedmessage,
				ExceptionRecord->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION
				? "An Illegal Instruction Exception Has Occured"
				: ExceptionRecord->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO
				? "A Divide By Zero Exception Has Occured"
				: ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION
				? "A Priveleged Instruction Exception Has Occured"
				: ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
				? "An Access Violation Exception Has Occured"
				: "A Processor Exception Has Occured",
				MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2) == IDYES)
	{
		_snprintf(expandedmessage, sizeof(expandedmessage),
				"This Information Will Be Emailed:\n\n"
				"Executable Name:%s  Timestamp:%08X\n"
				"ExceptionCode:%08X  EIP:%08X EFlags:%08X\n"
				"EAX:%08X EBX:%08X ECX:%08X EDX:%08X\n"
				"ESP:%08X EBP:%08X ESI:%08X EDI:%08X\n",
				myname,
				mytimestamp,
				ExceptionRecord->ExceptionCode,
				ContextRecord->Eip,
				ContextRecord->EFlags,
				ContextRecord->Eax,
				ContextRecord->Ebx,
				ContextRecord->Ecx,
				ContextRecord->Edx,
				ContextRecord->Esp,
				ContextRecord->Ebp,
				ContextRecord->Esi,
				ContextRecord->Edi
		);

		i = (ContextRecord->Eip - 64 - 16) & ~0x0000000F;

		while (i < ContextRecord->Eip + 64) {
			MEMORY_BASIC_INFORMATION mbi;
			i += 16;
			if (VirtualQuery((void *)i, &mbi, sizeof(mbi)) == sizeof(mbi)
					&& mbi.State != MEM_COMMIT) continue;

			_snprintf(expandedmessage + strlen(expandedmessage),
					sizeof(expandedmessage) - strlen(expandedmessage),
					"%08X: %02X %02X %02X %02X %02X %02X %02X %02X"
					"-%02X %02X %02X %02X %02X %02X %02X %02X\n",
					i,
					*(BYTE *)(i+0),
					*(BYTE *)(i+1),
					*(BYTE *)(i+2),
					*(BYTE *)(i+3),
					*(BYTE *)(i+4),
					*(BYTE *)(i+5),
					*(BYTE *)(i+6),
					*(BYTE *)(i+7),
					*(BYTE *)(i+8),
					*(BYTE *)(i+9),
					*(BYTE *)(i+10),
					*(BYTE *)(i+11),
					*(BYTE *)(i+12),
					*(BYTE *)(i+13),
					*(BYTE *)(i+14),
					*(BYTE *)(i+15)
			);
		}

		i = (ContextRecord->Esp - 16) & ~0x0000000F;
		while (i < ContextRecord->Esp + 128) {
			MEMORY_BASIC_INFORMATION mbi;
			i += 16;
			if (VirtualQuery((void *)i, &mbi, sizeof(mbi))
			== sizeof(mbi) && mbi.State != MEM_COMMIT) continue;

			_snprintf(expandedmessage + strlen(expandedmessage),
					sizeof(expandedmessage) - strlen(expandedmessage),
					"%08X: %08X %08X %08X %08X\n"
					, i,
					*(DWORD *)(i+0),
					*(DWORD *)(i+4),
					*(DWORD *)(i+8),
					*(DWORD *)(i+12)
			);
		}

		if ((MessageBox(NULL, expandedmessage, 
					"Please Confirm Permission To Email This Information",
					MB_OKCANCEL | MB_ICONASTERISK) == IDOK))
		{
			MapiMessage mm;
			MapiRecipDesc mrd;
			ULONG result;
			char *c = emailaddress;
			char subjectbuffer[64];
			HANDLE h_mapi32;
			ULONG PASCAL (*fpMAPISendMail) \
					(LHANDLE,ULONG,lpMapiMessage,FLAGS,ULONG) = NULL;

			if ((h_mapi32 = LoadLibrary("mapi32.dll")))
				fpMAPISendMail = (void *) GetProcAddress(h_mapi32, 
						"MAPISendMail");
			dump(h_mapi32);
			dump(fpMAPISendMail);

			while(*c) (*c++)--;
			//dumps(emailaddress);

			snprintf(subjectbuffer, sizeof(subjectbuffer),
					"%s %08X dump report", myname, (int)mytimestamp);

			zero(mrd);
			mrd.ulRecipClass = MAPI_TO;
			mrd.lpszName = "0x517A5D";
			mrd.lpszAddress = emailaddress;

			zero(mm);
			mm.lpszSubject = subjectbuffer;
			mm.lpszNoteText = expandedmessage;
			mm.nRecipCount = 1;
			mm.lpRecips = &mrd;

			if (fpMAPISendMail) {
				result = (*fpMAPISendMail)(0, 0, &mm,
						MAPI_LOGON_UI | MAPI_NEW_SESSION, 0);
				dump(result);
			}
			else result = MAPI_E_NOT_SUPPORTED;

			if (result == SUCCESS_SUCCESS) {
				MessageBox(NULL, "Mail succcesfully submitted\n"
						" to MAPI email system.",
						myname, MB_ICONINFORMATION);
			} else {
				MessageBox(NULL, "Failed.  Bummer.", myname, MB_ICONWARNING);
			}
		}
	}

	d_printf("exiting se_handler() and program\n");
	exit(ExceptionRecord->ExceptionCode & 0xFF);
}
#endif


// ============================================================================
#define best_of_2(var, a, b) \
	;var = best_of_2_helper(#var, a, b);
static DWORD best_of_2_helper(const char *varname, DWORD a, DWORD b) {
	if (a == b) return(a);
	if (!a && !b) {
		d_printf("best_of_2 for %s: trouble: %s:  a=%08X, b=%08X\n",
				varname, "both terms were null", a, b);
		return(0);
	}
	if (!a) {
		d_printf("best_of_2 for %s: trouble: %s:  a=%08X, b=%08X\n",
				varname, "first term was null", a, b);
		return(b);
	}
	if (!b) {
		d_printf("best_of_2 for %s: trouble: %s:  a=%08X, b=%08X\n",
				varname, "second term was null", a, b);
		return(a);
	}
	if (a != b) {
		d_printf("best_of_2 for %s: trouble: %s:  a=%08X, b=%08X\n",
				varname, "terms did not match", a, b);
		return(0);
	}
	return(0);
}


// ----------------------------------------------------------------------------
#define best_of_3(var, a, b, c) \
	var = best_of_3_helper(#var, a, b, c);
static DWORD best_of_3_helper(const char *varname, DWORD a, DWORD b, DWORD c) {

	if (a == b && a == c) return(a);	// best scenario, all match

	// deal with any term being NULL
	if (!a) {
		d_printf("best_of_3 for %s: trouble: %s:  a=%08X, b=%08X, c=%08X\n",
				varname, "first term was null", a, b, c);
		return(best_of_2_helper(varname, b, c));
	}
	if (!b) {
		d_printf("best_of_3 for %s: trouble: %s:  a=%08X, b=%08X, c=%08X\n",
				varname, "second term was null", a, b, c);
		return(best_of_2_helper(varname, a, c));
	}
	if (!c) {
		d_printf("best_of_3 for %s: trouble: %s:  a=%08X, b=%08X, c=%08X\n",
				varname, "third term was null", a, b, c);
		return(best_of_2_helper(varname, a, b));
	}


	if (a != b || a != c || b != c) {
		d_printf("best_of_3 for %s: trouble: %s:  a=%08X, b=%08X, c=%08X\n",
				varname, "a term didn't match other terms", a, b, c);
		if (a == b) return(a);			// two of three
		if (a == c) return(a);			// two of three
		if (b == c) return(b);			// two of three
		return(0);						// a != b != c, treat as no match
	}

	return(0);
}


// ----------------------------------------------------------------------------
#if 0
#define best_of_4(var, a, b, c, d) \
	var = best_of_4_helper(#var, a, b, c, d);
static DWORD best_of_4_helper(const char *varname, DWORD a, DWORD b, DWORD c,
								DWORD d)
{
	if (a == b && a == c && a == d) return(a);

	// deal with any term being NULL
	if (!a || !b || !c || !d)
		d_printf("best_of_4 for %s: trouble: %s:  a=%08X, b=%08X, c=%08X, "
				"d=%08X\n",
				varname, "a term didn't match other terms", a, b, c, d);
	if (!a) return(best_of_3_helper(varname, b, c, d));
	if (!b) return(best_of_3_helper(varname, a, c, d));
	if (!c) return(best_of_3_helper(varname, a, b, d));
	if (!d) return(best_of_3_helper(varname, a, b, c));

	d_printf("best_of_4: trouble: a term didn't match other terms: "
			"a=%08X, b=%08X, c=%08X, d=%08X\n", a, b, c, d);

	// 3 of 4
	if (a == b && (a == c || a == d)) return(a);
	if (a == c && a == d) return(a);
	if (b == c && b == d) return(b);

	// 2 and 2, treat as no match
	if ((a == b && c == d) || (a == c && b == d) || (a == d && b == c))
		return(0);

	// 2 of 4
	if (a == b || a == c || a == d) return(a);
	if (b == c || b == d) return(b);
	if (c == d) return(c);

	// all terms differ, no match
	return(0);
}
#endif


// ============================================================================
void find_map_data_variables(void) {
	static DWORD d = 0, m1 = 0, m2 = 0, m3 = 0, x1 = 0, x2 = 0, x3 = 0;
   	static DWORD y1 = 0, y2 = 0, y3 = 0, z1 = 0, z2 = 0, z3 = 0;
	// 20071221 added several fallback search patterns

	set_hexsearch2_limits_to_code();

	// this is the simplest type of hexsearch -- no meta tokens at all.
	d = hexsearch2(0xC6, 0x44, 0x24, 0x38, 0x3E, 0xBF, EOL);
	if (!d) d_printf("trouble!  map data search failed! 1\n");

	if (d) {
		// however this math complicates it.
		m1 = peekd(d + 0x18);
		x1 = peekd(d + 0x1E);
		y1 = peekd(d + 0x24);
		z1 = peekd(d + 0x2A);
		if (x1 + 4 != y1 || y1 + 4 != z1) {
			d_printf("trouble!  map data verification failed. 1\n");
			x1 = 0; y1 = 0; z1 = 0;
		}
	}


	// ---------
	//0F B7 59 04               movzx   ebx, word ptr [ecx+4]
	//0F B7 71 FE               movzx   esi, word ptr [ecx-2]
	//0F B7 39                  movzx   edi, word ptr [ecx]
	//0F B7 51 02               movzx   edx, word ptr [ecx+2]
	//0F B7 49 06               movzx   ecx, word ptr [ecx+6]
	//8B 2D 68 85 45 01         mov     ebp, map_data
	hexsearch2(
			0x0F, 0xB7, BYTERANGE, 0x40, 0x7F, 0x04,
			0x0F, 0xB7, BYTERANGE, 0x40, 0x7F, 0xFE,
			0x0F, 0xB7, BYTERANGE, 0x00, 0x3F,
			0x0F, 0xB7, BYTERANGE, 0x40, 0x7F, 0x02,
			0x0F, 0xB7, BYTERANGE, 0x40, 0x7F, 0x06,
			0x8B, BYTERANGE, 0x00, 0x3F, HERE, ADDRESS, EOL);
	d = here[1];
	if (!d) d_printf("trouble!  map data search failed! 2\n");
	m2 = peekd(d);
	if (!m2)
		d_printf("trouble!  map data verification failed. 2\n");


	//8D 4C 24 17               lea     ecx, [esp+24h+var_D]
	//B8 01 00 00 00            mov     eax, 1
	//51                        push    ecx
	//88 44 24 1B               mov     byte ptr [esp+28h+var_D], al
	//E8 6D 85 D5 FF            call    zeromem
	//39 1D 80 85 45 01         cmp     map_x_count, ebx
	//89 5C 24 18               mov     [esp+24h+var_D+1], ebx
	//0F 8E 7B 00 00 00         jle     loc_81037E
	//A1 84 85 45 01            mov     eax, map_y_count
	hexsearch2(
			0xB8, DWORD_, 0x00000001,
			0x51,
			0x88, 0x44, 0x24, 0x1B,
			CALL,
			0x39, RANGE_LO, 0x00, RANGE_HI, 0x3F, HERE, ADDRESS,
			0x89, EOL);
	if (!here[0]) d_printf("trouble!  map data search failed! 3\n");
	else {
		d = here[1];
		x2 = peekd(d);
		y2 = peekd(d+4+4+6+1);
		if (x2 && y2 && x2 + 4 != y2) {
			d_printf("trouble!  map data verification failed. 3\n");
			x2 = 0; y2 = 0;
		}
	}


	//66 89 44 3A 04            mov     [edx+edi+4], ax
	//8B 56 04                  mov     edx, [esi+4]
	//8B 14 13                  mov     edx, [ebx+edx]
	//33 C0                     xor     eax, eax
	//66 89 44 3A 06            mov     [edx+edi+6], ax
	//39 05 88 85 45 01         cmp     map_z_count, eax
	hexsearch2(
			0x66, 0x89, 0x44, RANGE_LO, 0x00, RANGE_HI, 0x3F, 0x04,
			0x8B, RANGE_LO, 0x40, RANGE_HI, 0x7F, 0x04,
			0x8B, 0x14, RANGE_LO, 0x00, RANGE_HI, 0x3F, 
			0x33, 0xC0,
			0x66, 0x89, 0x44, RANGE_LO, 0x00, RANGE_HI, 0x3F, 0x06,
			0x39, 0x05, HERE, ADDRESS,
			EOL);
	if (!here[0]) d_printf("trouble!  map data search failed! 4\n");
	else {
		z2 = peekd(here[1]);
		if (z2 && y2 + 4 != z2) {
			d_printf("trouble!  map data verification failed. 4\n");
			z2 = 0;
		}
	}


	// ---------
	//E8 AE 84 D5 FF            call    zeromem
	//39 1D 80 85 45 01         cmp     map_x_count, ebx
	//89 5C 24 20               mov     [esp+24h+var_4], ebx
	//0F 8E C9 00 00 00         jle     loc_81048B
	//A1 88 85 45 01            mov     eax, map_z_count
	//8B 0D 84 85 45 01         mov     ecx, map_y_count
	hexsearch2(
			0x39, RANGE_LO, 0x00, RANGE_HI, 0x3F, HERE, ADDRESS,
			0x89, RANGE_LO, 0x40, RANGE_HI, 0x7F, 0x24, ANYBYTE,
			0x0F, 0x8E, ANYBYTE, ANYBYTE, 0x00, 0x00,
			0xA1, HERE, ADDRESS,
			0x8B, RANGE_LO, 0x00, RANGE_HI, 0x3F, HERE, ADDRESS, EOL);
	x3 = peekd(here[1]);
	y3 = peekd(here[3]);
	z3 = peekd(here[2]);
	if (x3 + 4 != y3 || y3 + 4 != z3) {
		d_printf("trouble!  map data verification failed. 5\n");
		x3 = 0; y3 = 0; z3 = 0;
	}


	// ---------
	best_of_2(map_data, m1, m2);
	best_of_3(map_x_count, x1, x2, x3);
	best_of_3(map_y_count, y1, y2, y3);
	best_of_3(map_z_count, z1, z2, z3);

	dump(map_data);
	dump(map_x_count);
	dump(map_y_count);
	dump(map_z_count);
}


// ----------------------------------------------------------------------------
void find_map_data_designation_offset(void) {
	DWORD md1 = 0, md2 = 0, md3 = 0;

	set_hexsearch2_limits_to_code();
/*
8B 15 CC 29 49 01                 mov     edx, map_data
8B 44 82 04                       mov     eax, [edx+eax*4+4]
8B 4C 24 2C                       mov     ecx, [esp+2Ch]
8B 14 88                          mov     edx, [eax+ecx*4]
8B 44 24 30                       mov     eax, [esp+30h]
8B 34 10                          mov     esi, [eax+edx]
85 F6                             test    esi, esi
0F 84 47 01 00 00                 jz      near ptr unk_42AD8F
0F BF C3                          movsx   eax, bx
8B 8C 86 60 02 00 00              mov     ecx, [esi+eax*4+260h]
*/

	hexsearch2(
			0x8B, BYTERANGE, 0x00, 0x3F, DWORD_, map_data,
			0x8B,
			SKIP_UP_TO, 20,
			0x8B, BYTERANGE, 0x00, 0x3F, ANYBYTE,
			0x85, BYTERANGE, 0xC0, 0xFF,
			JZ,
			SKIP_UP_TO, 8,
			0x8B, BYTERANGE, 0x80, 0xBF, ANYBYTE, 
					HERE, DWORDRANGE, 0x00000200, 0x00000400, 
			EOL);
	if (here[1]) md1 = peekd(here[1]);

/*
8B 2D CC 29 49 01                mov     ebp, map_data
89 5C 24 14                      mov     [esp+14h], ebx
89 4C 24 10                      mov     [esp+10h], ecx
0F BF DE                         movsx   ebx, si
8B 6C 9D 00                      mov     ebp, [ebp+ebx*4+0]
0F BF CA                         movsx   ecx, dx
03 C9                            add     ecx, ecx
03 C9                            add     ecx, ecx
0F BF DF                         movsx   ebx, di
8B 5C 9D 00                      mov     ebx, [ebp+ebx*4+0]
8B 1C 19                         mov     ebx, [ecx+ebx]
0F BF 6C 24 10                   movsx   ebp, word ptr [esp+10h]
89 4C 24 1C                      mov     [esp+1Ch], ecx
0F BF 4C 24 14                   movsx   ecx, word ptr [esp+14h]
89 4C 24 24                      mov     [esp+24h], ecx
C1 E1 04                         shl     ecx, 4
89 6C 24 30                      mov     [esp+30h], ebp
03 E9                            add     ebp, ecx
81 8C AB 60 02 00 00 00 00 00+   or      dword ptr [ebx+ebp*4+260h], 8000000h
*/

	hexsearch2(
			0x8B, BYTERANGE, 0x00, 0x3F, DWORD_, map_data,
			SKIP_UP_TO, 80,
			0x81, BYTERANGE, 0x80, 0xBF, ANYBYTE,
					HERE, DWORDRANGE, 0x200, 0x400, DWORD_, 0x08000000,
			EOL);
	if (here[1]) md2 = peekd(here[1]);



	best_of_2(map_data_designation_offset, md1, md2);
}



// ============================================================================

DWORD demo;

// ============================================================================
int main(int argc, char *argv[]) {
#ifdef USE_SEH
	volatile void * handler;			// must be consecutive local stack vars.
	volatile void * prev_se_frame;
	void * frame_temp;
#endif

#ifdef USE_SEH
	// As this is stack-based, it cannot be done in a called function,
	//   it must be done here.

	if (&handler != &prev_se_frame + 1) croak("SEH frame misaligned");
	handler = &my_se_handler;
	frame_temp = &prev_se_frame;
	// push and pop, and the frame_temp variable, are used because I don't know
	// the proper way to load or store values from a register in GCC asm.
	asm(	"pushl %fs:0" );
	asm(	"popl %0" : "=m" (prev_se_frame) );
	asm(	"pushl %0" : : "m" (frame_temp) );
	asm(	"popl %fs:0" );
#endif

	argc = argc;						// shaddup the compiler warning

	d_printf("DBGVIEWCLEAR\n");
	if (strrchr(argv[0], '\\'))
		strncpy(myname, strrchr(argv[0], '\\') + 1, MAX_PATH);
	else strncpy(myname, argv[0], MAX_PATH);

	if (strrchr(myname, '.')) *(strrchr(myname, '.')) = '\0';

	// hack: parse timestamp out of the in-memory copy of our PE header.
	mytimestamp = *(DWORD *)((BYTE *)GetModuleHandle(NULL) +
			*(DWORD *)((BYTE *)GetModuleHandle(NULL)+0x3C) + 8);
	d_printf("I am %s, timestamped %08X\n", myname, mytimestamp);

	if (!open_dwarf_fortress()) {
		croak("Couldn't open Dwarf Fortress process:\n%s", errormessage);
	}

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	myininame = malloc(MAX_PATH+4);
	GetModuleFileName(NULL, myininame, MAX_PATH);

	if (strrchr(myininame, '.'))
		myininame[strrchr(myininame, '.') - myininame] = '\0';
	strcat(myininame, ".ini");
	dumps(myininame);

	sprintf(inisectionname, "%08X", (UINT)df_timestamp);

#define get_ini_dword(x) x = GetPrivateProfileInt( inisectionname, #x, \
		0, myininame )
	get_ini_dword(demo);
	get_ini_dword(dwarf_fortress);
	get_ini_dword(map_data);
	get_ini_dword(map_x_count);
	get_ini_dword(map_y_count);
	get_ini_dword(map_z_count);
	get_ini_dword(map_data_designation_offset);
#undef get_ini_dword

#define adjust_ini_address(xx) if (xx) xx = xx - 0x400000 + df_memory_base;
	adjust_ini_address(dwarf_fortress);
	adjust_ini_address(map_data);
	adjust_ini_address(map_x_count);
	adjust_ini_address(map_y_count);
	adjust_ini_address(map_z_count);
#undef adjust_ini_address

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (!dwarf_fortress)
		dwarf_fortress = df_memstr("Dwarf Fortress");
	
	if (!map_data || !map_x_count || !map_y_count || !map_z_count)
		find_map_data_variables();
	if (!map_data_designation_offset)
		find_map_data_designation_offset();

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	dump(demo);
	dump(dwarf_fortress);
	dump(map_data);
	dump(map_x_count);
	dump(map_y_count);
	dump(map_z_count);
	dump(map_data_designation_offset);

	close_dwarf_fortress();

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define croak_if_null(xx) \
	if (!xx) \
		croak("A needed location was not found by the search.\n" \
			"If you know the proper offset, you can add the\n" \
			"term to the INI file:\n" \
			"    [%s]\n" \
			"    %s=0xOFFSET\n", \
			inisectionname, #xx);
	croak_if_null(dwarf_fortress);
	croak_if_null(map_data);
	croak_if_null(map_x_count);
	croak_if_null(map_y_count);
	croak_if_null(map_z_count);
	croak_if_null(map_data_designation_offset);
#undef croak_if_null

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	FILE *fh;		// file handle
	DWORD bw = 0;	// bytes written
	fh = fopen("REVEAL.INI", "wt");
	if (!fh) croak ("Failed to create temporary file %s\n", "REVEAL.INI");

#define print_address_to_ini(xx) fprintf(fh, "%s=0x%08lX\n", #xx, xx);
	bw += fprintf(fh,
		"; This is a TEMPORARY file for the use of the %s module.\n",
		(is_sdl_app ? "reveal2_sdlapp.bin" : "reveal2_opengl.bin"));
	bw += fprintf(fh,
		"; This file is intended to be DELETED immediately ""after use.\n");
	bw += fprintf(fh, "; Changes to this file will not be preserved.\n");
	bw += fprintf(fh, "; How are you looking at this file anyway?\n");
	bw += fprintf(fh, "[generic]\n");
	bw += print_address_to_ini(dwarf_fortress);
	bw += fprintf(fh, "\n");
	bw += fprintf(fh, "[address]\n");
	bw += print_address_to_ini(map_data);
	bw += print_address_to_ini(map_x_count);
	bw += print_address_to_ini(map_y_count);
	bw += print_address_to_ini(map_z_count);
	bw += fprintf(fh, "\n");
	bw += fprintf(fh, "[offset]\n");
	bw += fprintf(fh, "%s=0x%04X\n", "map_data_designation_offset",
			map_data_designation_offset);
	bw += fprintf(fh, "\n");
	fclose(fh);
	dump(bw);

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	STARTUPINFO startupInfo;
	zero(startupInfo);
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo;
	zero(processInfo);

	if (0 ==
		CreateProcess(
			NULL,						// lpApplicationName
			(is_sdl_app ? "reveal2_sdlapp.bin" : "reveal2_opengl.bin"),
										// lpCommandLine
			NULL,						// lpProcessAttributes
			NULL,						// lpThreadAttributes
			FALSE,						// bInheritHandles
			CREATE_NO_WINDOW,			// dwCreationFlags
			NULL,						// lpEnvironment
			NULL,						// lpCurrentDirectory
			&startupInfo,				// lpStartupInfo
			&processInfo				// OUT lpProcessInformation
		)
	) {
		unlink("REVEAL.INI");
		croak("CreateProcess(%s) failed with error %08X\n", 
		(is_sdl_app ? "reveal2_sdlapp.bin" : "reveal2_opengl.bin"),
		GetLastError());
	}

    WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD retcode;
	GetExitCodeProcess(processInfo.hProcess, &retcode);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	unlink("REVEAL.INI");

	if (retcode != 0) {
		croak("%s returned an error code %08X\n%s",
				(is_sdl_app ? "reveal2_sdlapp.bin" : "reveal2_opengl.bin"),
			   	retcode);
	}


#ifdef USE_SEH
	d_printf("Uninstalling our SEH handler... ");
	asm(	"pushl %0" : : "m" (prev_se_frame) );
	asm(	"popl %fs:0" );
#endif

	return 0;
}


