
// DlgProxy.cpp : файл реализации
//

#include "stdafx.h"
#include "lr4_v3.h"
#include "DlgProxy.h"
#include "lr4_v3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Clr4_v3DlgAutoProxy

IMPLEMENT_DYNCREATE(Clr4_v3DlgAutoProxy, CCmdTarget)

Clr4_v3DlgAutoProxy::Clr4_v3DlgAutoProxy()
{
	EnableAutomation();
	
	// Чтобы приложение работало, пока объект 
	//	автоматизации активен, конструктор вызывает AfxOleLockApp.
	AfxOleLockApp();

	// Получает доступ к диалоговому окну через указатель
	//  главного окна приложения. Задает внутренний указатель прокси-сервера для
	//  доступа к диалогу, а также задает обратный указатель диалога на этот
	//  прокси-сервер.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(Clr4_v3Dlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(Clr4_v3Dlg)))
		{
			m_pDialog = reinterpret_cast<Clr4_v3Dlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

Clr4_v3DlgAutoProxy::~Clr4_v3DlgAutoProxy()
{
	// Чтобы прервать работу приложения, когда все объекты создаются
	// 	автоматически, деструктор вызывает AfxOleUnlockApp.
	//  Среди прочего будет уничтожено главное диалоговое окно
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void Clr4_v3DlgAutoProxy::OnFinalRelease()
{
	// Когда будет освобождена последняя ссылка на объект автоматизации,
	// Будет вызван OnFinalRelease. Базовый класс автоматически
	// удалит объект. Перед вызовом базового класса требуется
	// дополнительная очистка объекта.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(Clr4_v3DlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Clr4_v3DlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Примечание. Добавлена поддержка для IID_Ilr4_v3 для поддержки привязки typesafe
//  из VBA. IID должен совпадать с GUID, который вложен в 
//  disp-интерфейс в файле .IDL.

// {A7814054-F1DD-4C24-8ABE-8E1EA5A43223}
static const IID IID_Ilr4_v3 =
{ 0xA7814054, 0xF1DD, 0x4C24, { 0x8A, 0xBE, 0x8E, 0x1E, 0xA5, 0xA4, 0x32, 0x23 } };

BEGIN_INTERFACE_MAP(Clr4_v3DlgAutoProxy, CCmdTarget)
	INTERFACE_PART(Clr4_v3DlgAutoProxy, IID_Ilr4_v3, Dispatch)
END_INTERFACE_MAP()

// Макрос IMPLEMENT_OLECREATE2 определен в StdAfx.h этого проекта
// {05DADF5F-6361-49DD-9BAA-8CF0E688299D}
IMPLEMENT_OLECREATE2(Clr4_v3DlgAutoProxy, "lr4_v3.Application", 0x5dadf5f, 0x6361, 0x49dd, 0x9b, 0xaa, 0x8c, 0xf0, 0xe6, 0x88, 0x29, 0x9d)


// обработчики сообщений Clr4_v3DlgAutoProxy
