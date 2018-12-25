/*********************************************************************
 *                                                                   *
 * MODULE NAME :  mle.c                  AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  11-28-92                                           *
 *                                                                   *
 * MODULE DESCRIPTION:                                               *
 *                                                                   *
 *  Module that creates an MLE and associates it with the MLE        *
 *  Notebook page.                                                   *
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

#define  INCL_WINMLE
#define  INCL_WINERRORS
#define  INCL_WINFRAMEMGR
#define  INCL_WINMESSAGEMGR
#define  INCL_WINSTDBOOK
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


/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

static FNWP wpMLE;

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

static PFNWP pfnwpDefMLE;

/**********************************************************************/
/*-------------------------- CreateMlePage ---------------------------*/
/*                                                                    */
/*  CREATE AN MLE AND ASSOCIATE IT WITH A NOTEBOOK PAGE               */
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
HWND CreateMlePage( HWND hwndParent, HWND hwndNB, ULONG ulPageID )
{
    HWND hwndMLE = WinCreateWindow( hwndParent, WC_MLE, NULL,
                                    MLS_VSCROLL | MLS_HSCROLL | MLS_WORDWRAP,
                                    0, 0, 0, 0, hwndParent,
                                    HWND_TOP, MLE_PAGE, NULL, NULL );

    if( hwndMLE )
    {
        WinSetWindowText( hwndMLE, "Hello, Folks. I am an MLE. I like to be "
                          "sized. I know - it's a strange thing to like. "
                          "The problem is that I can't turn off these "
                          "feelings. That's just the way I am. Oh, well..." );

        if( !WinSendMsg( hwndNB, BKM_SETPAGEWINDOWHWND,
                         MPFROMLONG( ulPageID ), MPFROMLONG( hwndMLE ) ) )
        {
            WinDestroyWindow( hwndMLE );

            hwndMLE = NULLHANDLE;

            Msg( "CreateMlePage SETPAGEWINDOWHWND RC(%X)", HWNDERR( hwndNB ) );
        }

        pfnwpDefMLE = WinSubclassWindow( hwndMLE, wpMLE );

        if( !pfnwpDefMLE )
            Msg( "CreateMlePage WinSubclassWindow RC(%X)", HWNDERR( hwndNB ) );
    }
    else
        Msg( "CreateMlePage WinCreateWindow RC(%X)", HWNDERR( hwndNB ) );

    return hwndMLE;
}

/**********************************************************************/
/*------------------------------ wpMLE -------------------------------*/
/*                                                                    */
/*  MLE SUBCLASSED WINDOW PROCEDURE                                   */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
MRESULT EXPENTRY wpMLE( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    return pfnwpDefMLE( hwnd, msg, mp1, mp2 );
}

/************************************************************************
 *                      E N D   O F   S O U R C E                       *
 ************************************************************************/
