
// SmServerView.h : interface of the CSmServerView class
//

#pragma once


class CSmServerView : public CView
{
protected: // create from serialization only
	CSmServerView() noexcept;
	DECLARE_DYNCREATE(CSmServerView)

// Attributes
public:
	CSmServerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSmServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SmServerView.cpp
inline CSmServerDoc* CSmServerView::GetDocument() const
   { return reinterpret_cast<CSmServerDoc*>(m_pDocument); }
#endif

