/*********************************************************************
 *                                                                   *
 * MODULE NAME :  nbsize.h               AUTHOR:  Rick Fishman       *
 * DATE WRITTEN:  11-28-92                                           *
 *                                                                   *
 * DESCRIPTION:                                                      *
 *                                                                   *
 *  Common definitions and function prototypes for NBSIZE.EXE        *
 *                                                                   *
 * HISTORY:                                                          *
 *                                                                   *
 *  11-28-92 - Source copied from the NBBASE.EXE sample              *
 *                                                                   *
 *  Rick Fishman                                                     *
 *  Code Blazers, Inc.                                               *
 *  4113 Apricot                                                     *
 *  Irvine, CA. 92720                                                *
 *  CIS ID: 72251,750                                                *
 *                                                                   *
 *********************************************************************/

/*********************************************************************/
/*------------------- APPLICATION DEFINITIONS -----------------------*/
/*********************************************************************/

#define ID_NBWINFRAME           1
#define ID_NB                   10

#define IDD_DIALOG              1000
#define MLE_ON_DIALOG           1001
#define PB_UNDO                 1002
#define PB_HELP                 1003
#define PB_DEFAULT              1004

#define ID_CLIENTS_FRAME        2000

#define MLE_PAGE                3000

#define DEBUG_FILENAME          "nbsize.dbg"

#define NOTEBOOK_WINCLASS       "NoteBookSample"

#define PROGRAM_TITLE           "Notebook Control Sample Program"

#define UM_RESIZE               WM_USER  // Msg sent by notebook to all pages

typedef HWND FNCREATE( HWND hwndParent, HWND hwndNB, ULONG ulPageID );
typedef FNCREATE *PFNCREATE;

#define HABERR( hab )           (ERRORIDERROR( WinGetLastError( hab ) ))

#define ANCHOR( hwnd )          (WinQueryAnchorBlock( hwnd ))

#define HWNDERR( hwnd )         \
            (ERRORIDERROR( WinGetLastError( WinQueryAnchorBlock( hwnd ) ) ))

#define INSTDATA( hwnd )        (WinQueryWindowPtr( hwnd, 0 ))

#define PARENT( hwnd )          (WinQueryWindow( hwnd, QW_PARENT ))

#define GRANDPARENT( hwnd )   (WinQueryWindow( WinQueryWindow(hwnd, QW_PARENT)))

/**********************************************************************/
/*---------------------------- STRUCTURES ----------------------------*/
/**********************************************************************/

typedef struct _NBPAGE              // VARIABLES USED FOR A NOTEBOOK PAGE
{
    PFNCREATE pfncreate;            // 'CreateWindow' function pointer
    PSZ      szStatusLineText;      // Text to go on status line
    PSZ      szTabText;             // Text to go on major tab
    ULONG    idFocus;               // ID of the control to get the focus first
    BOOL     fParent;               // Is this a Parent page with minor pages
    USHORT   usTabType;             // BKA_MAJOR or BKA_MINOR
    USHORT   fsPageStyles;          // BKA_ styles for the page

} NBPAGE, *PNBPAGE;

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

VOID Msg( PSZ szFormat, ... );

FNCREATE CreateDialogPage,
         CreateClientPage,
         CreateMlePage;

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

#ifdef GLOBALS_DEFINED
#  define DATADEF
#else
#  define DATADEF extern
#endif

/****************************************************************************
 *                        E N D   O F   S O U R C E                         *
 ****************************************************************************/
