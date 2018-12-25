/*********************************************************************
 *                                                                   *
 * MODULE NAME :  client.c               AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  11-28-92                                           *
 *                                                                   *
 * MODULE DESCRIPTION:                                               *
 *                                                                   *
 *  Module that creates a frame/client window and associates the     *
 *  client with the Client Notebook page.                            *
 *                                                                   *
 * HISTORY:                                                          *
 *                                                                   *
 *  11-28-92 - Program coded.                                        *
 *                                                                   *
 *  Rick Fishman                                                     *
 *  Code Blazers, Inc.                                               *
 *  4113 Apricot                                                     *
 *  Irvine, CA. 92720                                                *
 *  CIS ID: 72251,750                                                *
 *                                                                   *
 *********************************************************************/

#pragma strings(readonly)   // used for debug version of memory mgmt routines

/*********************************************************************/
/*------- Include relevant sections of the OS/2 header files --------*/
/*********************************************************************/

#define  INCL_GPICONTROL
#define  INCL_GPILCIDS
#define  INCL_GPIPRIMITIVES
#define  INCL_WINERRORS
#define  INCL_WINFRAMEMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINSTDBOOK
#define  INCL_WINSYS
#define  INCL_WINWINDOWMGR

/**********************************************************************/
/*----------------------------- INCLUDES -----------------------------*/
/**********************************************************************/

#include <os2.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "nbsize.h"

/*********************************************************************/
/*------------------- APPLICATION DEFINITIONS -----------------------*/
/*********************************************************************/

#define CLIENT_WINCLASS    "ClientWinClass"

#define FONTFACE_NAME      "Helv"

#define STRING_TO_DISPLAY  "Hi"

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

static VOID wmCreate        ( HWND hwndClient );
static VOID wmPaint         ( HWND hwndClient );
static VOID wmSize          ( INT cxNew, INT cyNew );
static BOOL FillFattrs      ( HPS hps, HWND hwndClient );
static BOOL FillFattrsStruct( HPS hps, HWND hwndClient, PFONTMETRICS pfm );
static INT  GetFontCount    ( HPS hps, HWND hwndClient, PBOOL pfUseOurFont );
static VOID PaintText       ( HPS hps, HWND hwndClient );

static FNWP wpClient;

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

static FATTRS fattrs;
static LONG   lcid = 1;
static INT    cxClient, cyClient;

/**********************************************************************/
/*------------------------ CreateClientPage --------------------------*/
/*                                                                    */
/*  CREATE WINDOW AND ASSOCIATE IT WITH A NOTEBOOK PAGE               */
/*                                                                    */
/*  INPUT: window handle of parent and owner,                         */
/*         notebook window handle,                                    */
/*         notebook page ID                                           */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: client window handle                                      */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
HWND CreateClientPage( HWND hwndParent, HWND hwndNB, ULONG ulPageID )
{
    HWND  hwndFrame, hwndClient = NULLHANDLE;
    ULONG flFrame = 0;

    if( WinRegisterClass( ANCHOR( hwndNB ), CLIENT_WINCLASS, wpClient, 0, 0 ) )
    {
        hwndFrame = WinCreateStdWindow( hwndParent, 0, &flFrame,
                                        CLIENT_WINCLASS, NULL, 0, NULLHANDLE,
                                        ID_CLIENTS_FRAME, &hwndClient );

        if( hwndClient )
        {
            if( !WinSendMsg( hwndNB, BKM_SETPAGEWINDOWHWND,
                             MPFROMLONG( ulPageID ),
                             MPFROMLONG( hwndClient ) ) )
            {
                WinDestroyWindow( hwndFrame );

                hwndClient = NULLHANDLE;

                Msg( "CreateClientPage SETPAGEWINDOWHWND RC(%X)",
                     HWNDERR( hwndNB ) );
            }
        }
        else
            Msg( "CreateClientPage WinCreateStdWindow RC(%X)",
                 HWNDERR( hwndNB ) );
    }
    else
        Msg( "CreateClientPage WinRegisterClass RC(%X)", HWNDERR( hwndNB ) );

    return hwndClient;
}

/**********************************************************************/
/*----------------------------- wpClient -----------------------------*/
/*                                                                    */
/*  CLIENT WINDOW PROCEDURE                                           */
/*                                                                    */
/*  INPUT: window proc params                                         */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: retcode from processing message                           */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static MRESULT EXPENTRY wpClient( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        case WM_CREATE:

            wmCreate( hwnd );

            break;


        case WM_SIZE:

            wmSize( (INT) SHORT1FROMMP( mp2 ), (INT) SHORT2FROMMP( mp2 ) );

            break;


        case WM_PAINT:

            wmPaint( hwnd );

            break;
    }

    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*----------------------------- wmCreate -----------------------------*/
/*                                                                    */
/*  PROCESS WM_CREATE MESSAGE FOR CLIENT WINDOW                       */
/*                                                                    */
/*  INPUT: client window handle                                       */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID wmCreate( HWND hwndClient )
{
    HPS hps = WinGetPS( hwndClient );

    if( hps )
    {
        if( !FillFattrs( hps, hwndClient ) )
            lcid = 0;
    }
    else
        Msg( "wmCreate WinGetPS RC(%X)", HWNDERR( hwndClient ) );

    return;
}

/**********************************************************************/
/*----------------------------- wmPaint ------------------------------*/
/*                                                                    */
/*  PROCESS WM_PAINT MESSAGE FOR CLIENT WINDOW                        */
/*                                                                    */
/*  INPUT: client window handle                                       */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID wmPaint( HWND hwndClient )
{
    HPS hps = WinBeginPaint( hwndClient, NULLHANDLE, NULL );

    if( hps )
    {
        RECTL rcl;

        GpiSetColor( hps, CLR_RED );
        GpiSetMix( hps, FM_OVERPAINT );
        GpiSetBackColor( hps, CLR_WHITE );
        GpiSetBackMix( hps, BM_OVERPAINT );

        if( WinQueryWindowRect( hwndClient, &rcl ) )
        {
            if( !WinFillRect( hps, &rcl, CLR_WHITE ) )
                Msg( "wmPaint WinFillRect RC(%X)", HWNDERR( hwndClient ) );
        }
        else
            Msg( "wmPaint WinQueryWindowRect RC(%X)", HWNDERR( hwndClient ) );

        if( GPI_ERROR == GpiCreateLogFont( hps, NULL, lcid, &fattrs ) )
            Msg( "wmPaint GpiCreateLogFont RC(%X)", HWNDERR( hwndClient ) );
        else
        {
            if( GpiSetCharSet( hps, lcid ) )
                PaintText( hps, hwndClient );
            else
                Msg( "wmPaint GpiSetCharSet RC(%X)", HWNDERR( hwndClient ) );
        }

        if( !WinEndPaint( hps ) )
            Msg( "wmPaint WinEndPaint RC(%X)", HWNDERR( hwndClient ) );
    }
    else
        Msg( "wmPaint WinBeginPaint RC(%X)", HWNDERR( hwndClient ) );

    return;
}

/**********************************************************************/
/*------------------------------ wmSize ------------------------------*/
/*                                                                    */
/*  PROCESS WM_SIZE MESSAGE FOR CLIENT WINDOW                         */
/*                                                                    */
/*  INPUT: client window handle,                                      */
/*         new window width,                                          */
/*         new window height                                          */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID wmSize( INT cxNew, INT cyNew )
{
    cxClient = cxNew;

    cyClient = cyNew;

    return;
}

/**********************************************************************/
/*---------------------------- FillFattrs ----------------------------*/
/*                                                                    */
/*  DO EVERYTHING NECESSARY TO FILL IN THE FATTRS STRUCT FOR OUR FONT */
/*                                                                    */
/*  INPUT: presentation space handle,                                 */
/*         client window handle                                       */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static BOOL FillFattrs( HPS hps, HWND hwndClient )
{
    BOOL         fUseOurFont, fSuccess = FALSE;
    PFONTMETRICS afm;
    HDC          hdc;
    LONG         cFonts;
    INT          i;

    hdc = GpiQueryDevice( hps );

    if( hdc && hdc != HDC_ERROR )
    {
        cFonts = GetFontCount( hps, hwndClient, &fUseOurFont );

        afm = (PFONTMETRICS) malloc( cFonts * sizeof( FONTMETRICS ) );

        if( afm )
        {
            if( GPI_ALTERROR != GpiQueryFonts( hps, QF_PUBLIC,
                                        fUseOurFont ? FONTFACE_NAME : NULL,
                                        &cFonts, sizeof( FONTMETRICS ), afm ) )
            {
                for( i = 0; i < cFonts; i++)
                {
                    if( afm[ i ].fsDefn & FM_DEFN_OUTLINE )
                    {
                        if( FillFattrsStruct( hps, hwndClient, &afm[ i ] ) )
                            fSuccess = TRUE;

                        break;
                    }
                }
            }
            else
                Msg( "FillFattrs GpiQueryFonts RC(%X)", HWNDERR( hwndClient ) );

            free( afm );
        }
        else
            Msg( "Out of memory in FillFattrs!" );
    }
    else
        Msg( "FillFattrs GpiQueryDevice RC(%X)", HWNDERR( hwndClient ) );

    return fSuccess;
}

/**********************************************************************/
/*------------------------- FillFattrsStruct -------------------------*/
/*                                                                    */
/*  FILL THE FATTRS STRUCTURE FOR THE FONT THAT WE WILL USE.          */
/*                                                                    */
/*  INPUT: presentation space handle,                                 */
/*         client window handle,                                      */
/*         pointer to FONTMETRICS struct for our font                 */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static BOOL FillFattrsStruct( HPS hps, HWND hwndClient, PFONTMETRICS pfm )
{
    BOOL fSuccess = TRUE;

    fattrs.usCodePage = GpiQueryCp( hps );

    if( fattrs.usCodePage != GPI_ERROR )
    {
        (void) strcpy( fattrs.szFacename, pfm->szFacename );

        fattrs.usRecordLength = sizeof( FATTRS );
        fattrs.idRegistry = pfm->idRegistry;
        fattrs.fsFontUse = FATTR_FONTUSE_OUTLINE;
    }
    else
    {
        fSuccess = FALSE;

        Msg( "FillFattrsStruct GpiQueryCp RC(%X)", HWNDERR( hwndClient ) );
    }

    return fSuccess;
}

/**********************************************************************/
/*--------------------------- GetFontCount ---------------------------*/
/*                                                                    */
/*  GET A COUNT OF THE FONTS AVAILABLE.                               */
/*                                                                    */
/*  INPUT: presentation space handle,                                 */
/*         client window handle,                                      */
/*         pointer to bool that returns if we are using the original  */
/*           font requested                                           */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: count of fonts                                            */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static INT GetFontCount( HPS hps, HWND hwndClient, PBOOL pfUseOurFont )
{
    LONG cFonts, cFontsRequested = 0;

    *pfUseOurFont = TRUE;

    cFonts = GpiQueryFonts( hps, QF_PUBLIC, FONTFACE_NAME, &cFontsRequested, 0,
                            NULL );

    if( cFonts == GPI_ALTERROR )
    {
        cFonts = 0;

        Msg( "GetFontCount GpiQueryFonts RC(%X)", HWNDERR( hwndClient ) );
    }
    else if( !cFonts )
    {
        *pfUseOurFont = FALSE;

        cFontsRequested  = 0;

        cFonts = GpiQueryFonts( hps, QF_PUBLIC, NULL, &cFontsRequested, 0,
                                NULL );

        if( !cFonts )
            Msg( "GetFontCount GpiQueryFonts found NO FONTS in your system!" );
        else if( cFonts == GPI_ALTERROR )
        {
            cFonts = 0;

            Msg( "GetFontCount GpiQueryFonts RC(%X)", HWNDERR( hwndClient ) );
        }
    }

    return cFonts;
}

/**********************************************************************/
/*--------------------------- PaintText ------------------------------*/
/*                                                                    */
/*  PAINT TEXT IN THE CLIENT AREA                                     */
/*                                                                    */
/*  INPUT: presentation space handle,                                 */
/*         client window handle                                       */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID PaintText( HPS hps, HWND hwndClient )
{
    SIZEF  sizf;
    POINTL ptl;

    sizf.cx = MAKEFIXED( cxClient, 0 );
    sizf.cy = MAKEFIXED( cyClient, 0 );

    if( GpiSetCharBox( hps, &sizf ) )
    {
        ptl.x = 0;
        ptl.y = 0;

        if( GPI_ERROR == GpiCharStringPosAt( hps, &ptl, NULL, 0,
                  strlen( STRING_TO_DISPLAY ), STRING_TO_DISPLAY, NULL ) )
            Msg( "PaintText GpiCharStringPosAt RC(%X)", HWNDERR( hwndClient ) );
    }
    else
        Msg( "PaintText GpiSetCharBox RC(%X)", HWNDERR( hwndClient ) );

    return;
}

/************************************************************************
 *                      E N D   O F   S O U R C E                       *
 ************************************************************************/
