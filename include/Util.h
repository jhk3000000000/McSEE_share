#pragma once

#include <QtWidgets>

struct SkinDoseData {
	qint32 phantomId;
	double x;
	double y;
	double z;
	double dE;
};


struct BodySizeInfo {
	QString HtWtName;
	QString HtWtName_dummy;

	double xyScale =1;
	double zScale =1;

	double xyScale_dummy = 1;
	double zScale_dummy = 1;

};

struct Point_SolidAngle {
	double theta, phi, x, y, z;
};

class Vector3 {
public:
	double x;
	double y;
	double z;

	// Constructor
	Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

	// Copy constructor
	Vector3(const Vector3& other) : x(other.x), y(other.y), z(other.z) {}

	// Assignment operator
	Vector3& operator=(const Vector3& other) {
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	// Dot product
	double dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// Cross product
	Vector3 cross(const Vector3& other) const {
		return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	// Magnitude
	double magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	// Normalize
	Vector3 normalize() const {
		double mag = magnitude();
		if (mag == 0.0) {
			return Vector3(0.0, 0.0, 0.0);
		}
		else {
			return Vector3(x / mag, y / mag, z / mag);
		}
	}
};


namespace SkinDoseBinaryUtils
{
	void initializeBinaryFile(const QString& binaryFilePath);
	bool appendFromTextFile(const QString& textFilePath, const QString& binaryFilePath);
	std::pair<std::pair<qint64, double>, std::vector<SkinDoseData>> readAllData(const QString& binaryFilePath);
}

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