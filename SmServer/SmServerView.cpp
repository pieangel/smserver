
// SmServerView.cpp : implementation of the CSmServerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SmServer.h"
#endif

#include "SmServerDoc.h"
#include "SmServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmServerView

IMPLEMENT_DYNCREATE(CSmServerView, CView)

BEGIN_MESSAGE_MAP(CSmServerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CSmServerView construction/destruction

CSmServerView::CSmServerView() noexcept
{
	// TODO: add construction code here

}

CSmServerView::~CSmServerView()
{
}

BOOL CSmServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSmServerView drawing

void CSmServerView::OnDraw(CDC* /*pDC*/)
{
	CSmServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CSmServerView printing

BOOL CSmServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSmServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSmServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CSmServerView diagnostics

#ifdef _DEBUG
void CSmServerView::AssertValid() const
{
	CView::AssertValid();
}

void CSmServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSmServerDoc* CSmServerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmServerDoc)));
	return (CSmServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSmServerView message handlers
