#pragma once

#include <QtWidgets>

struct SkinDoseData {
	qint32 phantomId;
	double x;
	double y;
	double z;
	double dE;
};

struct PhantomConstants {
    static const int const_Phantom_MaximumCount = 5;
    static const int const_Clothing_MaximumCount = 10;
    static const int const_ClothingLayer_MaximumCount = 5;
    static const int m_Dosimeter_Maximum_Count = 20; 
};
enum PhantomMainInfo {
    E_PHANTOMMAININFO_CATEGORY = 0, // Human phantom: 0, Air phantom: 1
    E_PHANTOMMAININFO_GENDER = 1, // Male: 1 , Female: 2
    E_PHANTOMMAININFO_TYPE = 2, // MRCP: 0, Deformable: 1, Imported: 2
    E_PHANTOMMAININFO_POSTURE = 3, // Default: 0, Bending: 1, Kneeling: 2, Sitting: 3, Squatting: 4, Walking: 5
    E_PHANTOMMAININFO_HEIGHT = 4,
    E_PHANTOMMAININFO_WEIGHT = 5,
    E_PHANTOMMAININFO_POS_X = 6,
    E_PHANTOMMAININFO_POS_Y = 7,
    E_PHANTOMMAININFO_POS_Z = 8,
    E_PHANTOMMAININFO_ROT_X = 9,
    E_PHANTOMMAININFO_ROT_Y = 10,
    E_PHANTOMMAININFO_ROT_Z = 11,
    E_PHANTOMMAININFO_AGE = 12, // Adult(35y): 0, 15y: 1, 10y: 2, 5y: 3, 1y: 4, 0y: 5
    E_PHANTOMMAININFO_DUMMY = 13,
    E_PHANTOMMAININFO_FETALAGE = 14 // for PFMRCP phantom only: 0: 8w, 1: 10w, 2:15w, 3:20w, 4:25w, 5:30w, 6:35w, 7:38w  
};

enum PhantomCategory {
    E_PHANTOMCATEGORY_HUMAN = 0,
    E_PHANTOMCATEGORY_AIR = 1
};

enum PhantomGender {
    E_PHANTOMGENDER_MALE = 0,
    E_PHANTOMGENDER_FEMALE = 1
};

enum PhantomType {
    E_PHANTOMTYPE_ADULTMRCP = 0,
    E_PHANTOMTYPE_PEDIATRICMRCP = 1,
    E_PHANTOMTYPE_PFMRCP = 2,
    E_PHANTOMTYPE_TRANSFORMED = 3,
    E_PHANTOMTYPE_IMPORTED = 4
};

enum PhantomPosture {
    E_PHANTOMPOSTURE_STANDING = 0,
    E_PHANTOMPOSTURE_BENDING = 1,
    E_PHANTOMPOSTURE_KNEELING = 2,
    E_PHANTOMPOSTURE_SITTING = 3,
    E_PHANTOMPOSTURE_SQUATTING = 4,
    E_PHANTOMPOSTURE_WALKING = 5,
};

enum PhantomAge {
    E_PHANTOMAGE_ADULT = 0,
    E_PHANTOMAGE_15y = 1,
    E_PHANTOMAGE_10y = 2,
    E_PHANTOMAGE_5y = 3,
    E_PHANTOMAGE_1y = 4,
    E_PHANTOMAGE_0y = 5,
};

enum PhantomFetalAge {
    E_PHANTOMFETALAGE_8w = 0,
    E_PHANTOMFETALAGE_10w = 1,
    E_PHANTOMFETALAGE_15w = 2,
    E_PHANTOMFETALAGE_20w = 3,
    E_PHANTOMFETALAGE_25w = 4,
    E_PHANTOMFETALAGE_30w = 5,
    E_PHANTOMFETALAGE_35w = 6,
    E_PHANTOMFETALAGE_38w = 7
};

enum PhantomDummy {
    E_PHANTOMDUMMY_NO = 0,
    E_PHANTOMDUMMY_YES = 1
};

enum ClothingMainInfo {
    E_CLOTHINGMAININFO_THICKNESS = 0,
    E_CLOTHINGMAININFO_DENSITY = 1,
    E_CLOTHINGMAININFO_COMPOSITION = 2
};

enum ClothingMainInfo_Composition {
    E_CLOTHINGMAININFO_COMPOSITION_CLOTH = 0,
    E_CLOTHINGMAININFO_COMPOSITION_LEAD = 1,
    E_CLOTHINGMAININFO_COMPOSITION_AIR = 2
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