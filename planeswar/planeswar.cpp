//planeswar.cpp: 定义应用程序的入口点。
//
#include "stdafx.h"
#include "planeswar.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANESWAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANESWAR));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BIGICON));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MYCURSOR));
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   int cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(
	   szWindowClass, 
	   szTitle, 
	   WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
       (cxScreen - WNDWIDTH) / 2, 
	   (cyScreen - WNDHEIGHT - 50) / 2, 
	   WNDWIDTH, 
	   WNDHEIGHT + 35, 
	   nullptr, 
	   nullptr, 
	   hInstance, 
	   nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE: {
		initgame(lParam);
	}
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		drawSenceUI(hdc, p_head);
		EndPaint(hWnd, &ps);
	}
        break;
	case WM_LBUTTONDOWN: {
		lButtondownProc(hWnd,lParam);
	}
		break;
	case WM_TIMER: {
		
	}
		break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//	自定义函数
//
//  声明在planeswar.h头文件中
//

//init:
VOID initgame(LPARAM lParam) {
	for (int i = 0; i < BMPCOUNT; i++) {
		hbBmp[i] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(uiBmpName[i]));
	}

	s_game.g_status = WELCOME;
	s_game.g_score = 0;

	big_info.t_maxHP = 3;
	big_info.t_speed = 5;
	big_info.t_hbmp = hbBmp[1];

	middle_info.t_maxHP = 2;
	middle_info.t_speed = 10;
	middle_info.t_hbmp = hbBmp[5];

	small_info.t_maxHP = 1;
	small_info.t_speed = 15;
	small_info.t_hbmp = hbBmp[7];

	p_head = (PLANE *)malloc(sizeof(PLANE));
	createHeadPlane(p_head);
}

//sence:
VOID drawSenceUI(HDC hdc,PLANE *p_head) {
	HBITMAP hBmpMem;
	HDC hdcMem, hdcTmp;
	static BITMAP bmp;
	
	// 双缓冲法绘图，解决重绘时闪烁的问题 
	// 创建与hDC环境相关的设备兼容的位图内存画布 
	hBmpMem = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	// 创建与hdc相兼容的内存dc 
	hdcMem = CreateCompatibleDC(hdc);
	SelectObject(hdcMem, hBmpMem);
	// hDcTmp是另一个临时的内存dc，用来储存部件，如背景，飞机，按钮等等 
	hdcTmp = CreateCompatibleDC(hdc);

	//画出背景图
	drawbackground(hdc, hdcTmp, hbBmp);

	switch (s_game.g_status) {
	case WELCOME:
		//TODO:welcome场景下的界面绘制
		drawSenceI(hdc, hdcTmp, hbBmp, bmp);
		break;
	case RUNING:
		//TODO:runing场景下的界面绘制
		drawSenceII(p_head, hdc, hdcTmp, bmp);
		break;
	case OVER:
		//TODO:over场景下的界面绘制
		break;
	}
	BitBlt(hdcMem, 0, 0, WNDWIDTH, WNDHEIGHT,
		hdcTmp, 0, 0, SRCCOPY);
	//DeleteObject(hdcTmp);
	//DeleteDC(hdc);
	DeleteObject(hBmpMem);
	DeleteDC(hdcMem);
	DeleteDC(hdcTmp);
}
VOID drawbackground(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp) {
	SelectObject(hdcTmp, hbBmp[0]);
	BitBlt(hdcMem, 0, 0, WNDWIDTH, WNDHEIGHT,
		hdcTmp, 0, 0, SRCCOPY);
}
VOID drawSenceI(HDC hdcMem, HDC hdcTmp, HBITMAP *hbBmp, BITMAP bmp) {
	//画logo
	SelectObject(hdcTmp, hbBmp[4]);
	GetObject(hbBmp[4], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 35, 150, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
	//画button
	SelectObject(hdcTmp, hbBmp[8]);
	GetObject(hbBmp[8], sizeof(BITMAP), &bmp);
	TransparentBlt(hdcMem, 125, 380, bmp.bmWidth, bmp.bmHeight,
		hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
}
VOID drawSenceII(PLANE *p_head, HDC hdcMem, HDC hdcTmp, BITMAP bmp) {
	//敌机增减频繁，用链表装下敌机
	PLANE *p = p_head;
	while (p != nullptr) {
		SelectObject(hdcTmp, p->p_hbmp);
		GetObject(p->p_hbmp, sizeof(BITMAP), &bmp);
		TransparentBlt(hdcMem, (p->p_point).x, (p->p_point).y, bmp.bmWidth, bmp.bmHeight,
			hdcTmp, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));
		p = p->next;
	}
	getRandomPlaneInfo(p_head);
}

//opreate:
VOID lButtondownProc(HWND hwnd, LPARAM lParam) {
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	switch (s_game.g_status) {
	case WELCOME:
		// TODO:welcome场景下的鼠标事件
		if (checkbutton_start(ptMouse)) {
			s_game.g_status = RUNING;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	case RUNING:
		// TODO:runing场景下的鼠标事件
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case OVER:
		// TODO:over场景下的鼠标事件
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	}
}
BOOL checkbutton_start(POINT ptMouse) {
	RECT rect;
	BITMAP bmp;
	GetObject(hbBmp[8], sizeof(BITMAP), &bmp);
	rect.left = 125;
	rect.top = 380;
	rect.right = bmp.bmWidth + rect.left;
	rect.bottom = bmp.bmHeight + rect.top;
	return PtInRect(&rect, ptMouse);
}

//enime:
VOID createHeadPlane(PLANE *head) {
	getRandomPlaneInfo(head);
}
VOID getRandomPlaneInfo(PLANE *plane) {
	UINT idex;
	UINT r_base1 = 3, r_base2 = 380;
	srand(GetTickCount());
	idex = rand() % r_base1;
	switch (idex) {
	case BIG: {
		srand(GetTickCount());
		idex = rand() % r_base2 - 140;
		plane->p_maxHP = big_info.t_maxHP;
		plane->p_speed = big_info.t_speed;
		plane->p_hbmp = big_info.t_hbmp;
		plane->p_hitCount = 0;
		plane->p_type = BIG;
		plane->next = NULL;
		(plane->p_point).x = idex;
		(plane->p_point).y = -130;
	}
		break;
	case MIDDLE: {
		srand(GetTickCount());
		idex = rand() % r_base2 - 140;
		plane->p_maxHP = middle_info.t_maxHP;
		plane->p_speed = middle_info.t_speed;
		plane->p_hbmp = middle_info.t_hbmp;
		plane->p_hitCount = 0;
		plane->p_type = BIG;
		plane->next = NULL;
		(plane->p_point).x = idex;
		(plane->p_point).y = -90;

	}
		break;
	case SMALL: {
		srand(GetTickCount());
		idex = rand() % r_base2 - 140;
		plane->p_maxHP = small_info.t_maxHP;
		plane->p_speed = small_info.t_speed;
		plane->p_hbmp = small_info.t_hbmp;
		plane->p_hitCount = 0;
		plane->p_type = BIG;
		plane->next = NULL;
		(plane->p_point).x = idex;
		(plane->p_point).y = -30;
	}
		break;
	}
}