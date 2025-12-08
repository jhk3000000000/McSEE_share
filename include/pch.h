#pragma once

// ====================================================================
// VTK 모듈 초기화 (main.cpp로 옮기는 것이 더 좋지만, 여기 있어도 동작합니다)
// ====================================================================
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);

// ====================================================================
// C++ 표준 라이브러리
// ====================================================================
#include <iostream>
#include <map>
#include <stdio.h>
#include <io.h>

// ====================================================================
// 외부 라이브러리 헤더
// ====================================================================
// --- Eigen ---
#include <Eigen/Dense>

// --- Qt5 ---
#include <QPainterPath>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPoint>
#include <QPen>
#include <QPainter>
#include <QtWidgets> // QApplication, QWidget 등 대부분의 위젯 포함

// --- VTK ---
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkLine.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkMatrix4x4.h>
#include <vtkProperty2D.h>
#include <vtkTransform.h>
#include <vtkCellPicker.h>
#include <vtkProperty.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkImageData.h>
#include <vtkQImageToImageSource.h>
#include <vtkImageActor.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyLine.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkPropPicker.h>
#include <vtkAxesActor.h>
#include <vtkSimplePointsReader.h>
#include <vtkPLYReader.h>
#include <vtkOBJReader.h>

#include <vtkPointSource.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkPCANormalEstimation.h>
#include <vtkContourFilter.h>
#include <vtkReverseSense.h>
#include <vtkTriangleFilter.h>
#include <vtkSTLWriter.h>
#include <vtkAppendPolyData.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkLineSource.h>
#include <vtkActor2DCollection.h>

#include <vtkOpenGLProperty.h>
#include <vtkShaderProgram.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkBoundingBox.h>

#include <vtkPlaneCollection.h>
#include <vtkPlane.h>
#include <vtkPixelBufferObject.h>

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

#include <vtkBMPWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>
#include <vtkImageResize.h>

#include <vtkPlaneSource.h>
#include <vtkNamedColors.h>
#include <vtkCylinderSource.h>
#include <vtkCutter.h>
#include <vtkStripper.h>
#include <vtkTubeFilter.h>
#include <vtkTriangleMeshPointNormals.h>
#include <vtkOBJImporter.h>
#include <vtkLODActor.h>
#include <vtkLODProp3D.h>
#include <vtkQuadricLODActor.h>
#include <vtkCallbackCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkBMPReader.h>
#include <vtkTexture.h>
#include <vtkDataSetSurfaceFilter.h>

// --- Windows 전용 헤더 ---
#include "atlstr.h"
#include <comutil.h>

// ====================================================================
// 프로젝트 내부 헤더
// ====================================================================
//#include "StyleSheet.h"
#include "StyleSheet.h"

// ====================================================================
// 공용 매크로 정의
// ====================================================================
#define RELEASE(ptr)   if(ptr)  {(ptr)->Release(); (ptr) = 0;}
#define SAFE_RELEASE(comPointer) if (comPointer) { (comPointer)->Release(); (comPointer) = NULL; }
#define SAFE_DELETE(comPointer) if (comPointer) { delete(comPointer); (comPointer) = NULL; }

inline float RADIAN(int x) { return float(x)*0.017453292519943295769236907684885f; }
inline float RADIAN(float x) { return (x)*0.017453292519943295769236907684885f; }
inline double RADIAN(double x) { return (x)*0.017453292519943295769236907684885; }
inline float DEGREE(float x) { return (x)*57.295779513082320876798154814114f; }
inline double DEGREE(double x) { return (x)*57.295779513082320876798154814114; }

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif