// GeometryObjects.h
#pragma once
#include <memory>
#include "ETHuman3DApp.h" 

// [1] 도형들의 공통 조상 (인터페이스)
class GeometryObject {
public:
    virtual ~GeometryObject() {}

    // 1. 생성 명령
    virtual void Generate(int objectID) = 0; 
    
    // 2. [위치 수정됨] UI 업데이트 명령 (여기에 있어야 합니다!)
    virtual void UpdateUI(int objectID) = 0; 
};

// [2] 상자 전문가
class BoxObject : public GeometryObject {
public:
    void Generate(int objectID) override;
    void UpdateUI(int objectID) override; // [추가됨]
};

// [3] 구 전문가
class SphereObject : public GeometryObject {
public:
    void Generate(int objectID) override;
    void UpdateUI(int objectID) override; // [추가됨]
};

// [4] 실린더 전문가
class CylinderObject : public GeometryObject {
public:
    void Generate(int objectID) override;
    void UpdateUI(int objectID) override; // [추가됨]

private:
    vtkSmartPointer<vtkPolyData> CreateTubeMesh(double R_in, double R_out, double halfH, double startDeg, double deltaDeg, int resolution);
};

// [5] 도형 공장 (Factory)
class GeometryFactory {
public:
    // 공장은 "물건을 만들어주는 곳"이지, "물건 그 자체"가 아닙니다.
    // 따라서 Generate나 UpdateUI 함수는 여기 있으면 안 됩니다. (삭제함)

    static std::unique_ptr<GeometryObject> Create(int type) {
        switch (type) {
        case 0: return std::make_unique<BoxObject>();
        case 1: return std::make_unique<SphereObject>();
        case 2: return std::make_unique<CylinderObject>();
        default: return nullptr;
        }
    }
};