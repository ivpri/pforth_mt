/* @(#) pfcompil.h 96/12/18 1.11 */

#ifndef _pforth_compile_h
#define _pforth_compile_h

/***************************************************************
** Include file for PForth Compiler
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

Err   ffPushInputStream( FileStream *InputFile );
ExecToken NameToToken( const ForthString *NFA );
FileStream * ffConvertSourceIDToStream( cell_t id );
FileStream *ffPopInputStream( void );
cell_t  ffConvertStreamToSourceID( FileStream *Stream );
cell_t  ffFind( const ForthString *WordName, ExecToken *pXT );
cell_t  ffFindC( DL_TASK const char *WordName, ExecToken *pXT );
cell_t  ffFindNFA( const ForthString *WordName, const ForthString **NFAPtr );
cell_t  ffNumberQ( DL_TASK const char *FWord, cell_t *Num );
cell_t  ffRefill( DL_TASK_VOID );
cell_t  ffTokenToName( ExecToken XT, const ForthString **NFAPtr );
cell_t *NameToCode( ForthString *NFA );
PForthDictionary pfBuildDictionary( DL_TASK cell_t HeaderSize, cell_t CodeSize );
char *ffWord( DL_TASK char c );
char *ffLWord( DL_TASK char c );
const ForthString *NameToPrevious( const ForthString *NFA );
cell_t FindSpecialCFAs( void );
cell_t FindSpecialXTs( DL_TASK_VOID );
cell_t NotCompiled( const char *FunctionName );
void  CreateDicEntry( ExecToken XT, const ForthStringPtr FName, ucell_t Flags );
void  CreateDicEntryC( ExecToken XT, const char *CName, ucell_t Flags );
void  ff2Literal( cell_t dHi, cell_t dLo );
void  ffALiteral( cell_t Num );
void  ffColon( DL_TASK_VOID );
void  ffCreate( DL_TASK_VOID );
void  ffCreateSecondaryHeader( const ForthStringPtr FName);
void  ffDefer( DL_TASK_VOID );
void  ffFinishSecondary( void );
void  ffLiteral( cell_t Num );
void  ffStringCreate( ForthStringPtr FName);
void  ffStringDefer( const ForthStringPtr FName, ExecToken DefaultXT );
void  pfHandleIncludeError( void );

ThrowCode ffSemiColon( DL_TASK_VOID );
ThrowCode ffOK( DL_TASK_VOID );
ThrowCode ffInterpret( DL_TASK_VOID );
ThrowCode ffOuterInterpreterLoop( DL_TASK_VOID );
ThrowCode ffIncludeFile( DL_TASK FileStream *InputFile );

#ifdef PF_SUPPORT_FP
void ffFPLiteral( PF_FLOAT fnum );
#endif

#ifdef __cplusplus
}
#endif

#endif /* _pforth_compile_h */
