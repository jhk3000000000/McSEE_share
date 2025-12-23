#pragma once
#include "Util.h"
#include "ETHuman3DApp.h"
#include <string>
#include <vector>

class PhantomObjects
{
public:
    // =========================================================
    // 1. [Public] 외부 호출 함수 (이름 그대로 유지)
    // =========================================================
    
    // [본체 관련]
    void PhantomPolydataActor_Generate(QString strFileName, int SelectedIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom);
    void UpdatePhantom_InfoStatus_InActorMouseControl(int phantomIndex);
    //void UpdatePhantom_ActorHighlighted(int phantomIndex);
    void SkinLayerGeneration();
    // [의복/러버밴드 관련]
    void PhantomClothingGenerate(QString strFileName, bool IsPreDefinedClothing);
    void RubberBandInitialization();
    void RubberBandUserClothingGenerate();

    // [안경/선량계 관련]
    void PhantomFlatGlassesGenerate();
    void PhantomWraparoundGlassesGenerate();
    void DosimeterGenerate(int no);
    void RefreshDosimeter3DShpere(double PickedPos[3], int no);
    void RemoveDosimeter3DShpere();

    BodySizeInfo CalcBoydSizeScaleFactor(int phantomType, int phantomGender, int phantomAge, int phantomPosture, double phantomHeight, double phantomWeight);

    void AppendPhantomPolyData_GeneratePhantomActor(int PhantomIdx);
    QString GetPhantomFile_AbsolutePath(int phantomType, int phantomGender, int phantomAge, int phantomPosture, int phantomFetalAge, int phantomIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom);
    std::string GetPhantomFileTitle(std::string phantomDir);

private:
    // =========================================================
    // 2. [Private] 내부 도우미 함수 (이름 그대로 유지)
    // =========================================================
    
    
    
    std::string ExtractPhantomOBJ(std::string PATH, std::string Name, std::vector<std::string> list);
};