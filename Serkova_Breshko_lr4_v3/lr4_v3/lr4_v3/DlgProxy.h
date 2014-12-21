
// DlgProxy.h: файл заголовка
//

#pragma once

class Clr4_v3Dlg;


// конечный объект команды Clr4_v3DlgAutoProxy

class Clr4_v3DlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(Clr4_v3DlgAutoProxy)

	Clr4_v3DlgAutoProxy();           // защищенный конструктор, используемый при динамическом создании

// Атрибуты
public:
	Clr4_v3Dlg* m_pDialog;

// Операции
public:

// Переопределение
	public:
	virtual void OnFinalRelease();

// Реализация
protected:
	virtual ~Clr4_v3DlgAutoProxy();

	// Созданные функции схемы сообщений

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(Clr4_v3DlgAutoProxy)

	// Автоматически созданные функции диспетчерской карты OLE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

