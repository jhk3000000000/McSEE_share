#pragma once

#include <vector>
#include <map>
#include <QString>
#include "Util.h"


// =================================================================
// 1. [Enums] 팬텀 및 액세서리 관련 열거형 정의
// =================================================================


// =================================================================
// 2. [Constants] 상수 정의
// =================================================================

// =================================================================
// 3. [Model] PhantomWidget 데이터 구조체
// =================================================================
struct PhantomModel 
{
    // --- System Status ---
    int m_Phantom_RAM_Usage = 0;

    // --- Phantom Data ---
    // [Index List]
    std::vector<int> m_Phantom_SequenceVector;
    
    // [Index Management]
    int m_Phantom_MakingIndex = 0;
    int m_Phantom_SelectedIndex = -1;
    
    // [Properties]
    std::map<int, QString> m_Phantom_ButtonName;
    std::map<int, std::map<int, double>> m_Phantom_MainInfo; // [index][enum_info] = value

    // --- Control Flags (Dialog State) ---
    bool m_Is_PhantomSetting_Closed = false;
    bool m_Is_PhantomSetting_OKbutton_Clicked = false;
    
    
    // --- Clothing Data ---
    // [Index Management]
    int m_Clothing_MakingIndex = 0;
    int m_Clothing_SelectedIndex = -1;
    
    // [Structure]
    std::map<int, std::vector<int>> m_Clothing_SequenceVector; // [PhantomIdx] -> Vector of ClothingIdx
    std::map<int, std::map<int, QString>> m_Clothing_ButtonName; // [PhantomIdx][ClothingIdx] -> Name
    std::map<int, std::map<int, int>> m_Clothing_Type; // [PhantomIdx][ClothingIdx] -> Type
    
    // [Properties]
    // [PhantomIdx][ClothingIdx][LayerIdx][InfoEnum] -> Value
    // 참고: Dosimeter 정보도 여기에 포함됨 (Index 1000)
    std::map<int, std::map<int, std::map<int, std::map<int, double>>>> m_Clothing_MainInfo; 

    // [Wearable Names]
    // m_PhantomWearableName[0~99:PhantomIdx][1~10 or 21~40:UserWearableIdx][3:Wearable Name]
    std::map<int, std::map<int, std::map<int, QString>>> m_PhantomWearableName; 

    // [Flags]
    bool m_Is_ClothingSetting_Closed = false;
    bool m_Is_ClothingSetting_OKbutton_Clicked = false;


    // --- Clothing Layer Data ---
    // [Index Management]
    int m_ClothingLayer_MakingIndex = 0;
    int m_ClothingLayer_SelectedIndex = -1;
    
    // [Structure]
    std::map<int, std::map<int, std::vector<int>>> m_ClothingLayer_SequenceVector; // [PhantomIdx][ClothingIdx] -> Vector of LayerIdx
    std::map<int, std::map<int, std::map<int, QString>>> m_ClothingLayer_ButtonName;
    
    // [Flags]
    bool m_Is_ClothingLayer_Setting_Closed = false;
    bool m_Is_ClothingLayer_Setting_OKbutton_Clicked = false;


    // --- Eyewear Data ---
    bool m_Is_EyewearSetting_Closed = false;
    bool m_Is_EyewearSetting_OKClicked = false;
    double m_Eyewear_Thickness = 0.0;
    double m_Eyewear_Radius = 0.0;
    double m_Eyewear_Distance = 0.0;
    double m_Eyewear_Density = 0.0;
    double m_Eyewear_composition = 0.0;

    // --- Dosimeter Data ---
    int m_Dosimeter_Making_Index = 0;
    int m_Dosimeter_Selected_Index = -1;
    std::map<int, std::vector<int>> m_DosimeterSequenceVector; // [PhantomIdx] -> Vector of DosimeterIdx
    double m_Dosimeter_Radius = 0.0;
};