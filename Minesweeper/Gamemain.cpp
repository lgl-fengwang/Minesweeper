#include <Windows.h>
#include <mmsystem.h>
#include <ctime>
#include "resource.h"
#include "Gamemain.h"
#include "Game.h"

#pragma comment(lib,"winmm.lib")

int g_WidthWindow = 800;//窗口宽度
int g_HeighWindow = 800;//窗口高度
int g_LeftWindow = 10;//窗口左上角X坐标
int g_TopWindow = 10;//窗口左上角Y坐标
int g_LeftMap = 20;//地图左上角X坐标
int g_TopMap = 20;//地图左上角Y坐标
int g_WidthBox = 33;//格子宽
int g_HeighBox = 33;//格子高
int g_Row = 10;//地图逻辑行
int g_Column = 16;//地图逻辑列
int g_Mine = 10;//地雷个数
int g_WidthPen = 2;//画笔宽度
HDC hdc;
HDC mdc;
HDC bufdc;
HINSTANCE hInst;

HBRUSH hBackBrush = CreateSolidBrush(RGB(108, 166, 205));//背景画刷
HFONT hDialogFont = CreateFont(-15, -7, 0, 0, 500, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("微软雅黑"));//对话框字体
HICON hMine;//炸弹图标

Game game;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Fail(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK Option(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
	hInst = hInstance;
	hMine = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_MINE), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	static TCHAR szClassName[] = TEXT("minesweeper");
	static TCHAR szWindowTitle[] = TEXT("扫雷");
	HWND hWnd;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MINE));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = hBackBrush;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wndclass.lpszClassName = szClassName;

	RegisterClass(&wndclass);

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, szClassName, szWindowTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 30, 30, g_WidthWindow, g_HeighWindow, NULL, NULL, hInstance, NULL);
	

	
	ShowWindow(hWnd, iCmdShow);
	//UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	game.GameInitial(hWnd);



	DWORD tNow = GetTickCount();
	DWORD tPre = GetTickCount();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			tNow = GetTickCount();
			if (tNow - tPre > 5) {
				game.DrawWindow(hWnd);
				tPre = tNow;
			}
		}
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, NULL);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			game.GetShowTime();
		}
		break;
	case WM_MOUSEMOVE:
		game.GetMousePoint(hWnd, message, wParam, lParam);
		game.SetMouseState(Move);
		break;
	case WM_LBUTTONUP:
		game.GetMousePoint(hWnd, message, wParam, lParam);
		game.SetMouseState(LBtnUp);
		break;
	case WM_RBUTTONDOWN:
		game.GetMousePoint(hWnd, message, wParam, lParam);
		game.SetMouseState(RBtnDown);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_GAME_NEW:
			game.GameDestroy();
			game.GameInitial(hWnd);
			break;
		case IDM_GAME_OPTION:
			DialogBox(hInst, (LPCSTR)IDD_DIALOG_OPTION, hWnd, (DLGPROC)Option);
			break;
		case IDM_GAME_EXIT:
			game.GameDestroy();
			PostQuitMessage(0);
			KillTimer(hWnd, 1);
			DeleteObject(hBackBrush);
			DeleteObject(hDialogFont);
			ReleaseDC(hWnd, hdc);
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(hMine);
			break;
		case IDM_HELP_ABOUT:
			DialogBox(hInst, (LPCSTR)IDD_DIALOG_ABOUT, hWnd, (DLGPROC)About);
			break;
		}
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		DeleteObject(hBackBrush);
		DeleteObject(hDialogFont);
		ReleaseDC(hWnd, hdc);
		DeleteDC(mdc);
		DeleteDC(bufdc);
		DeleteObject(hMine);
		game.GameDestroy();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	HWND hLabCopyright;
	HWND hLabEdition;
	HWND hLabMotto;
	HWND hBtn;
	switch (message) {
	case WM_INITDIALOG:
		hLabCopyright = CreateWindow(TEXT("static"), TEXT("“扫雷”游戏由李国良开发"), WS_CHILD | WS_VISIBLE | SS_LEFT, 130, 20, 180, 40, hDlg, (HMENU)1, hInst, NULL);
		SendMessage(hLabCopyright, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabEdition = CreateWindow(TEXT("static"), TEXT("版本号：1.0.0"), WS_CHILD | WS_VISIBLE | SS_LEFT, 130, 60, 180, 40, hDlg, (HMENU)2, hInst, NULL);
		SendMessage(hLabEdition, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabMotto = CreateWindow(TEXT("static"), TEXT("需尽人事     亦听天命\n\n但行善事     莫问前程"), WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 130, 100, 180, 60, hDlg, (HMENU)3, hInst, NULL);
		SendMessage(hLabMotto, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hBtn = CreateWindow(TEXT("button"), TEXT("确定"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 290, 170, 75, 30, hDlg, (HMENU)4, hInst, NULL);
		SendMessage(hBtn, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		return true;
	//case WM_CTLCOLORSTATIC:
	//	hdcStatic = HDC(wParam);
	//	SetBkMode(hdcStatic, TRANSPARENT);
	//	return (INT_PTR)GetStockObject(NULL_BRUSH);
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		//GetClientRect(hDlg, &rect);
		//FillRect(hdc, &rect, CreateSolidBrush(RGB(154, 192, 205)));
		DrawIconEx(hdc, 10, 50, hMine, 100, 100, NULL, NULL, DI_NORMAL);
		EndPaint(hDlg, &ps);
		break;
	case WM_COMMAND:
	case WM_SYSCOMMAND:
		if (LOWORD(wParam) == 4 || LOWORD(wParam) == SC_CLOSE) {
			EndDialog(hDlg, NULL);
		}
		break;
	}
	return false;
}

LRESULT CALLBACK Fail(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hLabSlogan;
	HWND hLabTime;
	HWND hLabDate;
	HWND hBtnQuit;
	HWND hBtnAgain;
	SYSTEMTIME date;
	GetSystemTime(&date);
	char currentDate[20];
	wsprintf(currentDate, TEXT("日期：%d/%d/%d"), date.wYear, date.wMonth, date.wDay);
	char slogan[50];
	if (game.GameSuccess()) {
		wsprintf(slogan, TEXT("恭喜！您赢了！"));
	}
	else {
		wsprintf(slogan, TEXT("不好意思，您输了。下次走运！"));
	}
	switch (message) {
	case WM_INITDIALOG:
		hLabSlogan = CreateWindow(TEXT("static"), TEXT(slogan), WS_CHILD | WS_VISIBLE | SS_CENTER, 90, 20, 210, 40, hDlg, (HMENU)1, hInst, NULL);
		SendMessage(hLabSlogan, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabTime = CreateWindow(TEXT("static"), TEXT(game.GetTime()), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 60, 100, 40, hDlg, (HMENU)2, hInst, NULL);
		SendMessage(hLabTime, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabDate = CreateWindow(TEXT("static"), TEXT(currentDate), WS_CHILD | WS_VISIBLE | SS_LEFT, 220, 60, 150, 40, hDlg, (HMENU)3, hInst, NULL);
		SendMessage(hLabDate, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hBtnQuit = CreateWindow(TEXT("button"), TEXT("退出"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 170, 75, 30, hDlg, (HMENU)4, hInst, NULL);
		SendMessage(hBtnQuit, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hBtnAgain = CreateWindow(TEXT("button"), TEXT("再玩一局"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 290, 170, 75, 30, hDlg, (HMENU)5, hInst, NULL);
		SendMessage(hBtnAgain, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		return true;
	case WM_COMMAND:
	case WM_SYSCOMMAND:
		if (LOWORD(wParam) == 4) {
			EndDialog(hDlg, 0);
		}
		if (LOWORD(wParam) == 5 || LOWORD(wParam) == SC_CLOSE) {
			EndDialog(hDlg, 1);
		}
		break;
	}
	return false;
}

LRESULT CALLBACK Option(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hGroupDifficulty;
	HWND hRadioPrimary;
	HWND hRadioIntermediate;
	HWND hRadioSenior;
	HWND hRadioCustom;
	HWND hLabHeigh;
	HWND hLabWidth;
	HWND hLabMine;
	HWND hEditHeigh;
	HWND hEditWidth;
	HWND hEditMine;
	HWND hBtnOk;
	HWND hBtnCancel;
	switch (message) {
	case WM_INITDIALOG:
		hGroupDifficulty = CreateWindow(TEXT("button"), TEXT("难度"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 10, 430, 300, hDlg, (HMENU)1, hInst, NULL);
		SendMessage(hGroupDifficulty, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hRadioPrimary = CreateWindow(TEXT("button"), TEXT("初级\r10个雷\r9×9平铺网格"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_MULTILINE, 20, 30, 130, 65, hGroupDifficulty, (HMENU)2, hInst, NULL);
		SendMessage(hRadioPrimary, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hRadioIntermediate = CreateWindow(TEXT("button"), TEXT("中级\r40个雷\r16×16平铺网格"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_MULTILINE, 20, 110, 130, 65, hGroupDifficulty, (HMENU)3, hInst, NULL);
		SendMessage(hRadioIntermediate, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hRadioSenior = CreateWindow(TEXT("button"), TEXT("高级\r99个雷\r16×30平铺网格"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_MULTILINE, 20, 190, 130, 65, hGroupDifficulty, (HMENU)4, hInst, NULL);
		SendMessage(hRadioSenior, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hRadioCustom = CreateWindow(TEXT("button"), TEXT("自定义"), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_MULTILINE, 220, 30, 130, 25, hGroupDifficulty, (HMENU)5, hInst, NULL);
		SendMessage(hRadioCustom, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabHeigh = CreateWindow(TEXT("static"), TEXT("高度(9-16):"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_DISABLED, 240, 60, 100, 25, hGroupDifficulty, (HMENU)6, hInst, NULL);
		SendMessage(hLabHeigh, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabWidth = CreateWindow(TEXT("static"), TEXT("宽度(9-36):"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_DISABLED, 240, 100, 100, 25, hGroupDifficulty, (HMENU)7, hInst, NULL);
		SendMessage(hLabWidth, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hLabMine = CreateWindow(TEXT("static"), TEXT("雷数(10-460):"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_DISABLED, 240, 140, 100, 25, hGroupDifficulty, (HMENU)8, hInst, NULL);
		SendMessage(hLabMine, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hEditHeigh = CreateWindow(TEXT("edit"), TEXT("9"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | WS_DISABLED, 350, 60, 70, 25, hGroupDifficulty, (HMENU)9, hInst, NULL);
		SendMessage(hEditHeigh, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hEditWidth = CreateWindow(TEXT("edit"), TEXT("9"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | WS_DISABLED, 350, 100, 70, 25, hGroupDifficulty, (HMENU)10, hInst, NULL);
		SendMessage(hEditWidth, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hEditMine = CreateWindow(TEXT("edit"), TEXT("10"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | WS_DISABLED, 350, 140, 70, 25, hGroupDifficulty, (HMENU)11, hInst, NULL);
		SendMessage(hEditMine, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hBtnOk = CreateWindow(TEXT("button"), TEXT("确定"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 250, 350, 75, 30, hDlg, (HMENU)12, hInst, NULL);
		SendMessage(hBtnOk, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		hBtnCancel = CreateWindow(TEXT("button"), TEXT("取消"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 355, 350, 75, 30, hDlg, (HMENU)13, hInst, NULL);
		SendMessage(hBtnCancel, WM_SETFONT, (WPARAM)hDialogFont, NULL);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == 12) {
			EndDialog(hDlg, 0);
		}
	}
	return false;
}


void Game::GameInitial(HWND hWnd) {
	PlaySound(MAKEINTRESOURCE(IDW_WAVE_NEW), hInst, SND_RESOURCE | SND_ASYNC);

	row = 0;
	column = 0;
	timer = 0;
	mine = g_Mine;
	mouseState = Move;
	gameStart = false;
	gameOver = false;
	gameSuccess = false;

	boxState = new BoxState*[g_Row];
	for (int i = 0; i < g_Row; ++i) {
		boxState[i] = new BoxState[g_Column];
	}

	for (int i = 0; i < g_Row; ++i) {
		for (int j = 0; j < g_Column; ++j) {
			boxState[i][j].drawBox = Unknow;
			boxState[i][j].layMine = false;
		}
	}

	RECT rect;
	SetRect(&rect, 0, 0, g_LeftMap * 2 + (g_Column * (g_WidthBox + g_WidthPen)) + g_WidthPen, g_TopMap * 2 + (g_Row * (g_HeighBox + g_WidthPen)) + g_WidthPen + 40);
	AdjustWindowRect(&rect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, true);

	g_WidthWindow = rect.right - rect.left;
	g_HeighWindow = rect.bottom - rect.top;
	SetWindowPos(hWnd, NULL, g_LeftWindow, g_TopWindow, g_WidthWindow, g_HeighWindow, SWP_NOZORDER);

	compatibleBmp = CreateCompatibleBitmap(hdc, g_WidthWindow, g_HeighWindow);




	hBlackPen = CreatePen(PS_SOLID, g_WidthPen, RGB(0, 0, 0));
	hBlueBrush = CreateSolidBrush(RGB(0, 0, 255));
	hRedBrush = CreateSolidBrush(RGB(255, 0, 0));

	hUnknow = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_UNKNOWN), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hFlag = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_FLAG), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hQuestion = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_QUESTION), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hLightUnknow = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_LIGHTUNKNOW), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hLightFlag = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_LIGHTFLAG), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hLightQuestion = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_LIGHTQUESTION), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hClickBlast = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_CLICKBLAST), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hWrong = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_WRONG), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hBlast = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_BLAST), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hFlagBlast = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_FLAGBLAST), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hQuestionBlast = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_QUESTIONBLAST), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);

	hZero = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_ZERO), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hOne = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_ONE), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hTwo = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_TWO), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hThree = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_THREE), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hFour = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_FOUR), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hFive = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_FIVE), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hSix = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_SIX), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hSeven = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_SEVEN), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);
	hEight = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BITMAP_EIGHT), IMAGE_BITMAP, g_WidthBox, g_HeighBox, LR_DEFAULTCOLOR);

	hBlock = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_BLOCK), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);


	hCountFont = CreateFont(-24, -12, 0, 0, 400, false, false, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("微软雅黑"));
	hStaticTime = CreateWindow(TEXT("static"), TEXT("0"), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE, g_LeftMap + 50, g_TopMap + g_Row * (g_HeighBox + g_WidthPen) + 15, 60, 30, hWnd, (HMENU)1, hInst, NULL);
	SendMessage(hStaticTime, WM_SETFONT, (WPARAM)hCountFont, NULL);
	hStaticMine = CreateWindow(TEXT("static"), TEXT(GetMine()), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE, g_LeftMap + g_Column * (g_WidthBox + g_WidthPen) - 105, g_TopMap + g_Row * (g_HeighBox + g_WidthPen) + 15, 60, 30, hWnd, (HMENU)1, hInst, NULL);
	SendMessage(hStaticMine, WM_SETFONT, (WPARAM)hCountFont, NULL);

	

	DrawIconEx(hdc, g_LeftMap, g_TopMap + g_Row * (g_HeighBox + g_WidthPen) + 10, hBlock, 40, 40, NULL, NULL, DI_NORMAL);
	DrawIconEx(hdc, g_LeftMap + g_Column * (g_WidthBox + g_WidthPen) - 35, g_TopMap + g_Row * (g_HeighBox + g_WidthPen) + 10, hMine, 40, 40, NULL, NULL, DI_NORMAL);


	return;

}

void Game::GetMousePoint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	point = MAKEPOINTS(lParam);
	return;
}

void Game::DrawWindow(HWND hWnd) {
	SelectObject(mdc, compatibleBmp);
	SelectObject(mdc, hBlackPen);
	Rectangle(mdc, -1, -1, g_WidthWindow, g_HeighWindow);
	for (int i = g_WidthPen / 2; i < g_Row * (g_HeighBox + g_WidthPen) + g_WidthPen; i = i + g_HeighBox + g_WidthPen) {
		MoveToEx(mdc, 0, i, NULL);
		LineTo(mdc, g_Column * (g_WidthBox + g_WidthPen) + (g_WidthPen / 2) + 1, i);
	}
	for (int i = g_WidthPen / 2; i < g_Column * (g_WidthBox + g_WidthPen) + g_WidthPen; i = i + g_WidthBox + g_WidthPen) {
		MoveToEx(mdc, i, 0, NULL);
		LineTo(mdc, i, g_Row * (g_HeighBox + g_WidthPen) + (g_WidthPen / 2) + 1);
	}
	if (gameOver == false && gameSuccess == false) {

		for (int i = 0; i < g_Row; ++i) {
			for (int j = 0; j < g_Column; ++j) {
				switch (boxState[i][j].drawBox) {
				case Unknow:
					SelectObject(bufdc, hUnknow);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Flag:
					SelectObject(bufdc, hFlag);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Question:
					SelectObject(bufdc, hQuestion);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Zero:
					SelectObject(bufdc, hZero);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case One:
					SelectObject(bufdc, hOne);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Two:
					SelectObject(bufdc, hTwo);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Three:
					SelectObject(bufdc, hThree);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Four:
					SelectObject(bufdc, hFour);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Five:
					SelectObject(bufdc, hFive);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Six:
					SelectObject(bufdc, hSix);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Seven:
					SelectObject(bufdc, hSeven);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Eight:
					SelectObject(bufdc, hEight);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Blast:
					SelectObject(bufdc, hBlast);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case ClickBlast:
					SelectObject(bufdc, hClickBlast);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case QuestionBlast:
					SelectObject(bufdc, hQuestionBlast);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case FlagBlast:
					SelectObject(bufdc, hFlagBlast);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Wrong:
					SelectObject(bufdc, hWrong);
					BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				}
			}
		}
		GameSuccess();
		switch (mouseState) {
		case Move:
			if (Conversion()) {
				switch (boxState[row][column].drawBox) {
				case Unknow:
					SelectObject(bufdc, hLightUnknow);
					BitBlt(mdc, g_WidthPen + (column * (g_WidthBox + g_WidthPen)), g_WidthPen + (row * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Flag:
					SelectObject(bufdc, hLightFlag);
					BitBlt(mdc, g_WidthPen + (column * (g_WidthBox + g_WidthPen)), g_WidthPen + (row * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				case Question:
					SelectObject(bufdc, hLightQuestion);
					BitBlt(mdc, g_WidthPen + (column * (g_WidthBox + g_WidthPen)), g_WidthPen + (row * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
			break;
		case LBtnUp:
			if (Conversion()) {
				if (gameStart == false) {
					LayMines();
					PlaySound(MAKEINTRESOURCE(IDW_WAVE_START), hInst, SND_RESOURCE | SND_ASYNC);
					gameStart = true;
				}
				if (boxState[row][column].layMine == true) {
					boxState[row][column].drawBox = ClickBlast;
					gameOver = true;
					//return;
				}
				SetDrawBox(row, column);
				SetMouseState(Move);

			}

			break;
		case RBtnDown:
			if (Conversion()) {
				switch (boxState[row][column].drawBox) {
				case Unknow:
					SetRemoveMine();
					boxState[row][column].drawBox = Flag;
					break;
				case Flag:
					SetAddMine();
					boxState[row][column].drawBox = Question;
					break;
				case Question:
					boxState[row][column].drawBox = Unknow;
					break;
				default:
					break;
				}
				SetWindowText(hStaticMine, TEXT(GetMine()));
				SetMouseState(Move);
			}

			break;
		}

		BitBlt(hdc, g_LeftMap, g_TopMap, g_WidthBox * g_Column + g_WidthPen * (g_Column + 1), g_HeighBox * g_Row + g_WidthPen * (g_Row + 1), mdc, 0, 0, SRCCOPY);
	}
	else {
		if (gameOver) {
			SetDrawBox();
			PlaySound(MAKEINTRESOURCE(IDW_WAVE_BOOM), hInst, SND_RESOURCE | SND_ASYNC);
			for (int i = 0; i < g_Row; ++i) {
				for (int j = 0; j < g_Column; ++j) {
					switch (boxState[i][j].drawBox) {
					case Unknow:
						SelectObject(bufdc, hUnknow);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Flag:
						SelectObject(bufdc, hFlag);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Question:
						SelectObject(bufdc, hQuestion);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Zero:
						SelectObject(bufdc, hZero);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case One:
						SelectObject(bufdc, hOne);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Two:
						SelectObject(bufdc, hTwo);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Three:
						SelectObject(bufdc, hThree);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Four:
						SelectObject(bufdc, hFour);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Five:
						SelectObject(bufdc, hFive);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Six:
						SelectObject(bufdc, hSix);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Seven:
						SelectObject(bufdc, hSeven);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Eight:
						SelectObject(bufdc, hEight);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Blast:
						SelectObject(bufdc, hBlast);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case ClickBlast:
						SelectObject(bufdc, hClickBlast);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case QuestionBlast:
						SelectObject(bufdc, hQuestionBlast);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case FlagBlast:
						SelectObject(bufdc, hFlagBlast);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					case Wrong:
						SelectObject(bufdc, hWrong);
						BitBlt(mdc, g_WidthPen + (j * (g_WidthBox + g_WidthPen)), g_WidthPen + (i * (g_HeighBox + g_WidthPen)), g_WidthBox, g_HeighBox, bufdc, 0, 0, SRCCOPY);
						break;
					}
				}
			}
			BitBlt(hdc, g_LeftMap, g_TopMap, g_WidthBox * g_Column + g_WidthPen * (g_Column + 1), g_HeighBox * g_Row + g_WidthPen * (g_Row + 1), mdc, 0, 0, SRCCOPY);
		}

		if (gameSuccess) {
			PlaySound(MAKEINTRESOURCE(IDW_WAVE_SUCCESS), hInst, SND_RESOURCE | SND_ASYNC);
		}

		int value = DialogBox(hInst, (LPCSTR)IDD_DIALOG_ABOUT, hWnd, (DLGPROC)Fail);
		if (value == 0) {
			game.GameDestroy();
			PostQuitMessage(0);
			KillTimer(hWnd, 1);
			DeleteObject(hBackBrush);
			DeleteObject(hDialogFont);
			ReleaseDC(hWnd, hdc);
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(hMine);
		}
		if (value == 1) {
			game.GameInitial(hWnd);
		}
	}

}

bool Game::Conversion() {
	row = -1;
	column = -1;
	if (point.y - g_TopMap - g_WidthPen + (g_WidthPen / 2) > 0 && (point.y - g_TopMap - g_WidthPen + (g_WidthPen / 2)) % (g_HeighBox + g_WidthPen) <= g_HeighBox) {
		row = (point.y - g_TopMap - g_WidthPen + (g_WidthPen / 2)) / (g_HeighBox + g_WidthPen);
	}
	if (point.x - g_LeftMap - g_WidthPen + (g_WidthPen / 2) > 0 && (point.x - g_LeftMap - g_WidthPen + (g_WidthPen / 2)) % (g_WidthBox + g_WidthPen) <= g_WidthBox) {
		column = (point.x - g_LeftMap - g_WidthPen + (g_WidthPen / 2)) / (g_WidthBox + g_WidthPen);
	}
	if (row >= 0 && row < g_Row && column >= 0 && column < g_Column) {
		return true;
	}
	else {
		return false;
	}
}

void Game::SetMouseState(MouseState state) {
	mouseState = state;
	return;
}

void Game::LayMines() {
	srand((unsigned)time(NULL));
	int i, j, index = 0;
	while (index < g_Mine) {
		i = rand() % g_Row;
		j = rand() % g_Column;
		if (boxState[i][j].layMine == true ||( i == row && j == column) || (i == row - 1 && j == column) || (i == row && j == column - 1) || (i == row - 1 && j == column - 1) || (i == row  + 1 && j == column) || (i == row && j == column + 1) || (i == row + 1 && j == column + 1) || (i == row - 1 && j == column + 1) || (i == row + 1 && j == column - 1)) {
			continue;
		}
		boxState[i][j].layMine = true;
		++index;
	}
}

void Game::SetDrawBox(int row, int column) {
	if (boxState[row][column].drawBox == Unknow || boxState[row][column].drawBox == Question) {
		int index = 0;
		if (row > 0) {
			if (boxState[row - 1][column].layMine) {
				++index;
			}
			if (column > 0 && boxState[row - 1][column - 1].layMine) {
				++index;
			}
			if (column < (g_Column - 1) && boxState[row - 1][column + 1].layMine) {
				++index;
			}
		}
		if (row < (g_Row - 1)) {
			if (boxState[row + 1][column].layMine) {
				++index;
			}
			if (column < (g_Column - 1) && boxState[row + 1][column + 1].layMine) {
				++index;
			}
			if (column > 0 && boxState[row + 1][column - 1].layMine) {
				++index;
			}
		}
		if (column > 0 && boxState[row][column - 1].layMine) {
			++index;
		}

		if (column < (g_Column - 1) && boxState[row][column + 1].layMine) {
			++index;
		}
		switch (index) {
		case 0:
			boxState[row][column].drawBox = Zero;
			if (row > 0) {
				SetDrawBox(row - 1, column);
				if (column > 0) {
					SetDrawBox(row - 1, column - 1);
				}
				if (column < (g_Column - 1)) {
					SetDrawBox(row - 1, column + 1);
				}
			}
			if (row < (g_Row - 1)) {
				SetDrawBox(row + 1, column);
				if (column < (g_Column - 1)) {
					SetDrawBox(row + 1, column + 1);
				}
				if (column > 0) {
					SetDrawBox(row + 1, column - 1);
				}
			}
			if (column > 0) {
				SetDrawBox(row, column - 1);
			}

			if (column < (g_Column - 1)) {
				SetDrawBox(row, column + 1);
			}
			break;
		case 1:
			boxState[row][column].drawBox = One;
			break;
		case 2:
			boxState[row][column].drawBox = Two;
			break;
		case 3:
			boxState[row][column].drawBox = Three;
			break;
		case 4:
			boxState[row][column].drawBox = Four;
			break;
		case 5:
			boxState[row][column].drawBox = Five;
			break;
		case 6:
			boxState[row][column].drawBox = Six;
			break;
		case 7:
			boxState[row][column].drawBox = Seven;
			break;
		case 8:
			boxState[row][column].drawBox = Eight;
			break;
		}
	}
}

void Game::SetDrawBox() {
	for (int i = 0; i < g_Row; ++i) {
		for (int j = 0; j < g_Column; ++j) {
			if (boxState[i][j].drawBox == Unknow && boxState[i][j].layMine == true) {
				boxState[i][j].drawBox = Blast;
			}
			if (boxState[i][j].drawBox == Question && boxState[i][j].layMine == true) {
				boxState[i][j].drawBox = QuestionBlast;
			}
			if (boxState[i][j].drawBox == Flag) {
				if (boxState[i][j].layMine == true) {
					boxState[i][j].drawBox = FlagBlast;
				}
				else {
					boxState[i][j].drawBox = Wrong;
				}
			}
		}
	}
}

void Game::GetShowTime() {
	if (gameStart && !gameOver && !gameSuccess) {
		++timer;
		if (timer < 10000) {
			char strTime[5];
			wsprintf(strTime, TEXT("%d"), timer);
			SetWindowText(hStaticTime, TEXT(strTime));
		}
		else {
			SetWindowText(hStaticTime, TEXT("9999"));
		}
	}
}

void Game::SetAddMine() {
	++mine;
}

void Game::SetRemoveMine() {
	--mine;
}

char* Game::GetMine() {
	static char strMine[5];
	wsprintf(strMine, TEXT("%d"), mine);
	return strMine;
}

char* Game::GetTime() {
	static char strTime[20];
	wsprintf(strTime, TEXT("时间：%d秒"), timer);
	return strTime;
}

bool Game::GameSuccess() {
	int iUnknow = 0;
	int iMine = 0;
	for (int i = 0; i < g_Row; ++i) {
		for (int j = 0; j < g_Column; ++j) {
			if (boxState[i][j].drawBox == Unknow || boxState[i][j].drawBox == Question) {
				++iUnknow;
			}
			if (boxState[i][j].drawBox == Flag) {
				++iMine;
			}
		}
	}
	if (iUnknow == 0 && iMine == g_Mine) {
		gameSuccess = true;
		return true;
	}
	return false;
}

void Game::GameDestroy() {
	for (int i = 0; i < g_Row; ++i) {
		delete[]boxState[i];
		boxState[i] = NULL;
	}
	delete[]boxState;
	boxState = NULL;


	DeleteObject(hBlackPen);

	DeleteObject(hBlueBrush);
	DeleteObject(hRedBrush);

	DeleteObject(hCountFont);

	DeleteObject(compatibleBmp);
	DeleteObject(hUnknow);
	DeleteObject(hFlag);
	DeleteObject(hQuestion);
	DeleteObject(hLightUnknow);
	DeleteObject(hLightFlag);
	DeleteObject(hLightQuestion);
	DeleteObject(hClickBlast);
	DeleteObject(hWrong);
	DeleteObject(hBlast);
	DeleteObject(hFlagBlast);
	DeleteObject(hQuestionBlast);
	DeleteObject(hZero);
	DeleteObject(hOne);
	DeleteObject(hTwo);
	DeleteObject(hThree);
	DeleteObject(hFour);
	DeleteObject(hFive);
	DeleteObject(hSix);
	DeleteObject(hSeven);
	DeleteObject(hEight);
	DeleteObject(hBlock);
}