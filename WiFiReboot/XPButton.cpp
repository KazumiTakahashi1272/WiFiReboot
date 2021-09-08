// XPButton.cpp : implementation file
//

#include "stdafx.h"
#include "XPButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXPButton

IMPLEMENT_DYNCREATE(CXPButton, CButton)
//////////////////////////////////////////////////
// CBtnMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1999, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support. - KR
//
//           11/3/99 Fixed most common complaint. Added
//                   background color fill. - KR
//
//           11/3/99 Added support for mapping modes other than
//                   MM_TEXT as suggested by Lee Sang Hun. - KR
//
// This class implements a memory Device Context which allows
// flicker free drawing.

class CBtnMemDC : public CDC {
private:    
    CBitmap  m_bitmap;       // Offscreen bitmap
    CBitmap* m_oldBitmap;    // bitmap originally found in CBtnMemDC
    CDC*     m_pDC;          // Saves CDC passed in constructor
    CRect    m_rect;         // Rectangle of drawing area.
    BOOL     m_bMemDC;       // TRUE if CDC really is a Memory DC.
public:
    
    CBtnMemDC(CDC* pDC, const CRect* pRect = NULL) : CDC()
    {
        ASSERT(pDC != NULL); 

        // Some initialization
        m_pDC = pDC;
        m_oldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();

        // Get the rectangle to draw
        if (pRect == NULL) {
            pDC->GetClipBox(&m_rect);
        } else {
            m_rect = *pRect;
        }
        
        if (m_bMemDC) {
            // Create a Memory DC
            CreateCompatibleDC(pDC);
            pDC->LPtoDP(&m_rect);

            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_oldBitmap = SelectObject(&m_bitmap);
            
            SetMapMode(pDC->GetMapMode());
            pDC->DPtoLP(&m_rect);
            SetWindowOrg(m_rect.left, m_rect.top);
        } else {
            // Make a copy of the relevent parts of the current DC for printing
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }

        // Fill background 
        FillSolidRect(m_rect, pDC->GetBkColor());
    }

    
    ~CBtnMemDC()    
    {        
        if (m_bMemDC) {
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                this, m_rect.left, m_rect.top, SRCCOPY);            
            
            //Swap back the original bitmap.
            SelectObject(m_oldBitmap);        
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.            
            m_hDC = m_hAttribDC = NULL;
        }    
    }
    
    // Allow usage as a pointer    
    CBtnMemDC* operator->() 
    {
        return this;
    }    

    // Allow usage as a pointer    
    operator CBtnMemDC*() 
    {
        return this;
    }
};


CXPButton::CXPButton()
{
	pBoundryPen.CreatePen(PS_INSIDEFRAME|PS_SOLID,1,RGB(0,0,0));
	pInsideBoundryPenLeft.CreatePen(PS_INSIDEFRAME|PS_SOLID,3,RGB(250,196,88)); 
	pInsideBoundryPenRight.CreatePen(PS_INSIDEFRAME|PS_SOLID,3,RGB(251,202,106)); 
	pInsideBoundryPenTop.CreatePen(PS_INSIDEFRAME|PS_SOLID,2,RGB(252,210,121)); 
	pInsideBoundryPenBottom.CreatePen(PS_INSIDEFRAME|PS_SOLID,2,RGB(229,151,0)); 
	pFillActive.CreateSolidBrush(RGB(222,223,236));
	pFillInactive.CreateSolidBrush(RGB(222,223,236));
	
	pInsideBoundryPenLeftSel.CreatePen(PS_INSIDEFRAME|PS_SOLID,3,RGB(153,198,252)); 
	pInsideBoundryPenTopSel.CreatePen(PS_INSIDEFRAME|PS_SOLID,2,RGB(162,201,255));
	pInsideBoundryPenRightSel.CreatePen(PS_INSIDEFRAME|PS_SOLID,3,RGB(162,189,252));
	pInsideBoundryPenBottomSel.CreatePen(PS_INSIDEFRAME|PS_SOLID,2,RGB(162,201,255));
	
	m_bOverControl = m_bTracking = m_bSelected = Focus = 0;
}

CXPButton::~CXPButton()
{
	// TODO: Cleanup your control's instance data here.
	pBoundryPen.DeleteObject();
	
	pFillActive.DeleteObject();
	pFillInactive.DeleteObject();
	
	pInsideBoundryPenLeft.DeleteObject();
	pInsideBoundryPenRight.DeleteObject();
	pInsideBoundryPenBottom.DeleteObject();
	pInsideBoundryPenTop.DeleteObject();
	
	pInsideBoundryPenLeftSel.DeleteObject();
	pInsideBoundryPenTopSel.DeleteObject();
	pInsideBoundryPenRightSel.DeleteObject();
	pInsideBoundryPenBottomSel.DeleteObject();	
}


BEGIN_MESSAGE_MAP(CXPButton, CButton)
//{{AFX_MSG_MAP(CXPButton)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPButton message handlers

void CXPButton::DoGradientFill(CDC *pDC, CRect rect)
{
	CBrush pBrush[64];
	int nWidth = (rect.right) - (rect.left);
    int nHeight = (rect.bottom) - (rect.top);
    CRect rct;
	int i;

    for (i=0; i<64; i++)
	{
		if(m_bOverControl)
		{
			if(Focus)
			{
				pBrush[i].CreateSolidBrush(RGB(255-(i/4), 255-(i/4), 255-(i/3)));
			}
			else
			{
				pBrush[i].CreateSolidBrush(RGB(255-(i/4), 255-(i/4), 255-(i/5)));			
			}
		}
		else
		{
			if(Focus)
			{
				pBrush[i].CreateSolidBrush(RGB(255-(i/3), 255-(i/3), 255-(i/4)));
			}
			else
			{
				pBrush[i].CreateSolidBrush(RGB(255-(i/3), 255-(i/3), 255-(i/5)));
			}
		}
	}
	
	
    for (i=rect.top; i<nHeight+2; i++) 
	{
        rct.SetRect (rect.left, i, nWidth+2, i + 1);
        pDC->FillRect (&rct, &pBrush[((i * 63) / nHeight)]);
    }
	
    for (i=0; i<64; i++)
	{
		pBrush[i].DeleteObject();
	}
	
}

void CXPButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* ppDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
    CBtnMemDC pDC(ppDC);
	CRect rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;
	CPoint pt;
	CString strText;
	CString &m_title = strText;

	GetWindowText(strText);
	
    // draw the control edges (DrawFrameControl is handy!)
	
	pt.x = 10;
	pt.y = 10;
	
	pOldPen = pDC->SelectObject(&pBoundryPen);
	
	pDC->RoundRect(rect,pt);
	if(state & ODS_FOCUS)
	{
		Focus = TRUE;
		m_bSelected = TRUE;
	}
	else
	{
		m_bSelected = FALSE;
		Focus = FALSE;
	}
	
    if (state & ODS_SELECTED || state & ODS_DEFAULT   )
	{
		//pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
   		Focus = TRUE;
	}
    else
	{
		// pDC->DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);
		
	}
    
    pDC->SelectObject(pOldPen);
	// Deflate the drawing rect by the size of the button's edges
    rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));
    // Fill the interior color if necessary
	
	if (m_bOverControl)
	{
		pOldBrush = pDC->SelectObject(&pFillActive);   
		//pDC->FloodFill(rect.left+5,rect.top+5,RGB(0,0,0));  //pDC->FillSolidRect(rect, RGB(255, 255, 0)); // yellow
		DoGradientFill(pDC,rect);		
	}
	else
	{
		pOldBrush = pDC->SelectObject(&pFillInactive);   
		//pDC->FloodFill(rect.left+5,rect.top+5,RGB(0,0,0));  //pDC->FillSolidRect(rect, RGB(0, 0, 255)); // yellow
		DoGradientFill(pDC,rect);
	}
	if(m_bSelected || m_bOverControl)
	{
		DrawInsideBorder(pDC,rect);
	}
	pDC->SelectObject(pOldBrush);
    
	// Draw the text
    if (!m_title.IsEmpty()/*!strText.IsEmpty()*/)
    {
        CSize Extent = pDC->GetTextExtent(m_title/*strText*/);
        CPoint pt( rect.CenterPoint().x - Extent.cx/2, 
			rect.CenterPoint().y - Extent.cy/2 );
		
        if (state & ODS_SELECTED) 
            pt.Offset(1,1);
		
        int nMode = pDC->SetBkMode(TRANSPARENT);
        //pOldPen = pDC->SelectObject(pBoundryPen);
        //CFont *pOldFont = SelectStockFont( pDC );
		
        if (state & ODS_DISABLED)
            pDC->DrawState(pt, Extent, m_title/*strCaption*/, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
        else
            pDC->TextOut(pt.x, pt.y, m_title/*strCaption*/);
        //pDC->SelectObject(pOldPen);
		//pDC->SelectObject(pOldFont);
        pDC->SetBkMode(nMode);
    }
	
	return;
	
}

void CXPButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

void CXPButton::DrawInsideBorder(CDC *pDC, CRect rect)
{
	CPen *left, *right, *top, *bottom;
	if(m_bSelected && !m_bOverControl)
	{
		left = &pInsideBoundryPenLeftSel;
		top = &pInsideBoundryPenTopSel;
		right = &pInsideBoundryPenRightSel;
		bottom = &pInsideBoundryPenBottomSel;
	}
	else
	{
		left = &pInsideBoundryPenLeft;
		top = &pInsideBoundryPenTop;
		right = &pInsideBoundryPenRight;
		bottom = &pInsideBoundryPenBottom;
	}

	CPoint oldpoint = pDC->MoveTo(rect.left,rect.bottom-3);
	pOldPen = pDC->SelectObject(left);
	pDC->LineTo(rect.left,rect.top+2);
	pDC->SelectObject(right);
	pDC->MoveTo(rect.right-1,rect.bottom-3);
	pDC->LineTo(rect.right-1,rect.top+2);
	pDC->SelectObject(top);
	pDC->MoveTo(rect.left+2,rect.top);
	pDC->LineTo(rect.right-2,rect.top);
	pDC->SelectObject(bottom);
	pDC->MoveTo(rect.left+2,rect.bottom);
	pDC->LineTo(rect.right-2,rect.bottom);
	pDC->SelectObject(pOldPen);	
	pDC->MoveTo(oldpoint);
}

void CXPButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);		
	}	
	CButton::OnMouseMove(nFlags, point);
	
}

LRESULT CXPButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bOverControl=TRUE;
	Invalidate();
	return 1;
}

LRESULT CXPButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bOverControl = FALSE;
	Invalidate(FALSE);
	return 0;
}
