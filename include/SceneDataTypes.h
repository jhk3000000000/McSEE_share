#pragma once

#include <QString>
#include <QVector>
#include <qmetatype.h>

// ======================================================================
// JSON Key 상수 정의 (파일 형식의 명세 역할)
// ======================================================================
namespace JsonKeys {
	// 최상위 객체 키
	constexpr const char* PhantomPanel = "phantom";
	constexpr const char* GeometryPanel = "geometry";
	// ToDo: 향후 추가될 다른 패널 키 (e.g., Geometry, Source...)

	// Manifest 객체 내 리소스 정보 키
	namespace Manifest {
		constexpr const char* phantom_Type = "type";
		constexpr const char* OriginalName = "originalName";
		constexpr const char* Size = "size";
		constexpr const char* Offset = "offset";
	}

	// Phantom 객체 내 키
	namespace Phantom {
		constexpr const char* Name = "name";
		constexpr const char* Category = "category";
		constexpr const char* Type = "phantomType";
		constexpr const char* IsDummy = "isDummy";
		constexpr const char* Gender = "gender";
		constexpr const char* Age = "age";
		constexpr const char* Posture = "posture";
		constexpr const char* Height_cm = "height_cm";
		constexpr const char* Weight_kg = "weight_kg";
		constexpr const char* PosX_cm = "posX_cm";
		constexpr const char* PosY_cm = "posY_cm";
		constexpr const char* PosZ_cm = "posZ_cm";
		constexpr const char* RotX_deg = "rotX_deg";
		constexpr const char* RotY_deg = "rotY_deg";
		constexpr const char* RotZ_deg = "rotZ_deg";

		constexpr const char* ClothingArray = "clothing"; // 이는 ClothingArray로 할당됨

		// Imported Phantom 관련 리소스 ID 키
		constexpr const char* ObjResourceId = "objResourceId";
		constexpr const char* EleResourceId = "eleResourceId";
		constexpr const char* NodeResourceId = "nodeResourceId";
	}
	
	// Clothing 배열 내 객체 키
	namespace Clothing {
		constexpr const char* Name = "name";
		constexpr const char* LayerArray = "layers"; // 이는 LayerArray로 할당됨
	}

	// Clothing > Layers 배열 내 객체 키
	namespace Layer {
		constexpr const char* Thickness_cm = "thickness_cm";
		constexpr const char* Density_g_cm3 = "density_g_cm3";
		constexpr const char* Material = "material";
	}

	namespace GeometryPanelKeys {
		constexpr const char* ObjectArray = "objects";
		constexpr const char* Environment = "environment";
	}

	namespace Geometry {
		// 공통 속성
		constexpr const char* Name = "name";
		constexpr const char* Type = "type"; // "Box", "Sphere", "Cylinder"
		constexpr const char* PosX = "posX_cm";
		constexpr const char* PosY = "posY_cm";
		constexpr const char* PosZ = "posZ_cm";
		constexpr const char* RotX = "rotX_deg";
		constexpr const char* RotY = "rotY_deg";
		constexpr const char* RotZ = "rotZ_deg";
		constexpr const char* Density = "density_g_cm3";
		constexpr const char* MaterialFraction = "materialFraction"; // "mass" or "atom"
		constexpr const char* MaterialInfo = "materialInfo";
		constexpr const char* ChargedParticleTransport = "chargedParticleTransport";

		// Box 타입 속성
		constexpr const char* BoxHalfLengthX = "box_halfLengthX_cm";
		constexpr const char* BoxHalfLengthY = "box_halfLengthY_cm";
		constexpr const char* BoxHalfLengthZ = "box_halfLengthZ_cm";
		constexpr const char* BoxThicknessX = "box_thicknessX_cm";
		constexpr const char* BoxThicknessY = "box_thicknessY_cm";
		constexpr const char* BoxThicknessZ = "box_thicknessZ_cm";

		// Sphere 타입 속성
		constexpr const char* SphereInnerRadius = "sphere_innerRadius_cm";
		constexpr const char* SphereOuterRadius = "sphere_outerRadius_cm";
		constexpr const char* SphereStartPhi = "sphere_startPhi_deg";
		constexpr const char* SphereDeltaPhi = "sphere_deltaPhi_deg";
		constexpr const char* SphereStartTheta = "sphere_startTheta_deg";
		constexpr const char* SphereDeltaTheta = "sphere_deltaTheta_deg";

		// Cylinder 타입 속성
		constexpr const char* CylinderInnerRadius = "cylinder_innerRadius_cm";
		constexpr const char* CylinderOuterRadius = "cylinder_outerRadius_cm";
		constexpr const char* CylinderHalfHeightZ = "cylinder_halfHeightZ_cm";
		constexpr const char* CylinderStartAngle = "cylinder_startAngle_deg";
		constexpr const char* CylinderDeltaAngle = "cylinder_deltaAngle_deg";
	}
}

// ======================================================================
// 공용 데이터 구조체 정의
// ======================================================================

// 바이너리 리소스(obj, ele 등)의 정보를 담는 구조체 (Manifest의 한 항목)
struct ResourceInfo {
	QString id;             // 고유 식별자 (e.g., "res-1")
	QString type;           // 데이터 종류 (e.g., "OBJ", "ELE", "SPECTRUM")
	QString originalName;   // 원본 파일 이름 (e.g., "body.obj")
	qint64 offset = 0;      // 파일 내 데이터 시작 위치
	qint64 size = 0;        // 데이터 크기 (bytes)
};

struct PhantomData {
	QString name;
	QString category = "Normal";
	QString phantomType;
	bool isDummy = false;
	QString gender;
	QString age;
	QString posture;
	double height_cm = 0.0;
	double weight_kg = 0.0;
	double posX = 0.0, posY = 0.0, posZ = 0.0;
	double rotX = 0.0, rotY = 0.0, rotZ = 0.0;

	// 파일 경로 대신 리소스 ID를 저장
	QString objResourceId;
	QString eleResourceId;
	QString nodeResourceId;

	// 2단계(추출)에서 채워지는 정보
	QString objFilePath;
	QString eleFilePath;
	QString nodeFilePath;
};

struct GeometryData {
	QString name;
	QString type; // "Box", "Sphere", "Cylinder"
	double posX = 0.0, posY = 0.0, posZ = 0.0;
	double rotX = 0.0, rotY = 0.0, rotZ = 0.0;
	double density = 1.0;
	double materialFraction;
	QString materialInfo;
	bool chargedParticleTransport = false;

	// Box
	double box_halfLengthX = 0.0, box_halfLengthY = 0.0, box_halfLengthZ = 0.0;
	double box_thicknessX = 0.0, box_thicknessY = 0.0, box_thicknessZ = 0.0;
	// Sphere
	double sphere_innerRadius = 0.0, sphere_outerRadius = 0.0;
	double sphere_startPhi = 0.0, sphere_deltaPhi = 360.0;
	double sphere_startTheta = 0.0, sphere_deltaTheta = 180.0;
	// Cylinder
	double cylinder_innerRadius = 0.0, cylinder_outerRadius = 0.0;
	double cylinder_halfHeightZ = 0.0;
	double cylinder_startAngle = 0.0, cylinder_deltaAngle = 360.0;
};

struct SceneData {
	QVector<PhantomData> phantoms;
	QVector<GeometryData> geometries;
	QString environment;

	// --- 로딩 시에만 사용되는 임시 데이터 ---
	QMap<QString, ResourceInfo> resourceManifest;
	QFile* sourceFile = nullptr; // 실제 데이터가 담긴 원본 .mcsee 파일 포인터
};
Q_DECLARE_METATYPE(SceneData)