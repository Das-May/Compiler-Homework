
#include"framework.h"
#include"WindowsProject1.h"
#defineMAX_LOADSTRING100HINSTANCEhInst;
WCHARszTitle[MAX_LOADSTRING];
WCHARszWindowClass[MAX_LOADSTRING];
ATOMMyRegisterClass(HINSTANCEhInstance);
BOOLInitInstance(HINSTANCE,int );
LRESULTCALLBACKWndProc(HWND,UINT,WPARAM,LPARAM);
INT_PTRCALLBACKAbout(HWND,UINT,WPARAM,LPARAM);
int APIENTRYwWinMain(_In_HINSTANCEhInstance,_In_opt_HINSTANCEhPrevInstance,_In_LPWSTRlpCmdLine,_In_int nCmdShow){
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	LoadStringW(hInstance,IDS_APP_TITLE,szTitle,MAX_LOADSTRING);
	LoadStringW(hInstance,IDC_WINDOWSPROJECT1,szWindowClass,MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance,nCmdShow)){
		return FALSE;
		}
	HACCELhAccelTable=LoadAccelerators(hInstance,MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
	MSGmsg;
	while (GetMessage(&msg,nullptr,0,0)){
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}
	return (int )msg.wParam;
	}
ATOMMyRegisterClass(HINSTANCEhInstance){
	WNDCLASSEXWwcex;
	wcex.cbSize=sizeof (WNDCLASSEX);
	wcex.style=CS_HREDRAW|CS_VREDRAW;
	wcex.lpfnWndProc=WndProc;
	wcex.cbClsExtra=0;
	wcex.cbWndExtra=0;
	wcex.hInstance=hInstance;
	wcex.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
	wcex.hCursor=LoadCursor(nullptr,IDC_ARROW);
	wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName=MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName=szWindowClass;
	wcex.hIconSm=LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
	}
BOOLInitInstance(HINSTANCEhInstance,int nCmdShow){
	hInst=hInstance;
	HWNDhWnd=CreateWindowW(szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,0,CW_USEDEFAULT,0,nullptr,nullptr,hInstance,nullptr);
	if (!hWnd){
		return FALSE;
		}
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
	}
LRESULTCALLBACKWndProc(HWNDhWnd,UINTmessage,WPARAMwParam,LPARAMlParam){
	switch (message){
		case WM_COMMAND:{
			int wmId=LOWORD(wParam);
			switch (wmId){
				case IDM_ABOUT:DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUTBOX),hWnd,About);
				break ;
				case IDM_EXIT:DestroyWindow(hWnd);
				break ;
				default :return DefWindowProc(hWnd,message,wParam,lParam);
				}
			}
		break ;
		case WM_PAINT:{
			PAINTSTRUCTps;
			HDChdc=BeginPaint(hWnd,&ps);
			EndPaint(hWnd,&ps);
			}
		break ;
		case WM_DESTROY:PostQuitMessage(0);
		break ;
		default :return DefWindowProc(hWnd,message,wParam,lParam);
		}
	return 0;
	}
INT_PTRCALLBACKAbout(HWNDhDlg,UINTmessage,WPARAMwParam,LPARAMlParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message){
		case WM_INITDIALOG:return (INT_PTR)TRUE;
		case WM_COMMAND:if (LOWORD(wParam)==IDOK||LOWORD(wParam)==IDCANCEL){
			EndDialog(hDlg,LOWORD(wParam));
			return (INT_PTR)TRUE;
			}
		break ;
		}
	return (INT_PTR)FALSE;
	}
wParam)==IDCANCEL){
	EndDialog(hDlg,LOWORD(wParam));
	return (INT_PTR)TRUE;
	}
break ;
}
return (INT_PTR)FALSE;
}
