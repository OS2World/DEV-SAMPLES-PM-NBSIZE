/*********************************************************************
 *                                                                   *
 * MODULE NAME :  dialog.c               AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  11-28-92                                           *
 *                                                                   *
 * MODULE DESCRIPTION:                                               *
 *                                                                   *
 *  Module that loads a dialog and associates it with the Dialog     *
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

#define  INCL_WINDIALOGS
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

INT idButton[] = { PB_UNDO, PB_DEFAULT, PB_HELP };

#define BUTTON_COUNT    (sizeof( idButton ) / sizeof( INT ))

#define CONTROL_COUNT   BUTTON_COUNT + 1    // Add the MLE

#define BUTTONS_X          12  // Left origin of buttons on dlg   (dialog units)
#define BUTTONS_Y          5   // Botton origin of buttons on dlg (dialog units)
#define BUTTON_MAX_CX      65  // Maximum width of button         (dialog units)
#define BUTTON_MAX_CY      15  // Maximum heigth of button        (dialog units)
#define BUTTON_MIN_SPACING 3   // Minimum spacing betw buttons    (dialog units)

#define MLE_TOP_CUSHION    5   // Space betw top of dlg and MLE   (dialog units)
#define MLE_MIN_CY         20  // Minimum height of the MLE       (dialog units)
#define MLE_X              6   // x origin of MLE w/in dialog box (dialog units)
#define MLE_Y_OVER_BUTTONS 5   // y origin of MLE above buttons   (dialog units)

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

static VOID wmSize         ( HWND hwndDlg, ULONG cxNew, ULONG cyNew );
static INT  CalcButtonParms( ULONG cxDlg, INT iButtonCount, PINT pcxButton,
                             PINT pcyButton );
static VOID CalcMLESwp     ( HWND hwndDlg, INT cxDlg, INT cyDlg, PSWP pswp,
                             PINT pcyButton );

static FNWP wpDlg;

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

static POINTL ptlBtns;              // x,y origin of buttons
static SIZEL  sizlMaxBtn;           // Maximum cx,cy of button
static INT    cxMinBtnSpacing,      // Minimum spacing between buttons
              cyMinMle,             // Minimum height of an MLE
              cyMleTopCushion,      // Space betw top of MLE and top of dlgbox
              xMle,                 // x origin of MLE within dialog box
              yMleAboveButtons;     // y origin of MLE from top of buttons

/**********************************************************************/
/*------------------------ CreateDialogPage --------------------------*/
/*                                                                    */
/*  LOAD DIALOG AND ASSOCIATE IT WITH A NOTEBOOK PAGE                 */
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
HWND CreateDialogPage( HWND hwndParent, HWND hwndNB, ULONG ulPageID )
{
    HWND hwndDlg = WinLoadDlg( hwndParent, hwndParent, wpDlg, 0, IDD_DIALOG,
                               NULL );

    if( hwndDlg )
    {
        if( WinSendMsg( hwndNB, BKM_SETPAGEWINDOWHWND,
                        MPFROMLONG( ulPageID ), MPFROMLONG( hwndDlg ) ) )
        {
            POINTL aptl[ 5 ];

            aptl[ 0 ].x = BUTTONS_X;
            aptl[ 0 ].y = BUTTONS_Y;
            aptl[ 1 ].x = BUTTON_MAX_CX;
            aptl[ 1 ].y = BUTTON_MAX_CY;
            aptl[ 2 ].x = BUTTON_MIN_SPACING;
            aptl[ 2 ].y = MLE_MIN_CY;
            aptl[ 3 ].x = MLE_X;
            aptl[ 3 ].y = MLE_Y_OVER_BUTTONS;
            aptl[ 4 ].x = 0;
            aptl[ 4 ].y = MLE_TOP_CUSHION;

            if( WinMapDlgPoints( HWND_DESKTOP, aptl, 4, TRUE ) )
            {
                ptlBtns  = aptl[ 0 ];

                sizlMaxBtn.cx = aptl[ 1 ].x;
                sizlMaxBtn.cy = aptl[ 1 ].y;

                cxMinBtnSpacing = aptl[ 2 ].x;

                cyMinMle = aptl[ 2 ].y;

                xMle = aptl[ 3 ].x;
                yMleAboveButtons = aptl[ 3 ].y;

                cyMleTopCushion = aptl[ 4 ].y;
            }
            else
                Msg( "CreateDialogPage WinMagDlgPoints RC(%X)",
                     HWNDERR( hwndNB ) );
        }
        else
        {
            WinDestroyWindow( hwndDlg );

            hwndDlg = NULLHANDLE;

            Msg( "CreateDialogPage SETPAGEWINDOWHWND RC(%X)",
                 HWNDERR( hwndNB ) );
        }
    }
    else
        Msg( "CreateDialogPage WinLoadDlg RC(%X)", HWNDERR( hwndNB ) );

    return hwndDlg;
}

/**********************************************************************/
/*------------------------------ wpDlg -------------------------------*/
/*                                                                    */
/*  DIALOG BOX WINDOW PROCEDURE                                       */
/*                                                                    */
/*  INPUT: window handle, message id, message parameter 1 and 2.      */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: return code                                               */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static MRESULT EXPENTRY wpDlg( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    switch( msg )
    {
        // Calling WinDefWindowProc for this message will generate a WM_SIZE
        // message. We'll use the WM_ADJUSTWINDOWPOS message though...
        // (Normally dialogs don't get WM_SIZE messages)

        case WM_WINDOWPOSCHANGED:

            WinDefWindowProc( hwnd, msg, mp1, mp2 );

            break;


        case WM_SIZE:

            break;


        case WM_ADJUSTWINDOWPOS:
        {
            PSWP pswp = (PSWP) mp1;

            if( (pswp->fl & SWP_SIZE) && pswp->cx && pswp->cy )
                wmSize( hwnd, pswp->cx, pswp->cy );

            break;
        }

        case WM_COMMAND:

            return 0;
    }

    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/**********************************************************************/
/*------------------------------ wmSize ------------------------------*/
/*                                                                    */
/*  PROCESS A SIZE FOR THE DIALOG BOX (WM_ADJUSTWINDOWPOS).           */
/*                                                                    */
/*  INPUT: dialog box window handle,                                  */
/*         new window width,                                          */
/*         new window height                                          */
/*                                                                    */
/*  1.                                                                */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID wmSize( HWND hwndDlg, ULONG cxNew, ULONG cyNew )
{
    SWP swpDlg;

    if( WinQueryWindowPos( hwndDlg, &swpDlg ) )
    {
        if( swpDlg.cx != cxNew || swpDlg.cy != cyNew )
        {
            SWP aswp[ CONTROL_COUNT ];
            INT i, cxButton, cyButton, xButton = ptlBtns.x;
            INT iButtonSpacing = CalcButtonParms( cxNew, BUTTON_COUNT,
                                                  &cxButton, &cyButton );

            CalcMLESwp( hwndDlg, cxNew, cyNew, &aswp[ 0 ], &cyButton );

            for( i = 1; i <= BUTTON_COUNT; i++ )
            {
                aswp[ i ].hwnd = WinWindowFromID( hwndDlg, idButton[ i - 1 ] );
                aswp[ i ].hwndInsertBehind = HWND_BOTTOM;
                aswp[ i ].x                = xButton;
                aswp[ i ].y                = ptlBtns.y;
                aswp[ i ].cx               = cxButton;
                aswp[ i ].cy               = cyButton;
                aswp[ i ].fl               = SWP_SIZE | SWP_MOVE | SWP_SHOW;

                xButton += (cxButton + iButtonSpacing);
            }

            if( !WinSetMultWindowPos( ANCHOR( hwndDlg ), aswp, CONTROL_COUNT ) )
                Msg( "DialogPage wmSize WinSetMultWindowPos RC(%X)",
                     HWNDERR( hwndDlg ) );
        }
    }
    else
        Msg( "DialogPage wmSize WinQueryWindowPos RC(%X)", HWNDERR( hwndDlg ) );

    return;
}

/**********************************************************************/
/*-------------------------- CalcButtonParms -------------------------*/
/*                                                                    */
/*  CALCULATE SPACING BETWEEN BUTTONS AND BUTTON HEIGHT.              */
/*                                                                    */
/*  INPUT: width of the dialog box,                                   */
/*         number of buttons,                                         */
/*         pointer to variable to hold width of button,               */
/*         pointer to variable to hold height of button               */
/*                                                                    */
/*  1 -                                                               */
/*                                                                    */
/*  OUTPUT: button spacing in pixels                                  */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static INT CalcButtonParms( ULONG cxDlg, INT iButtonCount, PINT pcxButton,
                            PINT pcyButton )
{
    INT iSpacing, iSpaceLeft = cxDlg - (ptlBtns.x * 2);

    *pcxButton = sizlMaxBtn.cx;
    *pcyButton = sizlMaxBtn.cy;

    iSpaceLeft -= (iButtonCount * sizlMaxBtn.cx);

    iSpacing = iSpaceLeft / (iButtonCount - 1);

    if( iSpacing < cxMinBtnSpacing )
    {
        iSpacing = cxMinBtnSpacing;

        *pcxButton = ((cxDlg - (ptlBtns.x * 2)) -
                      (iSpacing * (iButtonCount - 1))) / iButtonCount;
    }

    return iSpacing;
}

/**********************************************************************/
/*---------------------------- CalcMLESwp ----------------------------*/
/*                                                                    */
/*  FILL IN THE SWP STRUCTURE FOR THE MLE.                            */
/*                                                                    */
/*  INPUT: dialog box window handle,                                  */
/*         dialog box width,                                          */
/*         dialog box height,                                         */
/*         pointer to SWP structure for MLE,                          */
/*         pointer to variable holding height of buttons              */
/*                                                                    */
/*  1 -                                                               */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
static VOID CalcMLESwp( HWND hwndDlg, INT cxDlg, INT cyDlg, PSWP pswp,
                        PINT pcyButton )
{
    pswp->hwnd             = WinWindowFromID( hwndDlg, MLE_ON_DIALOG );
    pswp->hwndInsertBehind = HWND_BOTTOM;
    pswp->x                = xMle;
    pswp->cx               = cxDlg - (xMle * 2);
    pswp->fl               = SWP_SIZE | SWP_MOVE | SWP_SHOW;
    pswp->y                = yMleAboveButtons + ptlBtns.y + *pcyButton;
    pswp->cy               = (cyDlg - pswp->y) - cyMleTopCushion;

    if( pswp->cy < cyMinMle )
    {
        *pcyButton -= (cyMinMle - pswp->cy);

        pswp->y -= (cyMinMle - pswp->cy);

        pswp->cy = cyMinMle;
    }

    return;
}

/************************************************************************
 *                      E N D   O F   S O U R C E                       *
 ************************************************************************/
