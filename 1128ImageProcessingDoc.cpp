
// ImageProcessingDoc.cpp : Implement a CImageProcessingDoc Class
//

#include "stdafx.h"
#include "ImageProcessing.h"

#include "DlgBrightnessOption.h"
#include "DlgMosaicOption.h"
#include "DlgCompositeOption.h"
#include "DlgIntensityTransformOption.h"
#include "DlgContrastStretchOption.h"

#include "ImageProcessingDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	ON_COMMAND(ID_PROCESS_MOSAIC, &CImageProcessingDoc::OnProcessMosaic)
	ON_COMMAND(ID_PROCESS_BRIGHTNESS, &CImageProcessingDoc::OnProcessBrightness)
	ON_COMMAND(ID_PROCESS_COMPOSITE, &CImageProcessingDoc::OnProcessComposite)
	ON_COMMAND(ID_PROCESS_INTENSITY_TRANSFORM, &CImageProcessingDoc::OnProcessIntensityTransform)
	ON_COMMAND(ID_PROCESS_CONTRAST_STRETCH, &CImageProcessingDoc::OnProcessContrastStretch)
	ON_COMMAND(ID_PROCESS_EQUALIZATION, &CImageProcessingDoc::OnProcessEqualization)
	ON_COMMAND(ID_32783, &CImageProcessingDoc::OnHistogramSpecification)
END_MESSAGE_MAP()


// CImageProcessingDoc Contruction/Destuction

CImageProcessingDoc::CImageProcessingDoc()
{
	//// TODO: Add an one-time generating code here
	m_pImage = NULL;
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	if (NULL != m_pImage)
		delete m_pImage;
}

BOOL CImageProcessingDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO: load imagefile // DONE
	m_pImage = new CxImage;
	m_pImage->Load(lpszPathName, FindType(lpszPathName));

	CalculateHistogram();

	return TRUE;
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//// TODO: Add a re-initialization code here
	//// SDI documents will reuse this article

	return TRUE;
}




// CImageProcessingDoc serialization

void CImageProcessingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//// TODO: Add a saving code here
	}
	else
	{
		//// TODO: Add a loading code here
	}
}


// CImageProcessingDoc diagnosis

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessingDoc command

CString CImageProcessingDoc::FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (name[i] == '.') {
			return name.Mid(i + 1);
		}
	}
	return CString(_T(""));
}

CString CImageProcessingDoc::RemoveExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (name[i] == '.') {
			return name.Mid(0, i);
		}
	}
	return name;
}

int CImageProcessingDoc::FindType(const CString& ext)
{
	return CxImage::GetTypeIdFromName(ext);
}

void CImageProcessingDoc::CalculateHistogram()
{
	// TODO: Add a calculating histogram code here
	if (m_pImage) {
		// Histogram function, which is implemented in Cximage
		//m_histogramMax = m_pImage->Histogram(m_histogramRed, m_histogramGreen, m_histogramBlue, m_histogramGray);


		// 가짜코드
		for (int i = 0; i < 256; i++) {
			m_histogramRed[i] = rand() % 200;
			m_histogramGreen[i] = rand() % 200;
			m_histogramBlue[i] = rand() % 200;
			m_histogramGray[i] = rand() % 200;
		}

		m_histogramMax = 0;
		for (int i = 0; i < 256; i++) {
			m_histogramMax = max(m_histogramMax, m_histogramRed[i]);
			m_histogramMax = max(m_histogramMax, m_histogramGreen[i]);
			m_histogramMax = max(m_histogramMax, m_histogramBlue[i]);
			m_histogramMax = max(m_histogramMax, m_histogramGray[i]);
		}
		//////////////////////////////////////////////////////////////
	}
}

void CImageProcessingDoc::OnProcessBrightness()
{
	// TODO: Add a changing the brightness histogram code here
	if (m_pImage) {
		DlgBrightnessOption dlg;

		if (dlg.DoModal() == IDOK) {
			// write your own code
			// for applying your effect, you must use m_pImage
			// this code is a simple example for manufacturing image : grayscaling

			int nPlusMinus = dlg.m_nPlusMinus;
			BYTE byModifyValue = dlg.m_byModifyValue;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD color;
			RGBQUAD newcolor;

			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {
					color = m_pImage->GetPixelColor(x, y);

					newcolor.rgbBlue = (BYTE)RGB2GRAY(color.rgbRed, color.rgbGreen, color.rgbBlue);
					newcolor.rgbGreen = (BYTE)RGB2GRAY(color.rgbRed, color.rgbGreen, color.rgbBlue);
					newcolor.rgbRed = (BYTE)RGB2GRAY(color.rgbRed, color.rgbGreen, color.rgbBlue);

					m_pImage->SetPixelColor(x, y, newcolor);
				}
			}

			// code to view overflow, Keep observation with the debugger
			BYTE a = 255;
			BYTE b = 1;
			int  c = 10;
			BYTE r;

			r = a + b;
			r = a + c;
			r = b + c;
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessMosaic()
{
	// TODO: Add a mosaic code here
	if (m_pImage) {
		DlgMosaicOption dlg;

		if (dlg.DoModal() == IDOK) {
			// write your own code
			// for applying your effect, you must use m_pImage
			// this code is a simple example for manufacturing image : grayscaling

			DWORD dwWindowSize = dlg.m_dwWindowSize;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();

			DWORD XStep;
			DWORD YStep;
			int x, y;
			int i, j;
			int color_diff[3];
			int brightness;
			int compare = 0;
			int diff[4][3];
			int diffx[4] = { 0, 0, 1, -1 };
			int diffy[4] = { 1, -1, 0, 0 };
			unsigned long long CIx = 0;
			unsigned long long CIy = 0;
			unsigned long long CIp = 0;
			DWORD xIndex, yIndex;
			DWORD realSize;

			RGBQUAD color;
			RGBQUAD newcolor;

			DWORD tempRed, tempGreen, tempBlue;

			CxImage * buffer = new CxImage;
			buffer->Create(width, height, 24, CXIMAGE_FORMAT_BMP);

			/*
			RGBQUAD ** buffer;
			buffer = (RGBQUAD**)malloc(sizeof(RGBQUAD*) * height);
			buffer[0] = (RGBQUAD*)malloc(sizeof(RGBQUAD) * width * height);
			for (x = 1; x < height; x++) {
			buffer[x] = buffer[x - 1] + width;
			}
			*/
			if (dwWindowSize == 0) {
				for (YStep = 0; YStep < height; YStep++) {
					for (XStep = 0; XStep < width; XStep++) {
						color = m_pImage->GetPixelColor(XStep, YStep);
						color_diff[0] = color.rgbBlue - color.rgbGreen;
						if (color_diff[0] < 0) {
							color_diff[0] *= -1;
						}
						color_diff[1] = color.rgbGreen - color.rgbRed;
						if (color_diff[1] < 0) {
							color_diff[1] *= -1;
						}
						color_diff[2] = color.rgbRed - color.rgbBlue;
						if (color_diff[2] < 0) {
							color_diff[2] *= -1;
						}
						brightness = (color.rgbBlue + color.rgbGreen + color.rgbRed) / 3;
						if (XStep == width - 20 && YStep == height / 2) {
							i = 1;
						}

						if (color_diff[0] + color_diff[1] + color_diff[2] < 20 && brightness > 150) {
							newcolor.rgbBlue = color.rgbBlue;
							newcolor.rgbGreen = color.rgbGreen;
							newcolor.rgbRed = color.rgbRed;
							m_pImage->SetPixelColor(XStep, YStep, newcolor);
							CIx += XStep;
							CIy += YStep;
							CIp++;
						}
						else {
							newcolor.rgbBlue = color.rgbBlue;
							newcolor.rgbGreen = color.rgbGreen;
							newcolor.rgbRed = color.rgbRed;
							m_pImage->SetPixelColor(XStep, YStep, newcolor);
						}/*

						color = buffer->GetPixelColor(XStep, YStep);
						for (i = 0; i < 4; i++) {
							for (j = 1; j <= 20; j++) {
								x = XStep + diffx[i] * j;
								y = YStep + diffy[i] * j;
								if (x >= width || x < 0 || y >= height || y < 0) {
									continue;
								}
								newcolor = buffer->GetPixelColor(x, y);
								diff[i][0] = color.rgbBlue - newcolor.rgbBlue;
								if (diff[i][0] < 0) {
									diff[i][0] *= -1;
								}
								diff[i][1] = color.rgbGreen - newcolor.rgbGreen;
								if (diff[i][1] < 0) {
									diff[i][1] *= -1;
								}
								diff[i][2] = color.rgbRed - newcolor.rgbRed;
								if (diff[i][2] < 0) {
									diff[i][2] *= -1;
								}
								//diff[i][3] = color.rgbBlue - newcolor.rgbBlue;
								if (diff[i][0] + diff[i][1] + diff[i][2] > 100) {
									compare = 1;
									break;
								}
							}
							if (compare == 1) {
								break;
							}
						}
						if (compare == 0) {
							newcolor.rgbBlue = 255;
							newcolor.rgbGreen = 255;
							newcolor.rgbRed = 255;
							m_pImage->SetPixelColor(XStep, YStep, newcolor);
						}*/
					}
				}
				CIx /= CIp;
				CIy /= CIp;
				for (YStep = 0; YStep < height; YStep++) {
					for (XStep = 0; XStep < width; XStep++) {
						if ((XStep > CIx - 55 && XStep < CIx - 50) || (XStep > CIx + 50 && XStep < CIx + 55)) {
							if (YStep > CIy - 55 && YStep < CIy + 55) {
								newcolor.rgbBlue = 0;
								newcolor.rgbGreen = 0;
								newcolor.rgbRed = 255;
								m_pImage->SetPixelColor(XStep, YStep, newcolor);
							}
						}
						if ((YStep > CIy - 55 && YStep < CIy - 50) || (YStep > CIy + 50 && YStep < CIy + 55)) {
							if (XStep > CIx - 55 && XStep < CIx + 55) {
								newcolor.rgbBlue = 0;
								newcolor.rgbGreen = 0;
								newcolor.rgbRed = 255;
								m_pImage->SetPixelColor(XStep, YStep, newcolor);
							}
						}
					}
				}
			}
			/*
			if (dwWindowSize == 0) { // Blurring
			for (YStep = 0; YStep < height; YStep++) {
			for (XStep = 0; XStep < width; XStep++) {
			tempRed = 0;
			tempGreen = 0;
			tempBlue = 0;
			realSize = 9;
			for (y = -1; y <= 1; y++) {
			for (x = -1; x <= 1; x++) {
			xIndex = XStep + x;
			yIndex = YStep + y;
			if (yIndex >= height || yIndex < 0) {
			realSize--;
			continue;
			}
			else if (xIndex >= width || xIndex < 0) {
			realSize--;
			continue;
			}
			color = buffer->GetPixelColor(xIndex, yIndex);
			tempRed += (DWORD)color.rgbRed;
			tempGreen += (DWORD)color.rgbGreen;
			tempBlue += (DWORD)color.rgbBlue;
			}
			}
			tempRed /= realSize;
			tempGreen /= realSize;
			tempBlue /= realSize;

			newcolor.rgbRed = (BYTE)tempRed;
			newcolor.rgbGreen = (BYTE)tempGreen;
			newcolor.rgbBlue = (BYTE)tempBlue;

			m_pImage->SetPixelColor(XStep, YStep, newcolor);
			}
			}
			}*/
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessComposite()
{
	// TODO: Add a composite code here
	if (m_pImage) {
		DlgCompositeOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nOperatorID = dlg.GetCompositeOperatorID(); // 0 ADD, 1 SUB
			int addORsub;
			if (nOperatorID == 0) {
				addORsub = 1;
			}
			else if (nOperatorID == 1) {
				addORsub = -1;
			}
			CxImage * pSecondImage = dlg.GetSecondImage();

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD firstColor;
			RGBQUAD secondColor;
			RGBQUAD newColor;
			float tempBlue, tempGreen, tempRed, tempGray;
			int bx, by;
			int tempSize;
			int diff;
			float brightScale = 1;
			float firstBright;
			float secondBright;
			int isCigar = 0;
			int boundary[3];

			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {

					diff = 0;
					firstColor = m_pImage->GetPixelColor(x, y);
					tempBlue = firstColor.rgbBlue;
					tempGreen = firstColor.rgbGreen;
					tempRed = firstColor.rgbRed;
					
					secondColor = pSecondImage->GetPixelColor(x, y);
					firstBright = tempBlue + tempGreen + tempRed;
					secondBright = secondColor.rgbBlue + secondColor.rgbGreen + secondColor.rgbRed;
					
					//if ((abs(secondColor.rgbBlue - secondColor.rgbGreen) + abs(secondColor.rgbGreen - secondColor.rgbRed) + abs(secondColor.rgbRed - secondColor.rgbBlue)) > 20) {
						brightScale = firstBright / secondBright;
					//}

					tempBlue = abs(secondColor.rgbBlue * brightScale - tempBlue);
					tempGreen = abs(secondColor.rgbGreen * brightScale - tempGreen);
					tempRed = abs(secondColor.rgbRed * brightScale - tempRed);
					tempGray = tempBlue + tempGreen + tempRed;
					
					if (tempBlue > 10) {
						diff = 1;
					}
					else if (tempGreen > 10) {
						diff = 1;
					}
					else if (tempRed > 10) {
						diff = 1;
					}
					else if (tempGray > 60) {
						diff = 1;
					}
					if (diff == 1) {
						newColor.rgbBlue = secondColor.rgbBlue;
						newColor.rgbGreen = secondColor.rgbGreen;
						newColor.rgbRed = secondColor.rgbRed;
					}
					else {
						newColor.rgbBlue = 0;
						newColor.rgbGreen = 0;
						newColor.rgbRed = 0;
					}

					m_pImage->SetPixelColor(x, y, newColor);
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessIntensityTransform()
{
	// TODO: Add a Contrast conversion code here
	if (m_pImage) {
		DlgIntensityTransformOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nTransformType = dlg.m_nTransformType;
			double nLinearTransfromCoefficient_a = dlg.m_nLinearTransfromCoefficient_a;
			double nLinearTransfromCoefficient_b = dlg.m_nLinearTransfromCoefficient_b;

			int byThresholdValue = dlg.m_byThresholdValue;

			BYTE byContrastStretching_Start = dlg.m_byContrastStretching_Start;
			BYTE byContrastStretching_End = dlg.m_byContrastStretching_End;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD color;
			RGBQUAD newcolor;
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessContrastStretch()
{
	// TODO: Add a Contrast stretching code here
	if (m_pImage) {
		DlgContrastStretchOption dlg;

		if (dlg.DoModal() == IDOK) {
			int nStretchType = dlg.m_nStretchType;
			float fLow = dlg.m_fLow;
			float fHigh = dlg.m_fHigh;

			DWORD width = m_pImage->GetWidth();
			DWORD height = m_pImage->GetHeight();
			RGBQUAD color;
			RGBQUAD newcolor;

			BYTE LUT[256];

			// initialize thresholds
			int nLowTherhs = 0;
			int nHighThresh = 255;


			// compute thresholds
			if (nStretchType == 0) { // Auto

			}
			else if (nStretchType == 1) { // Ends-In

			}


			// compute LUT


			// trnasfer image
			for (DWORD y = 0; y < height; y++) {
				for (DWORD x = 0; x < width; x++) {
					color = m_pImage->GetPixelColor(x, y);

					// using LUT

					m_pImage->SetPixelColor(x, y, newcolor);
				}
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}

void CImageProcessingDoc::OnProcessEqualization()
{
	// TODO: Add a Histogram equalization code here
	if (m_pImage)
	{

		DWORD width = m_pImage->GetWidth();
		DWORD height = m_pImage->GetHeight();
		RGBQUAD color;	// Save the current color value
		RGBQUAD newcolor;	// After the conversion, save the color value

							//(1) Histogram has already been generated histogram(Omission) 
							//(2) Create a cumulative histogram
		DWORD sum = 0;
		float scale_factor = 255.0 / (width*height);
		DWORD sum_hist[256]; // a cumulative histogram

		for (int i = 0; i < 256; i++)
		{
			// (Coding)

		}


		//(3) Calculating look-up table
		BYTE LUT[256]; // look-up table

		for (int i = 0; i < 256; i++)
		{
			// (Coding)  


		}

		// (4) Image Conversion
		for (DWORD y = 0; y < height; y++)
		{
			for (DWORD x = 0; x < width; x++)
			{
				color = m_pImage->GetPixelColor(x, y);
				// (Coding)



				m_pImage->SetPixelColor(x, y, newcolor);
			}
		}
	}

	CalculateHistogram();
	UpdateAllViews(NULL);
}



void CImageProcessingDoc::OnHistogramSpecification()
{
	// TODO: Add a Histogram specifications code here
	if (m_pImage)
	{

		DWORD width = m_pImage->GetWidth();
		DWORD height = m_pImage->GetHeight();
		RGBQUAD color;	// Save the current color value
		RGBQUAD newcolor;	// After the conversion, save the color value

							//(1) Histogram has already been generated histogram(Omission) 
							//(2) Create a cumulative histogram
		DWORD sum = 0;
		float scale_factor = 255.0 / (width*height);
		DWORD sum_hist[256]; // a cumulative histogram

		for (int i = 0; i < 256; i++)
		{
			// (coding)

		}


		//(3) Calculating look-up table
		BYTE LUT[256]; // look-up table

		for (int i = 0; i < 256; i++)
		{
			// (coding)  


		}


		// Same as histogram equalization code from here.
		// However, After a cumulative histogram convert to float sum_hist[256] and
		// modifing to be entered in the real number on calculating sum_hist


		//(4) 
		DWORD desired_histogram[256];
		// Making desired_histogram
		/*
		//1.
		for (int i=0; i<=255; i++)
		{
		desired_histogram[i]=i;
		}
		*/
		/*
		//2.
		for (int i=0; i<=255; i++)
		{
		desired_histogram[i]=255-i;
		}
		*/
		/*
		//3.
		for (int i=0; i<=127; i++)
		{
		desired_histogram[i]=127-i;
		}
		for (int i=128; i<=255; i++)
		{
		desired_histogram[i]=i-127;
		}
		*/

		// redefinition
		sum = 0;
		scale_factor = 255.0 / (width*height);

		//(5) Create the cumulative histogram of histogram you want
		for (int i = 0; i < 256; i++)
		{
			//(coding)



		}

		//(6) Is the inverse.
		float difference;	// calculate the difference
		int min;	// Calculate the minimum of the difference
		DWORD inv_hist[256];	// reverse-histogram

								//(Hint) will use dual-route.
								// float fabs() Use library functions.
		for (int i = 0; i < 256; i++)
		{
			//(coding)




		}

		//(7) Create look-up table of reverse-histogram
		for (int i = 0; i < 256; i++)
		{
			//(coding)

		}



		// (8) Image Conversion
		for (DWORD y = 0; y < height; y++)
		{
			for (DWORD x = 0; x < width; x++)
			{
				color = m_pImage->GetPixelColor(x, y);
				// (coding)



				m_pImage->SetPixelColor(x, y, newcolor);
			}
		}
	} // if(m_pImage)

	CalculateHistogram();
	UpdateAllViews(NULL);



}