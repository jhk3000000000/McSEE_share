# ====================================================================
# GUI_module 서브프로젝트
# ====================================================================

# ====================================================================
# 리소스 파일 정의
# ====================================================================

# Qt 리소스 파일
set(QT_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc
)

# Windows 리소스 파일
set(WIN_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc
)

# ====================================================================
# 소스 파일 및 실행 파일 정의
# ====================================================================
file(GLOB GUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp    
)
file(GLOB GUI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_executable(GUI_module WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${WIN_RESOURCES}
    ${QT_RESOURCES}
)

# ====================================================================
# 타겟 속성
# ====================================================================
set_target_properties(GUI_module PROPERTIES
    OUTPUT_NAME "McSEE"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Precompiled Header 설정
target_precompile_headers(GUI_module PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h
) 

# ====================================================================
# 헤더 경로 및 라이브러리 연동
# ====================================================================

# 내부 헤더 경로
target_include_directories(GUI_module PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 라이브러리 링크
target_link_libraries(GUI_module PRIVATE
    #--- Qt 모듈 ---
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::PrintSupport
  
    # --- 1. VTK 핵심 모듈 (Common / DataModel) ---
    VTK::CommonCore
    VTK::CommonDataModel
    VTK::CommonMath
    VTK::CommonTransforms
    VTK::CommonComputationalGeometry
    VTK::CommonColor 

    # --- 2. VTK 필터 모듈 (Filters) ---
    VTK::FiltersCore
    VTK::FiltersGeneral
    VTK::FiltersSources
    VTK::FiltersPoints
    VTK::FiltersGeometry
    VTK::FiltersModeling
    VTK::FiltersExtraction

    # --- 3. VTK 렌더링 모듈 (Rendering) ---
    VTK::RenderingCore
    VTK::RenderingAnnotation
    VTK::RenderingLOD
    VTK::RenderingQt      
   
    # --- 4. VTK 렌더링 백엔드 ---
    VTK::RenderingOpenGL2 

    # --- 5. VTK 인터랙션 모듈 (Interaction) ---
    VTK::InteractionStyle
    VTK::InteractionWidgets
    
    # --- 6. VTK IO (Input/Output) 모듈 ---
    VTK::IOLegacy       
    VTK::IOPLY      
    VTK::IOGeometry     
    VTK::IOImage        
    VTK::IOImport       
    VTK::IOExport       

    # --- 7. VTK 이미징 모듈 (Imaging) ---
    VTK::ImagingCore
    VTK::ImagingHybrid

    # --- 8. VTK / Qt 연동 ---
    VTK::GUISupportQt
    VTK::GUISupportQtSQL    

    # --- 9. 기타 (차트 등) ---
    VTK::ChartsCore     
 
    #--- 기타 외부 라이브러리 ---
    Boost::filesystem
    CGAL::CGAL
    MariaDB
    Eigen3
)

message(STATUS "--- GUI_module 구성 완료 ---")