#ifndef RENDERTYPEWIN
#error Only for Windows
#endif

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0600

#include "compat.h"
#include "winlayer.h"
#include "build.h"
#include "startwin.h"

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shellapi.h>
#include <stdio.h>

#include "gameres.h"
#include "version.h"

#define TAB_CONFIG 0
#define TAB_GAME 1
#define TAB_MESSAGES 2

static struct soundQuality_t {
    int frequency;
    int samplesize;
    int channels;
} soundQualities[] = {
    { 44100, 16, 2 },
    { 22050, 16, 2 },
    { 11025, 16, 2 },
    { 0, 0, 0 },    // May be overwritten by custom sound settings.
    { 0, 0, 0 },
};

static HWND startupdlg = NULL;
static HWND pages[3] = { NULL, NULL, NULL };
static int mode = TAB_CONFIG;
static struct startwin_settings *settings;
static BOOL quiteventonclose = FALSE;
static int retval = -1;

static void populate_video_modes(BOOL firstTime)
{
    int i, j, mode3d = -1;
    int xdim = 0, ydim = 0, bpp = 0, fullscreen = 0;
    TCHAR modestr[64];
    int cd[] = { 32, 24, 16, 15, 8, 0 };
    HWND hwnd;

    hwnd = GetDlgItem(pages[TAB_CONFIG], IDC_VMODE3D);
    if (firstTime) {
        getvalidmodes();
        xdim = settings->xdim3d;
        ydim = settings->ydim3d;
        bpp  = settings->bpp3d;
        fullscreen = settings->fullscreen;
    } else {
        fullscreen = IsDlgButtonChecked(pages[TAB_CONFIG], IDC_FULLSCREEN) == BST_CHECKED;
        i = ComboBox_GetCurSel(hwnd);
        if (i != CB_ERR) i = ComboBox_GetItemData(hwnd, i);
        if (i != CB_ERR) {
            xdim = validmode[i].xdim;
            ydim = validmode[i].ydim;
            bpp  = validmode[i].bpp;
        }
    }

    // Find an ideal match.
    mode3d = checkvideomode(&xdim, &ydim, bpp, fullscreen, 1);
    if (mode3d < 0) {
        for (i=0; cd[i]; ) { if (cd[i] >= bpp) i++; else break; }
        for ( ; cd[i]; i++) {
            mode3d = checkvideomode(&xdim, &ydim, cd[i], fullscreen, 1);
            if (mode3d < 0) continue;
            break;
        }
    }

    // Repopulate the list.
    ComboBox_ResetContent(hwnd);
    for (i=0; i<validmodecnt; i++) {
        if (validmode[i].fs != fullscreen) continue;

        StringCbPrintf(modestr, sizeof(modestr), TEXT("%d x %d %d-bpp"),
            validmode[i].xdim, validmode[i].ydim, validmode[i].bpp);
        j = ComboBox_AddString(hwnd, modestr);
        ComboBox_SetItemData(hwnd, j, i);
        if (i == mode3d) {
            ComboBox_SetCurSel(hwnd, j);
        }
    }
}

static void populate_sound_quality(BOOL firstTime)
{
    int i, j, curidx = -1;
    TCHAR modestr[64];
    HWND hwnd;

    if (firstTime) {
        for (i = 0; soundQualities[i].frequency > 0; i++) {
            if (soundQualities[i].frequency == settings->samplerate &&
                soundQualities[i].samplesize == settings->bitspersample &&
                soundQualities[i].channels == settings->channels) {
                curidx = i;
                break;
            }
        }
        if (curidx < 0) {
            soundQualities[i].frequency = settings->samplerate;
            soundQualities[i].samplesize = settings->bitspersample;
            soundQualities[i].channels = settings->channels;
        }
    }

    hwnd = GetDlgItem(pages[TAB_CONFIG], IDC_SOUNDQUALITY);
    ComboBox_ResetContent(hwnd);
    for (i = 0; soundQualities[i].frequency > 0; i++) {
        StringCbPrintf(modestr, sizeof(modestr), TEXT("%d kHz, %d-bit, %s"),
            soundQualities[i].frequency / 1000,
            soundQualities[i].samplesize,
            soundQualities[i].channels == 1 ? "Mono" : "Stereo");
        j = ComboBox_AddString(hwnd, modestr);
        ComboBox_SetItemData(hwnd, j, i);
        if (i == curidx) {
            ComboBox_SetCurSel(hwnd, j);
        }
    }
}

static void populate_game_list(BOOL firstTime)
{
    struct grpfile *fg;
    int i;
    LVITEM lvi;
    HWND hwnd;

    ZeroMemory(&lvi, sizeof(lvi));

    if (firstTime) {
        hwnd = GetDlgItem(pages[TAB_GAME], IDC_GAMELIST);

        for (fg = foundgrps, i = 0; fg; fg = fg->next, i++) {
            if (!fg->ref) continue;

            lvi.mask = LVIF_PARAM | LVIF_TEXT;
            lvi.iItem = i;
            lvi.iSubItem = 0;
            lvi.pszText = (LPTSTR)fg->ref->name;
            lvi.lParam = (LPARAM)fg;
            ListView_InsertItem(hwnd, &lvi);

            ListView_SetItemText(hwnd, i, 1, (LPTSTR)fg->name);
        
            if (fg == settings->selectedgrp) {
                ListView_SetItemState(hwnd, i, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
}

static void set_settings(struct startwin_settings *thesettings)
{
    settings = thesettings;
}

static void set_page(int n)
{
    HWND tab = GetDlgItem(startupdlg, IDC_STARTWIN_TABCTL);
    int cur = (int)SendMessage(tab, TCM_GETCURSEL,0,0);

    ShowWindow(pages[cur], SW_HIDE);
    SendMessage(tab, TCM_SETCURSEL, n, 0);
    ShowWindow(pages[n], SW_SHOW);
    mode = n;

    SendMessage(startupdlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(startupdlg, IDC_STARTWIN_TABCTL), TRUE);
}

static void setup_config_mode(void)
{
    set_page(TAB_CONFIG);

    CheckDlgButton(startupdlg, IDC_ALWAYSSHOW, (settings->forcesetup ? BST_CHECKED : BST_UNCHECKED));
    EnableWindow(GetDlgItem(startupdlg, IDC_ALWAYSSHOW), TRUE);

    CheckDlgButton(pages[TAB_CONFIG], IDC_FULLSCREEN, (settings->fullscreen ? BST_CHECKED : BST_UNCHECKED));
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_FULLSCREEN), TRUE);

    populate_video_modes(TRUE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_VMODE3D), TRUE);

    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_USEMOUSE), TRUE);
    CheckDlgButton(pages[TAB_CONFIG], IDC_USEMOUSE, (settings->usemouse ? BST_CHECKED : BST_UNCHECKED));
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_USEJOYSTICK), TRUE);
    CheckDlgButton(pages[TAB_CONFIG], IDC_USEJOYSTICK, (settings->usejoy ? BST_CHECKED : BST_UNCHECKED));

    populate_sound_quality(TRUE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_SOUNDQUALITY), TRUE);

    if (!settings->netoverride) {
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_SINGLEPLAYER), TRUE);
        CheckRadioButton(pages[TAB_CONFIG], IDC_SINGLEPLAYER, IDC_HOSTMULTIPLAYER, IDC_SINGLEPLAYER);

        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_JOINMULTIPLAYER), TRUE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTFIELD), FALSE);

        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTMULTIPLAYER), TRUE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERS), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERSUD), TRUE);
        SetDlgItemInt(pages[TAB_CONFIG], IDC_NUMPLAYERS, 2, TRUE);
        SendDlgItemMessage(pages[TAB_CONFIG], IDC_NUMPLAYERSUD, UDM_SETPOS, 0, 2);
        SendDlgItemMessage(pages[TAB_CONFIG], IDC_NUMPLAYERSUD, UDM_SETRANGE, 0, MAKELPARAM(MAXPLAYERS, 2));
    } else {
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_SINGLEPLAYER), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_JOINMULTIPLAYER), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTFIELD), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTMULTIPLAYER), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERS), FALSE);
        EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERSUD), FALSE);
    }

    populate_game_list(TRUE);
    EnableWindow(GetDlgItem(pages[TAB_GAME], IDC_GAMELIST), TRUE);

    EnableWindow(GetDlgItem(startupdlg, IDCANCEL), TRUE);
    EnableWindow(GetDlgItem(startupdlg, IDOK), TRUE);
}

static void setup_messages_mode(BOOL allowcancel)
{
    set_page(TAB_MESSAGES);

    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_FULLSCREEN), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_VMODE3D), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_SINGLEPLAYER), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_JOINMULTIPLAYER), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTFIELD), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTMULTIPLAYER), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERS), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERSUD), FALSE);

    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_USEMOUSE), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_USEJOYSTICK), FALSE);
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_SOUNDQUALITY), FALSE);

    EnableWindow(GetDlgItem(pages[TAB_GAME], IDC_GAMELIST), FALSE);

    EnableWindow(GetDlgItem(startupdlg, IDC_ALWAYSSHOW), FALSE);

    EnableWindow(GetDlgItem(startupdlg, IDCANCEL), allowcancel);
    EnableWindow(GetDlgItem(startupdlg, IDOK), FALSE);
}

static void fullscreen_clicked(void)
{
    populate_video_modes(FALSE);
}

static void multiplayerradio_clicked(int sender)
{
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_HOSTFIELD), (sender == IDC_JOINMULTIPLAYER));
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERS), (sender == IDC_HOSTMULTIPLAYER));
    EnableWindow(GetDlgItem(pages[TAB_CONFIG], IDC_NUMPLAYERSUD), (sender == IDC_HOSTMULTIPLAYER));

    CheckRadioButton(pages[TAB_CONFIG], IDC_SINGLEPLAYER, IDC_HOSTMULTIPLAYER, sender);
}

static void cancelbutton_clicked(void)
{
    retval = STARTWIN_CANCEL;
    quitevent = quitevent || quiteventonclose;
}

static void startbutton_clicked(void)
{
    int i;
    HWND hwnd;
    LVITEM lvi;

    hwnd = GetDlgItem(pages[TAB_CONFIG], IDC_VMODE3D);
    i = ComboBox_GetCurSel(hwnd);
    if (i != CB_ERR) i = ComboBox_GetItemData(hwnd, i);
    if (i != CB_ERR) {
        settings->xdim3d = validmode[i].xdim;
        settings->ydim3d = validmode[i].ydim;
        settings->bpp3d  = validmode[i].bpp;
        settings->fullscreen = validmode[i].fs;
    }

    settings->usemouse = IsDlgButtonChecked(pages[TAB_CONFIG], IDC_USEMOUSE) == BST_CHECKED;
    settings->usejoy = IsDlgButtonChecked(pages[TAB_CONFIG], IDC_USEJOYSTICK) == BST_CHECKED;

    hwnd = GetDlgItem(pages[TAB_CONFIG], IDC_SOUNDQUALITY);
    i = ComboBox_GetCurSel(hwnd);
    if (i != CB_ERR) i = ComboBox_GetItemData(hwnd, i);
    if (i != CB_ERR) {
        settings->samplerate = soundQualities[i].frequency;
        settings->bitspersample = soundQualities[i].samplesize;
        settings->channels = soundQualities[i].channels;
    }

    settings->numplayers = 0;
    settings->joinhost = 0;
    if (IsDlgButtonChecked(pages[TAB_CONFIG], IDC_SINGLEPLAYER) == BST_CHECKED) {
        settings->numplayers = 1;
    } else if (IsDlgButtonChecked(pages[TAB_CONFIG], IDC_JOINMULTIPLAYER) == BST_CHECKED) {
        int joinhostlen, wcharlen;
        WCHAR *wcharstr;

        settings->numplayers = 2;
        
        hwnd = GetDlgItem(pages[TAB_CONFIG], IDC_HOSTFIELD);
        wcharlen = GetWindowTextLengthW(hwnd) + 1;
        wcharstr = (WCHAR *)malloc(wcharlen * sizeof(WCHAR));
        GetWindowTextW(hwnd, wcharstr, wcharlen);
        
        joinhostlen = WideCharToMultiByte(CP_UTF8, 0, wcharstr, -1, NULL, 0, NULL, NULL);
        settings->joinhost = (char *)malloc(joinhostlen + 1);
        WideCharToMultiByte(CP_UTF8, 0, wcharstr, -1, settings->joinhost, joinhostlen, NULL, NULL);

        free(wcharstr);
    } else if (IsDlgButtonChecked(pages[TAB_CONFIG], IDC_HOSTMULTIPLAYER) == BST_CHECKED) {
        settings->numplayers = (int)GetDlgItemInt(pages[TAB_CONFIG], IDC_NUMPLAYERS, NULL, TRUE);
    }

    // Get the chosen game entry.
    hwnd = GetDlgItem(pages[TAB_GAME], IDC_GAMELIST);
    ZeroMemory(&lvi, sizeof(lvi));
    lvi.mask = LVIF_STATE | LVIF_PARAM;
    lvi.stateMask = LVIS_SELECTED;
    for (i = ListView_GetItemCount(hwnd) - 1; i >= 0; i--) {
        lvi.iItem = i;
        if (ListView_GetItem(hwnd, &lvi)) {
            if (lvi.state & LVIS_SELECTED) {
                settings->selectedgrp = (struct grpfile *)lvi.lParam;
                break;
            }
        }
    }

    settings->forcesetup = IsDlgButtonChecked(startupdlg, IDC_ALWAYSSHOW) == BST_CHECKED;
 
    retval = STARTWIN_RUN;
}

static INT_PTR CALLBACK ConfigPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_INITDIALOG: {
            EnableThemeDialogTexture(hwndDlg, ETDT_ENABLETAB);
            SendDlgItemMessage(hwndDlg, IDC_NUMPLAYERSUD, UDM_SETBUDDY, (WPARAM)GetDlgItem(hwndDlg, IDC_NUMPLAYERS), 0);
            return TRUE;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_FULLSCREEN:
                    fullscreen_clicked();
                    return TRUE;

                case IDC_SINGLEPLAYER:
                case IDC_JOINMULTIPLAYER:
                case IDC_HOSTMULTIPLAYER:
                    multiplayerradio_clicked(LOWORD(wParam));
                    return TRUE;
                default: break;
            }
            break;
        default: break;
    }
    return FALSE;
}

static INT_PTR CALLBACK GamePageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_INITDIALOG: {
            LVCOLUMN lvc;
            HWND hwnd;

            hwnd = GetDlgItem(hwndDlg, IDC_GAMELIST);
            ListView_SetExtendedListViewStyle(hwnd, LVS_EX_FULLROWSELECT);

            ZeroMemory(&lvc, sizeof(lvc));
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_LEFT;

            lvc.iSubItem = 0;
            lvc.pszText = TEXT("Game name");
            lvc.cx = 220;
            ListView_InsertColumn(hwnd, 0, &lvc);

            lvc.iSubItem = 1;
            lvc.pszText = TEXT("File name");
            lvc.cx = 150;
            ListView_InsertColumn(hwnd, 1, &lvc);

            return TRUE;
        }
        default: break;
    }
    return FALSE;
}

static INT_PTR CALLBACK MessagesPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CTLCOLORSTATIC:
            if ((HWND)lParam == GetDlgItem(hwndDlg, IDC_MESSAGES))
                return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
            break;
    }
    return FALSE;
}

static INT_PTR CALLBACK startup_dlgproc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_INITDIALOG: {
            HWND hwnd;
            RECT r, rdlg, chrome, rtab, rcancel, rstart;
            int xoffset = 0, yoffset = 0;

            {
                TCITEM tab;
                TCHAR verstr[64];

                hwnd = GetDlgItem(hwndDlg, IDC_STARTWIN_APPVERSION);
                StringCbPrintf(verstr, sizeof(verstr), TEXT("Version %s\n%s"),
                    game_version, game_date);
                SetWindowText(hwnd, verstr);

                hwnd = GetDlgItem(hwndDlg, IDC_STARTWIN_TABCTL);

                // Add tabs to the tab control
                ZeroMemory(&tab, sizeof(tab));
                tab.mask = TCIF_TEXT;
                tab.pszText = TEXT("Configuration");
                TabCtrl_InsertItem(hwnd, TAB_CONFIG, &tab);
                tab.pszText = TEXT("Game");
                TabCtrl_InsertItem(hwnd, TAB_GAME, &tab);
                tab.pszText = TEXT("Messages");
                TabCtrl_InsertItem(hwnd, TAB_MESSAGES, &tab);

                // Work out the position and size of the area inside the tab control for the pages.
                ZeroMemory(&r, sizeof(r));
                GetClientRect(hwnd, &r);
                TabCtrl_AdjustRect(hwnd, FALSE, &r);

                // Create the pages and position them in the tab control, but hide them.
                pages[TAB_CONFIG] = CreateDialog((HINSTANCE)win_gethinstance(),
                    MAKEINTRESOURCE(IDD_PAGE_CONFIG), hwnd, ConfigPageProc);
                SetWindowPos(pages[TAB_CONFIG], NULL, r.left,r.top,r.right,r.bottom, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOSIZE);

                pages[TAB_GAME] = CreateDialog((HINSTANCE)win_gethinstance(),
                    MAKEINTRESOURCE(IDD_PAGE_GAME), hwnd, GamePageProc);
                SetWindowPos(pages[TAB_GAME], NULL, r.left,r.top,r.right,r.bottom, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOSIZE);

                pages[TAB_MESSAGES] = CreateDialog((HINSTANCE)win_gethinstance(),
                    MAKEINTRESOURCE(IDD_PAGE_MESSAGES), hwnd, MessagesPageProc);
                SetWindowPos(pages[TAB_MESSAGES], NULL, r.left,r.top,r.right,r.bottom, SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOSIZE);

                SendMessage(hwndDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hwndDlg, IDOK), TRUE);
            }
            {
                // Set the font of the application title.
                HFONT hfont = CreateFont(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                    TEXT("MS Shell Dlg"));
                if (hfont) {
                    hwnd = GetDlgItem(hwndDlg, IDC_STARTWIN_APPTITLE);
                    SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, FALSE);
                }
            }
            return TRUE;
        }

        case WM_NOTIFY: {
            LPNMHDR nmhdr = (LPNMHDR)lParam;
            if (nmhdr->idFrom == IDC_STARTWIN_TABCTL) {
                int cur = (int)SendMessage(nmhdr->hwndFrom, TCM_GETCURSEL,0,0);
                switch (nmhdr->code) {
                    case TCN_SELCHANGING: {
                        if (cur < 0 || !pages[cur]) break;
                        ShowWindow(pages[cur],SW_HIDE);
                        return TRUE;
                    }
                    case TCN_SELCHANGE: {
                        if (cur < 0 || !pages[cur]) break;
                        ShowWindow(pages[cur],SW_SHOW);
                        return TRUE;
                    }
                }
            }
            if (nmhdr->idFrom == IDC_STARTWIN_APPLINK) {
                PNMLINK pNMLink = (PNMLINK)lParam;

                if (nmhdr->code != NM_CLICK && nmhdr->code != NM_RETURN) {
                    break;
                }
                if (pNMLink->item.iLink == 0) {
                    ShellExecuteW(hwndDlg, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOW);
                }
            }
            break;
        }

        case WM_CLOSE:
            cancelbutton_clicked();
            return TRUE;

        case WM_DESTROY:
            if (pages[TAB_CONFIG]) {
                DestroyWindow(pages[TAB_CONFIG]);
                pages[TAB_CONFIG] = NULL;
            }

            if (pages[TAB_GAME]) {
                DestroyWindow(pages[TAB_GAME]);
                pages[TAB_GAME] = NULL;
            }

            if (pages[TAB_MESSAGES]) {
                DestroyWindow(pages[TAB_MESSAGES]);
                pages[TAB_MESSAGES] = NULL;
            }

            // Dispose of the font used for the application title.
            {
                HWND hwnd = GetDlgItem(hwndDlg, IDC_STARTWIN_APPTITLE);
                HFONT hfont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0);
                if (hfont) {
                    DeleteObject(hfont);
                }
            }

            startupdlg = NULL;
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDCANCEL:
                    cancelbutton_clicked();
                    return TRUE;
                case IDOK: {
                    startbutton_clicked();
                    return TRUE;
                }
            }
            break;

        default: break;
    }

    return FALSE;
}


int startwin_open(void)
{
    INITCOMMONCONTROLSEX icc;

    if (startupdlg) return 1;

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_TAB_CLASSES | ICC_UPDOWN_CLASS | ICC_LISTVIEW_CLASSES | ICC_LINK_CLASS;
    InitCommonControlsEx(&icc);
    startupdlg = CreateDialog((HINSTANCE)win_gethinstance(), MAKEINTRESOURCE(IDD_STARTWIN), NULL, startup_dlgproc);
    if (!startupdlg) {
        return -1;
    }
 
    quiteventonclose = TRUE;
    setup_messages_mode(TRUE);
    return 0;
}

int startwin_close(void)
{
    if (!startupdlg) return 1;

    quiteventonclose = FALSE;
    DestroyWindow(startupdlg);
    startupdlg = NULL;

    return 0;
}

int startwin_puts(const char *buf)
{
    const char *p = NULL, *q = NULL;
    char workbuf[1024];
    static int newline = 0;
    int curlen, linesbefore, linesafter;
    HWND edctl;
    int vis;

    if (!startupdlg) return 1;
    
    edctl = GetDlgItem(pages[TAB_MESSAGES], IDC_MESSAGES);
    if (!edctl) return -1;

    vis = ((int)SendMessage(GetDlgItem(startupdlg, IDC_STARTWIN_TABCTL), TCM_GETCURSEL,0,0) == TAB_MESSAGES);
    
    if (vis) SendMessage(edctl, WM_SETREDRAW, FALSE,0);
    curlen = SendMessage(edctl, WM_GETTEXTLENGTH, 0,0);
    SendMessage(edctl, EM_SETSEL, (WPARAM)curlen, (LPARAM)curlen);
    linesbefore = SendMessage(edctl, EM_GETLINECOUNT, 0,0);
    p = buf;
    while (*p) {
        if (newline) {
            SendMessage(edctl, EM_REPLACESEL, 0, (LPARAM)"\r\n");
            newline = 0;
        }
        q = p;
        while (*q && *q != '\n') q++;
        memcpy(workbuf, p, q-p);
        if (*q == '\n') {
            if (!q[1]) {
                newline = 1;
                workbuf[q-p] = 0;
            } else {
                workbuf[q-p] = '\r';
                workbuf[q-p+1] = '\n';
                workbuf[q-p+2] = 0;
            }
            p = q+1;
        } else {
            workbuf[q-p] = 0;
            p = q;
        }
        SendMessage(edctl, EM_REPLACESEL, 0, (LPARAM)workbuf);
    }
    linesafter = SendMessage(edctl, EM_GETLINECOUNT, 0,0);
    SendMessage(edctl, EM_LINESCROLL, 0, linesafter-linesbefore);
    if (vis) SendMessage(edctl, WM_SETREDRAW, TRUE,0);
    return 0;
}

int startwin_settitle(const char *str)
{
    if (startupdlg) {
        SetWindowText(startupdlg, str);
    }
    return 0;
}

int startwin_idle(void *v)
{
    if (!startupdlg || !IsWindow(startupdlg)) return 0;
    if (IsDialogMessage(startupdlg, (MSG*)v)) return 1;
    return 0;
}

int startwin_run(struct startwin_settings *settings)
{
    MSG msg;

    if (!startupdlg) return 1;

    set_settings(settings);
    setup_config_mode();

    while (retval < 0) {
        switch (GetMessage(&msg, NULL, 0,0)) {
            case 0: retval = STARTWIN_CANCEL; break;    //WM_QUIT
            case -1: return -1;     // error
            default:
                 if (IsWindow(startupdlg) && IsDialogMessage(startupdlg, &msg)) break;
                 TranslateMessage(&msg);
                 DispatchMessage(&msg);
                 break;
        }
    }

    setup_messages_mode(settings->numplayers > 1);
    set_settings(NULL);

    return retval;
}

