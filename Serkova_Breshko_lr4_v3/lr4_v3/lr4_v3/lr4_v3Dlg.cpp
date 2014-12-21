
// lr4_v3Dlg.cpp : файл реализации

#include "stdafx.h"
#include "lr4_v3.h"
#include "lr4_v3Dlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef void __stdcall TCreateIndex();
typedef void __stdcall TLoadPhoneBase();
typedef void __stdcall TInsert(char secondName[255]);
typedef int __stdcall TDelete(char secondName[255]);
typedef char*** __stdcall TDataBaseSearchBySecondName(char secondName[20]);
typedef char*** __stdcall TDataBaseSearchByAdress(char secondName[20]);
typedef char*** __stdcall TDataBaseSearchPhoneNumber(char phoneNumber[15]);

HMODULE dataBaseLibrary = LoadLibrary(L"IndexDll.dll");
TCreateIndex* CreateIndexFunction;
TDelete* DeleteFunction;
TLoadPhoneBase* LoadPhoneBaseFunction;
TDataBaseSearchBySecondName* DataBaseSearchBySecondNameFunction;
TDataBaseSearchByAdress* DataBaseSearchByAdressFunction;
TDataBaseSearchPhoneNumber* DataBaseSearchPhoneNumberFunction;

char* secondName;
char* adressHome;
char* phoneNumber;
char ***abonentsArray;

// Диалоговое окно CAboutDlg используется для описания сведений о приложении
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// диалоговое окно Clr4_v3Dlg
IMPLEMENT_DYNAMIC(Clr4_v3Dlg, CDialogEx);

Clr4_v3Dlg::Clr4_v3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Clr4_v3Dlg::IDD, pParent)
	, surename(_T(""))
	, number(_T(""))
	, adress(_T(""))
	, resOfSearch(_T(""))
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

Clr4_v3Dlg::~Clr4_v3Dlg()
{
	// Если для данного диалогового окна используется прокси-сервер автоматизации, установите
	//  для его обратного указателя на это диалоговое окно значение NULL, чтобы он узнал об
	//  удалении диалогового окна.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void Clr4_v3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, surename);
	DDX_Text(pDX, IDC_EDIT4, number);
	DDX_Text(pDX, IDC_EDIT5, adress);
	DDX_Text(pDX, IDC_EDIT6, resOfSearch);
}

BEGIN_MESSAGE_MAP(Clr4_v3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT5, &Clr4_v3Dlg::OnEnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, &Clr4_v3Dlg::OnEnChangeEdit6)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &Clr4_v3Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// обработчики сообщений Clr4_v3Dlg
HBRUSH Clr4_v3Dlg::OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor)//for transparent static_text
{
	HBRUSH hbr=CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
	CWnd* pStatic=GetDlgItem(IDC_STATIC1);
	ASSERT(pStatic);
	if(pStatic->m_hWnd==pWnd->m_hWnd)
	{
		hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	pStatic=GetDlgItem(IDC_STATIC);
	ASSERT(pStatic);
	if(pStatic->m_hWnd==pWnd->m_hWnd)
	{
		hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	pStatic=GetDlgItem(IDC_STATIC2);
	ASSERT(pStatic);
	if(pStatic->m_hWnd==pWnd->m_hWnd)
	{
		hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
		pStatic=GetDlgItem(IDC_STATIC4);
	ASSERT(pStatic);
	if(pStatic->m_hWnd==pWnd->m_hWnd)
	{
		hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
		
		pStatic=GetDlgItem(IDC_STATIC6);
	ASSERT(pStatic);
	if(pStatic->m_hWnd==pWnd->m_hWnd)
	{
		hbr=(HBRUSH)GetStockObject(NULL_BRUSH);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
	}
	return hbr;
}
BOOL Clr4_v3Dlg::OnInitDialog()
{
	CreateIndexFunction = (TCreateIndex*)GetProcAddress(dataBaseLibrary, "_CreateIndex@0");
	DeleteFunction = (TDelete*)GetProcAddress(dataBaseLibrary, "_DataBaseDelete@4");
	LoadPhoneBaseFunction = (TLoadPhoneBase*)GetProcAddress(dataBaseLibrary, "_LoadPhoneBase@0");
	DataBaseSearchBySecondNameFunction = (TDataBaseSearchBySecondName*)GetProcAddress(dataBaseLibrary, "_DataBaseSearchBySecondName@4");
	DataBaseSearchByAdressFunction = (TDataBaseSearchByAdress*)GetProcAddress(dataBaseLibrary, "_DataBaseSearchByAdress@4");
	DataBaseSearchPhoneNumberFunction = (TDataBaseSearchPhoneNumber*)GetProcAddress(dataBaseLibrary, "_DataBaseSearchPhoneNumber@4");
	CreateIndexFunction();
	LoadPhoneBaseFunction();

	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.
	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок
	// TODO: добавьте дополнительную инициализацию
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void Clr4_v3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void Clr4_v3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR Clr4_v3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Серверы автоматизации не должны завершать работу при закрытии интерфейса пользователя,
//  если контроллеры по-прежнему удерживают один из его объектов. Эти
//  обработчики сообщений гарантируют, что если прокси-сервер еще используется,
//  то пользовательский интерфейс скрывается, но диалоговое окно остается (если оно
//  закрыто).

void Clr4_v3Dlg::OnClose()
{

	if (CanExit())
		CDialogEx::OnClose();
}

BOOL Clr4_v3Dlg::CanExit()
{
	// Если объект прокси-сервера по-прежнему запущен, то контроллер
	//  автоматизации удерживает это приложение. Оставьте
	//  диалоговое окно запущенным, но скройте его пользовательский интерфейс.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void Clr4_v3Dlg::OnEnChangeEdit5()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
//#1015
	// TODO:  Добавьте код элемента управления
}


void Clr4_v3Dlg::OnEnChangeEdit6()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
//#1015
	// TODO:  Добавьте код элемента управления
}


void Clr4_v3Dlg::OnBnClickedButton1()//hendler for button
{
	std::vector<char*> resultsVector;
	char *result;

	UpdateData(TRUE);//эта штука для связи объектов формы и соответствующих им переменных. то, что есть на форме,
	//разбегается по своим переменным. Чтобы переменные были отправлены в свои ячейки на форме, нужно TRUE заменить на FALSE
	//MessageBox(surename+" "+adress+" "+number); //test message

	if (surename == "" && adress == "" && number == "")
				return;

	adressHome = (char*)malloc(sizeof(char)* 40);
	secondName = (char*)malloc(sizeof(char)* 40);
	phoneNumber = (char*)malloc(sizeof(char)* 40);

	strcpy_s(secondName, 40, CT2A(surename));
	strcpy_s(adressHome, 40, CT2A(adress));
	strcpy_s(phoneNumber, 40, CT2A(number));

	if (strcmp(secondName, "") != 0)
	{		
		abonentsArray = DataBaseSearchBySecondNameFunction(secondName);
		if (abonentsArray == 0)
			return;

		UniteResult(&result, resultsVector);
	}
	else
	{
		if (strcmp(adressHome, "") != 0)
		{
			abonentsArray = DataBaseSearchBySecondNameFunction(adressHome);
			if (abonentsArray == 0)
				return;

			UniteResult(&result, resultsVector);

		}
		else
		{
			if (strcmp(phoneNumber, "") != 0)
			{
				abonentsArray = DataBaseSearchBySecondNameFunction(phoneNumber);
				if (abonentsArray == 0)
					return;

				UniteResult(&result, resultsVector);
			}
		}
	}

	resOfSearch = "";

	for (int i = 0; i < resultsVector.size(); i++) 
	{
		if (strstr(resultsVector[i], secondName) != 0 || strcmp(secondName, "") == 0)
		{
			if (strstr(resultsVector[i], adressHome) != 0 || strcmp(adressHome, "") == 0)
			{
				if (strstr(resultsVector[i], phoneNumber) != 0 || strcmp(phoneNumber, "") == 0)
				{
					resOfSearch += resultsVector[i];
					resOfSearch += "\r\n";
				}
			}
		}
	}

	UpdateData(FALSE);
}

void Clr4_v3Dlg::UniteResult(char **result, std::vector<char*> &resultsVector)
{
	int count;
	count = _msize(abonentsArray) / sizeof(abonentsArray[0]);
	for (int i = 0; i < count; i++)
	{
		*result = new char[255];
		strcpy_s(*result, 2, "");
		for (int j = 0; j < 4; j++)
		{
			strcat_s(*result, _msize(*result) + 1, abonentsArray[i][j]);		
			strcat_s(*result, _msize(*result) + 1, " ");
		}
		resultsVector.push_back(*result);
	}
}

