
// ImageToolDoc.cpp : CImageToolDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageTool.h"
#endif

#include "ImageToolDoc.h"
#include "FileNewDlg.h"

#include <propkey.h>

#include "IppImage\IppImage.h"
#include "IppImage\IppDib.h"
#include "IppImage\IppConvert.h"
#include "IppImage\IppEnhance.h"
#include "BrightnessContrastDlg.h"
#include "GammaCorrectionDlg.h"
#include "HistogramDlg.h"
#include "ArithmeticLogicalDlg.h"
#include "IppFilter.h"
#include "GaussianDlg.h"

#define CONVERT_DIB_TO_BYTEIMAGE(m_Dib, img) \
do {\
IppByteImage img; IppDibToImage(m_Dib, img);\
} while(0);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageToolDoc

IMPLEMENT_DYNCREATE(CImageToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageToolDoc, CDocument)
	ON_COMMAND(ID_WINDOW_DUPLICATE, &CImageToolDoc::OnWindowDuplicate)
	ON_COMMAND(ID_EDIT_COPY, &CImageToolDoc::OnEditCopy)
	ON_COMMAND(ID_IMAGE_INVERSE, &CImageToolDoc::OnImageInverse)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_INVERSE, &CImageToolDoc::OnUpdateImageInverse)
	ON_COMMAND(ID_BRIGHTNESS_CONTRAST, &CImageToolDoc::OnBrightnessContrast)
	ON_COMMAND(ID_GAMMA_CORRECTION, &CImageToolDoc::OnGammaCorrection)
	ON_COMMAND(ID_VIEW_HISTOGRAM, &CImageToolDoc::OnViewHistogram)
	ON_COMMAND(ID_HISTO_STRETCHING, &CImageToolDoc::OnHistoStretching)
	ON_COMMAND(ID_HISTO_EQUALIZATION, &CImageToolDoc::OnHistoEqualization)
	ON_COMMAND(ID_ARITHMETIC_LOGICAL, &CImageToolDoc::OnArithmeticLogical)
	ON_COMMAND(ID_BITLANE_SLICING, &CImageToolDoc::OnBitlaneSlicing)
	ON_COMMAND(ID_FILTER_MEAN, &CImageToolDoc::OnFilterMean)
	ON_COMMAND(ID_FILTER_WEIGHTED_MEAN, &CImageToolDoc::OnFilterWeightedMean)
	ON_COMMAND(ID_FILTER_GAUSSIAN, &CImageToolDoc::OnFilterGaussian)
END_MESSAGE_MAP()


// CImageToolDoc 생성/소멸

CImageToolDoc::CImageToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CImageToolDoc::~CImageToolDoc()
{
}

BOOL CImageToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	BOOL ret = TRUE;
	
	if (theApp.m_pNewDib == NULL) {
		CFileNewDlg dlg;
		if (dlg.DoModal() == IDOK) {
			if (dlg.m_nType == 0)
				ret = m_Dib.CreateGrayBitmap(dlg.m_nWidth, dlg.m_nHeight);
			else
				ret = m_Dib.CreateRgbBitmap(dlg.m_nWidth, dlg.m_nHeight);
		}
		else {
			ret = FALSE;
		}
	}
	else {
		m_Dib = *(theApp.m_pNewDib);
		theApp.m_pNewDib = NULL;
	}
	return ret;
}




// CImageToolDoc serialization

void CImageToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImageToolDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CImageToolDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageToolDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageToolDoc 진단

#ifdef _DEBUG
void CImageToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageToolDoc 명령


BOOL CImageToolDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CT2A filename(lpszPathName);
	BOOL res = m_Dib.Load(filename);
	if (res)
		AfxPrintInfo(_T("[파일 열기] 파일 경로: %s, 가로 크기: %d픽셀, 세로 크기: %d픽셀, 색상 수: %d"), lpszPathName, m_Dib.GetWidth(), m_Dib.GetHeight(), 0x01 << m_Dib.GetBitCount());
	return res;
}


BOOL CImageToolDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CT2A filename(lpszPathName);
	return m_Dib.Save(filename);
}


void CImageToolDoc::OnWindowDuplicate()
{
	AfxNewBitmap(m_Dib);
}


void CImageToolDoc::OnEditCopy()
{
	if (m_Dib.IsValid())
		m_Dib.CopyToClipboard();
}


void CImageToolDoc::OnImageInverse()
{
	IppByteImage img;
	IppDibToImage(m_Dib, img);
	IppInverse(img);

	IppDib dib;
	IppImageToDib(img, dib);

	AfxPrintInfo(_T("[반전] 입력 영상: %s"), GetTitle());
	AfxNewBitmap(dib);
}


void CImageToolDoc::OnUpdateImageInverse(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_Dib.GetBitCount() == 8);
}


void CImageToolDoc::OnBrightnessContrast()
{
	CBrightnessContrastDlg dlg;
	if (dlg.DoModal() == IDOK) {
		IppByteImage img;
		IppDibToImage(m_Dib, img);

		IppBrightness(img, dlg.m_nBrightness);
		IppContrast(img, dlg.m_nContrast);

		IppDib dib;
		IppImageToDib(img, dib);

		AfxPrintInfo(_T("[밝기/명암비 조절] 입력 영상: %s, 밝기: %d, 명암비: %d%%"), GetTitle(), dlg.m_nBrightness, dlg.m_nContrast);
		AfxNewBitmap(dib);
	}
}


void CImageToolDoc::OnGammaCorrection()
{
	CGammaCorrectionDlg dlg;
	if (dlg.DoModal() == IDOK) {
		IppByteImage img;
		IppDibToImage(m_Dib, img);
		IppGammaCorrection(img, dlg.m_fGamma);

		IppDib dib;
		IppImageToDib(img, dib);

		AfxPrintInfo(_T("[감마 보정] 입력 영상: %s, 감마: %4.2f"), GetTitle(), dlg.m_fGamma);
		AfxNewBitmap(dib);
	}
}


void CImageToolDoc::OnViewHistogram()
{
	CHistogramDlg dlg;
	dlg.SetImage(&m_Dib);
	dlg.DoModal();
}


void CImageToolDoc::OnHistoStretching()
{
	IppByteImage img;
	IppDibToImage(m_Dib, img);

	IppHistogramStretching(img);

	IppDib dib;
	IppImageToDib(img, dib);

	AfxPrintInfo(_T("[히스토그램 스트레칭] 입력 영상: %s"), GetTitle());
	AfxNewBitmap(dib);
}


void CImageToolDoc::OnHistoEqualization()
{
	IppByteImage img;
	IppDibToImage(m_Dib, img);

	IppHistogramEqualization(img);

	IppDib dib;
	IppImageToDib(img, dib);

	AfxPrintInfo(_T("[히스토그램 균등화] 입력 영상: %s"), GetTitle());
	AfxNewBitmap(dib);
}



void CImageToolDoc::OnArithmeticLogical()
{
	CArithmeticLogicalDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CImageToolDoc* pDoc1 = (CImageToolDoc*)dlg.m_pDoc1;
		CImageToolDoc* pDoc2 = (CImageToolDoc*)dlg.m_pDoc2;

		IppByteImage img1, img2, img3;
		IppDibToImage(pDoc1->m_Dib, img1);
		IppDibToImage(pDoc2->m_Dib, img2);

		bool ret = false;

		switch (dlg.m_nFunction) {
		case 0:ret = IppAdd(img1, img2, img3); break;
		case 1:ret = IppSub(img1, img2, img3); break;
		case 2:ret = IppAve(img1, img2, img3); break;
		case 3:ret = IppDiff(img1, img2, img3); break;
		case 4:ret = IppAND(img1, img2, img3); break;
		case 5:ret = IppOR(img1, img2, img3); break;
		}

		if (ret) {
			IppDib dib;
			IppImageToDib(img3, dib);

			TCHAR* op[] = { _T("덧셈"), _T("뺄셈"), _T("평균"), _T("차이"), _T("논리 AND"), _T("논리 OR") };
			AfxPrintInfo(_T("[산술 및 논리 연산] [%s] 입력영상 #1: %s, 입력영상 #2: %s"), op[dlg.m_nFunction], pDoc1->GetTitle(), pDoc2->GetTitle());
			AfxNewBitmap(dib);
		}
		else {
			AfxMessageBox(_T("영상의 크기가 다릅니다"));
		}
	}
}


void CImageToolDoc::OnBitlaneSlicing()
{
	IppByteImage img, imgPlane;
	IppDib dib;
	IppDibToImage(m_Dib, img);

	for (int i = 0; i < 8; i++)
	{
		IppBitPlane(img, imgPlane, i);
		IppImageToDib(imgPlane, dib);

		AfxNewBitmap(dib);
	}
	AfxPrintInfo(_T("[비트 평면 분할] 입력 영상: %s"), GetTitle());
}


void CImageToolDoc::OnFilterMean()
{
	IppByteImage imgSrc, imgDst;
	IppDibToImage(m_Dib, imgSrc);
	IppFilterMean(imgSrc, imgDst);

	IppDib dib;
	IppImageToDib(imgDst, dib);

	AfxPrintInfo(_T("[평균 값 필터] 입력 영상: %s"), GetTitle());
	AfxNewBitmap(dib);
}


void CImageToolDoc::OnFilterWeightedMean()
{
	IppByteImage imgSrc, imgDst;
	IppDibToImage(m_Dib, imgSrc);
	IppFilterWeightedMean(imgSrc, imgDst);

	IppDib dib;
	IppImageToDib(imgDst, dib);

	AfxPrintInfo(_T("[가중 평균 값 필터] 입력 영상: %s"), GetTitle());
	AfxNewBitmap(dib);
}


void CImageToolDoc::OnFilterGaussian()
{
	CGaussianDlg dlg;
	if (dlg.DoModal() == IDOK) {
		IppByteImage imgSrc;
		IppFloatImage imgDst;
		IppDibToImage(m_Dib, imgSrc);
		IppFilterGaussian(imgSrc, imgDst, dlg.m_fSigma);

		IppDib dib;
		IppImageToDib(imgDst, dib);

		AfxPrintInfo(_T("[가우시안 값 필터] 입력 영상: %s, Sigma: %4.2f"), GetTitle(), dlg.m_fSigma);
		AfxNewBitmap(dib);
	}
}
