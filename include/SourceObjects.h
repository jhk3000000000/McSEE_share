#pragma once
#include "ETHuman3DApp.h"
#include <vector>
#include <tuple>   // std::tuple 사용용
#include <string>
#include <fstream> // ifstream 사용용
#include "Util.h"

class SourceObjects
{
public:
    // =========================================================
    // 1. [Public] 외부(Source Widget)에서 호출하는 함수들
    // =========================================================

    // [기하학적 선원 가시화]
    void GenerateSourceActor_sourceBB(); //Broad Beam - ICRP Standard geometries
    void GenerateSourceActor_sourcePS(double* ptCenter); // Point Source, originally EP
    void GenerateSourceActor_sourceFD(double* center_radius); // Floor Disk
    void GenerateSourceActor_sourceVS(); // Volume Source
    void GenerateSourceActor_sourcePSF(std::vector<std::tuple<double, double, double>> ptMultiple, std::vector<std::tuple<double, double, double>> dirMultiple); // Phase-space File, originally PS
    void GenerateSourceActor_sourceHP(double* ptCenter); // Hot Particle
    void GenerateSourceActor_sourceCB(double* ptCenter); // Cone Beam
    void GenerateSourceDirectionActor_sourceCB();
    void GenerateSourceActor_sourceRC(); // Room air Contamination
    // void GenerateSourceActor_sourceES(); // ICRP's Enviornment Source - Under Developement
    void GenerateSourceActor_sourcePB(double* center_radius, double* theta_phi); // Parallel Beam
    void Selecting3DShpere(double* ptCenter);
    void Selecting3DShpere_Delete();
    // [PSF 가시화]
    // 다수의 점(PSF 데이터)을 한 번에 그릴 때 사용
    
    // [PSF 파일 로드]
    bool LoadPSF_data(QString path);

    std::vector<Point_SolidAngle> GetPointsWithinSolidAngle_sourceCB(double maxAngle, double a, double b, double c, int numPoints);

    // MCNP/PHITS 파싱 로직 (가장 복잡한 부분)
    void PSF_MCNP_ssw_parse_file(const std::string& filename);
    int PSF_MCNP_ssw_loadrecord(std::ifstream& file, std::vector<char>& buf, uint32_t reclen, uint64_t& lbuf);
    bool PSF_MCNP_ssw_readbytes(std::ifstream& file, char* buf, size_t bytes);
    int32_t PSF_MCNP_conv_mcnp6_ssw2pdg(int32_t c);
    std::string PSF_PHITS_replaceDwithE(std::string input);
private:
    // =========================================================
    // 2. [Private] 내부 계산 및 파일 파싱 로직 (숨김)
    // =========================================================

    // Cone Beam 계산 도우미

};