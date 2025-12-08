#pragma once

#include <QtWidgets>

class Util
{
public:
	static vtkSmartPointer< vtkPolyData> GeneratePolyDataNormal(vtkSmartPointer<vtkPolyData> polydata);
	static vtkSmartPointer<vtkPolyData> CreatePolyData(vtkSmartPointer<vtkSTLReader> reader);
	static vtkSmartPointer<vtkPolyData> CreatePolyData(vtkSmartPointer<vtkPLYReader> reader);
	static vtkSmartPointer<vtkPolyData> CreatePolyData(vtkSmartPointer<vtkOBJReader> reader);
	static vtkSmartPointer<vtkPolyData> CreatePolyData(vtkSmartPointer<vtkAppendPolyData> appender);
	static vtkSmartPointer<vtkPolyData> CreatePolyData2(vtkSmartPointer<vtkSimplePointsReader> reader);
	static vtkSmartPointer<vtkActor> CreateActor(vtkSmartPointer<vtkPolyData> polyData);
	static vtkSmartPointer<vtkActor> CreateDeepCopyActor(vtkSmartPointer<vtkPolyData> polyData);
	static vtkSmartPointer<vtkImageData> CreateImageData(std::string image);
	static vtkSmartPointer<vtkCellPicker> CreateCellPicker();
	static vtkSmartPointer<vtkPropPicker> CreatePropPicker();
	static vtkSmartPointer<vtkActor> CreateWireBoundingBox();
	static vtkSmartPointer<vtkActor> CreateBoundingBox();
	static vtkSmartPointer<vtkActor> CopyVTKActor(vtkSmartPointer<vtkActor> spVTKActor, bool shareVTKPolyData = true);
	static vtkSmartPointer<vtkProperty> GetProperty();

	static inline std::string Wcs_to_mbs(std::wstring const& str, std::locale const& loc = std::locale(""))
	{
		typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
		std::mbstate_t state;// = 0;
		std::vector<char> buf((str.size() + 1) * codecvt.max_length());
		wchar_t const* in_next = str.c_str();
		char* out_next = &buf[0];
		codecvt_t::result r;
		r = codecvt.out(state, str.c_str(), str.c_str() + str.size(), in_next, &buf[0], &buf[0] + buf.size(), out_next);
		return std::string(&buf[0]);
	}
};