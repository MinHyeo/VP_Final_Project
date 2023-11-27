#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <TCHAR.H>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hlnst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND 	hwnd;
	MSG 	msg;
	WNDCLASS	WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	g_hlnst = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("Window Class Name"),
		_T("Window Title Name"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

#define AppleNUM 10

HBITMAP appleBitmap;
HBITMAP HeadBitmap;
HBITMAP BodyBitmap;
HBITMAP TailBitmap;
HBITMAP BackGroundBitmap;
HBITMAP BoardBitmap;
HBITMAP TeleportBitmap;
int board[22][22];	
int wormX[100], wormY[100];
int xDirect, yDirect, len;
int score;
int level;

void AppleGenerator()
{
	int i, x, y;
	for (i = 0; i < AppleNUM; i++)
	{
		x = rand() % 20 + 1;
		y = rand() % 20 + 1;
		if (board[y][x] == 0)
			board[y][x] = 2;
		else
		{
			i = i - 1;
			continue;
		}
	}
	return;
}

void AppleReload() {
	int x, y;
	while(1) {
		x = rand() % 20 + 1;
		y = rand() % 20 + 1;
		if (board[y][x] == 0) {
			board[y][x] = 2;
			break;
		}
	}
}

HBITMAP GetRotatedBitmap(HDC hdc, HBITMAP hBitmap, float radians, COLORREF clrBack)
{
	HDC sourceDC, destDC;
	BITMAP bm;

	HBITMAP hbmResult;
	HBITMAP hbmOldSource;
	HBITMAP hbmOldDest;
	HBRUSH hbrBack;
	HBRUSH hbrOld;

	XFORM xform;

	float cosine, sine;
	int x1, y1, x2, y2, x3, y3, minx, miny, maxx, maxy, w, h;


	sourceDC = CreateCompatibleDC(hdc);
	destDC = CreateCompatibleDC(hdc);

	GetObject(hBitmap, sizeof(bm), &bm);

	cosine = cos(radians);
	sine = sin(radians);

	x1 = (int)(bm.bmHeight * sine);
	y1 = (int)(bm.bmHeight * cosine);
	x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	x3 = (int)(bm.bmWidth * cosine);
	y3 = (int)(-bm.bmWidth * sine);

	minx = min(0, min(x1, min(x2, x3)));
	miny = min(0, min(y1, min(y2, y3)));
	maxx = max(0, max(x1, max(x2, x3)));
	maxy = max(0, max(y1, max(y2, y3)));

	w = maxx - minx;
	h = maxy - miny;

	hbmResult = CreateCompatibleBitmap(hdc, w, h);

	hbmOldSource = (HBITMAP)SelectObject(sourceDC, hBitmap);
	hbmOldDest = (HBITMAP)SelectObject(destDC, hbmResult);

	hbrBack = CreateSolidBrush(clrBack);
	hbrOld = (HBRUSH)SelectObject(destDC, hbrBack);
	PatBlt(destDC, 0, 0, w, h, PATCOPY);
	DeleteObject(SelectObject(destDC, hbrOld));

	SetGraphicsMode(destDC, GM_ADVANCED);
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;

	SetWorldTransform(destDC, &xform);

	BitBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY);

	SelectObject(sourceDC, hbmOldSource);
	SelectObject(destDC, hbmOldDest);

	DeleteDC(sourceDC);
	DeleteDC(destDC);

	return hbmResult;
}

void DrawGameBoard(HDC hdc)
{
	static HDC mem1dc, mem2dc;
	static HBITMAP hBit1, oldBit1, oldBit2;

	mem1dc = CreateCompatibleDC(hdc);
	mem2dc = CreateCompatibleDC(mem1dc);

	hBit1 = CreateCompatibleBitmap(hdc, 440, 440);
	
	oldBit1 = (HBITMAP)SelectObject(mem1dc, hBit1);
	oldBit2 = (HBITMAP) SelectObject(mem2dc, BackGroundBitmap);

	BitBlt(mem1dc, 0, 0, 440, 440, mem2dc, 0, 0, SRCCOPY);

	int i, x, y;
	for (y = 0; y < 22; y++) {
		for (x = 0; x < 22; x++)
		{
			switch (board[y][x])
			{
			case -1:
				oldBit2 = (HBITMAP)SelectObject(mem2dc, BoardBitmap);
				BitBlt(mem1dc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20, mem2dc, 0, 0, SRCCOPY);
				break;
			case 2:
				oldBit2 = (HBITMAP)SelectObject(mem2dc, appleBitmap);
				TransparentBlt(mem1dc, x * 20, y * 20, 20, 20, mem2dc, 0, 0, 20, 20, RGB(255, 255, 255));
				break;
			case 4:
				oldBit2 = (HBITMAP)SelectObject(mem2dc, TeleportBitmap);
				BitBlt(mem1dc, x * 20, y * 20, (x + 1) * 20, (y + 1) * 20, mem2dc, 0, 0, SRCCOPY);
				break;
			}
		}
	}

	oldBit2 = (HBITMAP)SelectObject(mem2dc, HeadBitmap);
	TransparentBlt(mem1dc, wormX[0] * 20, wormY[0] * 20, 20, 20, mem2dc, 0, 0, 20, 20, RGB(255, 255, 255));
	for (i = 1; i < len; i++) {
		if (i == len - 1) {
			oldBit2 = (HBITMAP)SelectObject(mem2dc, TailBitmap);
			TransparentBlt(mem1dc, wormX[i] * 20, wormY[i] * 20, 20, 20, mem2dc, 0, 0, 20, 20, RGB(255, 255, 255));
		}
		else {
			oldBit2 = (HBITMAP)SelectObject(mem2dc, BodyBitmap);
			TransparentBlt(mem1dc, wormX[i] * 20, wormY[i] * 20, 20, 20, mem2dc, 0, 0, 20, 20, RGB(255, 255, 255));
		}
	}

	BitBlt(hdc, 0, 0, 440, 440, mem1dc, 0, 0, SRCCOPY);

	TCHAR scoreText[20];
	TCHAR levelText[20];
	wsprintf(scoreText, _T("score : %5d"), score);
	wsprintf(levelText, _T("level : %d"), level);
	TextOut(hdc, 0, 22 * 20, scoreText, _tcslen(scoreText));
	TextOut(hdc, 390, 22 * 20, levelText, _tcslen(levelText));

	SelectObject(mem1dc, oldBit1);
	DeleteDC(mem2dc);
	DeleteDC(mem1dc);
}

void GameInit()
{
	HeadBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP3));
	BodyBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP4));
	TailBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP5));
	appleBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP1));
	BackGroundBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP6));
	BoardBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP10));
	TeleportBitmap = LoadBitmap(g_hlnst, MAKEINTRESOURCE(IDB_BITMAP11));
	int i;
	for (i = 0; i < 22; i++)
	{
		board[i][0] = -1;
		board[i][21] = -1;
		board[0][i] = -1;
		board[21][i] = -1;
	}

	board[1][21] = 4;
	board[2][21] = 4;

	board[19][0] = 4;
	board[20][0] = 4;

	wormX[0] = 2; wormY[0] = 1;
	wormX[1] = 1; wormY[1] = 1;
	board[wormY[0]][wormX[0]] = 3;
	board[wormY[1]][wormX[1]] = 3;

	AppleGenerator();

	len = 2;
	xDirect = 1; yDirect = 0;
	score = 0;
	level = 1;
}

void GameReset() {
	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 22; j++) {
			board[i][j] = 0;
		}
	}
	for (int i = 0; i < 100; i++) {
		wormX[i] = 0;
		wormY[i] = 0;
	}
}

bool desc(int a, int b) {
	return a > b;
}

void SaveScore() {
	HANDLE hFile;
	DWORD dwBytesRead, dwBytesWritten;
	std::vector<char> ReadBuf(4096, 0);
	std::string scoreStr = std::to_string(score);
	std::string newline = "\n";

	hFile = CreateFile(TEXT("score.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	ReadFile(hFile, ReadBuf.data(), ReadBuf.size() - 1, &dwBytesRead, NULL);
	std::string readStr(ReadBuf.begin(), ReadBuf.end());
	std::istringstream iss(readStr);
	std::vector<int> readData;
	while (std::getline(iss, readStr, '\n')) {
		try
		{
			std::istringstream iss2(readStr);
			std::getline(iss2, readStr, ':');
			std::getline(iss2, readStr, ':');
			readData.push_back(std::stoi(readStr));
		}
		catch (const std::exception&)
		{
			break;
		}
	}	
	readData.push_back(score);
	std::sort(readData.begin(), readData.end(), desc);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	for (int i = 0; i < 10; i++) {
		if (i >= readData.size())
			break;
		std::string number = std::to_string(i + 1) + " :";
		std::string bestScoreStr = std::to_string(readData[i]);
		WriteFile(hFile, number.c_str(), number.size(), &dwBytesWritten, NULL);
		WriteFile(hFile, bestScoreStr.c_str(), bestScoreStr.size(), &dwBytesWritten, NULL);
		WriteFile(hFile, newline.c_str(), newline.size(), &dwBytesWritten, NULL);
	}
	CloseHandle(hFile);
}

void GameOver(HWND hwnd) {
	KillTimer(hwnd, 1);
	TCHAR deathText[30];
	wsprintf(deathText, _T("ав╬З╫ю╢о╢ы!! \n score : %d"), score);

	SaveScore();

	if (IDOK == MessageBox(hwnd, deathText, _T("Game Over"), MB_ICONHAND)) {
		GameReset();
		GameInit();
		SetTimer(hwnd, 1, 100, NULL);
	}
}

void DirectControl(int DirectKey, HDC hdc)
{
	switch (DirectKey) {
	case VK_LEFT:
		if (xDirect == 1 || xDirect == -1)
			break;
		if (board[wormY[0]][wormX[0] - 1] != -1)
		{
			if (yDirect == 1) {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
			}
			else {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, M_PI / 2, RGB(0, 0, 0));
			}
			xDirect = -1;
			yDirect = 0;
		}
		break;
	case VK_RIGHT:
		if (xDirect == -1 || xDirect == 1)
			break;
		if (board[wormY[0]][wormX[0] + 1] != -1)
		{
			if (yDirect == 1) {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, M_PI / 2, RGB(0, 0, 0));
			}
			else {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
			}
			xDirect = 1;
			yDirect = 0;
		}
		break;
	case VK_UP:
		if (yDirect == 1 || yDirect == -1)
			break;
		if (board[wormY[0] - 1][wormX[0]] != -1 )
		{
			if (xDirect == 1) {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, M_PI / 2, RGB(0, 0, 0));
			}
			else {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
			}
			xDirect = 0;
			yDirect = -1;
		}
		break;
	case VK_DOWN:
		if (yDirect == -1 || yDirect == 1)
			break;
		if (board[wormY[0] + 1][wormX[0]] != -1)
		{
			if (xDirect == 1) {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, -1 * M_PI / 2, RGB(0, 0, 0));
			}
			else {
				HeadBitmap = GetRotatedBitmap(hdc, HeadBitmap, M_PI / 2, RGB(0, 0, 0));
				TailBitmap = GetRotatedBitmap(hdc, TailBitmap, M_PI / 2, RGB(0, 0, 0));
			}
			xDirect = 0;
			yDirect = 1;
		}
		break;
	}
}

void LevelUp(HWND hwnd) {
	level += 1;
	KillTimer(hwnd, 1);
	SetTimer(hwnd, 1, 100 - (10 * level), NULL);
}

void MovingWorm(HWND hwnd)
{
	int tmpx, tmpy, i;

	tmpx = wormX[0];
	tmpy = wormY[0];
	wormX[0] += xDirect; wormY[0] += yDirect;

	if (board[wormY[0]][wormX[0]] == -1 || board[wormY[0]][wormX[0]] == 3)
	{
		GameOver(hwnd);
	}
	else
	{
		if (board[wormY[0]][wormX[0]] == 2)
		{
			len = len + 1;
			score += 10;
			if (score % 500 == 0)
				LevelUp(hwnd);
				
			board[wormY[0]][wormX[0]] = 0;
			AppleReload();
		}
		else
			board[wormY[len - 1]][wormX[len - 1]] = 0;

		if (board[wormY[0]][wormX[0]] == 4) {
			if (wormX[0] == 21) {
				wormX[0] = 1;
				wormY[0] = 18 + wormY[0];
			}
			else {
				wormX[0] = 20;
				wormY[0] = wormY[0] - 18;
			}
		}

		for (i = len - 1; i > 1; i--)
		{
			wormX[i] = wormX[i - 1];
			wormY[i] = wormY[i - 1];
		}
		wormX[1] = tmpx;
		wormY[1] = tmpy;
		board[wormY[0]][wormX[0]] = 3;
	}
}

void ShowBestScore(int CommendKey , HWND hwnd) {
	switch (LOWORD(CommendKey)) {
	case ID_BESTSCORE:
		KillTimer(hwnd, 1);
		HANDLE hFile;
		DWORD dwBytesRead;
		char ReadBuf[200];

		hFile = CreateFile(TEXT("score.txt"), GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
		ReadFile(hFile, ReadBuf, sizeof(ReadBuf) - 1, &dwBytesRead, NULL);
		ReadBuf[dwBytesRead] = '\0';

		TCHAR BestScore[200];
		MultiByteToWideChar(CP_ACP, 0, ReadBuf, -1, BestScore, dwBytesRead + 1);
		if(IDOK == MessageBox(hwnd, BestScore, _T("Best Score!"), MB_OK))
			SetTimer(hwnd, 1, 100, NULL);
		CloseHandle(hFile);
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);

	switch (iMsg) {
	case WM_CREATE:
		GameInit();
		SetTimer(hwnd, 1, 100, NULL);
		return 0;
	case WM_PAINT:
		DrawGameBoard(hdc);
		break;
	case WM_KEYDOWN:
		DirectControl((int)wParam, hdc);
		break;
	case WM_COMMAND:
		ShowBestScore((int)wParam, hwnd);
		break;
	case WM_TIMER:
		MovingWorm(hwnd);
		InvalidateRgn(hwnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	EndPaint(hwnd, &ps);
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}