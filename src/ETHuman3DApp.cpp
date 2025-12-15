//ETHuman3DApp.cpp includes visualization functions (VTK 3D mesh-geometry process/Vector calculation), external file read/write/make/delete functions, repeated-/complicated- functions

#include <iostream>

#include <windows.h>
#include "ETHuman3DApp.h"
#include "ETHuman3D.h"
#include "pch.h"
#include <fstream>
#include <sys/stat.h>
#include <direct.h>
#include <string>
#include <iomanip>
#include <time.h>
#include <sstream>
#include <algorithm>
#include <cfloat>
#include <filesystem>
#include <cstdlib>

#include "ETQVTKWidget.h"
#include "ETInteractorStyleTrackballActor.h"
#include "ETInteractorStyleRubberBand.h"

#include "FunctionPanelTop.h"
#include "FunctionPanelRight.h"
#include "ProcessInformationPanel.h"
#include "RealTimeResultPanel.h"
#include "QHoverSensitiveButton.h"
#include "SimpleLogger.h"
#include "TaskManager.h"

#include <qsettings.h>
#include <QtWidgets>
#include <vtkSelectEnclosedPoints.h>
#include <vtkPLYWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkMultiThreshold.h>
#include <vtkDataSetMapper.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkFillHolesFilter.h>
#include <vtkOBJWriter.h>
#include <vtkTriangle.h>
#include <vtkOBJExporter.h>
#include <vtkOBBTree.h>
#include <vtkAppendFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkExtractCells.h>
#include <vtkCellLocator.h>	
#include <vtkCellArray.h>
#include <vtkThreshold.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkAreaPicker.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkPlanes.h>
#include <vtkIdFilter.h>
#include <vtkParametricTorus.h>
#include <vtkParametricFunctionSource.h>
#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>
#include <vtkDelaunay3D.h>
#include <vtkCylinderSource.h>
#include <vtkClipPolyData.h>
#include <vtkTubeFilter.h>
#include <vtkPlane.h>
#include <vtkCubeSource.h>
#include <vtkImplicitBoolean.h>
#include <vtkSampleFunction.h>
#include <vtkMarchingCubes.h>
#include <vtkSphere.h>
#include <vtkBox.h>
#include <vtkCylinder.h>
#include <vtkFeatureEdges.h>
#include <vtkStripper.h>
#include <vtkContourTriangulator.h>
#include <vtkLookupTable.h>
#include <vtkParametricSpline.h>
#include <vtkArrowSource.h>
#include <vtkSplineFilter.h>
#include <vtkArcSource.h>
#include <vtkDiskSource.h>
#include <vtkCutter.h>
#include <vtkDelaunay2D.h>
#include <vtkDiskSource.h>
#include <vtkDecimatePro.h>
#include <vtkPolygon.h>
#include <vtkOutlineFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkWedge.h>
#include <vtkUnstructuredGrid.h>
// #include <QTextCodec> Qt 6에서는 이제 모든 텍스트를 기본적으로 UTF-8로 가정합니다.

#include <QLoggingCategory>
#include <QDateTime>
#include <QTextStream>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Util.h"

#include "Tokenizer.h"

//CGAL
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <CGAL/Polygon_mesh_processing/intersection.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/IO/OBJ_reader.h>

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/IO/facets_in_complex_2_to_triangle_mesh.h>
#include <CGAL/Surface_mesh.h>

//************************************** Pre-defined Variables **************************************//
typedef CGAL::Simple_cartesian<double> SK;
typedef SK::Point_3 Point_3;
typedef SK::Vector_3 Vector_3;
typedef SK::Triangle_3 Triangle_3;
typedef CGAL::Polyhedron_3<SK> Polyhedron_SK;
typedef Polyhedron_SK::Vertex_iterator Vertex_iterator;
typedef Polyhedron_SK::Face_iterator Face_iterator;
typedef Polyhedron_SK::Halfedge_around_facet_circulator Halfedge_around_facet_circulator;
typedef Polyhedron_SK::Halfedge_around_vertex_circulator Halfedge_around_vertex_circulator;
typedef boost::graph_traits<Polyhedron_SK>::face_descriptor face_descriptor;
typedef boost::graph_traits<Polyhedron_SK>::vertex_descriptor vertex_descriptor;
typedef Polyhedron_SK::Halfedge_handle Halfedge_handle;

//************************************** Pre-defined Class/Struct **************************************//
class CGAL_LayerDef {
public:
	// 	tetrahedralization(points, faces); // 누적된 "points"와 "faces" 정보로 사면체화 진행
	//void CGAL_LayerDef::tetrahedralization(int phantomIdx, std::map<int, std::map<int, std::vector<Point_3>>>& points, std::map<int, std::map<int, std::vector<Point_3>>>& faces, int WearableType)
	void tetrahedralization(int phantomIdx, std::map<int, std::map<int, std::vector<Point_3>>>& points, std::map<int, std::map<int, std::vector<Point_3>>>& faces, int WearableType)
	{
		// 모든 Wearable을 장기 ID로 구분한 하나의 ele와 node를 생산 -> 이후 동일한 메커니즘인 dosimeter 추가
		std::ofstream node;
		node.precision(20);
		std::ofstream ele;
		if (WearableType == 0) // clothing
		{
			node.open("./data/wearable/Clothing_forTetra/clothing_tetra_" + std::to_string(phantomIdx) + ".node");
			ele.open("./data/wearable/Clothing_forTetra/clothing_tetra_" + std::to_string(phantomIdx) + ".ele");
		}		
		//if (WearableType == 3) // dosimeter
		//{
		//	node.open("./data/dbsend/wearable_send/dosimeter_tetra" + std::to_string(phantomIdx) + ".node");
		//	ele.open("./data/dbsend/wearable_send/dosimeter_tetra" + std::to_string(phantomIdx) + ".ele");
		//}
		//if (WearableType == 4) // dosimeter_side
		//{
		//	node.open("./data/dbsend/wearable_send/dosimeter_side_tetra" + std::to_string(phantomIdx) + ".node");
		//	ele.open("./data/dbsend/wearable_send/dosimeter_side_tetra" + std::to_string(phantomIdx) + ".ele");
		//}

		int vertex_size(0);
		std::vector<int> vertex_count;
		vertex_count.push_back(0);

		for (auto i = points.begin(); i != points.end(); i++) //dosim_idx->first는 장기 ID, j->first는 Layer ID
		{
			for (auto j = i->second.begin(); j != i->second.end(); j++) //dosim_idx->second는 points_3 벡터 // 이거는 LayerID 순환
			{
				vertex_size += j->second.size(); // j->second.size()는 point_3 벡터의 사이즈로 각 Layer의 point 개수임
				vertex_count.push_back(vertex_size); // vertex_count(Layer1 point 개수, Layer2 point 개수 ...)
			}
		}

		node << vertex_size << "  3  0  0" << endl;

		int num_v(0);

		// save the node file
		for (auto i = points.begin(); i != points.end(); i++) // 장기 ID 순환
		{
			for (auto j = i->second.begin(); j != i->second.end(); j++)  // Layer ID 순환
			{
				for (int k = 0; k < j->second.size(); k++)  // 각 레이어의 point_3 벡터의 크기 만큼 순환(즉 각 레이어의 point 개수만큼 순환)
				{
					node << k + num_v << "  " << points[i->first][j->first][k].x()
						<< "  " << points[i->first][j->first][k].y() << "  " << points[i->first][j->first][k].z() << endl;
				}
				num_v += j->second.size(); // num_v는 현재까지의 총 point 개수(각 레이어 마다 누적해감) 따라서 node가 각 레이어가 끝나고 장기가 끝나도 0부터 쭉 누적해감
			}
		}

		int face_size(0);
		std::vector<int> facet_count;
		facet_count.push_back(0);
		for (auto i = faces.begin(); i != faces.end(); i++) {
			for (auto j = i->second.begin(); j != i->second.end(); j++) {
				face_size += j->second.size();
				facet_count.push_back(face_size);
			}
		}

		ele << face_size * 3 << "  4  1" << endl;

		int num_f(0);
		int vertex_count_Index = 0;
		for (auto i = faces.begin(); i != faces.end(); i++) // dosim_idx->first는 location(20100)임, dosim_idx->second.size는 해당 (dosim_idx->first) 의복 ID의 레이어의 facet 개수임
		{// 새로운 MatID 시작(새로운 사면체 시작)
			//int convertedLocationNum = i->first - ((i->first / 10) % 10) * 10;
			for (auto j = i->second.begin(); j != i->second.end(); j++) // 레이어 순환
			{
				for (int k = 0; k < j->second.size(); k++)  // 각 레이어의 point_3 벡터의 크기 만큼 순환(즉 각 레이어의 point 개수만큼 순환)
				{
					ele << (k + num_f) * 3 << "   " <<
						(double)faces[i->first][j->first][k].x() + vertex_count[vertex_count_Index] << " " <<
						(double)faces[i->first][j->first][k].y() + vertex_count[vertex_count_Index] << " " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index] << " " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << "  " << i->first + j->first << endl;

					ele << (k + num_f) * 3 + 1 << "   " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index] << " " <<
						(double)faces[i->first][j->first][k].y() + vertex_count[vertex_count_Index] << " " <<
						(double)faces[i->first][j->first][k].x() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << " " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << "  " << i->first + j->first << endl;

					ele << (k + num_f) * 3 + 2 << "   " <<
						(double)faces[i->first][j->first][k].x() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << " " <<
						(double)faces[i->first][j->first][k].y() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << " " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index]
						+ (vertex_count[vertex_count_Index + 1] - vertex_count[vertex_count_Index]) / 2 << " " <<
						(double)faces[i->first][j->first][k].z() + vertex_count[vertex_count_Index] << "  " << i->first + j->first << endl;
				}
				num_f += j->second.size(); // 각 레이어 마다의 facet(면, 다면체, polygon) 개수만큼 누적해감
				vertex_count_Index++;
			}
		}

		vertex_count.clear();
		facet_count.clear();
		points.clear();
		faces.clear();
	}

	// 의복, 레이어를 고려하여 points[의복ID][레이어ID][Index] = Point_3 (즉, point_3 좌표를 vector형태로 누적) 및 faces를 누적해나감

	//void CGAL_LayerDef::tetra_prepare(bool IsFirstLayer, int location, int num, std::map<int, std::map<int, std::vector<Point_3>>>& points, std::map<int, std::map<int, std::vector<Point_3>>>& faces,
	void tetra_prepare(bool IsFirstLayer, int location, int num, std::map<int, std::map<int, std::vector<Point_3>>>& points, std::map<int, std::map<int, std::vector<Point_3>>>& faces,
		Polyhedron_SK& OBJ, Polyhedron_SK& OBJ_original, Polyhedron_SK& OBJ_update)
	{
		// Read OBJ for making first layer -> OBJ는 사전제작 or 직접제작한 base layer(10마이크론 마진둠) 
		if (IsFirstLayer == true) { // 0번 Layer에 대해서-> first layer만 만들기

			//OBJ_original은 사전제작 혹은 직접제작한 base layer
			for (Vertex_iterator v = OBJ_original.vertices_begin(); v != OBJ_original.vertices_end(); v++) // OBJ_original은 original[장기ID]
			{
				points[location][num].push_back(v->point());
			}

			// base layer의 point를 사면체화에 사용될 "points"에 쭉 입력한 뒤, OBJ_first(첫번째 layer(ID=0)의 레이어)의 point를 "points"에 쭉 입력
			for (Vertex_iterator v = OBJ.vertices_begin(); v != OBJ.vertices_end(); v++) // OBJ는 first[장기ID]
			{
				points[location][num].push_back(v->point());
			}

			CGAL::Inverse_index<Vertex_iterator> index_original(OBJ_original.vertices_begin(), OBJ_original.vertices_end());
			for (Face_iterator f = OBJ_original.facets_begin(); f != OBJ_original.facets_end(); f++)
			{
				Halfedge_around_facet_circulator hc = f->facet_begin();
				Halfedge_around_facet_circulator hc_end = hc;
				faces[location][num].push_back(Point_3(index_original[Vertex_iterator(hc->vertex())], index_original[Vertex_iterator(hc->next()->vertex())], index_original[Vertex_iterator(hc->prev()->vertex())]));
			}
			OBJ_update = OBJ; // 그리고 OBJ_first(첫번째 layer)가 OBJ_update가 됨 -> 이후 레이어의 base layer가 될거임
		}
		// Making layer after first layer
		else { // 두번째 레이어 이후
			
			for (Vertex_iterator v = OBJ_update.vertices_begin(); v != OBJ_update.vertices_end(); v++) // Update[장기ID]의 polyhedron
			{
				points[location][num].push_back(v->point());
			}

			for (Vertex_iterator v = OBJ.vertices_begin(); v != OBJ.vertices_end(); v++)
			{
				points[location][num].push_back(v->point());
			}
			// base layer인 OBJ_update의 point를 사면체화에 사용될 "points"에 쭉 입력한 뒤, OBJ(offset된 layer, first[장기ID], 두번째이후레이어)의 point를 "points"에 쭉 입력

			CGAL::Inverse_index<Vertex_iterator> index_original(OBJ_update.vertices_begin(), OBJ_update.vertices_end());
			for (Face_iterator f = OBJ_update.facets_begin(); f != OBJ_update.facets_end(); f++) {
				Halfedge_around_facet_circulator hc = f->facet_begin();
				Halfedge_around_facet_circulator hc_end = hc;
				faces[location][num].push_back(Point_3(index_original[Vertex_iterator(hc->vertex())], index_original[Vertex_iterator(hc->next()->vertex())], index_original[Vertex_iterator(hc->prev()->vertex())]));
			}
			OBJ_update = OBJ; // OBJ update는 그 다음 레이어의 base layer가 됨
		}
	}

	//void CGAL_LayerDef::Intersection_facet(Polyhedron_SK& OBJ, std::vector<SK::Point_3>& OBJ_points, std::map<vertex_descriptor, SK::Vector_3>& vnormals, int& initial_selfintersections)
	void Intersection_facet(Polyhedron_SK& OBJ, std::vector<SK::Point_3>& OBJ_points, std::map<vertex_descriptor, SK::Vector_3>& vnormals, int& initial_selfintersections)
	{
		std::vector<std::pair<face_descriptor, face_descriptor>> intersection;
		while (1) {
			CGAL::Polygon_mesh_processing::self_intersections(OBJ, back_inserter(intersection));
			//theApp.SetMessageBox(QString::number(intersection.size()));

			if (intersection.size() <= initial_selfintersections * 1.05) // No self intersections
			{
				vnormals.clear();
				intersection.clear();
				OBJ_points.clear();

				break; // While 문 끝내기
			}

			for (int i = 0; i < intersection.size(); i++)
			{
				int cnt(0);
				for (Vertex_iterator v = OBJ.vertices_begin(); v != OBJ.vertices_end(); v++)
				{
					if (v->point() == intersection[i].first->halfedge()->vertex()->point())  // halfedge와 vertex간 겹침
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z()); // 겹치면 노말의 0.00001만큼 이동
					}
					else if (v->point() == intersection[i].first->halfedge()->next()->vertex()->point()) //halfedge와 다음 vertex간 겹침
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z());
					}
					else if (v->point() == intersection[i].first->halfedge()->prev()->vertex()->point())  // halfedge와 이전 vertext간 겹침
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z());
					}
					else if (v->point() == intersection[i].second->halfedge()->vertex()->point())
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z());
					}
					else if (v->point() == intersection[i].second->halfedge()->next()->vertex()->point())
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z());
					}
					else if (v->point() == intersection[i].second->halfedge()->prev()->vertex()->point())
					{
						v->point() = Point_3(OBJ_points[cnt].x() + 0.00001 * vnormals[v].x(),
							OBJ_points[cnt].y() + 0.00001 * vnormals[v].y(),
							OBJ_points[cnt].z() + 0.00001 * vnormals[v].z());
					}

					cnt++;
				}
			}

			intersection.clear();
		}
	}


	//void CGAL_LayerDef::OFFSET(double offset, std::vector<SK::Point_3>& OBJ_points, Polyhedron_SK& OBJ, int& initial_selfintersections)
	void OFFSET(double offset, std::vector<SK::Point_3>& OBJ_points, Polyhedron_SK& OBJ, int& initial_selfintersections)
	{
		std::map<vertex_descriptor, SK::Vector_3> vnormals;
		std::map<face_descriptor, SK::Vector_3> fnormals;
		CGAL::Polygon_mesh_processing::compute_normals(OBJ, boost::make_assoc_property_map(vnormals), boost::make_assoc_property_map(fnormals));
		fnormals.clear();
		std::vector<Point_3> border_vertex;
		for (Halfedge_handle h : halfedges(OBJ)) {
			if (h->is_border()) border_vertex.push_back(h->vertex()->point());
		}
		for (Vertex_iterator v = OBJ.vertices_begin(); v != OBJ.vertices_end(); v++) {
			Point_3 pre_p = v->point();

			double move_x = v->point().x() + offset * vnormals[v].x();
			double move_y = v->point().y() + offset * vnormals[v].y();
			double move_z = v->point().z() + offset * vnormals[v].z();

			v->point() = Point_3(move_x, move_y, move_z);
		}
		Intersection_facet(OBJ, OBJ_points, vnormals, initial_selfintersections);
		border_vertex.clear();
	}

	//void CGAL_LayerDef::Self_Intersection_facet(Polyhedron_SK& OBJ, std::vector<SK::Point_3>& OBJ_points, std::map<vertex_descriptor, SK::Vector_3>& vnormals, int& initial_selfintersections)
	void Self_Intersection_facet(Polyhedron_SK& OBJ, std::vector<SK::Point_3>& OBJ_points, std::map<vertex_descriptor, SK::Vector_3>& vnormals, int& initial_selfintersections)
	{
		//ofstream a;
		//a.open("./Wearable/Imported/output");
		std::vector<std::pair<face_descriptor, face_descriptor>> intersection;
		CGAL::Polygon_mesh_processing::self_intersections(OBJ, back_inserter(intersection));
		initial_selfintersections = intersection.size(); // 최초 self-intersection 개수
		//a << "initial selfintersection: " << initial_selfintersections << endl;
		intersection.clear();
		int iteration_count = 0;
		while (1) {
			iteration_count++;
			CGAL::Polygon_mesh_processing::self_intersections(OBJ, back_inserter(intersection));
			//a << "selfintersection: " <<intersection.size() << endl;
			if (intersection.size() <= initial_selfintersections / 5 || iteration_count == 100) // Self intersection을 20%로 줄이거나 100번반복(최대 마진인 0.001 cm (10 um))하면 break; -> 피부와 닿을수 있음 
			{
				vnormals.clear();
				intersection.clear();
				OBJ_points.clear();

				break; // While 문 끝내기
			}

			for (int i = 0; i < intersection.size(); i++)
			{
				int cnt(0);
				for (Vertex_iterator v = OBJ.vertices_begin(); v != OBJ.vertices_end(); v++)
				{
					if (v->point() == intersection[i].first->halfedge()->vertex()->point())  // halfedge와 vertex간 겹침
					{
						//a << v->point().x() << " " << v->point().y()<< " "<< v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(), // 0.1 um 씩 밀기
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z()); // 겹치면 노말의 0.00001만큼 이동
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl<<endl;

					}
					else if (v->point() == intersection[i].first->halfedge()->next()->vertex()->point()) //halfedge와 다음 vertex간 겹침
					{
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(),
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z());
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl << endl;

					}
					else if (v->point() == intersection[i].first->halfedge()->prev()->vertex()->point())  // halfedge와 이전 vertext간 겹침
					{
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(),
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z());
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl << endl;

					}
					else if (v->point() == intersection[i].second->halfedge()->vertex()->point())
					{
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(),
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z());
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl << endl;

					}
					else if (v->point() == intersection[i].second->halfedge()->next()->vertex()->point())
					{
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(),
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z());
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl << endl;

					}
					else if (v->point() == intersection[i].second->halfedge()->prev()->vertex()->point())
					{
						//a << v->point().x()<< " " << v->point().y()<< " " << v->point().z()<< endl;
						//a << vnormals[v].x() << " " << vnormals[v].y() << " " << vnormals[v].z() << endl;
						v->point() = Point_3(v->point().x() - 0.00001 * vnormals[v].x(),
							v->point().y() - 0.00001 * vnormals[v].y(),
							v->point().z() - 0.00001 * vnormals[v].z());
						//a << v->point().x() << " " << v->point().y()<< " " << v->point().z() << endl << endl;

					}
					cnt++;
				}
			}
			intersection.clear();
		}
	}

	// num = layerIdx, OBJ = OBJ_first, OBJ_original = OBJ original(base layer를 남겨두기용), OBJ_update = base layer(10마이크론 마진 둔 첫레이어) 다음의 레이어들, ifp = 실제 파일 OBJ, offset = offset 값
	// 결과적으로 ifp에 해당하는 실제 파일.obj를 읽어서 first[의복ID], origianl[의복ID], update[의복ID]를 생성 
	// OBJ_to_Polyhedron(Layer, first[Name], original[Name], update[Name], *ifp[Name], thickness);
	//void CGAL_LayerDef::OBJ_to_Polyhedron(bool IsFirstLayer, Polyhedron_SK& OBJ, Polyhedron_SK& OBJ_original, Polyhedron_SK& OBJ_update, std::ifstream& ifp, double offset, int& initial_selfintersections, bool IsOverlappedClothingUp, int OverlappedStartedLayerIdx)
	void OBJ_to_Polyhedron(bool IsFirstLayer, Polyhedron_SK& OBJ, Polyhedron_SK& OBJ_original, Polyhedron_SK& OBJ_update, std::ifstream& ifp, double offset, int& initial_selfintersections, bool IsOverlappedClothingUp, int OverlappedStartedLayerIdx)
	{
		// Read OBJ for making first layer		
		if (IsFirstLayer == true) // 첫번재 layer(LayerID = 0)
		{
			std::vector<SK::Point_3> points; // node
			std::vector<std::vector<size_t>> faces; // faces = 삼각형 = 다각형 = polyhedron = ele

			//처음 Layer는 .obj에서 읽어옴 (OBJ_original = base layer)
			if (!ifp || !CGAL::read_OBJ(ifp, points, faces)) cerr << "failed to read file" << endl;
			//faces -> 면 = 삼각형 = 다각형 = polygon(2차원)
			//polyhedron -> 다면체(3차원) 
			CGAL::Polygon_mesh_processing::orient_polygon_soup(points, faces); // optional if your mesh is not correctly oriented
			CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, faces, OBJ); // OBJ라는 polyhedron_3에 points, faces 생성
			if (CGAL::is_closed(OBJ) && (!CGAL::Polygon_mesh_processing::is_outward_oriented(OBJ)))
				CGAL::Polygon_mesh_processing::reverse_face_orientations(OBJ);


			//  처음 만들어진 base layer의 self-intersection 지우기
			std::map<vertex_descriptor, SK::Vector_3> vnormals;
			std::map<face_descriptor, SK::Vector_3> fnormals;
			CGAL::Polygon_mesh_processing::compute_normals(OBJ, boost::make_assoc_property_map(vnormals), boost::make_assoc_property_map(fnormals));
			fnormals.clear();
			std::vector<Point_3> border_vertex;
			for (Halfedge_handle h : halfedges(OBJ))
			{
				if (h->is_border()) border_vertex.push_back(h->vertex()->point());
			}
			Self_Intersection_facet(OBJ, points, vnormals, initial_selfintersections);
			OBJ_original = OBJ; // first layer OBJ는 OBJ_original
			OBJ_update = OBJ; // first layer OBJ는 OBJ_original

			// Offset 나눠서 진행
			int RepeatCount = 0;
			if (offset < 0.1) RepeatCount = 40;
			if (offset < 0.5) RepeatCount = 60;
			if (offset < 1) RepeatCount = 80;
			if (offset >= 1) RepeatCount = int(offset * 100);
			double offset_new = offset / RepeatCount;
			for (int i = 0; i < RepeatCount; i++)
			{
				OFFSET(offset_new, points, OBJ, initial_selfintersections);
			}
			// OBJ는 offset 됨->즉 여기서 OBJ는 first_layer가 됨.
			points.clear();
			faces.clear();
		}
		// Making layer after first layer
		else // 두번째 layer부터(LayerId 가 1~5)
		{			
			std::vector<SK::Point_3> points;

			//OBJ_update의 points 정보 받아옴 -> 이전이 첫번째 레이어였다면, OBj_update 는 OBJ_first(첫번째 레이어)로 두번재 레이어의 base layer가 될 것임.
			for (Vertex_iterator v = OBJ_update.vertices_begin(); v != OBJ_update.vertices_end(); v++)
			{
				points.push_back(v->point());
			}

			OBJ = OBJ_update; // OBJ를 offset할 것임.

			int RepeatCount = 0;
			if (offset < 0.1) RepeatCount = 40;
			if (offset < 0.5) RepeatCount = 60;
			if (offset < 1) RepeatCount = 80;
			if (offset >= 1) RepeatCount = int(offset * 100);

			double offset_new = offset / RepeatCount;
			for (int i = 0; i < RepeatCount; i++)
			{
				OFFSET(offset_new, points, OBJ, initial_selfintersections); // points는 인풋(update obj로부터 생긴 points) -> 해당 점을 통해 OBJ를 업데이트 
			}
			// OBJ는 offset 됨(OBJ는 변화하는 layer -> first[장기ID]
			points.clear();
		}
	}
	void export_to_obj(const Polyhedron_SK& P, const std::string& filename) {
		std::ofstream out(filename);
		if (!out) {
			std::cerr << "Cannot open file " << filename << std::endl;
			return;
		}

		// Write vertices
		for (auto vit = P.points_begin(); vit != P.points_end(); ++vit) {
			out << "v " << *vit << std::endl;
		}

		// Write faces
		for (auto fit = P.facets_begin(); fit != P.facets_end(); ++fit) {
			out << "f";
			auto hc = fit->facet_begin();
			do {
				out << " " << std::distance(P.vertices_begin(), hc->vertex()) + 1;
			} while (++hc != fit->facet_begin());
			out << std::endl;
		}

		out.close();
	}
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

// QIODeviceStreamBuf 메서드 구현
std::streambuf::int_type QIODeviceStreamBuf::underflow() {
	if (!m_device || !m_device->isOpen() || m_device->atEnd())
		return traits_type::eof();
	qint64 readBytes = m_device->read(m_buffer, sizeof(m_buffer));
	if (readBytes <= 0)
		return traits_type::eof();
	setg(m_buffer, m_buffer, m_buffer + readBytes);
	return traits_type::to_int_type(*gptr());
}
std::streambuf::int_type QIODeviceStreamBuf::overflow(std::streambuf::int_type c) {
	if (!m_device || !m_device->isOpen())
		return traits_type::eof();
	sync();
	if (c != traits_type::eof()) {
		*pptr() = traits_type::to_char_type(c);
		pbump(1);
		return c;
	}
	return traits_type::not_eof(c);
}
int QIODeviceStreamBuf::sync() {
	if (!m_device || !m_device->isOpen())
		return -1;
	std::ptrdiff_t size = pptr() - pbase();
	if (size > 0) {
		qint64 written = m_device->write(pbase(), size);
		if (written != size)
			return -1;
		setp(m_buffer, m_buffer + sizeof(m_buffer) - 1);
	}
	return 0;
}
// QIODeviceIStream 구현
QIODeviceIStream::QIODeviceIStream(QIODevice* device) : std::istream(nullptr), m_buf(device) {
	device->open(QIODevice::ReadOnly);
	rdbuf(&m_buf);
}
QIODeviceIStream::~QIODeviceIStream() {
	m_buf.pubsync();
}
// QIODeviceOStream 구현
QIODeviceOStream::QIODeviceOStream(QIODevice* device) : std::ostream(nullptr), m_buf(device) {
	device->open(QIODevice::WriteOnly);
	rdbuf(&m_buf);
}
QIODeviceOStream::~QIODeviceOStream() {
	m_buf.pubsync();
}
// QIODeviceIOStream 구현
QIODeviceIOStream::QIODeviceIOStream(QIODevice* device) : std::iostream(nullptr), m_buf(device) {
	device->open(QIODevice::ReadWrite);
	rdbuf(&m_buf);
}
QIODeviceIOStream::~QIODeviceIOStream() {
	m_buf.pubsync();
}
// 로깅
//************************************** Pre-defined Functions **************************************//


//************************************** Constructor and Destructor **************************************//
ETHuman3DApp::ETHuman3DApp()
{
	PanelMode = E_GEOMETRY; // 최초 시작 패널
}
ETHuman3DApp::~ETHuman3DApp()
{
}

//************************************** TEST **************************************//

//  =====피부선량(HP) 계산 전용 네임스페이스: 지오데식 ROI + 세분화 유틸들
namespace HP_SkinDoseCalculate {

	/** (키 해시) 공유 edge 매핑용 */
	struct PairHash
	{
		std::size_t operator()(const std::pair<vtkIdType, vtkIdType>& p) const noexcept
		{
			uint64_t a = static_cast<uint64_t>(p.first);
			uint64_t b = static_cast<uint64_t>(p.second);
			a ^= b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2);
			return static_cast<size_t>(a);
		}
	};

	/** 3D 거리 */
	inline double norm3(const std::array<double, 3>& A, const std::array<double, 3>& B) {
		const double dx = A[0] - B[0], dy = A[1] - B[1], dz = A[2] - B[2];
		return std::sqrt(dx*dx + dy * dy + dz * dz);
	}

	/** 삼각형 cell 중심(무게중심) */
	inline std::array<double, 3> cell_center(vtkPolyData* pd, vtkIdType cid) {
		vtkCell* cell = pd->GetCell(cid);
		vtkIdList* ids = cell->GetPointIds();
		double p0[3], p1[3], p2[3];
		pd->GetPoint(ids->GetId(0), p0);
		pd->GetPoint(ids->GetId(1), p1);
		pd->GetPoint(ids->GetId(2), p2);
		return { (p0[0] + p1[0] + p2[0]) / 3.0, (p0[1] + p1[1] + p2[1]) / 3.0, (p0[2] + p1[2] + p2[2]) / 3.0 };
	}

	/** 삼각형 면적(cm²) */
	inline double cell_area(vtkPolyData* pd, vtkIdType cid) {
		vtkCell* cell = pd->GetCell(cid);
		vtkIdList* ids = cell->GetPointIds();
		double p0[3], p1[3], p2[3];
		pd->GetPoint(ids->GetId(0), p0);
		pd->GetPoint(ids->GetId(1), p1);
		pd->GetPoint(ids->GetId(2), p2);
		return vtkTriangle::TriangleArea(p0, p1, p2);
	}

	/** 입력 메쉬를 삼각형화 + 클린(병합) */
	inline vtkSmartPointer<vtkPolyData> ensure_tri_clean(vtkPolyData* in) {
		auto tri = vtkSmartPointer<vtkTriangleFilter>::New();
		tri->SetInputData(in);

		auto clean = vtkSmartPointer<vtkCleanPolyData>::New();
		clean->SetInputConnection(tri->GetOutputPort());
		clean->ConvertStripsToPolysOn();
		clean->ConvertLinesToPointsOff();
		clean->PointMergingOn();
		clean->Update();

		return clean->GetOutput();
	}

	/** 공유 edge 기반 인접 리스트 구성 (삼각형-삼각형) */
	inline void build_edge_adjacency(vtkPolyData* pd, std::vector<std::vector<vtkIdType>>& adj) {
		const vtkIdType nCells = pd->GetNumberOfCells();
		adj.assign(static_cast<size_t>(nCells), {});
		std::unordered_map<std::pair<vtkIdType, vtkIdType>, std::vector<vtkIdType>, PairHash> edge2cells;
		edge2cells.reserve(static_cast<size_t>(nCells) * 2);

		for (vtkIdType cid = 0; cid < nCells; ++cid) {
			vtkCell* cell = pd->GetCell(cid);
			if (!cell || cell->GetNumberOfPoints() != 3) continue;
			vtkIdList* ids = cell->GetPointIds();
			vtkIdType a = ids->GetId(0), b = ids->GetId(1), c = ids->GetId(2);
			auto add_edge = [&](vtkIdType u, vtkIdType v) {
				if (u > v) std::swap(u, v);
				edge2cells[{u, v}].push_back(cid);
			};
			add_edge(a, b); add_edge(b, c); add_edge(c, a);
		}

		for (auto& kv : edge2cells) {
			auto& cells = kv.second;
			const int L = static_cast<int>(cells.size());
			for (int i = 0; i < L; ++i)
				for (int j = i + 1; j < L; ++j) {
					vtkIdType u = cells[i], v = cells[j];
					adj[static_cast<size_t>(u)].push_back(v);
					adj[static_cast<size_t>(v)].push_back(u);
				}
		}
	}

	/** 지오데식(근사)으로 seed에서 면적 누적 → targetAreaCm2 도달 시까지 선택 */
	inline std::vector<vtkIdType>
		grow_area_by_geodesic(vtkPolyData* pd, vtkIdType seedCellId, double targetAreaCm2,
			double& outAchievedAreaCm2)
	{
		const vtkIdType nCells = pd->GetNumberOfCells();

		// 1) 인접
		std::vector<std::vector<vtkIdType>> adj;
		build_edge_adjacency(pd, adj);

		// 2) center/area 미리 계산
		std::vector<std::array<double, 3>> centers(static_cast<size_t>(nCells));
		std::vector<double> areas(static_cast<size_t>(nCells));
		double totalArea = 0.0;
		for (vtkIdType i = 0; i < nCells; ++i) {
			centers[static_cast<size_t>(i)] = cell_center(pd, i);
			const double a = cell_area(pd, i);
			areas[static_cast<size_t>(i)] = a;
			totalArea += a;
		}

		// 3) 검증
		if (seedCellId < 0 || seedCellId >= nCells) throw std::runtime_error("Invalid seedCellId");
		if (targetAreaCm2 > totalArea) {
			std::cerr << "[HP][Warn] target area(" << targetAreaCm2 << ") > mesh total area(" << totalArea << ")\n";
		}

		// 4) Dijkstra (C++11)
		const double INF = std::numeric_limits<double>::infinity();
		std::vector<double> dist(static_cast<size_t>(nCells), INF);
		std::vector<char>   done(static_cast<size_t>(nCells), 0);

		typedef std::pair<double, vtkIdType> QItem;
		struct Cmp { bool operator()(const QItem& a, const QItem& b) const { return a.first > b.first; } };
		std::priority_queue<QItem, std::vector<QItem>, Cmp> pq;

		dist[static_cast<size_t>(seedCellId)] = 0.0;
		pq.push(QItem(0.0, seedCellId));

		// 5) 누적
		std::vector<vtkIdType> selected; selected.reserve(1024);
		double accumArea = 0.0;

		while (!pq.empty()) {
			QItem top = pq.top(); pq.pop();
			double du = top.first;
			vtkIdType u = top.second;
			if (done[static_cast<size_t>(u)]) continue;
			done[static_cast<size_t>(u)] = 1;

			accumArea += areas[static_cast<size_t>(u)];
			selected.push_back(u);
			if (accumArea >= targetAreaCm2) break;

			const std::vector<vtkIdType>& nbrs = adj[static_cast<size_t>(u)];
			for (size_t k = 0; k < nbrs.size(); ++k) {
				vtkIdType v = nbrs[k];
				if (done[static_cast<size_t>(v)]) continue;
				double w = norm3(centers[static_cast<size_t>(u)], centers[static_cast<size_t>(v)]);
				double alt = du + w;
				if (alt < dist[static_cast<size_t>(v)]) {
					dist[static_cast<size_t>(v)] = alt;
					pq.push(QItem(alt, v));
				}
			}
		}

		outAchievedAreaCm2 = accumArea;
		return selected;
	}

	/** 선택 셀만 복사하여 새 PolyData 구성 (VTK 8.2에서도 안전) */
	inline vtkSmartPointer<vtkPolyData>
		extract_cells_as_polydata(vtkPolyData* src, const std::vector<vtkIdType>& cellIds)
	{
		auto outPoints = vtkSmartPointer<vtkPoints>::New();
		auto outPolys = vtkSmartPointer<vtkCellArray>::New();

		std::unordered_map<vtkIdType, vtkIdType> old2new; // old point id → new point id
		old2new.reserve(cellIds.size() * 2);

		double ptmp[3];

		for (size_t i = 0; i < cellIds.size(); ++i) {
			vtkIdType cid = cellIds[i];
			vtkCell* cell = src->GetCell(cid);
			if (!cell || cell->GetNumberOfPoints() != 3) continue;

			vtkIdList* ids = cell->GetPointIds();
			vtkIdType oldA = ids->GetId(0), oldB = ids->GetId(1), oldC = ids->GetId(2);

			auto mapPoint = [&](vtkIdType oldId)->vtkIdType {
				auto it = old2new.find(oldId);
				if (it != old2new.end()) return it->second;
				src->GetPoint(oldId, ptmp);
				vtkIdType newId = static_cast<vtkIdType>(outPoints->GetNumberOfPoints());
				outPoints->InsertNextPoint(ptmp);
				old2new.emplace(oldId, newId);
				return newId;
			};

			vtkIdType a = mapPoint(oldA);
			vtkIdType b = mapPoint(oldB);
			vtkIdType c = mapPoint(oldC);
			vtkIdType tri[3] = { a,b,c };
			outPolys->InsertNextCell(3, tri);
		}

		auto out = vtkSmartPointer<vtkPolyData>::New();
		out->SetPoints(outPoints);
		out->SetPolys(outPolys);
		out->BuildCells();
		return out;
	}

	/** PolyData 총면적(cm²) */
	inline double poly_total_area(vtkPolyData* pd) {
		const vtkIdType nCells = pd->GetNumberOfCells();
		double A = 0.0;
		for (vtkIdType cid = 0; cid < nCells; ++cid) {
			vtkCell* cell = pd->GetCell(cid);
			if (!cell || cell->GetNumberOfPoints() != 3) continue;
			vtkIdList* ids = cell->GetPointIds();
			double p0[3], p1[3], p2[3];
			pd->GetPoint(ids->GetId(0), p0);
			pd->GetPoint(ids->GetId(1), p1);
			pd->GetPoint(ids->GetId(2), p2);
			A += vtkTriangle::TriangleArea(p0, p1, p2);
		}
		return A;
	}

	/** refineFactor에 맞는 반복 횟수 선택: 4^n >= refineFactor → n=ceil(log(refine)/log(4)) */
	inline int choose_linear_subdiv_iterations(double refineFactor) {
		if (refineFactor <= 1.0) return 0;
		double n = std::ceil(std::log(refineFactor) / std::log(4.0));
		if (n < 0.0) n = 0.0;
		if (n > 8.0) n = 8.0; // 안전상한
		return static_cast<int>(n);
	}

	/** 선형 세분화(Linear Subdivision) → 면 보존에 유리, VTK 8.2 호환 */
	inline vtkSmartPointer<vtkPolyData> linear_subdivide(vtkPolyData* in, int nIters) {
		auto triClean = ensure_tri_clean(in);
		if (nIters <= 0) return triClean;

		auto subdiv = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
		subdiv->SetInputData(triClean);
		subdiv->SetNumberOfSubdivisions(nIters);
		subdiv->Update();

		// 세분화 후 약간의 정리 및 노멀 (선택)
		auto clean2 = vtkSmartPointer<vtkCleanPolyData>::New();
		clean2->SetInputConnection(subdiv->GetOutputPort());
		clean2->PointMergingOn();
		clean2->Update();

		auto normals = vtkSmartPointer<vtkPolyDataNormals>::New();
		normals->SetInputConnection(clean2->GetOutputPort());
		normals->ConsistencyOn();
		normals->SplittingOff();
		normals->ComputeCellNormalsOn();
		normals->ComputePointNormalsOff();
		normals->Update();

		return normals->GetOutput();
	}

	/** 삼각형 cell의 법선 벡터(단위벡터) 계산: ROI 조각에 CellNormals가 없을 때 fallback */
	inline std::array<double, 3> compute_cell_normal_from_geometry(vtkPolyData* pd, vtkIdType cid) {
		vtkCell* cell = pd->GetCell(cid);
		vtkIdList* ids = cell->GetPointIds();
		double p0[3], p1[3], p2[3];
		pd->GetPoint(ids->GetId(0), p0);
		pd->GetPoint(ids->GetId(1), p1);
		pd->GetPoint(ids->GetId(2), p2);

		// n = normalize( (p1-p0) x (p2-p0) )
		double u[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
		double v[3] = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };
		double n[3] = {
			u[1] * v[2] - u[2] * v[1],
			u[2] * v[0] - u[0] * v[2],
			u[0] * v[1] - u[1] * v[0]
		};
		double len = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (len <= 0.0) return { 0.0, 0.0, 0.0 };
		return { n[0] / len, n[1] / len, n[2] / len };
	}

	inline vtkSmartPointer<vtkPolyData> ExportGeodesicROIForPhantom(int phantomID,
		vtkPolyData* skinRaw,
		vtkIdType seed,
		double targetAreaCm2,
		double refineFactor,
		vtkSmartPointer<vtkPolyData>* roiBeforeSubdivision,
		std::map<vtkIdType, double>* outCellAreas = nullptr)
	{
		// 1) 삼각형화 & 클린
		auto triClean = ensure_tri_clean(skinRaw);

		// 2) 지오데식 확장으로 ROI cell 집합 구하기
		double achieved = 0.0;
		std::vector<vtkIdType> roiCells =
			grow_area_by_geodesic(triClean, seed, targetAreaCm2, achieved);

		// 3) ROI만 복사하여 PolyData 구성 (세분화 "전" ROI)
		vtkSmartPointer<vtkPolyData> roiPoly =
			extract_cells_as_polydata(triClean, roiCells);

		// (옵션) 외부에서 세분화 전 ROI를 쓰고 싶다면 out 파라미터로 전달
		if (roiBeforeSubdivision) {
			*roiBeforeSubdivision = roiPoly;
		}

		// 4) refineFactor에 맞춰 반복 횟수 선택 (예: 50 → n=3)
		const int nIters = choose_linear_subdiv_iterations(refineFactor);

		// 5) 선형 세분화 실행 → 세분화된 ROI PolyData (리턴값)
		vtkSmartPointer<vtkPolyData> roiSubdiv =
			linear_subdivide(roiPoly, nIters);

		// 6) facetID별 면적 계산해서 map에 저장
		if (outCellAreas) {
			outCellAreas->clear();
			const vtkIdType nCells = roiSubdiv->GetNumberOfCells();
			for (vtkIdType cid = 0; cid < nCells; ++cid) {
				vtkCell* cell = roiSubdiv->GetCell(cid);
				if (!cell || cell->GetNumberOfPoints() != 3) continue;

				vtkIdList* ids = cell->GetPointIds();
				double p0[3], p1[3], p2[3];
				roiSubdiv->GetPoint(ids->GetId(0), p0);
				roiSubdiv->GetPoint(ids->GetId(1), p1);
				roiSubdiv->GetPoint(ids->GetId(2), p2);

				double area = vtkTriangle::TriangleArea(p0, p1, p2); // cm²
				(*outCellAreas)[cid] = area;
			}
		}

		return roiSubdiv;
	}

	// 최근접 facet과 depth(표면까지 최단거리) 계산
	inline void find_closest_facet_and_depth(vtkCellLocator* locator, // vtkCellLocator (roiSubdiv 기반으로 build)
		vtkPolyData* pd, // 같은 polydata (cellId 유효성 확인 및 노멀 추출용)
		const double p[3], // 입력 좌표 {x,y,z} (cm)
		vtkIdType& outCellId, // 최근접 facet(cell) ID 반환
		double& outDepth, // 표면까지 최소거리(양의 값, cm). signedDepth가 true면 부호 포함
		double outClosestPoint[3], // 최근접 표면 점(선택)
		bool signedDepth = false) // signedDepth true면 부호 있는 깊이(노멀과 방향 내적 사용). ROI 조각 오리엔테이션이 불명확할 수 있으니 주의.
	{
		double closest[3] = { 0,0,0 };
		vtkIdType cellId = -1;
		int subId = -1;
		double dist2 = 0.0;

		// VTK 8.2 시그니처
		locator->FindClosestPoint(p, closest, cellId, subId, dist2);

		outCellId = cellId;
		if (outClosestPoint) {
			outClosestPoint[0] = closest[0];
			outClosestPoint[1] = closest[1];
			outClosestPoint[2] = closest[2];
		}

		double d = std::sqrt(dist2); // 최소거리(표면까지)
		if (!signedDepth || cellId < 0) {
			outDepth = d;
			return;
		}

		// === 부호 있는 깊이 계산(선택) ===
		// 벡터 v = (closest → p)
		double v[3] = { p[0] - closest[0], p[1] - closest[1], p[2] - closest[2] };

		// 1) cell data에 노멀 배열이 있으면 사용
		auto cellNormals = pd->GetCellData() ? pd->GetCellData()->GetNormals() : nullptr;
		std::array<double, 3> n = { 0,0,0 };
		if (cellNormals && cellId >= 0 && cellId < pd->GetNumberOfCells()) {
			double nn[3] = { 0,0,0 };
			cellNormals->GetTuple(cellId, nn);
			// 정규화
			double len = std::sqrt(nn[0] * nn[0] + nn[1] * nn[1] + nn[2] * nn[2]);
			if (len > 0.0) n = { nn[0] / len, nn[1] / len, nn[2] / len };
		}
		else {
			// 2) 없으면 기하학으로 계산
			n = compute_cell_normal_from_geometry(pd, cellId);
		}

		// 부호 = dot(n, v)의 부호. (관례: dot>0 이면 노멀 방향으로 내부/외부 중 하나)
		// 프로젝트 기준으로 "내부 방향이 + (inward)"가 되도록 필요시 n을 반전하세요.
		double dot = n[0] * v[0] + n[1] * v[1] + n[2] * v[2];
		outDepth = (dot >= 0.0 ? +d : -d);
	}


	//roiSubdiv상의 seed cell을 찾기 위해, "원본 seed 삼각형의 중심 좌표"에 가장 가까운 roiSubdiv의 cell ID를 반환
	inline vtkIdType closest_cell_id_for_point(vtkCellLocator* locator, const double p[3]) {
		double cp[3] = { 0,0,0 };
		vtkIdType cellId = -1;
		int subId = -1;
		double dist2 = 0.0;
		locator->FindClosestPoint(p, cp, cellId, subId, dist2);
		return cellId; // -1이면 실패
	}

	//지오데식(센터-센터 거리 가중)으로 seed에서 프런트를 확장하며누적 면적을 추적, 1차 / 2차 타겟 면적(예: 1 cm², 10 cm²)에 도달했을 때의 facetID 집합을 한 번의 탐색으로 뽑는다.
	inline void geodesic_dual_area_region(vtkPolyData* pd, // (세분화된) PolyData. 삼각형 메쉬를 가정.
		vtkIdType seedCellId, // 시작 cell ID (pd 기준)
		double areaTarget1, // 첫 번째 타겟 면적 (예: 1.0)
		double areaTarget2, // 두 번째 타겟 면적 (예: 10.0). areaTarget2 >= areaTarget1 가정
		std::vector<vtkIdType>& outIDs1, // areaTarget1까지 포함된 cell ID들
		std::vector<vtkIdType>& outIDs2, //  areaTarget2까지 포함된 cell ID들
		double* achieved1 = nullptr, // (선택) 실제 areaTarget1 지점의 누적 면적
		double* achieved2 = nullptr) //  (선택) 실제 areaTarget2 지점의 누적 면적
	{
		if (areaTarget2 < areaTarget1) std::swap(areaTarget1, areaTarget2);

		const vtkIdType nCells = pd->GetNumberOfCells();

		// 인접 리스트 + 센터/면적 미리 계산
		std::vector<std::vector<vtkIdType>> adj;
		build_edge_adjacency(pd, adj);

		std::vector<std::array<double, 3>> centers(static_cast<size_t>(nCells));
		std::vector<double> areas(static_cast<size_t>(nCells));
		double meshArea = 0.0;
		for (vtkIdType i = 0; i < nCells; ++i) {
			centers[static_cast<size_t>(i)] = cell_center(pd, i);
			double a = cell_area(pd, i);
			areas[static_cast<size_t>(i)] = a;
			meshArea += a;
		}

		if (seedCellId < 0 || seedCellId >= nCells) {
			throw std::runtime_error("geodesic_dual_area_region: invalid seed");
		}

		// Dijkstra: dist=센터-센터 거리
		typedef std::pair<double, vtkIdType> QItem;
		struct Cmp { bool operator()(const QItem& a, const QItem& b) const { return a.first > b.first; } };
		const double INF = std::numeric_limits<double>::infinity();
		std::vector<double> dist(static_cast<size_t>(nCells), INF);
		std::vector<char>   done(static_cast<size_t>(nCells), 0);
		std::priority_queue<QItem, std::vector<QItem>, Cmp> pq;

		dist[static_cast<size_t>(seedCellId)] = 0.0;
		pq.push(QItem(0.0, seedCellId));

		// 누적 면적과 select 벡터(10 cm²까지)
		std::vector<vtkIdType> selected;
		selected.reserve(1024);
		double accum = 0.0;

		// 첫 타겟 컷 위치
		size_t cutIndexFor1 = (size_t)-1;
		double achievedAt1 = 0.0;

		// 탐색
		while (!pq.empty()) {
			QItem top = pq.top(); pq.pop();
			double du = top.first;
			vtkIdType u = top.second;
			if (done[static_cast<size_t>(u)]) continue;
			done[static_cast<size_t>(u)] = 1;

			accum += areas[static_cast<size_t>(u)];
			selected.push_back(u);

			// 첫 타겟(1 cm²) 지점 기록 (한 번만)
			if (cutIndexFor1 == (size_t)-1 && accum >= areaTarget1) {
				cutIndexFor1 = selected.size(); // selected[0..cutIndexFor1-1]
				achievedAt1 = accum;
				if (accum >= areaTarget2) break; // 1과 2가 동일했으면 여기서 종료
			}

			// 두 번째 타겟(10 cm²) 도달 시 종료
			if (accum >= areaTarget2) break;

			// relax
			const auto& nbrs = adj[static_cast<size_t>(u)];
			for (size_t k = 0; k < nbrs.size(); ++k) {
				vtkIdType v = nbrs[k];
				if (done[static_cast<size_t>(v)]) continue;
				double w = norm3(centers[static_cast<size_t>(u)], centers[static_cast<size_t>(v)]);
				double alt = du + w;
				if (alt < dist[static_cast<size_t>(v)]) {
					dist[static_cast<size_t>(v)] = alt;
					pq.push(QItem(alt, v));
				}
			}
		}

		// 결과 구성
		if (cutIndexFor1 == (size_t)-1) {
			// 메쉬 면적이 areaTarget1 미만인 경우: 가능한 만큼만
			cutIndexFor1 = selected.size();
			achievedAt1 = accum;
		}

		outIDs1.assign(selected.begin(), selected.begin() + static_cast<std::ptrdiff_t>(cutIndexFor1));
		outIDs2.swap(selected); // selected 전체(= areaTarget2 시점)

		if (achieved1) *achieved1 = achievedAt1;
		if (achieved2) *achieved2 = accum;
	}
} // namespace HP_SkinDoseCalculate

//************************************** Common Util Functions **************************************//
void ETHuman3DApp::SetMessageBox(QString text)
{
	QMessageBox MsgBox;
	MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
	MsgBox.setText(text);
	MsgBox.exec();
}
void ETHuman3DApp::SetMessageBox_RadionuclideWarning(QString text)
{
	if (b_IsDontShowRadionuclideWarningChecked == false)
	{
		QMessageBox MsgBox;
		MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
		MsgBox.setText(text);
		MsgBox.setStyleSheet("QLabel{min-width: 800px;}");

		// Create a checkbox and add it to the message box
		QCheckBox dontShowAgain("Do not show this message again", &MsgBox);
		MsgBox.setCheckBox(&dontShowAgain);

		MsgBox.exec();

		// Save the state of the checkbox
		if (dontShowAgain.isChecked()) {
			b_IsDontShowRadionuclideWarningChecked = true;
		}
	}
}
void ETHuman3DApp::SetMessageBox_NoButton(QString text)
{
	QMessageBox MsgBox;
	MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
	MsgBox.setText(text);
	MsgBox.setStandardButtons(QMessageBox::NoButton);  
	MsgBox.exec();
}
int ETHuman3DApp::SetMessageBox_YesNo(QString text)
{
	QMessageBox MsgBox;
	MsgBox.setWindowFlag(Qt::WindowStaysOnTopHint);
	MsgBox.setStandardButtons(QMessageBox::Yes);
	MsgBox.addButton(QMessageBox::No);
	MsgBox.setDefaultButton(QMessageBox::No);

	MsgBox.setText(text);

	return MsgBox.exec();
}
void ETHuman3DApp::RestartProgram()
{
// McSEE_local
	std::string command = "\"./McSEE.exe\"";

// McSEE_NAS
	//std::string command = "\"./McSEE_NAS.exe\"";

	// Set up the command line parameters
	STARTUPINFOA startupInfo = { sizeof(startupInfo) };
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	//startupInfo.wShowWindow = SW_HIDE; // Hide the window
	startupInfo.wShowWindow = SW_SHOW; // Show the window

	// Launch the command prompt -> Restart
	PROCESS_INFORMATION processInfo;
	BOOL success = CreateProcessA(
		nullptr,                    // lpApplicationName (null to use command)
		const_cast<char*>(command.c_str()),  // lpCommandLine
		nullptr,                    // lpProcessAttributes
		nullptr,                    // lpThreadAttributes
		FALSE,                      // bInheritHandles
		0,                          // dwCreationFlags
		nullptr,                    // lpEnvironment
		nullptr,                    // lpCurrentDirectory
		&startupInfo,               // lpStartupInfo
		&processInfo                // lpProcessInformation
	);
	exit(0); // Exit program
}
void ETHuman3DApp::MouseControl_toActor()
{
	// Mouse Control InteractorActor로 전환	
	//m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
	m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(m_pVTKWidget->GetBaseInteractorStyleTrackballActor());
	m_pVTKWidget->GetBaseInteractorStyle()->OnLeftButtonUp();
	QCoreApplication::processEvents();

	m_pVTKWidget->ForceNonPickableForFixedObjects();
}
void ETHuman3DApp::MouseControl_toCamera()
{
	m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(m_pVTKWidget->GetBaseInteractorStyle());

	// 마우스 클릭 한 번 되어 있는 거 풀기	
	m_pVTKWidget->GetBaseInteractorStyle()->CallOnLeftButton();
	QCoreApplication::processEvents();

	m_pVTKWidget->ForceNonPickableForFixedObjects();
}
std::string ETHuman3DApp::formatToThreeDecimalPlaces(const std::string& numStr) { // string 변수를 받아서 소수점 셋째짜리까지 채워서 표현!
	double num;
	std::stringstream ss(numStr);
	ss >> num;

	std::stringstream out;
	out << std::scientific << std::setprecision(3) << num;

	return out.str();
}
float ETHuman3DApp::CalcDistance2D(QPointF v1, QPointF v2)
{
	float fRes = sqrt((v2.x() - v1.x()) * (v2.x() - v1.x()) +
		(v2.y() - v1.y()) * (v2.y() - v1.y()));
	return fRes;
}
// std::vector<int> vec = {0, 1, 2, 3, 4}에서 Remove the number 3 -> removeElement(vec, 3) -> vec = {0, 1, 2, 4}
int ETHuman3DApp::removeElement(std::vector<int>& vec, int toRemove) {
	// Find the element
	auto it = std::find(vec.begin(), vec.end(), toRemove);

	// Check if the element is found
	if (it != vec.end()) {
		// Calculate the index
		int index = std::distance(vec.begin(), it);

		// Remove the element
		vec.erase(it);

		// Return the original index
		if (index == vec.size()) return index - 1; // if index is last position of vector, minus 1
		else return index;
	}
	else {
		// Return -1 or some other value to indicate that the element was not found
		return -1;
	}
}
// std::vector<int> vec = {0, 1, 2, 3, 4}에서 Transpose the number 4 to 1(1번 index 자리로, 즉 0과 1 사이로)-> transposeElement(vec, 4, 1) -> vec = {0, 4, 1, 2, 3}
int ETHuman3DApp::transposeElement(std::vector<int>& vec, int elementToMove, QString LEFT_Right) {
	// Find the position of the element to move
	auto it = std::find(vec.begin(), vec.end(), elementToMove);

	// If the element is found
	if (it != vec.end()) {
		if (LEFT_Right == "right") {
			// If it's not the last element, swap with the next element
			if (it != vec.end() - 1) {
				std::iter_swap(it, it + 1);		
				return *(it + 1);
			}
		}
		else if (LEFT_Right == "left") {
			// If it's not the first element, swap with the previous element
			if (it != vec.begin()) {
				std::iter_swap(it, it - 1);
				return *(it - 1);
			}
		}
	}
	return -1;
}
QString ETHuman3DApp::getQStringNumberInSpecificDigit(double number, int digit) // double 변수를 받아서 숫자 개수를 특정 개수로 맞추기
{
	QString numberStr = QString::number(number, 'g', 15);

	// Split the number into integer and fractional parts
	QStringList parts = numberStr.split('.');
	int integerPartLength = parts[0].length();

	// Calculate the required number of decimal places
	int decimalPlaces = digit - integerPartLength;

	// Ensure decimal places are non-negative
	if (decimalPlaces < 0) {
		decimalPlaces = 0;
	}

	// Format the number with the calculated decimal places
	return QString::number(number, 'f', decimalPlaces);
}
std::string ETHuman3DApp::SetComputerID()
{
	std::string tmp_computerID_English;
	std::string tmp_computerID_Korean;
#ifdef _WIN32
	wchar_t hostNameW[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(hostNameW) / sizeof(hostNameW[0]);

	if (GetComputerNameW(hostNameW, &size)) {
		std::wstring computerNameW = hostNameW;

		// Korean to English converted version
		std::stringstream result;

		for (wchar_t ch : computerNameW) {
			if (ch >= 0xAC00 && ch <= 0xD7A3) {  // Korean Syllables range
				result << std::hex << static_cast<int>(ch);  // Convert to hexadecimal representation
			}
			else {
				int utf8Size = WideCharToMultiByte(CP_UTF8, 0, &ch, 1, NULL, 0, NULL, NULL);
				std::string utf8Char(utf8Size, 0);
				WideCharToMultiByte(CP_UTF8, 0, &ch, 1, &utf8Char[0], utf8Size, NULL, NULL);
				result << utf8Char;
			}
		}
		tmp_computerID_English = result.str();

		// Korean non-converted version(Korean name)
		int utf8Size = WideCharToMultiByte(CP_UTF8, 0, computerNameW.c_str(), -1, NULL, 0, NULL, NULL);
		std::string computerNameS(utf8Size - 1, 0);
		WideCharToMultiByte(CP_UTF8, 0, computerNameW.c_str(), -1, &computerNameS[0], utf8Size, NULL, NULL);
		tmp_computerID_Korean = computerNameS;
	}
	else {
		//std::cerr << "Failed to get computer name." << std::endl;
	}
#else
	char hostName[256];
	if (gethostname(hostName, sizeof(hostName)) == 0) {
		tmp_computerID_English = std::string(hostName);
	}
#endif
	std::time_t currentTime = std::time(nullptr);
	char timeStr[100];
	std::strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", std::localtime(&currentTime));
	tmp_computerID_English += "_" + std::string(timeStr);

	return tmp_computerID_English;
}
void ETHuman3DApp::MultipleUICloseTrigger() // 닫을 때 Ok버튼으로 눌러서 정상종료하지 않았으면 Closed 플래그 업데이트
{
	if (pRt->m_Is_PhantomSetting_OKbutton_Clicked == false) pRt->m_Is_PhantomSetting_Closed = true;
	if (pRt->m_Is_ClothingSetting_OKbutton_Clicked == false) pRt->m_Is_ClothingSetting_Closed = true;
	if (pRt->m_Is_ClothingLayer_Setting_OKbutton_Clicked == false) pRt->m_Is_ClothingLayer_Setting_Closed = true;
	if (pRt->b_IsObjectSettingOKClicked == false) pRt->b_IsObjectSettingClosed = true;		
	if (pRt->b_IsSourceOV_AddingSettingOKClicked == false) pRt->b_IsSourceOV_AddingSettingClosed = true;
}
std::string ETHuman3DApp::ExtractInnerString(std::string& input)
{
	if (input.length() >= 2 && input.front() == '[' && input.back() == ']') {
		// 첫 번째 문자 '['와 마지막 문자 ']'을 제외한 부분 반환
		return input.substr(1, input.length() - 2);
	}
	else {
		// 잘못된 입력 처리
		return "";
	}
}
void ETHuman3DApp::Selecting3DShpere(double* ptCenter) // 마우스 클릭한 지점을 임시로 표시하는 기능
{
	if (NowSelecting3DSphereActor != nullptr)
	{
		m_pVTKWidget->GetSceneRenderer()->RemoveActor(NowSelecting3DSphereActor);
		NowSelecting3DSphereActor = nullptr; // Actor 삭제
		//m_pVTKWidget->renderWindow()->Render();
		m_pVTKWidget->renderWindow()->Render();
	}

	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	 
	NowSelecting3DSphereActor = vtkSmartPointer<vtkActor>::New();
	NowSelecting3DSphereActor->SetMapper(mapper);
	NowSelecting3DSphereActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	NowSelecting3DSphereActor->SetPosition(ptCenter);
	NowSelecting3DSphereActor->SetPickable(false);
	NowSelecting3DSphereActor->GetProperty()->SetOpacity(0.2);	

	m_pVTKWidget->GetSceneRenderer()->AddActor(NowSelecting3DSphereActor);
	//m_pVTKWidget->renderWindow()->Render();
	m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::Selecting3DShpere_Delete()
{
	if (NowSelecting3DSphereActor != nullptr)
	{
		m_pVTKWidget->GetSceneRenderer()->RemoveActor(NowSelecting3DSphereActor);
		NowSelecting3DSphereActor = nullptr; // Actor 삭제
		//m_pVTKWidget->renderWindow()->Render();
		m_pVTKWidget->renderWindow()->Render();
	}
}
void ETHuman3DApp::CleaningPolyData(vtkSmartPointer<vtkPolyData> polyData, std::string OutputPath)
{
	vtkSmartPointer<vtkPoints> OldPoints = polyData->GetPoints(); // Cleaning with deletion of duplicated vertex and face.
	vtkSmartPointer<vtkCellArray> OldCells = polyData->GetPolys(); // Cleaning with deletion of duplicated vertex and face.
	vtkSmartPointer<vtkPoints> NewPoints = vtkSmartPointer<vtkPoints>::New(); // Create a new Points object to store the unique vertices
	vtkSmartPointer<vtkCellArray> NewCells = vtkSmartPointer<vtkCellArray>::New(); // Create a new CellArray to store the processed cells

	std::unordered_map<std::string, int> vertexMap; // vertexMap["0.1,0.2,0.3"] = 2(vertexID) -> 0부터 시작
	std::unordered_set<std::string> cellIndexSet; // cellIndexSet = ["1,2,3", "2,3,4" ...] -> 오름차순 정렬된 cell을 이루는 vertexId
	int newVertextId = 0;

	// Iterate over the cells in the input PolyData
	vtkIdType nCells = polyData->GetNumberOfCells();
	for (vtkIdType i = 0; i < nCells; i++) // cell 의 총 개수
	{
		// Get the cell from the input PolyData
		vtkCell* OldCell = polyData->GetCell(i); // cell[0] 을 불러옴

		// Create a new cell to store the updated vertex IDs
		vtkSmartPointer<vtkIdList> newCellId = vtkSmartPointer<vtkIdList>::New();
		vtkSmartPointer<vtkTriangle> newCell = vtkSmartPointer<vtkTriangle>::New();

		// Iterate over the vertices in the cell
		vtkIdType nVerts = OldCell->GetNumberOfPoints(); // cell[0]의 vertex 개수
		for (vtkIdType j = 0; j < nVerts; j++) // Cell을 이루는 vertex 3개이므로 0, 1, 2에 대해서 순환
		{
			// Get the original vertex ID
			vtkIdType vertexId = OldCell->GetPointId(j); // vertexId는 cell[0]의 0번째, 1번째, 2번째 vertexId
			double* OldSpecificPoint = OldPoints->GetPoint(vertexId);
			std::string	key = std::to_string((OldSpecificPoint[0])) + "," + std::to_string(OldSpecificPoint[1]) + "," + std::to_string(OldSpecificPoint[2]);
			auto itr = vertexMap.find(key);
			if (itr != vertexMap.end()) // 만약 값이 있으면
			{
				newCellId->InsertNextId(itr->second);
			}
			else // 만약 값이 없으면
			{
				vertexMap.insert(std::make_pair(key, newVertextId)); // 0부터 생성
				NewPoints->InsertNextPoint(OldSpecificPoint);
				newCellId->InsertNextId(newVertextId);
				newVertextId++; // 그 후 더하기
			}
		}
		// cell을 이루는 vertexID 오름차순 정렬
		int a = newCellId->GetId(0);
		int b = newCellId->GetId(1);
		int c = newCellId->GetId(2);
		if (a > b) std::swap(a, b);
		if (b > c) std::swap(b, c);
		if (a > b) std::swap(a, b);
		std::string	CellString = std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c);
		if (cellIndexSet.count(CellString) <= 0) // 동일한 cell이 존재하지 않을때만 추가
		{
			cellIndexSet.insert(CellString);
			newCell->GetPointIds()->SetId(0, newCellId->GetId(0));
			newCell->GetPointIds()->SetId(1, newCellId->GetId(1));
			newCell->GetPointIds()->SetId(2, newCellId->GetId(2));
			NewCells->InsertNextCell(newCell);
		}
	}
	vtkSmartPointer<vtkPolyData> FixedPolyData = vtkSmartPointer<vtkPolyData>::New();
	FixedPolyData->SetPoints(NewPoints);
	FixedPolyData->SetPolys(NewCells);
	// Clean한 것을 obj로 쓰기  
	QString qPath_fixed = QString::fromStdString(OutputPath);
	vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
	writer->SetFileName(Util::Wcs_to_mbs(qPath_fixed.toStdWString()).c_str());
	writer->SetInputData(FixedPolyData);
	writer->Write();
}

//************************************** Phantom Widget **************************************//

////////////////// Multiple Phantom //////////////////
void ETHuman3DApp::PhantomPolydataActor_Generate(QString strFileName, int SelectedIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom)
{	
	// Save phantom file name from phantomDir
	PhantomFileTitle[SelectedIndex] = QString::fromStdString(GetPhantomFileTitle(strFileName.toStdString()));
	// Save phantom sclae factor
	PhantomPolyDataScaleFactor[SelectedIndex][0] = BodySizeInfo_for_ThisPhantom.xyScale;
	PhantomPolyDataScaleFactor[SelectedIndex][1] = BodySizeInfo_for_ThisPhantom.zScale;
	// Generate phantom polydata
	vtkSmartPointer<vtkPolyData> polydata_phantom;
	if (pRt->m_Phantom_MainInfo[SelectedIndex][pRt->E_PHANTOMMAININFO_TYPE] != pRt->E_PHANTOMTYPE_IMPORTED) // Imported가 아닐 때
	{
		vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		reader->SetFileName(Util::Wcs_to_mbs(strFileName.toStdWString()).c_str());
		reader->Update();
		polydata_phantom = Util::CreatePolyData(reader);
	}
	if (pRt->m_Phantom_MainInfo[SelectedIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_IMPORTED) // Imported 일때
	{
		std::vector<std::string> extract_organlist;
		extract_organlist.push_back("12200_Skin_surface");
		extract_organlist.push_back("6700_Cornea_left");
		extract_organlist.push_back("6900_Cornea_right");

		// 캡처한 변수들을 사용하여 ExtractPhantomOBJ 함수의 인자를 준비합니다.
		std::string path = m_ImportedPhantomFilePath_NoExtention[SelectedIndex].toStdString();
		std::string name = PhantomFileTitle[SelectedIndex].toStdString();

		// 백그라운드에서 시간이 오래 걸리는 함수를 호출합니다. 
		std::string outPath = ExtractPhantomOBJ(path, name, extract_organlist);

		QString QoutPath = QString::fromStdString(outPath);
		vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		reader->SetFileName(Util::Wcs_to_mbs(QoutPath.toStdWString()).c_str());
		reader->Update();
		polydata_phantom = Util::CreatePolyData(reader);

		if (QFile::exists(QoutPath)) QFile::remove(QoutPath);
	}

	float SkinColor[3] = { 255, 192, 160 }; // Skin color
	vtkNew<vtkUnsignedCharArray> PhantomColors;
	PhantomColors->SetNumberOfComponents(3);
	PhantomColors->SetNumberOfTuples(polydata_phantom->GetNumberOfPoints());
	for (int i = 0; i < polydata_phantom->GetNumberOfPoints(); ++i)
	{
		PhantomColors->SetTuple(i, SkinColor);
	}
	polydata_phantom->GetPointData()->SetScalars(PhantomColors);
	// Generate polydata_original, polydata_base (which is set to 0,0,0 center and scaled (for body-size-dependent phantom)
	PhantomPolyData_original[SelectedIndex] = polydata_phantom; // Set polydata_original (.obj 불러온 그대로의 팬텀파일)
	double *pCenter = polydata_phantom->GetCenter();
	PhantomOrigianlPolyDataCenter[SelectedIndex][0] = pCenter[0]; PhantomOrigianlPolyDataCenter[SelectedIndex][1] = pCenter[1]; PhantomOrigianlPolyDataCenter[SelectedIndex][2] = pCenter[2]; // Set phantomOriginal center position
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(-PhantomOrigianlPolyDataCenter[SelectedIndex][0], -PhantomOrigianlPolyDataCenter[SelectedIndex][1], -PhantomOrigianlPolyDataCenter[SelectedIndex][2]); // 원본 파일을 offset 진행하여 GUI 상에서 (0,0,0)에 맞도록 설정
	transform->Scale(PhantomPolyDataScaleFactor[SelectedIndex][0], PhantomPolyDataScaleFactor[SelectedIndex][0], PhantomPolyDataScaleFactor[SelectedIndex][1]); // Body-size-dependent phantom sacling
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polydata_phantom);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	PhantomPanelPolyData_Collection[SelectedIndex][0][0] = transformFilter->GetOutput(); // Set PhantomPanelPolyData_Collection (phantom_base -> 원점이동/스케일링된 polydata)
	
	// Generate actor
	vtkSmartPointer<vtkActor> actor_phantom = Util::CreateActor(PhantomPanelPolyData_Collection[SelectedIndex][0][0]);
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	actor_phantom->SetProperty(property);	
	// Full/Dummy phantom Actor save
	FullPhantomActor[SelectedIndex] = actor_phantom; //Full phantom
	if(pRt->m_Phantom_MainInfo[SelectedIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_ADULTMRCP
	|| pRt->m_Phantom_MainInfo[SelectedIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_PEDIATRICMRCP
	|| pRt->m_Phantom_MainInfo[SelectedIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_TRANSFORMED) 
	{
		//Dummy phantom
		vtkSmartPointer<vtkOBJReader> reader_dummy = vtkSmartPointer<vtkOBJReader>::New();
		QString dummyQstr;
		dummyQstr = "./data/phantom/dummy/" + BodySizeInfo_for_ThisPhantom.HtWtName_dummy + ".obj";		
		reader_dummy->SetFileName(Util::Wcs_to_mbs(dummyQstr.toStdWString()).c_str());
		reader_dummy->Update();
		vtkSmartPointer<vtkPolyData> polydata_dummy = Util::CreatePolyData(reader_dummy);
		float SkinColor_dummy[3] = { 255, 192, 160 }; // Skin color
		vtkNew<vtkUnsignedCharArray> PhantomColors_dummy;
		PhantomColors_dummy->SetNumberOfComponents(3);
		PhantomColors_dummy->SetNumberOfTuples(polydata_dummy->GetNumberOfPoints());
		for (int i = 0; i < polydata_dummy->GetNumberOfPoints(); ++i)
		{
			PhantomColors_dummy->SetTuple(i, SkinColor_dummy);
		}
		polydata_dummy->GetPointData()->SetScalars(PhantomColors_dummy);
		double *pCenter_dummy = polydata_dummy->GetCenter();
		DummyPolyDataInfo[SelectedIndex][0] = pCenter_dummy[0];
		DummyPolyDataInfo[SelectedIndex][1] = pCenter_dummy[1];
		DummyPolyDataInfo[SelectedIndex][2] = pCenter_dummy[2];
		DummyPolyDataInfo[SelectedIndex][3] = BodySizeInfo_for_ThisPhantom.xyScale_dummy;
		DummyPolyDataInfo[SelectedIndex][4] = BodySizeInfo_for_ThisPhantom.xyScale_dummy;
		DummyPolyDataInfo[SelectedIndex][5] = BodySizeInfo_for_ThisPhantom.zScale_dummy;
		vtkSmartPointer<vtkTransform> transform_dummy = vtkSmartPointer<vtkTransform>::New();
		transform_dummy->PostMultiply(); //this is the key line
		transform_dummy->Translate(-pCenter_dummy[0], -pCenter_dummy[1], -pCenter_dummy[2]);
		transform_dummy->Scale(BodySizeInfo_for_ThisPhantom.xyScale_dummy, BodySizeInfo_for_ThisPhantom.xyScale_dummy, BodySizeInfo_for_ThisPhantom.zScale_dummy); // Body-size-dependent phantom sacling
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_dummy = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_dummy->SetInputData(polydata_dummy);
		transformFilter_dummy->SetTransform(transform_dummy);
		transformFilter_dummy->Update();
		vtkSmartPointer<vtkActor> actor_dummy = Util::CreateActor(transformFilter_dummy->GetOutput());
		auto property_dummy = Util::GetProperty();
		property_dummy->SetInterpolationToPhong();
		property_dummy->SetDiffuse(1.0);
		actor_dummy->SetProperty(property_dummy);
		DummyPhantomActor[SelectedIndex] = actor_dummy;
	}
	AppendPhantomPolyData_GeneratePhantomActor(SelectedIndex);
}

void ETHuman3DApp::AppendPhantomPolyData_GeneratePhantomActor(int PhantomIdx)
{
	// Appending with phantom	
	PhantomPanelAccumulatedPolyData[PhantomIdx] = PhantomPanelPolyData_Collection[PhantomIdx][0][0]; // phantom polydata_base
	for (auto itr_PolyData : PhantomPanelPolyData_Collection[PhantomIdx][1]) // Clothing
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}
	for (auto itr_PolyData : PhantomPanelPolyData_Collection[PhantomIdx][2]) // Dosimeter
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}
	for (auto itr_PolyData : PhantomPanelPolyData_Collection[PhantomIdx][3]) // Glasses
	{
		vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
		appendFilter->AddInputData(PhantomPanelAccumulatedPolyData[PhantomIdx]);
		appendFilter->AddInputData(itr_PolyData.second);
		appendFilter->Update();

		vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New(); // For making appendfilter to polydata 
		geomFilter->SetInputData(appendFilter->GetOutput());
		geomFilter->Update();

		PhantomPanelAccumulatedPolyData[PhantomIdx] = geomFilter->GetOutput();
	}

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(PhantomPanelAccumulatedPolyData[PhantomIdx]);
	mapper->SetScalarVisibility(true);

	// Generate actor
	m_pVTKWidget->GetSceneRenderer()->RemoveActor(PhantomPanelActor[PhantomIdx]);
	PhantomPanelActor[PhantomIdx] = nullptr;
	PhantomPanelActor[PhantomIdx] = Util::CreateActor(PhantomPanelAccumulatedPolyData[PhantomIdx]);
	PhantomPanelActor[PhantomIdx]->SetMapper(mapper);
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	PhantomPanelActor[PhantomIdx]->SetProperty(property);

	// Translate actor
	double height = pRt->m_Phantom_MainInfo[PhantomIdx][4]; // Set phantomInfo
	double weight = pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterX = pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterY = pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double CenterZ = pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorX = pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorY = pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double VectorZ = pRt->m_Phantom_MainInfo[PhantomIdx][11];
	PhantomPanelActor[PhantomIdx]->SetPosition(CenterX, CenterY, CenterZ);
	PhantomPanelActor[PhantomIdx]->SetOrientation(VectorX, VectorY, VectorZ);

	// Add Actor to View
	m_pVTKWidget->GetSceneRenderer()->AddActor(PhantomPanelActor[PhantomIdx]);
	//m_pVTKWidget->renderWindow()->Render();
	m_pVTKWidget->renderWindow()->Render();
}
BodySizeInfo ETHuman3DApp::CalcBoydSizeScaleFactor(int phantomType, int phantomGender, int phantomAge, int phantomPosture, double phantomHeight, double phantomWeight)
{
	BodySizeInfo BodySizeInfo_for_ThisPhantom;

	// Set Deformed Phantom List
	//std::vector<double> AdultMaleLibrary_Height = { 160,160,160,160,160,160,165,165,165,165,165,165,165,165,165,165,165,165,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,175,175,
	//	175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,185,185,185,185,185,185,185,185,185,
	//	185,185,185,185,185,185,185,185,185,185,190,190,190,190,190,190,190,190,190,190,190,190,190,190,190 };
	//std::vector<double> AdultMaleLibrary_Weight = { 50,55,60,65,70,75,50,55,60,65,70,75,80,85,90,95,100,105,110,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,55,60,65,70,75,80,85,90,95,100,
	//	105,110,115,120,125,130,135,140,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,155,70,75,80,
	//	85,90,95,100,105,110,115,120,125,130,135,140};
	//std::vector<double> AdultFemaleLibrary_Height = { 150,150,150,150,150,150,150,150,150,150,150,150,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,160,160,160,160,160,160,160,160,
	//	160,160,160,160,160,160,160,160,160,160,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,
	//	170,170,170,170,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175 };
	//std::vector<double> AdultFemaleLibrary_Weight = { 40,45,50,55,60,65,70,75,80,85,90,95,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,
	//	130,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150,55,60,65,70,75,80,85,90,95,100,105,110,115,
	//	120,125,130,135	};
	std::vector<double> AdultMaleAnchor_Height = { 160,185,160,190,170,165,175,170,170,170,165 };
	std::vector<double> AdultMaleAnchor_Weight = { 65,90,70,105,85,85,100,95,100,105,105 };
	std::vector<double> AdultFemaleAnchor_Height = { 155,175,160,150,175,175,175,165,175,165,170 };	
	std::vector<double> AdultFemaleAnchor_Weight = { 60,80,70,65,95,100,105,95,110,105,120 };

	std::vector<double> Pediatric_15M_Anchor_Height = { 170, 170, 170, 170, 170, 170, 170 };
	std::vector<double> Pediatric_15M_Anchor_Weight = { 63.777, 67.059, 70.698, 74.754, 79.304, 84.444, 90.296 };
	std::vector<double> Pediatric_10M_Anchor_Height = { 135, 135, 135, 135, 135, 135, 135 };
	std::vector<double> Pediatric_10M_Anchor_Weight = { 32.265, 33.708, 35.287, 37.020, 38.933, 41.053, 43.419 };
	std::vector<double> Pediatric_05M_Anchor_Height = { 105, 105, 105, 105, 105 };
	std::vector<double> Pediatric_05M_Anchor_Weight = { 17.942, 18.786, 19.713, 20.736, 21.871 }; 
	std::vector<double> Pediatric_01M_Anchor_Height = { 81, 81, 81, 81, 81 };
	std::vector<double> Pediatric_01M_Anchor_Weight = { 10.317, 10.650, 11.004, 11.383, 11.789 };
	std::vector<double> Pediatric_00M_Anchor_Height = { 48, 48, 50 };
	std::vector<double> Pediatric_00M_Anchor_Weight = { 2.7, 3.1, 3.7 };

	std::vector<double> Pediatric_15F_Anchor_Height = { 165, 165, 165, 165, 165, 165, 165 };
	std::vector<double> Pediatric_15F_Anchor_Weight = { 59.212, 62.439, 66.038, 70.078, 74.644, 79.847, 85.829 };
	std::vector<double> Pediatric_10F_Anchor_Height = { 134, 134, 134, 134, 134, 134, 134 };
	std::vector<double> Pediatric_10F_Anchor_Weight = { 31.800, 33.281, 34.908, 36.702, 38.690, 40.905, 43.600 };
	std::vector<double> Pediatric_05F_Anchor_Height = { 105, 105, 105, 105, 105 };
	std::vector<double> Pediatric_05F_Anchor_Weight = { 18.163, 18.944, 19.795, 20.726, 21.749 };
	std::vector<double> Pediatric_01F_Anchor_Height = { 79, 79, 79, 79, 79 };
	std::vector<double> Pediatric_01F_Anchor_Weight = { 9.814, 10.227, 10.676, 11.166, 11.703 };
	std::vector<double> Pediatric_00F_Anchor_Height = { 47, 49, 51 };
	std::vector<double> Pediatric_00F_Anchor_Weight = { 2.6, 3.2, 3.8 };

	// Get global variables
	double phantomBMI = phantomWeight / phantomHeight / phantomHeight * 10000;
	// Initialize variables
	double xy_scale = 1;
	double z_scale = 1;
	QString filename;	
	double new_weight = 1.;
	std::vector<double> HeightVector;
	std::vector<double> WeightVector;
	double reference_height;
	double reference_weight;

	if (phantomType == pRt->E_PHANTOMTYPE_ADULTMRCP || phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP) // MRCP
	{
		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		BodySizeInfo_for_ThisPhantom.HtWtName = ""; // For MRCP, not used
		// male_MRCP
		if (phantomGender == 0 && phantomAge == 0) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM";
		if (phantomGender == 0 && phantomAge == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15M";
		if (phantomGender == 0 && phantomAge == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10M";
		if (phantomGender == 0 && phantomAge == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05M";
		if (phantomGender == 0 && phantomAge == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01M";
		if (phantomGender == 0 && phantomAge == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00M";
		// female_MRCP
		if (phantomGender == 1 && phantomAge == 0) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF";
		if (phantomGender == 1 && phantomAge == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15F";
		if (phantomGender == 1 && phantomAge == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10F";
		if (phantomGender == 1 && phantomAge == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05F";
		if (phantomGender == 1 && phantomAge == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01F";
		if (phantomGender == 1 && phantomAge == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00F";

		return BodySizeInfo_for_ThisPhantom; //MRCP 종료
	}
	if (phantomType == pRt->E_PHANTOMTYPE_IMPORTED || phantomType == pRt->E_PHANTOMTYPE_PFMRCP) //Imported and pfMRCP -> no dummy phantom
	{
		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		BodySizeInfo_for_ThisPhantom.HtWtName = "";
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "";

		return BodySizeInfo_for_ThisPhantom; // Imported 종료
	}

	// Transformed phantom - standing (body-size only)
	if (phantomGender == 0 && phantomAge == 0 && phantomPosture ==0)
	{
		reference_height = 176.;
		reference_weight = 73.;		
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM"; // Dummy phantom Name
		HeightVector = AdultMaleAnchor_Height;
		WeightVector = AdultMaleAnchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 1 && phantomPosture == 0)
	{
		reference_height = 167.;
		reference_weight = 56.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15M"; // Dummy phantom Name
		HeightVector = Pediatric_15M_Anchor_Height;
		WeightVector = Pediatric_15M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 2 && phantomPosture == 0)
	{
		reference_height = 138.;
		reference_weight = 32.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10M"; // Dummy phantom Name
		HeightVector = Pediatric_10M_Anchor_Height;
		WeightVector = Pediatric_10M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 3 && phantomPosture == 0)
	{
		reference_height = 109.;
		reference_weight = 19.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05M"; // Dummy phantom Name
		HeightVector = Pediatric_05M_Anchor_Height;
		WeightVector = Pediatric_05M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 4 && phantomPosture == 0)
	{
		reference_height = 76.;
		reference_weight = 10.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01M"; // Dummy phantom Name
		HeightVector = Pediatric_01M_Anchor_Height;
		WeightVector = Pediatric_01M_Anchor_Weight;
	}
	if (phantomGender == 0 && phantomAge == 5 && phantomPosture == 0)
	{
		reference_height = 51.;
		reference_weight = 3.5;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00M"; // Dummy phantom Name
		HeightVector = Pediatric_00M_Anchor_Height;
		WeightVector = Pediatric_00M_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 0 && phantomPosture == 0)
	{
		reference_height = 163.;
		reference_weight = 60.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF"; // Dummy phantom Name
		HeightVector = AdultFemaleAnchor_Height;
		WeightVector = AdultFemaleAnchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 1 && phantomPosture == 0)
	{
		reference_height = 161.;
		reference_weight = 53.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_15F"; // Dummy phantom Name
		HeightVector = Pediatric_15F_Anchor_Height;
		WeightVector = Pediatric_15F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 2 && phantomPosture == 0)
	{
		reference_height = 138.;
		reference_weight = 32.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_10F"; // Dummy phantom Name
		HeightVector = Pediatric_10F_Anchor_Height;
		WeightVector = Pediatric_10F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 3 && phantomPosture == 0)
	{
		reference_height = 109.;
		reference_weight = 19.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_05F"; // Dummy phantom Name
		HeightVector = Pediatric_05F_Anchor_Height;
		WeightVector = Pediatric_05F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 4 && phantomPosture == 0)
	{
		reference_height = 76.;
		reference_weight = 10.;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_01F"; // Dummy phantom Name
		HeightVector = Pediatric_01F_Anchor_Height;
		WeightVector = Pediatric_01F_Anchor_Weight;
	}
	if (phantomGender == 1 && phantomAge == 5 && phantomPosture == 0)
	{
		reference_height = 51.;
		reference_weight = 3.5;
		BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_00F"; // Dummy phantom Name
		HeightVector = Pediatric_00F_Anchor_Height;
		WeightVector = Pediatric_00F_Anchor_Weight;
	}

	// Deformed phantom - other postures (body-size and posture)
	if (phantomGender == 0 && phantomAge == 0 && phantomPosture != 0)
	{
		reference_height = 176.;
		reference_weight = 73.;	
		HeightVector;
		WeightVector;
		if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_bending"; // Dummy phantom Name
		if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_kneeling"; // Dummy phantom Name
		if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_sitting"; // Dummy phantom Name
		if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_squatting"; // Dummy phantom Name
		if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AM_walking"; // Dummy phantom Name
	}
	if (phantomGender == 1 && phantomAge == 0 && phantomPosture != 0)
	{
		reference_height = 163.;
		reference_weight = 60.;
		HeightVector;
		WeightVector;
		if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_bending"; // Dummy phantom Name
		if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_kneeling"; // Dummy phantom Name
		if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_sitting"; // Dummy phantom Name
		if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_squatting"; // Dummy phantom Name
		if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName_dummy = "MRCP_AF_walking"; // Dummy phantom Name
	}

	if (phantomPosture != 0) // 임시: 자세변형 팬텀은 아직 체형변형 안되어서 무조건 표준팬텀으로부터 변경
	{
		if (phantomGender == 0)
		{
			if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_bending"; // Dummy phantom Name
			if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_kneeling"; // Dummy phantom Name
			if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_sitting"; // Dummy phantom Name
			if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_squatting"; // Dummy phantom Name
			if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AM_walking"; // Dummy phantom Name
		}
		if (phantomGender == 1)
		{
			// Set Full phantom scale info
			if (phantomPosture == 1) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_bending"; // Dummy phantom Name
			if (phantomPosture == 2) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_kneeling"; // Dummy phantom Name
			if (phantomPosture == 3) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_sitting"; // Dummy phantom Name
			if (phantomPosture == 4) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_squatting"; // Dummy phantom Name
			if (phantomPosture == 5) BodySizeInfo_for_ThisPhantom.HtWtName = "MRCP_AF_walking"; // Dummy phantom Name
		}

		BodySizeInfo_for_ThisPhantom.zScale = 1;
		BodySizeInfo_for_ThisPhantom.xyScale = 1;
		BodySizeInfo_for_ThisPhantom.zScale_dummy = 1;
		BodySizeInfo_for_ThisPhantom.xyScale_dummy = 1;

		return BodySizeInfo_for_ThisPhantom; // Posture 임시 종료 -> 현재는 자세-체형 동시변형 불가
	}

	// Calculate height within X% and closest BMI phantom
	double min_height = phantomHeight * 0.0;
	double max_height = phantomHeight * 1000.0;
	std::vector<int> height_within_index;
	for (size_t i = 0; i < HeightVector.size(); ++i) {
		if (HeightVector[i] >= min_height && HeightVector[i] <= max_height) {
			height_within_index.push_back(i);
		}
	}
	std::map<int, double> BMI_within;
	for (auto itr_index : height_within_index)
	{
		BMI_within[itr_index] = (WeightVector[itr_index] / HeightVector[itr_index] / HeightVector[itr_index] * 10000);
	}
	int closest_ID = -1;
	double smallestDifference = DBL_MAX;
	for (const auto& itr : BMI_within)
	{		
		double difference = std::abs(itr.second - phantomBMI);
		if (difference < smallestDifference) 
		{
			smallestDifference = difference;
			closest_ID = itr.first;
		}
	}
	
	// Set Full phantom scale info
	BodySizeInfo_for_ThisPhantom.HtWtName = "H" + QString::number((int)HeightVector[closest_ID]) + "M" + QString::number((int)(WeightVector[closest_ID]*1000));
	BodySizeInfo_for_ThisPhantom.zScale = phantomHeight / HeightVector[closest_ID];
	BodySizeInfo_for_ThisPhantom.xyScale = sqrt(phantomWeight / (WeightVector[closest_ID] * BodySizeInfo_for_ThisPhantom.zScale));

	//Set Dummy phantom scale info except for pre-determined phantom name
	BodySizeInfo_for_ThisPhantom.zScale_dummy = phantomHeight / reference_height;
	BodySizeInfo_for_ThisPhantom.xyScale_dummy = sqrt(phantomWeight / (reference_weight * BodySizeInfo_for_ThisPhantom.zScale_dummy));

	return BodySizeInfo_for_ThisPhantom;
}
std::string ETHuman3DApp::GetPhantomFileTitle(std::string phantomDir)
{
	std::string delimiter = "/";
	size_t pos = phantomDir.rfind(delimiter);
	phantomDir = phantomDir.substr(pos + 1); // pTitle.XXX만 남기기
	pos = phantomDir.find(".");
	if (pos != std::string::npos)
	{
		phantomDir.erase(pos);
	}

	return phantomDir;
}
QString ETHuman3DApp::GetPhantomFile_AbsolutePath(int phantomType, int phantomGender, int phantomAge, int phantomPosture, int phantomFetalAge, int phantomIndex, BodySizeInfo BodySizeInfo_for_ThisPhantom)
{
	QString phantomPath;
	
	if (phantomType == pRt->E_PHANTOMTYPE_IMPORTED) 
	{
		phantomPath = m_ImportedPhantomFilePath_NoExtention[phantomIndex];

		// Set m_ImportedPhantom_ParentPhantomFileName

		// male_MRCP
		if (phantomGender == 0 && phantomAge == 0) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_AM";
		if (phantomGender == 0 && phantomAge == 1) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_15M";
		if (phantomGender == 0 && phantomAge == 2) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_10M";
		if (phantomGender == 0 && phantomAge == 3) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_05M";
		if (phantomGender == 0 && phantomAge == 4) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_01M";
		if (phantomGender == 0 && phantomAge == 5) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_00M";
		// female_MRCP
		if (phantomGender == 1 && phantomAge == 0) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_AF";
		if (phantomGender == 1 && phantomAge == 1) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_15F";
		if (phantomGender == 1 && phantomAge == 2) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_10F";
		if (phantomGender == 1 && phantomAge == 3) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_05F";
		if (phantomGender == 1 && phantomAge == 4) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_01F";
		if (phantomGender == 1 && phantomAge == 5) m_ImportedPhantom_ParentPhantomFileName[phantomIndex] = "MRCP_00F";						
	}
	
	// male_MRCP
	if (phantomType == pRt->E_PHANTOMTYPE_ADULTMRCP && phantomGender == 0 && phantomAge == 0) phantomPath = "./data/phantom/MRCP/MRCP_AM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 1) phantomPath = "./data/phantom/MRCP/MRCP_15M.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 2) phantomPath = "./data/phantom/MRCP/MRCP_10M.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 3) phantomPath = "./data/phantom/MRCP/MRCP_05M.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 4) phantomPath = "./data/phantom/MRCP/MRCP_01M.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 0 && phantomAge == 5) phantomPath = "./data/phantom/MRCP/MRCP_00M.obj";
	// female_MRCP
	if (phantomType == pRt->E_PHANTOMTYPE_ADULTMRCP && phantomGender == 1 && phantomAge == 0) phantomPath = "./data/phantom/MRCP/MRCP_AF.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 1) phantomPath = "./data/phantom/MRCP/MRCP_15F.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 2) phantomPath = "./data/phantom/MRCP/MRCP_10F.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 3) phantomPath = "./data/phantom/MRCP/MRCP_05F.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 4) phantomPath = "./data/phantom/MRCP/MRCP_01F.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PEDIATRICMRCP && phantomGender == 1 && phantomAge == 5) phantomPath = "./data/phantom/MRCP/MRCP_00F.obj";
	// pf_MRCP
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_8w) phantomPath = "./data/phantom/MRCP/08wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_10w) phantomPath = "./data/phantom/MRCP/10wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_15w) phantomPath = "./data/phantom/MRCP/15wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_20w) phantomPath = "./data/phantom/MRCP/20wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_25w) phantomPath = "./data/phantom/MRCP/25wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_30w) phantomPath = "./data/phantom/MRCP/30wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_35w) phantomPath = "./data/phantom/MRCP/35wM.obj";
	if (phantomType == pRt->E_PHANTOMTYPE_PFMRCP && phantomFetalAge == pRt->E_PHANTOMFETALAGE_38w) phantomPath = "./data/phantom/MRCP/38wM.obj";	
	// male_deformed - standing (body-size only)
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 0) phantomPath = "./data/phantom/bodysize/AM_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 1) phantomPath = "./data/phantom/bodysize/15M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 2) phantomPath = "./data/phantom/bodysize/10M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 3) phantomPath = "./data/phantom/bodysize/05M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 4) phantomPath = "./data/phantom/bodysize/01M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 0 && phantomAge == 5) phantomPath = "./data/phantom/bodysize/00M_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	// female_deformed - standing (body-size only)
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 0) phantomPath = "./data/phantom/bodysize/AF_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 1) phantomPath = "./data/phantom/bodysize/15F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 2) phantomPath = "./data/phantom/bodysize/10F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 3) phantomPath = "./data/phantom/bodysize/05F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 4) phantomPath = "./data/phantom/bodysize/01F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomPosture == 0 && phantomGender == 1 && phantomAge == 5) phantomPath = "./data/phantom/bodysize/00F_" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
		
	// male_deformed - other posture (posture/body-size) - 지금은 body-size X
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 1) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 2) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 3) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 4) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 0 && phantomPosture == 5) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	// female_deformed - other posture (posture/body-size) - 지금은 body-size X
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 1) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 2) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 3) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 4) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";
	if (phantomType == pRt->E_PHANTOMTYPE_TRANSFORMED && phantomGender == 1 && phantomPosture == 5) phantomPath = "./data/phantom/posture/" + BodySizeInfo_for_ThisPhantom.HtWtName + ".obj";

	return phantomPath;
}
std::string ETHuman3DApp::ExtractPhantomOBJ(std::string PATH, std::string Name, std::vector<std::string> list)
{
	std::string input = PATH + ".obj";
	std::string output = PATH + "_o.obj"; // 바깥에서 참조함

	QFile* inFile = new QFile(QString::fromStdString(input));
	QIODeviceIStream in(inFile);
		
	std::string temp_str;
	int temp_int;
	double temp_double;
	char temp_char[256];
	char* remain = NULL;

	std::map<int, std::vector<std::tuple<double, double, double>>> vertex_vec;
	std::map<int, std::vector<std::tuple<int, int, int>>> face_vec;
	int num = 0;

	int OrganID;
	std::vector<double> OrganID_vec;
	std::vector<std::string> OrganName_vec;

	std::vector<std::pair<std::string, std::pair<double, std::vector<std::tuple<double, double, double>>>>> vertex;
	std::vector<std::pair<std::string, std::pair<double, std::vector<std::tuple<int, int, int>>>>> face;

	while (!in.eof())
	{
		in >> temp_str;
		if (temp_str.size() == 1 && temp_str == "v")
		{
			double vertex[3] = { 0,0,0 };
			for (int i = 0; i < 3; i++)
			{
				in >> temp_double;
				vertex[i] = temp_double;
			}
			vertex_vec[num].push_back(std::make_tuple(vertex[0], vertex[1], vertex[2]));
		}

		if (temp_str == "g")
		{
			in >> temp_char;
			char* newe = strtok_s(temp_char, "_", &remain);
			OrganID = atoi(newe);
			OrganID_vec.push_back(OrganID);

		}
		if (temp_str == "usemtl")
		{
			in >> temp_str;
			//cout << temp_str << endl;
			OrganName_vec.push_back(temp_str);
		}

		if (temp_str.size() == 1 && temp_str == "f")
		{
			int facet[3] = { 0,0,0 };
			for (int i = 0; i < 3; i++)
			{
				in >> temp_int;
				facet[i] = temp_int; 
			}
			face_vec[num].push_back(std::make_tuple(facet[0], facet[1], facet[2]));
		}

		num = OrganName_vec.size();
	}
	inFile->close();


	for (int i = 0; i < OrganName_vec.size(); i++)
	{
		bool skip = true;
		for (auto itr : list)
		{
			if (OrganName_vec[i] == itr) skip = false;
		}
		if (!skip)
		{
			vertex.push_back(make_pair(OrganName_vec[i], make_pair(OrganID_vec[i], vertex_vec[i])));
			face.push_back(make_pair(OrganName_vec[i], make_pair(OrganID_vec[i], face_vec[i + 1])));
		}
	}


	std::vector<int> x;
	std::vector<int> y;
	std::vector<int> z;
	std::vector<int> min_face;

	int min_find[3];

	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			x.push_back(get<0>(face[i].second.second[j]));
			y.push_back(get<1>(face[i].second.second[j]));
			z.push_back(get<2>(face[i].second.second[j]));
		}

		min_find[0] = *min_element(x.begin(), x.end());
		min_find[1] = *min_element(y.begin(), y.end());
		min_find[2] = *min_element(z.begin(), z.end());

		int min = min_find[0];
		for (int j = 0; j < 3; j++) {
			if (min_find[j] < min) min = min_find[j];
		}

		min_face.push_back(min);

		x.clear();
		y.clear();
		z.clear();
	}

	std::pair<std::string, std::pair<double, std::vector<std::tuple<double, double, double>>>> vertex_new;
	std::pair<std::string, std::pair<double, std::vector<std::tuple<int, int, int>>>> face_new;

	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			get<0>(face[i].second.second[j]) = get<0>(face[i].second.second[j]) - min_face[i] + 1;
			get<1>(face[i].second.second[j]) = get<1>(face[i].second.second[j]) - min_face[i] + 1;
			get<2>(face[i].second.second[j]) = get<2>(face[i].second.second[j]) - min_face[i] + 1;
		}
	}

	for (int i = 0; i < vertex.size() - 1; i++) {
		for (int j = i + 1; j < vertex.size(); j++) {
			if (vertex[j].second.first <= vertex[i].second.first) {
				vertex_new = vertex[j];
				vertex[j] = vertex[i];
				vertex[i] = vertex_new;
			}
		}
		for (int j = i + 1; j < face.size(); j++) {
			if (face[j].second.first <= face[i].second.first) {
				face_new = face[j];
				face[j] = face[i];
				face[i] = face_new;
			}
		}
	}

	std::vector<int> size_vertex;
	int vertex_s = 0;

	for (int i = 0; i < face.size(); i++) {
		size_vertex.push_back(vertex_s);
		vertex_s += vertex[i].second.second.size();
	}
	for (int i = 0; i < face.size(); i++) {
		for (int j = 0; j < face[i].second.second.size(); j++) {
			get<0>(face[i].second.second[j]) = get<0>(face[i].second.second[j]) + size_vertex[i];
			get<1>(face[i].second.second[j]) = get<1>(face[i].second.second[j]) + size_vertex[i];
			get<2>(face[i].second.second[j]) = get<2>(face[i].second.second[j]) + size_vertex[i];
		}
	}
	
	QFile* outFile = new QFile(QString::fromStdString(output));
	QIODeviceOStream result(outFile);

	result << "#Obj File : Exported From RapidForm (INUS Technology Co. Ltd.)" << endl << endl;
	result << "mtllib " + Name + ".mtl" << endl << endl;

	for (int i = 0; i < vertex.size(); i++) {
		for (int j = 0; j < vertex[i].second.second.size(); j++) {
			result << "v " << get<0>(vertex[i].second.second[j]) << " " << get<1>(vertex[i].second.second[j]) << " " << get<2>(vertex[i].second.second[j]) << endl;
		}

		result << endl;
		result << "g " << vertex[i].first << endl << "usemtl " << vertex[i].first << endl << "s" << endl << endl;

		if (vertex[i].first == OrganName_vec[vertex.size() - 1]) {
			for (int j = 0; j < face[i].second.second.size() - 1; j++) {
				result << "f " << get<0>(face[i].second.second[j]) << " " << get<1>(face[i].second.second[j]) << " " << get<2>(face[i].second.second[j]) << endl;
			}
		}
		else {
			for (int j = 0; j < face[i].second.second.size(); j++) {
				result << "f " << get<0>(face[i].second.second[j]) << " " << get<1>(face[i].second.second[j]) << " " << get<2>(face[i].second.second[j]) << endl;
			}
		}

		result << endl;

	}
	outFile->close();

	return output;
}

void ETHuman3DApp::UpdatePhantom_InfoStatus_InActorMouseControl(int phantomIndex) // 마우스로 움직인 팬텀 현재 위치 및 회전으로 패널값(line) 업데이트
{		
	double* pCenter = PhantomPanelActor[phantomIndex]->GetPosition();
	double TransformPos[3];
	TransformPos[0] = pCenter[0];
	TransformPos[1] = pCenter[1];
	TransformPos[2] = pCenter[2];
	QString StringTranslatedPosX = getQStringNumberInSpecificDigit(TransformPos[0], 6);
	QString StringTranslatedPosY = getQStringNumberInSpecificDigit(TransformPos[1], 6);
	QString StringTranslatedPosZ = getQStringNumberInSpecificDigit(TransformPos[2], 6);

	double* pRotation = PhantomPanelActor[phantomIndex]->GetOrientation();
	double TransformRot[3];
	TransformRot[0] = pRotation[0];
	TransformRot[1] = pRotation[1];
	TransformRot[2] = pRotation[2];
	QString StringRotX = getQStringNumberInSpecificDigit(TransformRot[0], 6);
	QString StringRotY = getQStringNumberInSpecificDigit(TransformRot[1], 6);
	QString StringRotZ = getQStringNumberInSpecificDigit(TransformRot[2], 6);

	pRt->PhantomPosX_QLineEdit->setText(StringTranslatedPosX);
	pRt->PhantomPosY_QLineEdit->setText(StringTranslatedPosY);
	pRt->PhantomPosZ_QLineEdit->setText(StringTranslatedPosZ);

	pRt->PhantomRotX_QLineEdit->setText(StringRotX);
	pRt->PhantomRotY_QLineEdit->setText(StringRotY);
	pRt->PhantomRotZ_QLineEdit->setText(StringRotZ);
}
void ETHuman3DApp::UpdatePhantom_ActorHighlighted(int phantomIndex) // 현재 선택된 팬텀버튼에 해당하는 팬텀만 pickable 하도록
{
	for (auto itr_phanntom : pRt->m_Phantom_SequenceVector)
	{
		if (itr_phanntom == phantomIndex)
		{
			PhantomPanelActor[itr_phanntom]->GetProperty()->SetDiffuse(1.0);
		}
		else
		{
			PhantomPanelActor[itr_phanntom]->GetProperty()->SetDiffuse(0.6);
		}
	}
	//m_pVTKWidget->renderWindow()->Render();
	m_pVTKWidget->renderWindow()->Render();
}

////////////////// Phantom Clothing ////////////////// 
void ETHuman3DApp::PhantomClothingGenerate(QString strFileName, bool IsPreDefinedClothing) // called by (1) slot_ClothingAddingOK_ButtonClicked, (2) RubberBandUserClothingGenerate, (3) LoadReconsturctionFile_previous
{
	int phantomIdx = pRt->m_Phantom_SelectedIndex;
	int clothingIdx = pRt->m_Clothing_MakingIndex; 	

	if (strFileName == "") return; //Imported 팬텀이어서 없으면 빠져나가기	
	
	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
	reader->SetFileName(Util::Wcs_to_mbs(strFileName.toStdWString()).c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> clothingPolydata_forTetra = vtkSmartPointer<vtkPolyData>::New();
	clothingPolydata_forTetra = Util::CreatePolyData(reader);
	vtkSmartPointer<vtkPolyData> clothingPolydata = vtkSmartPointer<vtkPolyData>::New();
	clothingPolydata = Util::CreatePolyData(reader);
	
	// 사면체화 시 이용할 polydata 저장(pCenter 0,0,0 안 맞추고 체형 스케일링만 된 상태 -> 추후 Geant4에서 의복의 스케일링은 진행되지 않도록 해야함)
	vtkSmartPointer<vtkTransform> transform_forTetra = vtkSmartPointer<vtkTransform>::New();
	transform_forTetra->PostMultiply(); //this is the key line	
	if (IsPreDefinedClothing == true) transform_forTetra->Scale(PhantomPolyDataScaleFactor[phantomIdx][0], PhantomPolyDataScaleFactor[phantomIdx][0], PhantomPolyDataScaleFactor[phantomIdx][1]); // 체형 스케일링
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_forTetra = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_forTetra->SetInputData(clothingPolydata_forTetra);
	transformFilter_forTetra->SetTransform(transform_forTetra);
	transformFilter_forTetra->Update();
	OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx] = transformFilter_forTetra->GetOutput(); // 이후 dosimeter 겹침 체크하기 위해 polydata로 저장
	vtkSmartPointer<vtkOBJWriter> writer_ClothingOBJ = vtkSmartPointer<vtkOBJWriter>::New();
	std::string ClothingOBJStr = "./data/wearable/Clothing_forTetra/" + std::to_string(phantomIdx) + "_" + std::to_string(clothingIdx) + ".obj"; // 사면체화에 사용할 OBJ 파일 생성
	writer_ClothingOBJ->SetFileName(ClothingOBJStr.c_str());
	writer_ClothingOBJ->SetInputData(OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx]);
	writer_ClothingOBJ->Update();

	// 가시화용 의복 polydata pCenter 0,0,0 맞추고 체형스케일링
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(-PhantomOrigianlPolyDataCenter[phantomIdx][0], -PhantomOrigianlPolyDataCenter[phantomIdx][1], -PhantomOrigianlPolyDataCenter[phantomIdx][2]);
	if (IsPreDefinedClothing == true) transform->Scale(PhantomPolyDataScaleFactor[phantomIdx][0], PhantomPolyDataScaleFactor[phantomIdx][0], PhantomPolyDataScaleFactor[phantomIdx][1]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(clothingPolydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	clothingPolydata = transformFilter->GetOutput();
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(clothingPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance;
	offsetDistance = 0.0005; // 가시화를 위해 offset 5 um from phantom skin (dosimeter is 10 um offset)
	for (vtkIdType i = 0; i < clothingPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		clothingPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance;
		p[1] = p[1] + n[1] * offsetDistance;
		p[2] = p[2] + n[2] * offsetDistance;
		clothingPolydata->GetPoints()->SetPoint(i, p);
	}
	clothingPolydata->Modified();

	float SelectedColor[3];
	float ClothesColor[3] = { 240, 228, 140 }; // Wearable color
	SelectedColor[0] = ClothesColor[0]; SelectedColor[1] = ClothesColor[1]; SelectedColor[2] = ClothesColor[2];
	vtkNew<vtkUnsignedCharArray> WearableColors;
	WearableColors->SetNumberOfComponents(3);
	WearableColors->SetNumberOfTuples(clothingPolydata->GetNumberOfPoints());
	for (int i = 0; i < clothingPolydata->GetNumberOfPoints(); ++i)
	{
		WearableColors->SetTuple(i, SelectedColor);
	}
	clothingPolydata->GetPointData()->SetScalars(WearableColors);
	PhantomPanelPolyData_Collection[phantomIdx][1][clothingIdx] = clothingPolydata;

	// Appending with phantom
	AppendPhantomPolyData_GeneratePhantomActor(phantomIdx);

	// Save clothing information
	pRt->SaveClothingInformation_InClothingGenerate();

	// Set clothing panel
	pRt->SetClothingPanelInfo(clothingIdx, 0); // Generate 중에는 0번 layer index를 생성
	pRt->InitializeClothingPanel_InClothingAdd();
}
void ETHuman3DApp::RubberBandInitialization() // called by (1) slot_ClothingAddingOK_ButtonClicked
{
	// Generate areaPickerand set to interactor
	vtkSmartPointer<vtkAreaPicker> areaPicker = vtkSmartPointer<vtkAreaPicker>::New();
	m_pVTKWidget->renderWindow()->GetInteractor()->SetPicker(areaPicker);

	const double PI = vtkMath::Pi() / 180; // deg to rad 변환용 PI
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;

	// Origianl polydata 생성 
	vtkSmartPointer<vtkPolyData> PhantomPolydataOriginal;
	PhantomPolydataOriginal = PhantomPolyData_original[PhantomIdx];

	// Move original polydata to center (0,0,0)
	vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
	transform_p->PostMultiply(); //this is the key line
	transform_p->Translate(-PhantomOrigianlPolyDataCenter[PhantomIdx][0], -PhantomOrigianlPolyDataCenter[PhantomIdx][1], -PhantomOrigianlPolyDataCenter[PhantomIdx][2]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_p->SetInputData(PhantomPolydataOriginal);
	transformFilter_p->SetTransform(transform_p);
	transformFilter_p->Update();
	PhantomPolydataOriginal = transformFilter_p->GetOutput(); 

	// 원점이동된 polydata를 위치/회전/스케일링 적용하여 polydata 생성
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	double xy_scale = PhantomPolyDataScaleFactor[PhantomIdx][0];
	double z_scale = PhantomPolyDataScaleFactor[PhantomIdx][1];

	double PosX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double PosY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double PosZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double RotX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double RotY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double RotZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][11];

	Eigen::Matrix4f X;
	Eigen::Matrix4f Y;
	Eigen::Matrix4f Z;
	Eigen::Matrix4f Scale;

	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;

	X << 1, 0, 0, 0,
		0, cos(RadianX), -sin(RadianX), 0,
		0, sin(RadianX), cos(RadianX), 0,
		0, 0, 0, 1;

	Y << cos(RadianY), 0, sin(RadianY), 0,
		0, 1, 0, 0,
		-sin(RadianY), 0, cos(RadianY), 0,
		0, 0, 0, 1;

	Z << cos(RadianZ), -sin(RadianZ), 0, 0,
		sin(RadianZ), cos(RadianZ), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	Scale << xy_scale, 0, 0, 0,
		0, xy_scale, 0, 0,
		0, 0, z_scale, 0,
		0, 0, 0, 1;

	Eigen::Matrix4f Result = Z * X * Y * Scale; // Rotate Z->X->Y and scaling

	double elementsResult[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elementsResult[i * 4 + j] = Result(i, j);
		}
	}
	transform->SetMatrix(elementsResult);	
	transform->Translate(PosX, PosY, PosZ);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(PhantomPolydataOriginal);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	vtkSmartPointer<vtkPolyData> TransformedRotatedBasePolydata = vtkSmartPointer<vtkPolyData>::New();
	TransformedRotatedBasePolydata = transformFilter->GetOutput();

	//누적(Accumulated) 팬텀 기반으로 Transformed 팬텀 제작-> kdTree point 추출용
	vtkSmartPointer<vtkTransform> transform_AC = vtkSmartPointer<vtkTransform>::New();
	transform_AC->PostMultiply(); //this is the key line
	double xy_scale_AC = 1; // 체형변형 팬텀 또한 이미 scale 되어 있으므로 스케일링은 형식상 존재
	double z_scale_AC = 1;

	double PosX_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double PosY_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double PosZ_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double RotX_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double RotY_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];
	double RotZ_AC = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][11];

	Eigen::Matrix4f X_AC;
	Eigen::Matrix4f Y_AC;
	Eigen::Matrix4f Z_AC;
	Eigen::Matrix4f Scale_AC;

	double RadianX_AC = RotX_AC * PI;
	double RadianY_AC = RotY_AC * PI;
	double RadianZ_AC = RotZ_AC * PI;

	X_AC << 1, 0, 0, 0,
		0, cos(RadianX_AC), -sin(RadianX_AC), 0,
		0, sin(RadianX_AC), cos(RadianX_AC), 0,
		0, 0, 0, 1;

	Y_AC << cos(RadianY_AC), 0, sin(RadianY_AC), 0,
		0, 1, 0, 0,
		-sin(RadianY_AC), 0, cos(RadianY_AC), 0,
		0, 0, 0, 1;

	Z_AC << cos(RadianZ_AC), -sin(RadianZ_AC), 0, 0,
		sin(RadianZ_AC), cos(RadianZ_AC), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	Scale_AC << xy_scale_AC, 0, 0, 0,
		0, xy_scale_AC, 0, 0,
		0, 0, z_scale_AC, 0,
		0, 0, 0, 1;

	Eigen::Matrix4f Result_AC = Z_AC * X_AC * Y_AC * Scale_AC; // Rotate Z->X->Y and scaling

	double elementsResult_AC[16];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elementsResult_AC[i * 4 + j] = Result_AC(i, j);
		}
	}
	transform_AC->SetMatrix(elementsResult_AC);
	transform_AC->Translate(PosX_AC, PosY_AC, PosZ_AC);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_AC = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_AC->SetInputData(PhantomPanelAccumulatedPolyData[PhantomIdx]); // 그냥 polydata(의복, 선량계, 안경까지 모두 누적된 polydata)
	transformFilter_AC->SetTransform(transform_AC);
	transformFilter_AC->Update();
	vtkSmartPointer<vtkPolyData> TransformedAccumulatedPolydata = vtkSmartPointer<vtkPolyData>::New();
	TransformedAccumulatedPolydata = transformFilter_AC->GetOutput();

	m_pVTKWidget->GetBaseInteractorStyleRubberBand()->SetPolyData(TransformedRotatedBasePolydata, TransformedAccumulatedPolydata);
	m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(m_pVTKWidget->GetBaseInteractorStyleRubberBand()); // ETInteractorStyleRubberBand를 실행함
	theApp.m_pVTKWidget->GetBaseInteractorStyleRubberBand()->CallOnLeftButton(); // 최초 wheel button 움직이기 위해 한 번 풀어주기
}
void ETHuman3DApp::RubberBandUserClothingGenerate() // called by (1) ETInteractorStyleRubberBand::Action4Function_Rubberband
{
	if (RubberBandSelectedPointIDs.size() == 0) // 선택한 게 하나도 없을 때
	{
		theApp.SetMessageBox("No region is selected");

		RubberBandSelectedPointIDs.clear();
		theApp.MouseControl_toCamera();
		theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle()); // 다 종료 후 cameraview모드로 돌아가기
		return;
	}	
	int phantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	int clothingIdx = theApp.pRt->m_Clothing_MakingIndex;

	// 의복까지 누적된 팬텀에 해당하는 pointID에 일치하는 point와 일치하는 cell data 얻어서 polydata 제작
	vtkSmartPointer<vtkPoints> originalPoints = PhantomPanelAccumulatedPolyData[phantomIdx]->GetPoints(); // 누적 팬텀에서 points 데이터 획득

	vtkSmartPointer<vtkCellArray> SelectedCells = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	
	float SkinColor[3] = { 255, 192, 160 };
	vtkNew<vtkUnsignedCharArray> Colors; // RubberBand 포함된 point 다시 skincolor로
	Colors->DeepCopy(PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointData()->GetScalars());

	for (vtkIdType i = 0; i < originalPoints->GetNumberOfPoints(); i++)
	{
		auto iter = theApp.RubberBandSelectedPointIDs.find(i);
		if (iter != theApp.RubberBandSelectedPointIDs.end()) // RubberBand 포함된 pointID이면
		{
			Colors->SetTuple(i, SkinColor); // Selection된 부분 다시 skin color로
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointCells(i, cellIds); // cellIds에 dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPanelAccumulatedPolyData[phantomIdx]->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				SelectedCells->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	PhantomPanelAccumulatedPolyData[phantomIdx]->GetPointData()->SetScalars(Colors); // Selection된 부분 다시 skin color로 초기화
	vtkSmartPointer<vtkPolyData> SelectedPolyData = vtkSmartPointer<vtkPolyData>::New();
	SelectedPolyData->SetPoints(originalPoints); //Original point(누적 polydata의 node)
	SelectedPolyData->SetPolys(SelectedCells); // 누적 polydata에서 selected된 cell

	//polydata를 0.001 cm offset 진행 후 OBJ로 뽑기(base layer, 이후 thickness에 맞춰서 offset 후 추가 layer들 정의 되고 이를 기반으로 사면체화진행)
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInputData(SelectedPolyData);
	connectivityFilter->SetExtractionModeToAllRegions(); // 이걸 해야 VTK 가시화창에서 의복색상이 깔끔하게 나옴
	connectivityFilter->Update();

	vtkSmartPointer<vtkPolyData> TempSelectedPolydata = vtkSmartPointer<vtkPolyData>::New();
	TempSelectedPolydata = connectivityFilter->GetOutput();

	//offset
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(TempSelectedPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	for (vtkIdType i = 0; i < TempSelectedPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		TempSelectedPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * 0.001; // 10 um(0.001 cm) margin
		p[1] = p[1] + n[1] * 0.001;
		p[2] = p[2] + n[2] * 0.001;
		TempSelectedPolydata->GetPoints()->SetPoint(i, p);
	}
	TempSelectedPolydata->Modified();

	// pCenter를 0,0,0 맞추지 않은 것으로 되돌리기(pre-defined 의복의 원본 obj파일과 같이 맞춰주기 위함)
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply(); //this is the key line
	transform->Translate(PhantomOrigianlPolyDataCenter[phantomIdx][0], PhantomOrigianlPolyDataCenter[phantomIdx][1], PhantomOrigianlPolyDataCenter[phantomIdx][2]); // Center 0,0,0 맞추기
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(TempSelectedPolydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	TempSelectedPolydata = transformFilter->GetOutput();

	// 사용자제작 의복 obj 파일 경로
	std::string UserDefinedClothingFileName = "./data/wearable/Clothing_UserDefined/" + std::to_string(phantomIdx) + "_" + std::to_string(clothingIdx) + ".obj";

	//Cleaning polydata and Generate OBJ file
	CleaningPolyData(TempSelectedPolydata, UserDefinedClothingFileName);

	PhantomClothingGenerate(QString::fromStdString(UserDefinedClothingFileName), false);

	RubberBandSelectedPointIDs.clear();
	theApp.MouseControl_toCamera();
	theApp.m_pVTKWidget->renderWindow()->GetInteractor()->SetInteractorStyle(theApp.m_pVTKWidget->GetBaseInteractorStyle()); // 다 종료 후 cameraview모드로 돌아가기
	m_pVTKWidget->GetBaseInteractorStyleRubberBand()->b_IsClothingMakingDraggingOn = false;
}

////////////////// Phantom Eyewear ////////////////// 
void ETHuman3DApp::PhantomFlatGlassesGenerate() //
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][0] = pRt->EyewearDistance_QLineEdit->text().toDouble(); // distance
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][1] = pRt->EyewearRadius_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][2] = pRt->EyewearThickness_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][3] = pRt->EyewearDensity_QLineEdit->text().toDouble(); // density
	// composition(1: Glasses, 2: Leaded glasses)
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][4] = pRt->EyewearComposition_QComboBox->currentIndex();

	double centerX, centerY, centerZ, nx, ny, nz, pointX, pointY, pointZ;
	int pointID;	

	vtkSmartPointer<vtkPolyData> PhantomPolydataBase; // 원점이동 및 스케일링만 (이동 및 회전은 X)
	PhantomPolydataBase = m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base;

	for (int i = 1; i <= 2; i++)
	{
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		if (i == 1)
		{
		
			pointID = m_GlassesInfo.Glasses1_ID;

			// create a vtkPolyDataNormals object and set input
			vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
			normals->SetInputData(PhantomPolydataBase);

			// compute point normals and update
			normals->ComputePointNormalsOn();
			normals->ComputeCellNormalsOff();
			normals->Update();

			// get the normal data array
			vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

			// get the normal vector at a specific point index			
			double normal[3];
			double originalNormal[3];
			normalData->GetTuple(pointID, originalNormal);
			double averagedNormal[3] = { 0,0,0 };
			
			vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints(); // Polydata_base의 좌표들
			double Pickedpoint[3];
			points->GetPoint(pointID, Pickedpoint); // Polydata_base에서 pickeID를 이용해서 구한 선택한 좌표

			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) 
			{
				double point[3];
				points->GetPoint(i, point);
				double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);//Polydata_base에서 선택한 좌표와 polydata_base 모든 좌표들간의 거리를 구함.
				
				double angleThreshold = 60;
				if (distance < 0.6)
				{
					normalData->GetTuple(i, normal);					

					const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

					vtkMath::Normalize(originalNormal); // polydata_base에서 선택한 좌표의 normal값
					vtkMath::Normalize(normal); // polydata_base의 모든 좌표들의 normal값

					const double dotProduct = vtkMath::Dot(originalNormal, normal);
					if(dotProduct>=cosineThreshold)
					{
						averagedNormal[0] += normal[0];
						averagedNormal[1] += normal[1];
						averagedNormal[2] += normal[2];
					}
				}
			}
			vtkMath::Normalize(averagedNormal);
			nx = averagedNormal[0];
			ny = averagedNormal[1];
			nz = averagedNormal[2];
			//QString str = "x: " + QString::number(nx) + "y: " + QString::number(ny) + "z: " + QString::number(nz);
			//theApp.SetMessageBox(str);

			centerX = Pickedpoint[0]; // InteractorStyle에서 지정한 선택한 좌표
			centerY = Pickedpoint[1];
			centerZ = Pickedpoint[2];


		}
		if (i == 2)
		{
			pointID = m_GlassesInfo.Glasses2_ID;

			// create a vtkPolyDataNormals object and set input
			vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
			normals->SetInputData(PhantomPolydataBase);

			// compute point normals and update
			normals->ComputePointNormalsOn();
			normals->ComputeCellNormalsOff();
			normals->Update();

			// get the normal data array
			vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

			// get the normal vector at a specific point index			
			double normal[3];
			double originalNormal[3];
			normalData->GetTuple(pointID, originalNormal);
			double averagedNormal[3] = { 0,0,0 };

			vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints();
			double Pickedpoint[3];
			points->GetPoint(pointID, Pickedpoint);
			
			for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
			{
				double point[3];
				points->GetPoint(i, point);
				double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);
				
				double angleThreshold = 60; // Normal이 급격하게 다른 건 제외하는 threhsold(각)
				if (distance < 0.6) // 0.6 cm 내에 위치한 점들의 평균 노말선택
				{
					normalData->GetTuple(i, normal);

					const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

					vtkMath::Normalize(originalNormal);
					vtkMath::Normalize(normal);

					const double dotProduct = vtkMath::Dot(originalNormal, normal);
					if (dotProduct >= cosineThreshold)
					{
						averagedNormal[0] += normal[0];
						averagedNormal[1] += normal[1];
						averagedNormal[2] += normal[2];			
					}
				}
			}
			vtkMath::Normalize(averagedNormal);
			nx = averagedNormal[0];
			ny = averagedNormal[1];
			nz = averagedNormal[2];

			centerX = Pickedpoint[0];
			centerY = Pickedpoint[1];
			centerZ = Pickedpoint[2];
		}	

		cylinderSource->SetRadius(pRt->EyewearRadius_QLineEdit->text().toDouble());
		cylinderSource->SetHeight(pRt->EyewearThickness_QLineEdit->text().toDouble());
		cylinderSource->SetCenter(0, (cylinderSource->GetHeight() / 2) + pRt->EyewearDistance_QLineEdit->text().toDouble(), 0); // Height(두께)의 중점에서 시작 + 눈으로부터 이격거리 만큼 이동
		cylinderSource->SetResolution(15);
				
		vtkSmartPointer<vtkMath> math = vtkSmartPointer<vtkMath>::New();
		double normalVector[3] = { nx, ny, nz };
		math->Normalize(normalVector);
		double originalVector[3] = { 0, 1, 0 }; // 실린더 기본 방향은 y-axis에 along한 방향

		double rotationAxis[3], rotationAngle;
		vtkMath::Cross(originalVector, normalVector, rotationAxis);
		rotationAngle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(originalVector, normalVector));

		// Create the transform -> 이렇게 생성되는 Input glasses.obj들은 polydata_base에 맞음(원점이동 되어 있음)
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->PostMultiply();
		transform->RotateWXYZ(rotationAngle, rotationAxis);
		transform->Translate(centerX, centerY, centerZ);

		// obj 파일이 원점이동 전 origianl polydata(즉 .obj파일)과 맞아야 하는데, 현재는 원점이동 및 스케일링 이루어진 polydata_base에서 이루어졌으므로 역원점이동 시켜주어야 함
		// Geant4에 사용되는 tet(node)파일도 origianl .obj파일과 좌표 맞아야 함.
		//transform->Translate(centerX + m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[0], centerY + m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[1], centerZ + m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[2]);

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputConnection(cylinderSource->GetOutputPort());
		transformFilter->SetTransform(transform);
		transformFilter->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(transformFilter->GetOutputPort());

		if (i == 1)
		{
			//vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
			//actor1->SetMapper(mapper);

			//theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor1);
			//theApp.m_pVTKWidget->renderWindow()->Render();

			std::string OutputFilename = "./data/wearable/Glasses/glassesL_" + std::to_string(PhantomIdx) + ".obj";
			
			vtkSmartPointer<vtkOBJWriter> objWriter = vtkSmartPointer<vtkOBJWriter>::New();
			objWriter->SetFileName(OutputFilename.c_str());

			objWriter->SetInputConnection(transformFilter->GetOutputPort());
			objWriter->Write();
		}
		if (i == 2)
		{
			//vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
			//actor2->SetMapper(mapper);

			//theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor2);
			//theApp.m_pVTKWidget->renderWindow()->Render();

			std::string OutputFilename = "./data/wearable/Glasses/glassesR_" + std::to_string(PhantomIdx)+ ".obj";

			vtkSmartPointer<vtkOBJWriter> objWriter = vtkSmartPointer<vtkOBJWriter>::New();
			objWriter->SetFileName(OutputFilename.c_str());

			objWriter->SetInputConnection(transformFilter->GetOutputPort());
			objWriter->Write();
		}
	}
	
	// append two glasses
	std::string GlassesLFileName = "./data/wearable/Glasses/glassesL_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJReader> reader1 = vtkSmartPointer<vtkOBJReader>::New();
	reader1->SetFileName(GlassesLFileName.c_str());
	reader1->Update();

	std::string GlassesRFileName = "./data/wearable/Glasses/glassesR_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJReader> reader2 = vtkSmartPointer<vtkOBJReader>::New();
	reader2->SetFileName(GlassesRFileName.c_str());
	reader2->Update();

	// Create a vtkAppendPolyData object to combine the two vtkPolyData objects
	vtkSmartPointer<vtkAppendPolyData> appendFilterGlasses = vtkSmartPointer<vtkAppendPolyData>::New();
	appendFilterGlasses->AddInputData(reader1->GetOutput());
	appendFilterGlasses->AddInputData(reader2->GetOutput());
	appendFilterGlasses->Update();

	// Write the combined vtkPolyData object to a file
	vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
	std::string glassName = "./data/wearable/Glasses/glasses_" + std::to_string(PhantomIdx) + ".obj"; // 이 파일이 통합된 polydata_base에 맞는 obj 파일로 append에 사용됨
	writer->SetFileName(glassName.c_str());
	writer->SetInputData(appendFilterGlasses->GetOutput());
	writer->Update();	
	
	// 통합된 glass_obj 다시 불러오기
	vtkSmartPointer<vtkOBJReader> reader_obj = vtkSmartPointer<vtkOBJReader>::New();
	QString strPath = QString::fromStdString(glassName);
	reader_obj->SetFileName(Util::Wcs_to_mbs(strPath.toStdWString()).c_str());
	reader_obj->Update();
	vtkSmartPointer<vtkPolyData> WearablePolydata = vtkSmartPointer<vtkPolyData>::New();
	WearablePolydata = Util::CreatePolyData(reader_obj); // polydata_original에 맞음

	//// VTK 상에서 사용할 glasses는 다시 원점이동 시켜주어야 함(polydata_base에 맞아야 하므로)
	//vtkSmartPointer<vtkTransform> transform_pCenter = vtkSmartPointer<vtkTransform>::New();
	//transform_pCenter->PostMultiply();
	//transform_pCenter->Translate(-m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[0],-m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[1], -m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[2]);

	//vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_pCenter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	//transformFilter_pCenter->SetInputData(clothingPolydata);
	//transformFilter_pCenter->SetTransform(transform_pCenter);
	//transformFilter_pCenter->Update();
	//clothingPolydata = transformFilter_pCenter->GetOutput();

	///////////////////////// Color scalar 설정 //////////////////////
	double radius = pRt->EyewearRadius_QLineEdit->text().toDouble(); // 기본 radius = 1 
	double thickness = pRt->EyewearThickness_QLineEdit->text().toDouble(); // 기본 thickness(height) = 1

	float LeadColor[3] = { 240, 255, 255 }; // Wearable color

	vtkNew<vtkUnsignedCharArray> WearableColors;
	WearableColors->SetNumberOfComponents(3);
	WearableColors->SetNumberOfTuples(WearablePolydata->GetNumberOfPoints());
	for (int i = 0; i < WearablePolydata->GetNumberOfPoints(); ++i)
	{
		WearableColors->SetTuple(i, LeadColor);
	}
	WearablePolydata->GetPointData()->SetScalars(WearableColors);

	//////////////////*** append 순서는 항상 누적되는 .polydata를 먼저!!!////////////////////////////
	vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
	appendFilter->AddInputData(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata); // 원점이동 및 스케일링, 의복 누적 된 polydata과 append
	appendFilter->AddInputData(WearablePolydata);
	appendFilter->Update();
		
	vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(appendFilter->GetOutput());
	geomFilter->Update();

	m_3DHumanData_MultiplePhantom[PhantomIdx].polydata = geomFilter->GetOutput();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	mapper->SetScalarVisibility(true);

	m_pVTKWidget->GetSceneRenderer()->RemoveActor(m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor = nullptr;
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor = Util::CreateActor(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetMapper(mapper);

	double height = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][3]; // Set phantomInfo
	double weight = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][4];
	double CenterX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double VectorX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];

	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetPosition(CenterX, CenterY, CenterZ);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetOrientation(VectorX, VectorY, VectorZ);
			
	//   
	//선택된 팬텀 하이라이트 
	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetProperty(property);

	m_pVTKWidget->GetSceneRenderer()->AddActor(m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	m_pVTKWidget->renderWindow()->Render();

	// For remeshing cylinder
	//// Generate the normals for the polydata
	//vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
	//normals->SetInputData(transformFilter->GetOutput());
	//normals->ComputePointNormalsOn();
	//normals->Update();
	//// Generate a new set of polygons for the cylinder
	//vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	//triangleFilter->SetInputConnection(normals->GetOutputPort());
	//triangleFilter->Update();

}
void ETHuman3DApp::PhantomWraparoundGlassesGenerate() //
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][0] = pRt->EyewearDistance_QLineEdit->text().toDouble(); // distance
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][1] = pRt->EyewearRadius_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][2] = pRt->EyewearThickness_QLineEdit->text().toDouble(); // radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][3] = pRt->EyewearDensity_QLineEdit->text().toDouble(); // density
	// composition(1: Glasses, 2: Leaded glasses)
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][100][0][4] = pRt->EyewearComposition_QComboBox->currentIndex();

	std::map<int, double> pickedPointCenterX, pickedPointCenterY, pickedPointCenterZ, pickedPointNx, pickedPointNy, pickedPointNz, pointX, pointY, pointZ;
	int pointID;

	vtkSmartPointer<vtkPolyData> PhantomPolydataBase; // 원점이동 및 스케일링만 (이동 및 회전은 X)
	PhantomPolydataBase = m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base;

	for (int index = 0; index < 2; index++)
	{
		if (index == 0) pointID = m_GlassesInfo.Glasses1_ID;
		if (index == 1) pointID = m_GlassesInfo.Glasses2_ID;

		// create a vtkPolyDataNormals object and set input
		vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
		normals->SetInputData(PhantomPolydataBase);

		// compute point normals and update
		normals->ComputePointNormalsOn();
		normals->ComputeCellNormalsOff();
		normals->Update();

		// get the normal data array
		vtkDataArray* normalData = PhantomPolydataBase->GetPointData()->GetNormals();

		// get the normal vector at a specific point index			
		double normal[3];
		double originalNormal[3];
		normalData->GetTuple(pointID, originalNormal);
		double averagedNormal[3] = { 0,0,0 };

		vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints(); // Polydata_base의 좌표들
		double Pickedpoint[3];
		points->GetPoint(pointID, Pickedpoint); // Polydata_base에서 pickeID를 이용해서 구한 선택한 좌표

		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
		{
			double point[3];
			points->GetPoint(i, point);
			double distance = vtkMath::Distance2BetweenPoints(point, Pickedpoint);//Polydata_base에서 선택한 좌표와 polydata_base 모든 좌표들간의 거리를 구함.

			double angleThreshold = 60;
			if (distance < 0.6)
			{
				normalData->GetTuple(i, normal);

				const double cosineThreshold = cos(vtkMath::RadiansFromDegrees(angleThreshold));

				vtkMath::Normalize(originalNormal); // polydata_base에서 선택한 좌표의 normal값
				vtkMath::Normalize(normal); // polydata_base의 모든 좌표들의 normal값

				const double dotProduct = vtkMath::Dot(originalNormal, normal);
				if (dotProduct >= cosineThreshold)
				{
					averagedNormal[0] += normal[0];
					averagedNormal[1] += normal[1];
					averagedNormal[2] += normal[2];
				}
			}
		}
		vtkMath::Normalize(averagedNormal);
		pickedPointNx[index] = averagedNormal[0];
		pickedPointNy[index] = averagedNormal[1];
		pickedPointNz[index] = averagedNormal[2];

		pickedPointCenterX[index] = Pickedpoint[0]; // InteractorStyle에서 지정한 선택한 좌표
		pickedPointCenterY[index] = Pickedpoint[1];
		pickedPointCenterZ[index] = Pickedpoint[2];
	}
	vtkSmartPointer<vtkCubeSource> cubeSource_center = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_center->SetXLength(3.0 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_center->SetYLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_center->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());

	double normalVector[3] = { (pickedPointNx[0] + pickedPointNx[1]) * 0.5, (pickedPointNy[0] + pickedPointNy[1]) * 0.5, (pickedPointNz[0] + pickedPointNz[1]) * 0.5 };
	vtkMath::Normalize(normalVector);
	double originalVector[3] = { 0, -1, 0 }; // 실린더 기본 방향은 y-axis에 along한 방향

	double rotationAxis[3], rotationAngle;
	vtkMath::Cross(originalVector, normalVector, rotationAxis);
	rotationAngle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(originalVector, normalVector));

	// Create the transform -> 이렇게 생성되는 Input glasses.obj들은 polydata_base에 맞음(원점이동 되어 있음)
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->PostMultiply();
	transform->RotateWXYZ(rotationAngle, rotationAxis);
	transform->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5, ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - cubeSource_center->GetYLength()*0.5 - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputConnection(cubeSource_center->GetOutputPort());
	transformFilter->SetTransform(transform);
	transformFilter->Update();

	std::string OutputFilename_center = "./data/wearable/Glasses/GoggleCenter_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_center = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_center->SetFileName(OutputFilename_center.c_str());
	objWriter_center->SetInputConnection(transformFilter->GetOutputPort());
	objWriter_center->Write();

	vtkSmartPointer<vtkCubeSource> cubeSource_side1 = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_side1->SetXLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_side1->SetYLength(abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_side1->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());
	
	vtkSmartPointer<vtkTransform> transform_side1 = vtkSmartPointer<vtkTransform>::New();
	transform_side1->PostMultiply();
	transform_side1->RotateWXYZ(rotationAngle, rotationAxis);
	transform_side1->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5 + 1.5 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]) - 0.5 * cubeSource_side1->GetXLength(), ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble() + 0.5 * cubeSource_side1->GetYLength(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_side1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_side1->SetInputConnection(cubeSource_side1->GetOutputPort());
	transformFilter_side1->SetTransform(transform_side1);
	transformFilter_side1->Update();

	std::string OutputFilename_side1 = "./data/wearable/Glasses/GoggleSide1_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_side1 = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_side1->SetFileName(OutputFilename_side1.c_str());
	objWriter_side1->SetInputConnection(transformFilter_side1->GetOutputPort());
	objWriter_side1->Write();

	vtkSmartPointer<vtkCubeSource> cubeSource_side2 = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource_side2->SetXLength(theApp.pRt->EyewearThickness_QLineEdit->text().toDouble());
	cubeSource_side2->SetYLength(abs(pickedPointCenterX[0] - pickedPointCenterX[1]));
	cubeSource_side2->SetZLength(2.0 * theApp.pRt->EyewearRadius_QLineEdit->text().toDouble());

	vtkSmartPointer<vtkTransform> transform_side2 = vtkSmartPointer<vtkTransform>::New();
	transform_side2->PostMultiply();
	transform_side2->RotateWXYZ(rotationAngle, rotationAxis);
	transform_side2->Translate((pickedPointCenterX[0] + pickedPointCenterX[1]) * 0.5 - 1.5 * abs(pickedPointCenterX[0] - pickedPointCenterX[1]) + 0.5 * cubeSource_side2->GetXLength(), ((pickedPointCenterY[0] + pickedPointCenterY[1]) * 0.5) - theApp.pRt->EyewearDistance_QLineEdit->text().toDouble() + 0.5 * cubeSource_side2->GetYLength(), (pickedPointCenterZ[0] + pickedPointCenterZ[1]) * 0.5);

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_side2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_side2->SetInputConnection(cubeSource_side2->GetOutputPort());
	transformFilter_side2->SetTransform(transform_side2);
	transformFilter_side2->Update();

	std::string OutputFilename_side2 = "./data/wearable/Glasses/GoggleSide2_" + std::to_string(PhantomIdx) + ".obj";
	vtkSmartPointer<vtkOBJWriter> objWriter_side2 = vtkSmartPointer<vtkOBJWriter>::New();
	objWriter_side2->SetFileName(OutputFilename_side2.c_str());
	objWriter_side2->SetInputConnection(transformFilter_side2->GetOutputPort());
	objWriter_side2->Write();

	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetColor(0.5,0.5,0.5);
	property->SetOpacity(0.5);
	
	vtkSmartPointer<vtkPolyDataMapper> mapper_center = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_center->SetInputConnection(transformFilter->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_center = vtkSmartPointer<vtkActor>::New();
	actor_center->SetMapper(mapper_center);
	actor_center->SetProperty(property);

	vtkSmartPointer<vtkPolyDataMapper> mapper_side1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_side1->SetInputConnection(transformFilter_side1->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_side1 = vtkSmartPointer<vtkActor>::New();
	actor_side1->SetMapper(mapper_side1);
	actor_side1->SetProperty(property);	

	vtkSmartPointer<vtkPolyDataMapper> mapper_side2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_side2->SetInputConnection(transformFilter_side2->GetOutputPort());
	vtkSmartPointer<vtkActor> actor_side2 = vtkSmartPointer<vtkActor>::New();
	actor_side2->SetMapper(mapper_side2);
	actor_side2->SetProperty(property);

	m_pVTKWidget->GetSceneRenderer()->AddActor(actor_center);
	m_pVTKWidget->GetSceneRenderer()->AddActor(actor_side1);
	m_pVTKWidget->GetSceneRenderer()->AddActor(actor_side2);
	m_pVTKWidget->renderWindow()->Render();
}

////////////////// Phantom Dosimeter ////////////////// 
void ETHuman3DApp::DosimeterGenerate(int no) // 가시화용(10 um offset, pCenter 0,0,0 맞춤) polydata, 의복비교용(면적 30% margin, pCenter 안 맞춤) polydata, 사면체화용(1 um + separation distance offset, pCenter 안 맞춤) obj 파일 생성
{
	int PhantomIdx = theApp.pRt->m_Phantom_SelectedIndex;
	int DosimeterIdx = no; 
	// dosimeter info 업데이트
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][999][0][0] = theApp.pRt->m_DosimeterSequenceVector[PhantomIdx].size();
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][3] = pRt->DosimeterPosX_QLineEdit->text().toDouble(); // X
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][4] = pRt->DosimeterPosY_QLineEdit->text().toDouble(); // Y
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][5] = pRt->DosimeterPosZ_QLineEdit->text().toDouble(); // Z
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][1] = pRt->DosimeterRadius_QLineEdit->text().toDouble(); // Radius
	theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][2] = pRt->DosimeterSeparationDistance_QLineEdit->text().toDouble(); // Distance
		
	vtkSmartPointer<vtkPolyData> PhantomPolydataBase = m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base; // 이 데이터는 BASE POLYDATA임
	vtkSmartPointer<vtkPoints> points = PhantomPolydataBase->GetPoints();
	double PickedPoint[3];
	points->GetPoint(m_DosimeterInfo.Dosimeter_PointID, PickedPoint); // 3D Pick 및 location update 통해 받아오는 좌표 ID
		
	// 가시화용 사용자가 지정한 영역만으로 polydata 생성
	vtkSmartPointer<vtkCellArray> nearbyCells = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	vtkSmartPointer<vtkPoints> PhantomPolydataBasePoints = PhantomPolydataBase->GetPoints();
	for (vtkIdType i = 0; i < PhantomPolydataBasePoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydataBasePoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
		double radius = pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * radius) // radius = 2 cm -> dist = 2^2 = 4
		{			
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydataBase->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydataBase->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}		
	vtkSmartPointer<vtkPolyData> nearbyPolyData = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData->SetPoints(PhantomPolydataBasePoints);
	nearbyPolyData->SetPolys(nearbyCells);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter;
	connectivityFilter_Dosimeter->SetInputData(nearbyPolyData);
	connectivityFilter_Dosimeter->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter->Update();
	vtkSmartPointer<vtkPolyData> PolydataOriginalDosimeter = vtkSmartPointer<vtkPolyData>::New();
	PolydataOriginalDosimeter = connectivityFilter_Dosimeter->GetOutput(); // 사용자가 지정한 선량계 polydata (마진 offset 하기 전)
			
	// 0.001 cm offset -> 가시화만 진행
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(PolydataOriginalDosimeter);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance;
	offsetDistance = 0.001; // Just for visulaziation for offset 10 um from phantom skin
	for (vtkIdType i = 0; i < PolydataOriginalDosimeter->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		PolydataOriginalDosimeter->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance;
		p[1] = p[1] + n[1] * offsetDistance;
		p[2] = p[2] + n[2] * offsetDistance;
		PolydataOriginalDosimeter->GetPoints()->SetPoint(i, p);
	}
	PolydataOriginalDosimeter->Modified();

	vtkSmartPointer<vtkOBJWriter> writer_OriginalDosimeter = vtkSmartPointer<vtkOBJWriter>::New();
	std::string OrigianlDosimeterStr = "./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + "_tempForVis.obj";
	writer_OriginalDosimeter->SetFileName(OrigianlDosimeterStr.c_str());
	writer_OriginalDosimeter->SetInputData(PolydataOriginalDosimeter);
	writer_OriginalDosimeter->Update();
	
	// Importing polydata for matching with phantom and visualizing by color	
	vtkSmartPointer<vtkOBJReader> reader_dosimeter = vtkSmartPointer<vtkOBJReader>::New(); // TempDosimeterPolydata의 texture coordinate는 scalar 할당 안 되어 있는 원래 좌표도 갖고 있으므로 OBJ를 다시 불러옴
	QString dosimeterQstr = QString::fromStdString("./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + "_tempForVis.obj"); // 생성된 main region OBJ 불러오기
	reader_dosimeter->SetFileName(Util::Wcs_to_mbs(dosimeterQstr.toStdWString()).c_str());
	reader_dosimeter->Update();
	vtkSmartPointer<vtkPolyData> FinalDosimeterPolydata = vtkSmartPointer<vtkPolyData>::New(); // 해당 선량계는 polydata_base와 좌표 평행이동 위치가 맞음 (따라서 추후 사면체화 후, node 정렬할때 pCenter 원점이동 거침)
	FinalDosimeterPolydata = Util::CreatePolyData(reader_dosimeter);

	float GoldColor[3] = { 50, 50, 50 }; // Wearable color
	vtkNew<vtkUnsignedCharArray> DosimeterColors;
	DosimeterColors->SetNumberOfComponents(3);
	DosimeterColors->SetNumberOfTuples(FinalDosimeterPolydata->GetNumberOfPoints());
	for (int i = 0; i < FinalDosimeterPolydata->GetNumberOfPoints(); ++i)
	{
		DosimeterColors->SetTuple(i, GoldColor);
	}
	FinalDosimeterPolydata->GetPointData()->SetScalars(DosimeterColors);

	// Append dosimeter and phantom(accumulated with wearable) 
	vtkSmartPointer<vtkAppendFilter> appendFilter = vtkSmartPointer<vtkAppendFilter>::New();
	appendFilter->AddInputData(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	appendFilter->AddInputData(FinalDosimeterPolydata);
	appendFilter->Update();

	vtkSmartPointer<vtkGeometryFilter> geomFilter = vtkSmartPointer<vtkGeometryFilter>::New();
	geomFilter->SetInputData(appendFilter->GetOutput());
	geomFilter->Update();
	m_3DHumanData_MultiplePhantom[PhantomIdx].polydata = geomFilter->GetOutput();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	mapper->SetScalarVisibility(true);

	m_pVTKWidget->GetSceneRenderer()->RemoveActor(m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor = nullptr;
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor = Util::CreateActor(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetMapper(mapper);

	double height = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][3]; // Set phantomInfo
	double weight = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][4];
	double CenterX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][5];
	double CenterY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][6];
	double CenterZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][7];
	double VectorX = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][8];
	double VectorY = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][9];
	double VectorZ = theApp.pRt->m_Phantom_MainInfo[PhantomIdx][10];

	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetPosition(CenterX, CenterY, CenterZ);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetOrientation(VectorX, VectorY, VectorZ);

	auto property = Util::GetProperty();
	property->SetInterpolationToPhong();
	property->SetDiffuse(1.0);
	m_3DHumanData_MultiplePhantom[PhantomIdx].actor->SetProperty(property);

	m_pVTKWidget->GetSceneRenderer()->AddActor(m_3DHumanData_MultiplePhantom[PhantomIdx].actor);
	m_pVTKWidget->renderWindow()->Render();

	// 겹치는 의복을 만들기 위한 margin을 둔 OriginalDosimeterPolydata_forTetra 생성
	vtkSmartPointer<vtkCellArray> nearbyCells_margin = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	for (vtkIdType i = 0; i < PhantomPolydataBasePoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydataBasePoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
		double radius = pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * 1.3 * radius * 1.3) // 30% margin
		{
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydataBase->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydataBase->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells_margin->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	vtkSmartPointer<vtkPolyData> nearbyPolyData_margin = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData_margin->SetPoints(PhantomPolydataBasePoints);
	nearbyPolyData_margin->SetPolys(nearbyCells_margin);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter_margin;
	connectivityFilter_Dosimeter_margin->SetInputData(nearbyPolyData_margin);
	connectivityFilter_Dosimeter_margin->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter_margin->Update();
	vtkSmartPointer<vtkPolyData> PolydataDosimeter_margin = vtkSmartPointer<vtkPolyData>::New();
	PolydataDosimeter_margin = connectivityFilter_Dosimeter_margin->GetOutput();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	double pCenterX = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[0];
	double pCenterY = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[1];
	double pCenterZ = theApp.m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[2];
	transform->Translate(pCenterX, pCenterY, pCenterZ);  // pCenter 안 맞춘걸로 돌리기
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(PolydataDosimeter_margin);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	OriginalDosimeterPolydata_forTetra[PhantomIdx][DosimeterIdx] = transformFilter->GetOutput(); // 이 데이터는 pCenter 0,0,0으로 안 맞춘(즉, 팬텀 original 파일) 좌표와 일치함 -> 여기에 offset해서 가시화용 + 의복과 겹침 확인용
	
	// 실제 사면체화에 사용될 offset된 팬텀 데이터로부터 제작하는 선량계 사면체
	vtkSmartPointer<vtkPolyData> PhantomPolydata_offset;
	// polydata_base로부터 pCenter 0,0,0 안 맞춘 상태로 돌리기
	vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
	transform_p->PostMultiply(); //this is the key line
	transform_p->Translate(m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[0], m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[1], m_3DHumanData_MultiplePhantom[PhantomIdx].pCenterOriginal[2]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_p->SetInputData(m_3DHumanData_MultiplePhantom[PhantomIdx].polydata_base);
	transformFilter_p->SetTransform(transform_p);
	transformFilter_p->Update();
	PhantomPolydata_offset = transformFilter_p->GetOutput();
	//선량계 거리만큼 offset offset
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter_offset = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter_offset->SetInputData(PhantomPolydata_offset);
	normalsFilter_offset->ComputePointNormalsOn();
	normalsFilter_offset->ComputeCellNormalsOff();
	normalsFilter_offset->Update();
	vtkDataArray* normalData_offset = normalsFilter_offset->GetOutput()->GetPointData()->GetNormals();
	double offsetDistance_offset = 0.0001 + theApp.pRt->m_Clothing_MainInfo[PhantomIdx][1000][DosimeterIdx][2]; // 1um 마진 + separation distance
	for (vtkIdType i = 0; i < PhantomPolydata_offset->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		PhantomPolydata_offset->GetPoint(i, p);
		normalData_offset->GetTuple(i, n);
		p[0] = p[0] + n[0] * offsetDistance_offset;
		p[1] = p[1] + n[1] * offsetDistance_offset;
		p[2] = p[2] + n[2] * offsetDistance_offset;
		PhantomPolydata_offset->GetPoints()->SetPoint(i, p);
	}
	PhantomPolydata_offset->Modified();

	vtkSmartPointer<vtkPoints> points_offset = PhantomPolydata_offset->GetPoints();
	double PickedPoint_offset[3];
	points_offset->GetPoint(m_DosimeterInfo.Dosimeter_PointID, PickedPoint_offset); // 3D Pick 및 location update 통해 받아오는 좌표 ID
	vtkSmartPointer<vtkCellArray> nearbyCells_offset = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦

	// 사용자가 지정한 영역의 polydata 생성
	vtkSmartPointer<vtkPoints> PhantomPolydata_offsetPoints = PhantomPolydata_offset->GetPoints();
	for (vtkIdType i = 0; i < PhantomPolydata_offsetPoints->GetNumberOfPoints(); i++)
	{
		// Check if the point is within radius of the specified point
		double dist = vtkMath::Distance2BetweenPoints(PhantomPolydata_offsetPoints->GetPoint(i), PickedPoint_offset); // 거리 제곱을 반환
		double radius = pRt->DosimeterRadius_QLineEdit->text().toDouble();
		if (dist <= radius * radius) // radius = 2 cm -> dist = 2^2 = 4
		{
			vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
			PhantomPolydata_offset->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
			for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
			{
				vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydata_offset->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
				nearbyCells_offset->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
			}
		}
	}
	vtkSmartPointer<vtkPolyData> nearbyPolyData_offset = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	nearbyPolyData_offset->SetPoints(PhantomPolydata_offsetPoints);
	nearbyPolyData_offset->SetPolys(nearbyCells_offset);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_Dosimeter_offset;
	connectivityFilter_Dosimeter_offset->SetInputData(nearbyPolyData_offset);
	connectivityFilter_Dosimeter_offset->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_Dosimeter_offset->Update();
	vtkSmartPointer<vtkPolyData> PolydataDosimeter_offset = vtkSmartPointer<vtkPolyData>::New();
	PolydataDosimeter_offset = connectivityFilter_Dosimeter_offset->GetOutput(); // 사용자가 지정한 선량계 polydata (마진 offset 하기 전)

	vtkSmartPointer<vtkOBJWriter> writer_OffsetDosimeter = vtkSmartPointer<vtkOBJWriter>::New();
	std::string OffsetDosimeterStr = "./data/wearable/Dosimeter/" + std::to_string(PhantomIdx) + "_" + std::to_string(DosimeterIdx) + ".obj";
	writer_OffsetDosimeter->SetFileName(OffsetDosimeterStr.c_str());
	writer_OffsetDosimeter->SetInputData(PolydataDosimeter_offset);
	writer_OffsetDosimeter->Update();
}
void ETHuman3DApp::RefreshDosimeter3DShpere(double PickedPos[3], int no)
{
	if (m_3DHumanData.dosimeterActorSphereClicked)
		m_pVTKWidget->GetSceneRenderer()->RemoveActor(m_3DHumanData.dosimeterActorSphereClicked);
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource =	vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);
	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);
	double centerX = PickedPos[0];
	double centerY = PickedPos[1];
	double centerZ = PickedPos[2];
	// actor 생성
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());
	m_3DHumanData.dosimeterActorSphereClicked = vtkSmartPointer<vtkActor>::New();
	m_3DHumanData.dosimeterActorSphereClicked->SetMapper(mapper);
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	m_3DHumanData.dosimeterActorSphereClicked->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());
	m_3DHumanData.dosimeterActorSphereClicked->GetProperty()->SetOpacity(0.4);
	m_3DHumanData.dosimeterActorSphereClicked->SetPosition(centerX, centerY, centerZ);	
	m_pVTKWidget->GetSceneRenderer()->AddActor(m_3DHumanData.dosimeterActorSphereClicked);
	m_pVTKWidget->renderWindow()->Render();
	m_DosimeterInfo.Dosimeter_point[0] = centerX;
	m_DosimeterInfo.Dosimeter_point[1] = centerY;
	m_DosimeterInfo.Dosimeter_point[2] = centerZ;
}
void ETHuman3DApp::RemoveDosimeter3DShpere() {
	m_pVTKWidget->GetSceneRenderer()->RemoveActor(m_3DHumanData.dosimeterActorSphereClicked);
}
//////////////////////////////////////////////////////

//************************************** Geometry Widget **************************************//
void ETHuman3DApp::UpdateObject_ActorHighlighted(int objectIndex) // 현재 선택된 팬텀버튼에 해당하는 팬텀만 pickable 하도록
{
	for (auto itr_objectID : pRt->m_Object_SequenceVector)
	{
		if (itr_objectID == objectIndex)
		{
			ObjectPanelActors[itr_objectID]->GetProperty()->SetDiffuse(1.0);
		}
		else
		{
			ObjectPanelActors[itr_objectID]->GetProperty()->SetDiffuse(0.3);
		}
	}
	m_pVTKWidget->renderWindow()->Render();
}

//************************************** Source Widget **************************************//
void ETHuman3DApp::GenerateSourceActor_sourceBB() // Broad Beam 선원 actor 가시화 -> 전용코드로 따로 관리
{
	// Reset process
	for (auto itr : actors_cylinderB)
	{
		if (itr != nullptr)
		{
			m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	actors_cylinderB.clear();
	for (auto itr : actors_coneB)
	{
		if (itr != nullptr)
		{
			m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	actors_coneB.clear();
	for (auto itr : actors_arrowB)
	{
		if (itr != nullptr)
		{
			m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}
	actors_arrowB.clear();

	m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor_circleB);
	actor_circleB = nullptr;
	m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor_sphereB);
	actor_sphereB = nullptr;
	m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor_single_cylinderB);
	actor_single_cylinderB = nullptr;

	// set varialbes
	double DistanceFromPhantomBox = 100; // 1 m

	double PhantomBoxXmin = DBL_MAX;
	double PhantomBoxXmax = -DBL_MAX;
	double PhantomBoxYmin = DBL_MAX;
	double PhantomBoxYmax = -DBL_MAX;
	double PhantomBoxZmin = DBL_MAX;
	double PhantomBoxZmax = -DBL_MAX;
	for (auto phantomID : pRt->m_Phantom_SequenceVector)
	{
		double* bounds = PhantomPanelActor[phantomID]->GetBounds();
		if (bounds[0] < PhantomBoxXmin) PhantomBoxXmin = bounds[0];
		if (bounds[1] > PhantomBoxXmax) PhantomBoxXmax = bounds[1];
		if (bounds[2] < PhantomBoxYmin) PhantomBoxYmin = bounds[2];
		if (bounds[3] > PhantomBoxYmax) PhantomBoxYmax = bounds[3];
		if (bounds[4] < PhantomBoxZmin) PhantomBoxZmin = bounds[4];
		if (bounds[5] > PhantomBoxZmax) PhantomBoxZmax = bounds[5];
	}

	double xLength = PhantomBoxXmax - PhantomBoxXmin;
	double yLength = PhantomBoxYmax - PhantomBoxYmin;
	double zLength = PhantomBoxZmax - PhantomBoxZmin;

	double centerX = (PhantomBoxXmax + PhantomBoxXmin) / 2;
	double centerY = (PhantomBoxYmax + PhantomBoxYmin) / 2;
	double centerZ = (PhantomBoxZmax + PhantomBoxZmin) / 2;
	
	// Actor property
	auto property = Util::GetProperty();
	double color[3] = { 1.0, 0., 0. };
	property->SetColor(color);
	property->SetInterpolationToPhong();
	//property->SetSpecular(1.0);
	//property->SetSpecularPower(0.01);

	// Actor parameter
	double CylinderHeight = 20;
	double CylinderRadius = 1;
	double ConeHeight = 10;
	double ConeRadius = 2;
	int iterationNo = 1;
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 4) iterationNo = 8;
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 5) iterationNo = 14;
	for (int actorNo = 0; actorNo < iterationNo; actorNo++)
	{
		// Create a cylinder (shaft of the arrow)
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		cylinderSource->SetHeight(CylinderHeight);
		cylinderSource->SetRadius(CylinderRadius);
		cylinderSource->SetResolution(60);
		vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		cylinderMapper->SetInputConnection(cylinderSource->GetOutputPort());
		actors_cylinderB.push_back(vtkSmartPointer<vtkActor>::New());
		actors_cylinderB.back()->SetMapper(cylinderMapper);
		actors_cylinderB.back()->SetProperty(property);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actors_cylinderB.back());
		// Create a cone (arrowhead)
		vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
		coneSource->SetHeight(ConeHeight);
		coneSource->SetRadius(ConeRadius);
		coneSource->SetResolution(60);
		vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		coneMapper->SetInputConnection(coneSource->GetOutputPort());
		actors_coneB.push_back(vtkSmartPointer<vtkActor>::New());
		actors_coneB.back()->SetMapper(coneMapper);
		actors_coneB.back()->SetProperty(property);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actors_coneB.back());
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 0) // AP
	{
		// Create a circle
		double radius_AP;
		if (xLength >= zLength) radius_AP = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_AP = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_AP); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);
		// Set position	
		actor_circleB->SetPosition(centerX, centerY - DistanceFromPhantomBox, centerZ);
		actor_circleB->SetOrientation(90, 0, 0);
		actors_cylinderB[0]->SetPosition(centerX, centerY - DistanceFromPhantomBox + CylinderHeight / 2, centerZ);
		actors_cylinderB[0]->SetOrientation(0, 0, 0);
		actors_coneB[0]->SetOrientation(0, 0, 90);
		actors_coneB[0]->SetPosition(centerX, centerY - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight, centerZ);
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 1) //PA
	{
		double radius_PA;
		if (xLength >= zLength) radius_PA = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_PA = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_PA); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);
		// Set position	
		actor_circleB->SetPosition(centerX, centerY + DistanceFromPhantomBox, centerZ);
		actor_circleB->SetOrientation(90, 0, 0);
		actors_cylinderB[0]->SetPosition(centerX, centerY + DistanceFromPhantomBox - CylinderHeight / 2, centerZ);
		actors_cylinderB[0]->SetOrientation(0, 0, 0);
		actors_coneB[0]->SetOrientation(0, 0, 270);
		actors_coneB[0]->SetPosition(centerX, centerY + DistanceFromPhantomBox - ConeHeight / 2 - CylinderHeight, centerZ); 
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 2) // LLAT
	{
		double radius_LLAT;
		if (xLength >= zLength) radius_LLAT = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_LLAT = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_LLAT); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);
		// Set position	
		actor_circleB->SetPosition(centerX - DistanceFromPhantomBox, centerY, centerZ);
		actor_circleB->SetOrientation(90, 0, 90);
		actors_cylinderB[0]->SetPosition(centerX - DistanceFromPhantomBox + CylinderHeight / 2, centerY, centerZ);
		actors_cylinderB[0]->SetOrientation(0, 0, 90);
		actors_coneB[0]->SetOrientation(0, 0, 0);
		actors_coneB[0]->SetPosition(centerX - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight, centerY, centerZ);
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 3) // RLAT
	{
		double radius_AP;
		if (xLength >= zLength) radius_AP = xLength / 2 * 1.05;
		if (zLength >= xLength) radius_AP = zLength / 2 * 1.05;
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_AP); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);
		// Set position	
		actor_circleB->SetPosition(centerX + DistanceFromPhantomBox, centerY, centerZ);
		actor_circleB->SetOrientation(90, 0, 90);
		actors_cylinderB[0]->SetPosition(centerX + DistanceFromPhantomBox - CylinderHeight / 2, centerY, centerZ);
		actors_cylinderB[0]->SetOrientation(0, 0, 90);
		actors_coneB[0]->SetOrientation(0, 0, 180);
		actors_coneB[0]->SetPosition(centerX + DistanceFromPhantomBox - ConeHeight / 2 - CylinderHeight, centerY, centerZ);
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 4) // ROT
	{
		// Create a circle
		double radius_ROT = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05; // 5% margin
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_ROT); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);

		// Create a cylinder
		double distance_ROT = xLength / 2 + 50; // 원점(팬텀박스중심)으로부터 원판이 떨어진 거리 + 50 cm (도합 100 cm)
		vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
		cylinderSource->SetRadius(distance_ROT); // Set the radius
		cylinderSource->SetHeight(radius_ROT * 2);
		cylinderSource->SetResolution(30); // Set the resolution 		
		vtkSmartPointer<vtkPolyDataMapper> mapper_cylinder = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_cylinder->SetInputConnection(cylinderSource->GetOutputPort());
		actor_single_cylinderB = vtkSmartPointer<vtkActor>::New();
		actor_single_cylinderB->SetMapper(mapper_cylinder);
		auto property_cylinder = Util::GetProperty();
		double color[3] = { 1.0, 0., 0. };
		property_cylinder->SetColor(color);
		property_cylinder->SetInterpolationToPhong();
		property_cylinder->SetOpacity(0.05);
		actor_single_cylinderB->SetProperty(property_cylinder);
		actor_single_cylinderB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_single_cylinderB);
		 
		// Create curved arrow
		double radiusX_ROT = distance_ROT / 3; // Larger radius for x-axis
		double radiusY_ROT = distance_ROT / 2; // Smaller radius for y-axis
		int numSegments = 2000;
		double tubeRadius = distance_ROT / 50;
		double coneHeight_ROT = distance_ROT / 12;
		double coneRaidus_ROT = distance_ROT / 25;
		for (int i = 0; i < 1; i++)
		{
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
			double angleStep = (60.0 / numSegments) * (M_PI / 180.0); // angle을 조정하여 어디까지 타원의 어디까지 생성할 건지 조정 가능
			for (int i = 0; i <= numSegments; ++i) {
				double angle = angleStep * i;
				double x = radiusX_ROT * cos(angle);
				double y = radiusY_ROT * sin(angle);
				points->InsertNextPoint(x, y, 0.0);

				if (i > 0) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, i - 1);
					line->GetPointIds()->SetId(1, i);
					lines->InsertNextCell(line);
				}
			}
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(points);
			polyData->SetLines(lines);
			vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
			tubeFilter->SetInputData(polyData);
			tubeFilter->SetRadius(tubeRadius); // Set the radius of the tube
			tubeFilter->SetNumberOfSides(50); // Set the number of sides for the tube's cross-section
			tubeFilter->CappingOn(); // Cap the ends of the tube
			vtkSmartPointer<vtkPolyDataMapper> mapper_tube = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper_tube->SetInputConnection(tubeFilter->GetOutputPort());
			vtkSmartPointer<vtkActor> tubeActor = vtkSmartPointer<vtkActor>::New();
			tubeActor->SetMapper(mapper_tube);
			tubeActor->SetProperty(property);

			vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
			coneSource->SetHeight(coneHeight_ROT); // Set the height of the cone (adjust as needed)
			coneSource->SetRadius(coneRaidus_ROT); // Set the radius of the cone (adjust as needed)
			coneSource->SetResolution(20); // Set the resolution of the cone
			coneSource->SetCenter(radiusX_ROT, -coneHeight_ROT / 2, 0); // Position at the end of the curve (+X)
			coneSource->SetDirection(0, -1, 0);
			vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			coneMapper->SetInputConnection(coneSource->GetOutputPort());
			vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
			coneActor->SetMapper(coneMapper);
			coneActor->SetProperty(property);
			actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
			actors_arrowB[i]->AddPart(tubeActor);
			actors_arrowB[i]->AddPart(coneActor);
			actors_arrowB[i]->PickableOff();
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actors_arrowB[i]);
		}

		// Set position
		actors_arrowB[0]->SetPosition(centerX + distance_ROT / 3, centerY - distance_ROT + radiusY_ROT / 2, centerZ);
		actors_arrowB[0]->SetOrientation(0, 0, -85);
		//actors_arrowB[1]->SetPosition(centerX, centerY, centerZ);
		//actors_arrowB[1]->SetOrientation(0, 0, 0);
		actor_circleB->SetPosition(centerX + distance_ROT, centerY, centerZ);
		actor_circleB->SetOrientation(90, 90, 0);
		actor_single_cylinderB->SetPosition(centerX, centerY, centerZ);
		actor_single_cylinderB->SetOrientation(90, 0, 0);
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 5) // ISO
	{
		// Create a circle
		double XYarea = xLength * yLength;
		double XZarea = xLength * zLength;
		double YZarea = yLength * zLength;
		double radius_ISO;				
		if (XYarea >= YZarea && XYarea >= YZarea) {
			radius_ISO = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2)) * 1.05;
		}
		else if (YZarea >= XYarea && YZarea >= XZarea) {
			radius_ISO = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		else {
			radius_ISO = sqrt((xLength / 2 * xLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_ISO); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		actor_circleB = vtkSmartPointer<vtkActor>::New();
		actor_circleB->SetMapper(mapper_disk);
		actor_circleB->SetProperty(property);
		actor_circleB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circleB);

		// Create a sphere
		double distance_ISO = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05 + 50; // 원점(팬텀박스중심)으로부터 원판이 떨어진 거리 + 50 cm (도합 100 cm)
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetRadius(distance_ISO); // Set the radius
		sphereSource->SetThetaResolution(30); // Set the resolution (number of subdivisions) in theta
		sphereSource->SetPhiResolution(30); // Set the resolution in phi
		vtkSmartPointer<vtkPolyDataMapper> mapper_sphere = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_sphere->SetInputConnection(sphereSource->GetOutputPort());
		actor_sphereB = vtkSmartPointer<vtkActor>::New();
		actor_sphereB->SetMapper(mapper_sphere);
		auto property_sphere = Util::GetProperty();
		double color[3] = { 1.0, 0., 0. };
		property_sphere->SetColor(color);
		property_sphere->SetInterpolationToPhong();
		property_sphere->SetOpacity(0.05);
		actor_sphereB->SetProperty(property_sphere);
		actor_sphereB->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_sphereB);

		// Create curved arrow
		double radiusX_ISO = distance_ISO / 1.5; // Larger radius for x-axis
		double radiusY_ISO = distance_ISO / 5; // Smaller radius for y-axis
		int numSegments = 2000;
		double tubeRadius = distance_ISO / 50;
		double coneHeight_ISO = distance_ISO / 12;
		double coneRaidus_ISO = distance_ISO / 25;
		for (int i = 0; i < 2; i++)
		{
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
			double angleStep = (45.0 / numSegments) * (M_PI / 180.0); // angle을 조정하여 어디까지 타원의 어디까지 생성할 건지 조정 가능
			for (int i = 0; i <= numSegments; ++i) {
				double angle = angleStep * i;
				double x = radiusX_ISO * cos(angle);
				double y = radiusY_ISO * sin(angle);
				points->InsertNextPoint(x, y, 0.0);

				if (i > 0) {
					vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
					line->GetPointIds()->SetId(0, i - 1);
					line->GetPointIds()->SetId(1, i);
					lines->InsertNextCell(line);
				}
			}
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(points);
			polyData->SetLines(lines);
			vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
			tubeFilter->SetInputData(polyData);
			tubeFilter->SetRadius(tubeRadius); // Set the radius of the tube
			tubeFilter->SetNumberOfSides(50); // Set the number of sides for the tube's cross-section
			tubeFilter->CappingOn(); // Cap the ends of the tube
			vtkSmartPointer<vtkPolyDataMapper> mapper_tube = vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper_tube->SetInputConnection(tubeFilter->GetOutputPort());
			vtkSmartPointer<vtkActor> tubeActor = vtkSmartPointer<vtkActor>::New();
			tubeActor->SetMapper(mapper_tube);
			tubeActor->SetProperty(property);

			vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
			coneSource->SetHeight(coneHeight_ISO); // Set the height of the cone (adjust as needed)
			coneSource->SetRadius(coneRaidus_ISO); // Set the radius of the cone (adjust as needed)
			coneSource->SetResolution(20); // Set the resolution of the cone
			coneSource->SetCenter(radiusX_ISO, -coneHeight_ISO / 2, 0); // Position at the end of the curve (+X)
			coneSource->SetDirection(0, -1, 0);
			vtkSmartPointer<vtkPolyDataMapper> coneMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
			coneMapper->SetInputConnection(coneSource->GetOutputPort());
			vtkSmartPointer<vtkActor> coneActor = vtkSmartPointer<vtkActor>::New();
			coneActor->SetMapper(coneMapper);
			coneActor->SetProperty(property);
			actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
			actors_arrowB[i]->AddPart(tubeActor);
			actors_arrowB[i]->AddPart(coneActor);
			actors_arrowB[i]->PickableOff();
			theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actors_arrowB[i]);
		}		

		// Set position
		actors_arrowB[0]->SetPosition(centerX + radius_ISO - radiusX_ISO / 2, centerY, centerZ + distance_ISO - radiusY_ISO);
		actors_arrowB[0]->SetOrientation(85, 90, -90);
		actors_arrowB[1]->SetPosition(centerX, centerY - radius_ISO + radiusX_ISO / 2, centerZ + distance_ISO - radiusY_ISO);
		actors_arrowB[1]->SetOrientation(90, 85,-180);
		actor_circleB->SetPosition(centerX, centerY, centerZ + distance_ISO);
		actor_sphereB->SetPosition(centerX, centerY, centerZ);
	}
	if (theApp.pRt->m_comboBoxBeamdirection->currentIndex() == 6) //User-defined
	{
		double radius_USER;
		double XYarea = xLength * yLength;
		double XZarea = xLength * zLength;
		double YZarea = yLength * zLength;
		if (XYarea >= YZarea && XYarea >= YZarea) {
			radius_USER = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2)) * 1.05;
		}
		else if (YZarea >= XYarea && YZarea >= XZarea) {
			radius_USER = sqrt((yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		else {
			radius_USER = sqrt((xLength / 2 * xLength / 2) + (zLength / 2 * zLength / 2)) * 1.05;
		}
		DistanceFromPhantomBox = sqrt((xLength / 2 * xLength / 2) + (yLength / 2 * yLength / 2) + (zLength / 2 * zLength / 2)) * 1.05; // 
		vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
		diskSource->SetInnerRadius(0); // Set the inner radius
		diskSource->SetOuterRadius(radius_USER); // Set the outer radius 
		diskSource->SetRadialResolution(1); // Minimal radial divisions, since it's a circle
		diskSource->SetCircumferentialResolution(100); // Adjust the resolution as needed
		vtkSmartPointer<vtkPolyDataMapper> mapper_disk = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper_disk->SetInputConnection(diskSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor_circle = vtkSmartPointer<vtkActor>::New();
		actor_circle->SetMapper(mapper_disk);
		actor_circle->SetProperty(property);
		actor_circle->PickableOff();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actor_circle);
		// Set position	
		actor_circle->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox);
		actor_circle->SetOrientation(0, 0, 0);
		actors_cylinderB[0]->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox + CylinderHeight / 2);
		actors_cylinderB[0]->SetOrientation(90, 0, 0);		
		actors_coneB[0]->SetPosition(centerX, centerY, centerZ - DistanceFromPhantomBox + ConeHeight / 2 + CylinderHeight);
		actors_coneB[0]->SetOrientation(0, -90, 180);
		actors_arrowB.push_back(vtkSmartPointer<vtkAssembly>::New());
		actors_arrowB[0]->AddPart(actor_circle);
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actor_circle);
		actors_arrowB[0]->AddPart(actors_cylinderB[0]);
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actors_cylinderB[0]);
		actors_arrowB[0]->AddPart(actors_coneB[0]);
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(actors_coneB[0]);
		actors_arrowB[0]->PickableOff();

		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(actors_arrowB[0]);
		actors_arrowB[0]->SetOrientation(0, theApp.pRt->m_lineEditPolarAngle->text().toDouble(), theApp.pRt->m_lineEditAzimuthalAngle->text().toDouble());
	}

	theApp.m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::GenerateSourceActor_sourceEP(double* ptCenter)
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	SourcePanelActor_Position.back()->SetMapper(mapper);
	SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	SourcePanelActor_Position.back()->SetPosition(ptCenter);
	SourcePanelActor_Position.back()->SetPickable(false);
	SourcePanelActor_Position.back()->SetScale(pow(const_PointSource_ScaleFactorPerClick, m_Point_Source_Scale));

	m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position.back());

	// 임시 actor 삭제
	Selecting3DShpere_Delete();

	m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::GenerateSourceActor_sourceFD(double* center_radius)
{	
	double center_x = center_radius[0];
	double center_y = center_radius[1];
	double center_z = center_radius[2];
	double center[3] = { center_x , center_y , center_z };
	double radius = center_radius[3];

	// Create a filled circle (disk with inner radius 0)
	vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
	diskSource->SetInnerRadius(0.0); // 내부 반지름을 0으로 설정하여 채워진 원(circle) 생성
	diskSource->SetOuterRadius(radius); // 외부 반지름 설정 (circle의 반지름)

	// 원의 세그먼트 수 설정 (선택 사항)
	diskSource->SetRadialResolution(30);
	diskSource->SetCircumferentialResolution(60);

	// Apply a transform to orient the disk perpendicular to the Z-axis
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateX(90); // X축을 기준으로 90도 회전하여 Z축에 수직 배치

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(diskSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성
	SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	if (theApp.SourcePanelActor_Position.size() != 1)// 벡터 크기가 1이 아니어서 기존에 있었을때
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(theApp.SourcePanelActor_Position[0]);
		theApp.SourcePanelActor_Position = { theApp.SourcePanelActor_Position.back() }; // 마지막 원소 하나로만 있게 변경
	}
	SourcePanelActor_Position.back()->SetMapper(mapper);
	double color[3] = { 0.66, 0.33, 0.0 };
	SourcePanelActor_Position.back()->GetProperty()->SetColor(color); // 원의 색상 설정
	SourcePanelActor_Position.back()->SetPosition(center); // 원 위치 설정
	SourcePanelActor_Position.back()->SetPickable(false);

	m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position.back());

	m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::GenerateSourceActor_sourceHP(double* ptCenter)
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	SourcePanelActor_Position.back()->SetMapper(mapper);
	SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	SourcePanelActor_Position.back()->SetPosition(ptCenter);
	SourcePanelActor_Position.back()->SetPickable(false);
	SourcePanelActor_Position.back()->SetScale(pow(const_PointSource_ScaleFactorPerClick, m_Point_Source_Scale));

	m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position.back());

	// 임시 actor 삭제
	Selecting3DShpere_Delete();

	m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::GenerateSourceActor_sourceCB(double* ptCenter)
{
	// Create a sphere
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(0.5);

	// Make the surface smooth.
	sphereSource->SetPhiResolution(60);
	sphereSource->SetThetaResolution(30);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	// actor 생성	
	if(SourcePanelActor_Position.size()==0) SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	else
	{
		m_pVTKWidget->GetSceneRenderer()->RemoveActor(SourcePanelActor_Position[0]); // 반투명 임시 actor 삭세
		SourcePanelActor_Position[0] = nullptr;
	}
	SourcePanelActor_Position[0]->SetMapper(mapper);
	SourcePanelActor_Position[0]->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	SourcePanelActor_Position[0]->SetPosition(ptCenter);
	SourcePanelActor_Position[0]->SetPickable(false);

	m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position[0]);
	m_pVTKWidget->GetSceneRenderer()->RemoveActor(NowSelecting3DSphereActor); // 반투명 임시 actor 삭세
	NowSelecting3DSphereActor = nullptr;

	m_pVTKWidget->renderWindow()->Render();
}
void ETHuman3DApp::GenerateSourceDirectionActor_sourceCB()
{
	// 방향
	double maxAngle = pRt->m_lineEditConeBeamDirectionSolidAngle->text().toDouble();  // 예: 30도
	double a = pRt->m_lineEditConeBeamDirectionX->text().toDouble();
	double b = pRt->m_lineEditConeBeamDirectionY->text().toDouble();
	double c = pRt->m_lineEditConeBeamDirectionZ->text().toDouble();
	double cx = pRt->m_lineEditConeBeamPointX->text().toDouble();
	double cy = pRt->m_lineEditConeBeamPointY->text().toDouble();
	double cz = pRt->m_lineEditConeBeamPointZ->text().toDouble();
	int numPoints = 5 * pRt->m_lineEditConeBeamDirectionSolidAngle->text().toDouble();

	if(maxAngle <= 0 || (a == 0 && b ==0 && c ==0)) return;

	std::vector<Point_SolidAngle> points = GetPointsWithinSolidAngle_sourceCB(maxAngle, a, b, c, numPoints);

	// Delete remaining actors
	for (auto itr_actors : SourceDirectionActor)
	{
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr_actors);
	}
	SourceDirectionActor.clear();

	m_pVTKWidget->renderWindow()->Render();

	for (const Point_SolidAngle& point : points) {
		// Create a sphere
		vtkSmartPointer<vtkLineSource> lineSource =
			vtkSmartPointer<vtkLineSource>::New();

		lineSource->SetPoint1(0, 0, 0);  // origin
		lineSource->SetPoint2(point.x, point.y, point.z);

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(lineSource->GetOutputPort());

		vtkSmartPointer<vtkNamedColors> colors =
			vtkSmartPointer<vtkNamedColors>::New();

		// actor 생성
		SourceDirectionActor.push_back(vtkSmartPointer<vtkActor>::New());
		SourceDirectionActor.back()->SetMapper(mapper);
		SourceDirectionActor.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		SourceDirectionActor.back()->GetProperty()->SetOpacity(1.0);
		SourceDirectionActor.back()->SetScale(100);

		double ptCenter[3];
		ptCenter[0] = cx;
		ptCenter[1] = cy;
		ptCenter[2] = cz;

		SourceDirectionActor.back()->SetPosition(ptCenter);
		SourceDirectionActor.back()->SetPickable(false);

		m_pVTKWidget->GetSceneRenderer()->AddActor(SourceDirectionActor.back());
	}

	m_pVTKWidget->renderWindow()->Render();
}
std::vector<Point_SolidAngle> ETHuman3DApp::GetPointsWithinSolidAngle_sourceCB(double maxAngle, double a, double b, double c, int numPoints) {
	// a, b, c 벡터를 정규화
	Vector3 InputDirectionalVector(a, b, c);
	InputDirectionalVector = InputDirectionalVector.normalize();
	std::vector<Point_SolidAngle> result;

	while (result.size() < numPoints) {
		double theta = M_PI * ((double)rand() / RAND_MAX);
		double phi = 2.0 * M_PI * ((double)rand() / RAND_MAX);

		// 구 좌표를 직교 좌표로 변환
		double X = sin(theta) * cos(phi);
		double Y = sin(theta) * sin(phi);
		double Z = cos(theta);
		Vector3 randomDirectionVector(X, Y, Z);
		randomDirectionVector = randomDirectionVector.normalize();

		// (a, b, c)와의 각도 계산
		double cosMaxAngle = cos(maxAngle*M_PI / 180.0);
		double cosInputAngle = InputDirectionalVector.dot(randomDirectionVector);

		// 각도가 maxAngle 이내면 결과에 추가
		if (cosInputAngle >= cosMaxAngle) {
			result.push_back({ theta, phi, X, Y, Z });
		}
	}
	return result;
}
bool ETHuman3DApp::LoadPSF_data(QString path) // 위의 file header 주석은 사용자가 직접 지워주어야 함
{
	PSF_data.clear(); // 입자 정보 순서대로 나열한 data
	PSF_location_vector.clear(); // 위치만 포함된 vector (가시화용)
	PSF_direction_vector.clear(); // 방향만 포함된 vector (가시화용)

	QFont font;
	font.setFamily("Arial");    // Font family
	font.setPointSize(theApp.pRt->FontSizeScaling(10));      // Font size
	font.setWeight(QFont::Bold);   // Font weight

	std::string stdPath = path.toStdString();
	QString qPath = QString::fromStdString(stdPath);

	// Progress Dialog 초기화
	QProgressDialog progressDialog;
	progressDialog.setWindowFlag(Qt::WindowStaysOnTopHint);
	progressDialog.setWindowModality(Qt::WindowModal);
	progressDialog.setLabelText("Initializing...");
	progressDialog.setCancelButton(nullptr);
	progressDialog.setMinimumDuration(0);
	progressDialog.setRange(0, 0); // Indeterminate
	progressDialog.setFont(font);
	progressDialog.show();
	QCoreApplication::processEvents();

	// ====== [IAEA 처리 시작] ======
	if (pRt->sourcePS_IAEA_button->isChecked())
	{
		// 경로에서 .IAEAphsp 확장자 제거
		std::string IAEA_filename = stdPath;
		std::string IAEA_extension = ".IAEAphsp";
		size_t extension_pos = IAEA_filename.find(IAEA_extension);
		if (extension_pos != std::string::npos)
			IAEA_filename.erase(extension_pos);

		// .IAEAheader 파일 존재 확인
		QString headerFilePath = QString::fromStdString(IAEA_filename) + ".IAEAheader";
		QFileInfo headerFileInfo(headerFilePath);
		if (!headerFileInfo.exists()) {
			theApp.SetMessageBox("Cannot find IAEAheader file");
			return false;
		}

		QString basePath = QString::fromStdString(IAEA_filename);
		QString exePath = ".\\Ext_lib\\IAEA2ascii.exe";

		progressDialog.setLabelText("Loading " + basePath + ".IAEAphsp\nThis process will take a few minutes.");
		QCoreApplication::processEvents();

		QString output_IAEA;
		// QProcess로 실행 -> 외부.exe 실행 폴링방식으로 메인스레드에서 while문 내에서 0.1초마다 잠깐 porcessEvents()를 호출하여 로딩창 업데이트
		{
			QProcess process;
			process.setProgram(exePath);
			process.setArguments(QStringList() << basePath);
			process.setProcessChannelMode(QProcess::MergedChannels); // stdout + stderr 같이 읽기
			process.start();

			if (!process.waitForStarted()) {
				theApp.SetMessageBox("Cannot start IAEA2ascii.exe");
				return false;
			}

			while (!process.waitForFinished(100)) {
				// 0.1초마다 UI 갱신 (중간에 응답 없음 방지)
				QCoreApplication::processEvents();
			}

			output_IAEA = process.readAllStandardOutput();
		}
		
		if (output_IAEA.contains("Normal Program Termination")) {
		}
		else {
			theApp.SetMessageBox("IAEA2ascii conversion failed.");
			return false;
		}
		QStringList output_lines = output_IAEA.split('\n');
		QString history_keyword = " Total number of histories:";
		int totalHistories = -1;

		for (const QString& line : output_lines)
		{
			if (line.contains(history_keyword))
			{
				// 키워드 이후의 문자열 잘라냄
				QString numberPart = line.section(history_keyword, 1).trimmed();

				// 숫자만 남기기 (혹시 공백 등 있을 경우 대비)
				bool ok = false;
				totalHistories = numberPart.toInt(&ok);
				if (ok) break;
			}
		}
		if (totalHistories == -1)
		{
			qDebug() << "Keyword not found or number parse error";
			return false;
		}

		stdPath = IAEA_filename + ".txt"; // ASCII 파일로 경로 변경
	}
	// ====== [일반 PSF 파일 처리] ======
	std::ifstream ifp_phsp_size(stdPath, std::ios::binary);
	ifp_phsp_size.seekg(0, std::ios::end);
	std::streamsize size = ifp_phsp_size.tellg();
	double fileSize = static_cast<double>(size);
	ifp_phsp_size.close();

	double size_scale_factor = 100.0 / static_cast<double>(fileSize);

	// ====== [데이터 읽기 시작] ======

	std::ifstream ifp(stdPath);
	std::string line;
	std::string particle, x, y, z, u, v, w, E, SWF;
	double pCode;
	std::string pCode_str_PHITS;
	pRt->PhaseSpaceFileRow = 0;

	progressDialog.setLabelText("Reading phase space file...");
	progressDialog.setRange(0, 100);
	QCoreApplication::processEvents();

	int lastPercent = -1;
	while (std::getline(ifp, line)) {
		// Trim any leading whitespace from the line for safety
		line.erase(0, line.find_first_not_of(" \t"));

		// Skip lines starting with '#' -> In FLUKA or 주석
		if (!line.empty() && line[0] == '#') {
			continue;
		}

		std::istringstream iss(line); // Create a stream from the line for parsing
		if (pRt->sourcePS_MCNP_button->isChecked()) // MCNP6
		{
			iss >> pCode >> x >> y >> z >> u >> v >> w >> E >> SWF; // Converted ASCII-formatted MCNP WSSA file
			if (pCode == 22) particle = "gamma";
			else if (pCode == 11) particle = "e-";
			else if (pCode == -11) particle = "e+";
			else if (pCode == 2112) particle = "neutron";
			else if (pCode == 2212) particle = "proton";
			else if (pCode == 2000004) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				PSF_data.clear();
				PSF_location_vector.clear();
				PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (pRt->sourcePS_PHITS_button->isChecked()) // PHITS
		{
			iss >> pCode_str_PHITS >> x >> y >> z >> u >> v >> w >> E >> SWF; // PHITS dmp file format (pCode, cm, cm, cm, u, v, w, MeV, SWF), manual 98p.
			pCode = std::stod(PSF_PHITS_replaceDwithE(pCode_str_PHITS)); // PHITS는 거듭제곱으로 E대신 D를 씀
			x = PSF_PHITS_replaceDwithE(x);
			y = PSF_PHITS_replaceDwithE(y);
			z = PSF_PHITS_replaceDwithE(z);
			u = PSF_PHITS_replaceDwithE(u);
			v = PSF_PHITS_replaceDwithE(v);
			w = PSF_PHITS_replaceDwithE(w);
			E = PSF_PHITS_replaceDwithE(E);
			SWF = PSF_PHITS_replaceDwithE(SWF);
			if (pCode == 22) particle = "gamma";
			else if (pCode == 11) particle = "e-";
			else if (pCode == -11) particle = "e+";
			else if (pCode == 2112) particle = "neutron";
			else if (pCode == 2212) particle = "proton";
			else if (pCode == 2000004) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				PSF_data.clear();
				PSF_location_vector.clear();
				PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (pRt->sourcePS_FLUKA_button->isChecked()) // FLUKA
		{
			iss >> pCode >> E >> x >> y >> z >> u >> v >> w >> SWF; // FLUKA phsp file 
			//iss >> pCode >> E >> z >> x >> y >> w >> u >> v >> SWF; // Coords converted Otto FULKA phsp file 
			if (pCode == 22 || pCode == 7) particle = "gamma";
			else if (pCode == 11 || pCode == 3) particle = "e-";
			else if (pCode == -11 || pCode == 4) particle = "e+";
			else if (pCode == 2112 || pCode == 8) particle = "neutron";
			else if (pCode == 2212 || pCode == 1) particle = "proton";
			else if (pCode == 2000004 || pCode == -6) particle = "alpha";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				PSF_data.clear();
				PSF_location_vector.clear();
				PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
			E = std::to_string(std::stod(E) * 1000.); // GeV to MeV
		}
		if (pRt->sourcePS_IAEA_button->isChecked()) // IAEAphsp
		{
			double charge, history;
			iss >> pCode >> charge >> x >> y >> z >> u >> v >> w >> E >> SWF >> history; 
			if (pCode == 1) particle = "gamma";
			else if (pCode == 2) particle = "e-";
			else if (pCode == 3) particle = "e+";
			else if (pCode == 4) particle = "neutron";
			else if (pCode == 5) particle = "proton";
			else
			{
				theApp.SetMessageBox("Invalid particle code!!");
				PSF_data.clear();
				PSF_location_vector.clear();
				PSF_direction_vector.clear();
				ifp.close();
				return false;
			}
		}
		if (pRt->sourcePS_USER_Button->isChecked())
		{
			iss >> particle >> x >> y >> z >> u >> v >> w >> E >> SWF; // USER sequence (particle_ASCII, cm, cm, cm, u, v, w, MeV, SWF)
		}

		//
		std::streampos currentPos = ifp.tellg();
		if (currentPos != -1)
		{
			int percent = static_cast<int>(currentPos * size_scale_factor);

			if (percent != lastPercent && percent % 1 == 0)
			{
				lastPercent = percent;
				progressDialog.setValue(percent);
				progressDialog.setLabelText(QString("Reading phase space file... %1%").arg(percent));
				QCoreApplication::processEvents();
			}
		}
		PSF_data.push_back(std::make_tuple(particle, x, y, z, u, v, w, E, SWF));
		PSF_location_vector.push_back(std::make_tuple(std::stof(x), std::stof(y), std::stof(z)));
		PSF_direction_vector.push_back(std::make_tuple(std::stof(u), std::stof(v), std::stof(w)));
		pRt->PhaseSpaceFileRow++;

	}
	progressDialog.close();
	GenerateSourceActor_sourcePS(PSF_location_vector, PSF_direction_vector);

	ifp.close();
	if (pRt->sourcePS_IAEA_button->isChecked())
	{
		remove(stdPath.c_str());
	}
	return true;
}
void ETHuman3DApp::GenerateSourceActor_sourcePS(std::vector<std::tuple<double, double, double>> ptMultiple, std::vector<std::tuple<double, double, double>> dirMultiple)
{
	int actor_count = 500;
	int count;
	int DividingFactor;

	if (ptMultiple.size() <= actor_count)
	{
		count = ptMultiple.size();
		DividingFactor = 1;
	}
	else
	{
		count = actor_count;
		int DividingFactor = ptMultiple.size() / actor_count;
	}

	for (int i = 0; i < count; i++)
	{
		// Create a sphere
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(0.8);

		// Make the surface smooth.
		sphereSource->SetPhiResolution(60);
		sphereSource->SetThetaResolution(30);

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());

		vtkSmartPointer<vtkNamedColors> colors =
			vtkSmartPointer<vtkNamedColors>::New();

		double ptCenter[3];
		ptCenter[0] = std::get<0>(ptMultiple[i*DividingFactor]);
		ptCenter[1] = std::get<1>(ptMultiple[i*DividingFactor]);
		ptCenter[2] = std::get<2>(ptMultiple[i*DividingFactor]);

		// actor 생성
		SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
		SourcePanelActor_Position[i]->SetMapper(mapper);
		SourcePanelActor_Position[i]->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		SourcePanelActor_Position[i]->SetPosition(ptCenter);
		SourcePanelActor_Position[i]->SetPickable(false);

		m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position[i]);

		//dir actor
		vtkSmartPointer<vtkLineSource> lineSource =
			vtkSmartPointer<vtkLineSource>::New();

		lineSource->SetPoint1(0, 0, 0);  // origin
		lineSource->SetPoint2(std::get<0>(dirMultiple[i*DividingFactor]), std::get<1>(dirMultiple[i*DividingFactor]), std::get<2>(dirMultiple[i*DividingFactor]));
		vtkSmartPointer<vtkPolyDataMapper> mapper2 =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper2->SetInputConnection(lineSource->GetOutputPort());

		// actor 생성
		SourcePanelActor_Direction.push_back(vtkSmartPointer<vtkActor>::New());
		SourcePanelActor_Direction.back()->SetMapper(mapper2);
		SourcePanelActor_Direction.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
		SourcePanelActor_Direction.back()->SetScale(100); // 100 cm length
		SourcePanelActor_Direction.back()->SetPosition(ptCenter);
		SourcePanelActor_Direction.back()->SetPickable(false);

		m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Direction.back());
		m_pVTKWidget->renderWindow()->Render();
	}
}
void ETHuman3DApp::PSF_MCNP_ssw_parse_file(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		theApp.SetMessageBox("Unable to open WSSA file!");
	}
	std::ofstream out("./data/WSSA_converted_ASCII_file");    //  output file 출력

	uint32_t reclen = 4;  // 기본적으로 4바이트로 설정
	uint64_t lbuf;
	std::vector<char> buf(1024);  // 초기 버퍼 크기 설정

	while (file) {
		if (!PSF_MCNP_ssw_loadrecord(file, buf, reclen, lbuf)) {
			break;  // 파일 끝이나 오류 발생 시 종료
		}

		// np1, nrss, nrcd, njsw 값을 추출
		if (lbuf >= 88) {  // double은 8바이트이므로 최소 32바이트가 필요
			// 데이터를 double 타입으로 해석
			double np1 = *reinterpret_cast<double*>(buf.data());
			double np2 = *reinterpret_cast<double*>(buf.data() + 8);
			int64_t nx = np2;
			if (nx < 0) nx = nx * (-1);
			nx /= 4;
			double np3 = *reinterpret_cast<double*>(buf.data() + 16);
			double np4 = *reinterpret_cast<double*>(buf.data() + 24);
			double np5 = *reinterpret_cast<double*>(buf.data() + 32);
			double np6 = *reinterpret_cast<double*>(buf.data() + 40);
			double np7 = *reinterpret_cast<double*>(buf.data() + 48);
			double np8 = *reinterpret_cast<double*>(buf.data() + 56);
			double np9 = *reinterpret_cast<double*>(buf.data() + 64);   //  dirx
			double dirx = np9;
			double np10 = *reinterpret_cast<double*>(buf.data() + 72);  //  diry
			double diry = np10;
			double np11 = *reinterpret_cast<double*>(buf.data() + 80);


			double dirz = sqrt(fmax(0.0, 1.0 - dirx * dirx - diry * diry));

			if (np2 < 0)
			{
				dirz = dirz * (-1);
			}

			int pdgcode = PSF_MCNP_conv_mcnp6_ssw2pdg(nx);

			std::cout << pdgcode << " " << np6 << " " << np7 << " " << np8 << " " << np9 << " " << np10 << " " << dirz << " " << np4 << " " << np3 << std::endl;

			if (pdgcode != 0) out << pdgcode << " " << np6 << " " << np7 << " " << np8 << " " << np9 << " " << np10 << " " << dirz << " " << np4 << " " << np3 << std::endl;

		}
	}
	out.close();

	file.close();
}
int ETHuman3DApp::PSF_MCNP_ssw_loadrecord(std::ifstream& file, std::vector<char>& buf, uint32_t reclen, uint64_t& lbuf)
{
	if (reclen == 4) {
		uint32_t rl;
		if (!PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 4)) {
			return 0;
		}
		lbuf = rl;
	}
	else {
		uint64_t rl;
		if (!PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 8)) {
			return 0;
		}
		lbuf = rl;
	}

	if (lbuf > buf.size()) {
		// 필요한 경우 버퍼 크기를 늘림
		buf.resize(static_cast<size_t>(lbuf)); // 여기서 uint64_t -> size_t 캐스팅
	}

	if (!PSF_MCNP_ssw_readbytes(file, buf.data(), static_cast<size_t>(lbuf))) {
		return 0;
	}

	// 레코드 끝에서 동일한 크기인지 확인
	if (reclen == 4) {
		uint32_t rl;
		return PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 4) && lbuf == rl;
	}
	else {
		uint64_t rl;
		return PSF_MCNP_ssw_readbytes(file, reinterpret_cast<char*>(&rl), 8) && lbuf == rl;
	}
}
bool ETHuman3DApp::PSF_MCNP_ssw_readbytes(std::ifstream& file, char* buf, size_t bytes) {
	if (!file.read(buf, bytes)) {
		return false;
	}
	return true;
}
int32_t ETHuman3DApp::PSF_MCNP_conv_mcnp6_ssw2pdg(int32_t c) {
	std::vector<int32_t> conv_mcnp6_to_pdg_0to36 = { 0, 2112, 22, 11, 13, -2112, 12, 14, -11, 2212, 3122,
												3222, 3112, 3322, 3312, 3334, -13, -12, -14, -2212, 211,
												111, 321, 310, 130, -3122, -3222, -3112, -3322, -3312, -3334,
												1000010020, 1000010030, 1000020030, 1000020040, -211, -321 };
	if (c < 0)
		return 0;  // 유효하지 않은 값일 경우 0 반환

	int antibit = c % 2;  // 반입자 여부 결정 (1이면 반입자)
	c /= 2;               // 나머지 데이터 추출

	int ptype = c % 64;    // 입자 유형 추출 (0~63 범위)
	c /= 64;

	if (ptype <= 36) {
		// 입자 유형이 36 이하일 때 PDG 코드를 변환
		int32_t p = conv_mcnp6_to_pdg_0to36[ptype];

		// 반입자일 경우 음수로 변환, 단 감마선(p == 22) 제외
		return (antibit && p != 22) ? -p : p;
	}

	return 0;  // ptype이 36보다 크면 0 반환 (유효하지 않은 경우)
}
std::string ETHuman3DApp::PSF_PHITS_replaceDwithE(std::string input)
{
	std::string result = input;
	size_t pos = result.find('D');  // Find the position of 'D'
	if (pos != std::string::npos) { // Check if 'D' was found
		result[pos] = 'E';          // Replace 'D' with 'E'
	}
	return result;
}
void ETHuman3DApp::GenerateSourceActor_sourcePB(double* center_radius, double* theta_phi)
{
	// Reset process
	for (auto itr : SourcePanelActor_Position)
	{
		if (itr != nullptr)
		{
			m_pVTKWidget->GetSceneRenderer()->RemoveActor(itr);
			itr = nullptr;
		}
	}

	double center_x = center_radius[0];
	double center_y = center_radius[1];
	double center_z = center_radius[2];
	double center[3] = { center_x , center_y , center_z };
	double radius = center_radius[3];
	// theta_phi[0] = theta (polar angle in degrees)
	// theta_phi[1] = phi (azimuthal angle in degrees)
	double theta = theta_phi[0];  // Keep in degrees for SetOrientation
	double phi = theta_phi[1];    // Keep in degrees for SetOrientation

	double theta_rad = theta * vtkMath::Pi() / 180.0;
	double phi_rad = phi * vtkMath::Pi() / 180.0;
	double dir_x = sin(theta_rad) * cos(phi_rad);
	double dir_y = sin(theta_rad) * sin(phi_rad);
	double dir_z = cos(theta_rad);

	// ===== DISK (원판) =====

	// Create a filled circle (disk with inner radius 0)
	vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
	diskSource->SetInnerRadius(0.0); // 내부 반지름을 0으로 설정하여 채워진 원(circle) 생성
	diskSource->SetOuterRadius(radius); // 외부 반지름 설정 (circle의 반지름)
	// 원의 세그먼트 수 설정 (선택 사항)
	diskSource->SetRadialResolution(30);
	diskSource->SetCircumferentialResolution(60);
	
	// ===== Arrow (화살표) =====

	// Arrow parameters
	double ArrowLength = radius * 0.5;  // 화살표 전체 길이 (원 반경의 0.5배)
	if (ArrowLength < 5) ArrowLength = 5;
	double CylinderHeight = ArrowLength * 0.3;  // 실린더 높이 (화살표 길이의 0.3)
	double CylinderRadius = radius * 0.02;  // 실린더 반경 (원 반경의 0.02)
	if (CylinderRadius < 0.2) CylinderRadius = 0.2;
	double ConeHeight = ArrowLength * 0.1;  // 콘 높이 (화살표 길이의 0.1)
	double ConeRadius = radius * 0.03;  // 콘 반경 (원 반경의 0.03)
	if (ConeRadius < 0.3) ConeRadius = 0.3;

	// Create a cylinder (shaft of the arrow)
	vtkSmartPointer<vtkCylinderSource> cylinderSource = vtkSmartPointer<vtkCylinderSource>::New();
	cylinderSource->SetHeight(CylinderHeight);
	cylinderSource->SetRadius(CylinderRadius);
	cylinderSource->SetResolution(30);
		
	vtkSmartPointer<vtkTransform> transform_cylinder = vtkSmartPointer<vtkTransform>::New();
	transform_cylinder->PostMultiply(); //this is the key line
	transform_cylinder->RotateX(90);
	transform_cylinder->Translate(0, 0, CylinderHeight / 2.0); // (0,0,1) +z방향이 기본 형태	
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_cylinder = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_cylinder->SetInputConnection(cylinderSource->GetOutputPort());
	transformFilter_cylinder->SetTransform(transform_cylinder);
	transformFilter_cylinder->Update();
	
	// Create a cone (arrowhead)
	vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
	coneSource->SetHeight(ConeHeight);
	coneSource->SetRadius(ConeRadius);
	coneSource->SetResolution(30);

	vtkSmartPointer<vtkTransform> transform_cone = vtkSmartPointer<vtkTransform>::New();
	transform_cone->PostMultiply(); //this is the key line
	transform_cone->RotateY(-90);
	transform_cone->Translate(0, 0, CylinderHeight + ConeHeight / 2.0); // (0,0,1) +z방향이 기본 형태	
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_cone = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_cone->SetInputConnection(coneSource->GetOutputPort());
	transformFilter_cone->SetTransform(transform_cone);
	transformFilter_cone->Update();

	auto appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
	appendFilter->AddInputConnection(diskSource->GetOutputPort());
	appendFilter->AddInputData(transformFilter_cylinder->GetOutput());
	appendFilter->AddInputData(transformFilter_cone->GetOutput());
	if (radius == 0) // pencil beam이면 점으로 추가
	{
		// Create a sphere
		vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(0.0, 0.0, 0.0);
		sphereSource->SetRadius(0.5);

		// Make the surface smooth.
		sphereSource->SetPhiResolution(60);
		sphereSource->SetThetaResolution(30);

		appendFilter->AddInputConnection(sphereSource->GetOutputPort());
	}
	appendFilter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(appendFilter->GetOutputPort());

	SourcePanelActor_Position.push_back(vtkSmartPointer<vtkActor>::New());
	SourcePanelActor_Position.back()->SetMapper(mapper);
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
	SourcePanelActor_Position.back()->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
	SourcePanelActor_Position.back()->SetPickable(false);

	// Add cylinder actor to renderer
	m_pVTKWidget->GetSceneRenderer()->AddActor(SourcePanelActor_Position.back());

	SourcePanelActor_Position.back()->SetPosition(center_x, center_y, center_z);

	double orientationAngles[3] = { 0, 0, 0 };
	orientationAngles[0] = 0;      // X축 회전 불필요
	orientationAngles[1] = theta;  // Y축 회전으로 theta 적용
	orientationAngles[2] = phi;    // Z축 회전으로 phi 적용

	SourcePanelActor_Position.back()->SetOrientation(orientationAngles);

	m_pVTKWidget->renderWindow()->Render();
}

//************************************** Calculation Process **************************************//
bool ETHuman3DApp::SettingContentsChangeCheck()
{
	bool is_changed = false;
	for (int i = 0; i < pRt->OrganNameList.size(); i++)
	{
		if (pRt->m_CalculationSetting_TargetError[i] != pRt->CalculationSetting_TargetError_QLineEdit[pRt->OrganNameList[i]]->text()) is_changed = true;
	}
	if (pRt->m_CalculationSetting_TargetNPS != pRt->CalculationSetting_TargetNPS_QLineEdit->text()) is_changed = true;
	if (pRt->m_CalculationSetting_ResultFileName != pRt->CalculationSetting_ResultFileName_QLineEdit->text()) is_changed = true;
	if (pRt->m_CalculationSetting_Thread != pRt->CalculationSetting_Thread_QLineEdit->text()) is_changed = true;
	if (pRt->SettingSkinDepthModeInfo == 0)
	{
		if (pRt->m_RadioButton_SkinDepthDoseOff->isChecked() == false) is_changed = true;
	}
	if (pRt->SettingSkinDepthModeInfo == 1)
	{
		if (pRt->m_RadioButton_SkinDepthDoseOff->isChecked() == true) is_changed = true;
	}
	if (pRt->SettingVisualizationModeInfo == 0)
	{
		if (pRt->m_radioVisualizationModeOff->isChecked() == false) is_changed = true;
	}
	if (pRt->SettingVisualizationModeInfo == 1)
	{
		if (pRt->m_radioVisualizationModeOff->isChecked() == true) is_changed = true;
	}

	return is_changed;
}
////////////////// Calculate_Local ////////////////// 

void ETHuman3DApp::VisualizeVolumeRatio(int phantomIndex)
{
// 1. 디버그 파일 열기
	std::string debugFileName = "Debug_Volume_Phantom_" + std::to_string(phantomIndex) + ".txt";
	std::ifstream infile(debugFileName);

	if (!infile.is_open()) {
		qDebug() << "Error: Cannot open debug file:" << QString::fromStdString(debugFileName);
		return;
	}

	vtkSmartPointer<vtkPolyData> polyData = SkinLayer_PolyData[phantomIndex];
	if (!polyData) return;

	vtkIdType numPoints = polyData->GetNumberOfPoints();

	// 2. Vertex별 부피 누적을 위한 임시 벡터 ([0]:WedgeSum, [1]:PrismSum)
	std::vector<std::pair<double, double>> vertexVolSums(numPoints, {0.0, 0.0});

	// 3. 파일 파싱 (Facet -> Vertex 누적)
	std::string line;
	std::getline(infile, line); // 헤더 스킵

	int facetID;
	double cx, cy, cz, wVol, pVol, diff, ratio;
	vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();

	while (infile >> facetID >> cx >> cy >> cz >> wVol >> pVol >> diff >> ratio)
	{
		if (facetID < polyData->GetNumberOfCells()) {
			polyData->GetCellPoints(facetID, ptIds);
			if (ptIds->GetNumberOfIds() == 3) {
				for (int i = 0; i < 3; ++i) {
					vtkIdType pid = ptIds->GetId(i);
					if (pid < numPoints) {
						vertexVolSums[pid].first += wVol;
						vertexVolSums[pid].second += pVol;
					}
				}
			}
		}
	}
	infile.close();

	// 4. Vertex별 Ratio 계산 및 Min/Max 찾기
	std::vector<double> vertexRatios(numPoints);
	double minRatio = 100.0;
	double maxRatio = -100.0;

	for (vtkIdType i = 0; i < numPoints; ++i) {
		double totalWedge = vertexVolSums[i].first;
		double totalPrism = vertexVolSums[i].second;
		
		// 0 나누기 방지
		double vRatio = (totalPrism > 1e-15) ? (totalWedge / totalPrism) : 1.0;
		vertexRatios[i] = vRatio;

		if (vRatio < minRatio) minRatio = vRatio;
		if (vRatio > maxRatio) maxRatio = vRatio;
	}

	// (선택) 가시화 범위를 너무 넓지 않게 제한 (색상 대비를 위해)
	// 예: 0.8 ~ 1.2 사이만 색상 변화를 주고, 그 밖은 진한 파랑/빨강으로 고정
	double rangeMin = 0.9; // 파란색 시작점
	double rangeMax = 1.1; // 빨간색 끝점
	
	qDebug() << "Ratio Range (Raw):" << minRatio << "~" << maxRatio;

	// ------------------------------------------------------------------
	// [User's Structure] 직접 Color Array를 생성하여 주입하는 방식
	// ------------------------------------------------------------------
	
	// Mapper 생성 및 설정
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(SkinLayer_PolyData[phantomIndex]); 
	mapper->SetScalarVisibility(true);

	vtkNew<vtkUnsignedCharArray> Colors; 
	Colors->SetNumberOfComponents(3);
	Colors->SetNumberOfTuples(numPoints);

	// [설정] 임계값 정의
	const double SAFE_MIN = 0.95;
	const double SAFE_MAX = 1.05;
	
	// [Blue Zone]
	// 0.85 이하로 떨어지면 바로 진한 파랑 (유지)
	const double VISUAL_BLUE_LIMIT = 0.85; 

	// [Red Zone - 3 Stages of Severity]
	const double LIMIT_RED   = 1.25; // 여기까지 오면 이미 새빨강 (Vivid Red)
	const double LIMIT_DARK  = 2.00; // 여기까지 오면 검붉은색 (Dark Red)
	const double LIMIT_BLACK = 50.0; // 여기까지 오면 완전 검정 (Jet Black)

	// 극한 단계(Black)를 위한 로그 스케일 상수
	const double LOG_DARK_START = std::log(LIMIT_DARK);
	const double LOG_BLACK_END  = std::log(LIMIT_BLACK);
	const double LOG_RANGE_BLACK = LOG_BLACK_END - LOG_DARK_START;

	// [기본 색상: Skin Color] (255, 192, 160)
	const unsigned char sR = 255, sG = 192, sB = 160;

	for (vtkIdType i = 0; i < numPoints; ++i) 
	{
		double val = vertexRatios[i];
		unsigned char r = sR, g = sG, b = sB;

		// ---------------------------------------------------------
		// Case 1: 안전 구간 (Safe Zone) -> Skin Color
		// ---------------------------------------------------------
		if (val >= SAFE_MIN && val <= SAFE_MAX) {
			r = sR; g = sG; b = sB;
		}
		// ---------------------------------------------------------
		// Case 2: 압축 구간 (Compression) -> Blue to Skin
		// ---------------------------------------------------------
		else if (val < SAFE_MIN) {
			double effectiveVal = (val < VISUAL_BLUE_LIMIT) ? VISUAL_BLUE_LIMIT : val;
			
			// t: 0(Blue Limit) ~ 1(Safe Skin)
			double t = (effectiveVal - VISUAL_BLUE_LIMIT) / (SAFE_MIN - VISUAL_BLUE_LIMIT);

			// Blue(0,0,255) ~ Skin(255,192,160)
			r = static_cast<unsigned char>(0 + (sR - 0) * t);     
			g = static_cast<unsigned char>(0 + (sG - 0) * t);     
			b = static_cast<unsigned char>(255 + (sB - 255) * t); 
		} 
		// ---------------------------------------------------------
		// Case 3: 확장 구간 (Expansion) - 3단계 심화 (Red -> Dark -> Black)
		// ---------------------------------------------------------
		else { // val > SAFE_MAX
			
			// [Phase 1] 즉각적 경고: 1.05(Skin) -> 1.25(Red)
			// 노란색 없이 바로 빨간색으로 변합니다.
			if (val <= LIMIT_RED) {
				double t = (val - SAFE_MAX) / (LIMIT_RED - SAFE_MAX); // 0.0 ~ 1.0
				
				// Skin(255,192,160) -> Vivid Red(255,0,0)
				r = 255; // 유지
				g = static_cast<unsigned char>(sG * (1.0 - t)); // 192 -> 0 (녹색 제거)
				b = static_cast<unsigned char>(sB * (1.0 - t)); // 160 -> 0 (파란색 제거)
			}
			// [Phase 2] 심화 경고: 1.25(Red) -> 2.0(Dark Red)
			// 명도를 낮춰서 검붉게 만듭니다.
			else if (val <= LIMIT_DARK) {
				double t = (val - LIMIT_RED) / (LIMIT_DARK - LIMIT_RED); // 0.0 ~ 1.0
				
				// Vivid Red(255,0,0) -> Dark Red(100,0,0)
				// R값을 255에서 100까지 떨어뜨림
				r = static_cast<unsigned char>(255 - (155 * t)); 
				g = 0;
				b = 0;
			}
			// [Phase 3] 극한 경고: 2.0(Dark Red) -> 50.0(Black)
			// Log Scale로 아주 천천히 완전한 검정색이 됩니다.
			else {
				double effectiveVal = (val > LIMIT_BLACK) ? LIMIT_BLACK : val;
				
				// Log t: 0.0(2.0지점) ~ 1.0(50.0지점)
				double t = (std::log(effectiveVal) - LOG_DARK_START) / LOG_RANGE_BLACK;

				// Dark Red(100,0,0) -> Black(0,0,0)
				r = static_cast<unsigned char>(100 * (1.0 - t));
				g = 0;
				b = 0;
			}
		}

		float mappedColor[3] = { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b) };
		Colors->SetTuple(i, mappedColor);
	}

	// Data 할당
	SkinLayer_PolyData[phantomIndex]->GetPointData()->SetScalars(Colors);
	
	// Actor 업데이트
	if (SkinPhantomActor[phantomIndex]) {
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinPhantomActor[phantomIndex]); // 기존 제거 (안전장치)
		SkinPhantomActor[phantomIndex] = nullptr;
		
		SkinPhantomActor[phantomIndex] = vtkSmartPointer<vtkActor>::New();
		SkinPhantomActor[phantomIndex]->SetMapper(mapper);
		SkinPhantomActor[phantomIndex]->VisibilityOn();
		
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinPhantomActor[phantomIndex]);
	}

	// Render
	theApp.m_pVTKWidget->renderWindow()->Render();

	PhantomPanelActor[phantomIndex]->VisibilityOff();
}
void ETHuman3DApp::SkinLayerGeneration() // called by (1) DataInitialization_Local
{
	// [Helper Lambda] vtkTetra 헤더 없이 수학적으로 사면체 부피 계산
	// V = |(a-d) . ((b-d) x (c-d))| / 6
	auto ComputeTetraVolume = [](double p0[3], double p1[3], double p2[3], double p3[3]) -> double {
		double v1[3] = { p0[0] - p3[0], p0[1] - p3[1], p0[2] - p3[2] };
		double v2[3] = { p1[0] - p3[0], p1[1] - p3[1], p1[2] - p3[2] };
		double v3[3] = { p2[0] - p3[0], p2[1] - p3[1], p2[2] - p3[2] };
		double cross[3];
		vtkMath::Cross(v2, v3, cross);
		return std::abs(vtkMath::Dot(v1, cross)) / 6.0;
	};

	for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector)
	{
		// 더미팬텀, 에어스피어팬텀이면 continue
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES ||
			pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;

		SkinPhantomActor[itr_phantomIndex] = vtkSmartPointer<vtkActor>::New();
		SkinDoseVisualization_ScalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
		SkinDoseVisualization_Label0percent = vtkSmartPointer<vtkTextActor>::New();
		SkinDoseVisualization_Label20percent = vtkSmartPointer<vtkTextActor>::New();
		SkinDoseVisualization_Label40percent = vtkSmartPointer<vtkTextActor>::New();
		SkinDoseVisualization_Label60percent = vtkSmartPointer<vtkTextActor>::New();
		SkinDoseVisualization_Label80percent = vtkSmartPointer<vtkTextActor>::New();
		SkinDoseVisualization_Label100percent = vtkSmartPointer<vtkTextActor>::New();
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinPhantomActor[itr_phantomIndex]);

		// Skin Density and skin depth(for HP)      
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][1] == 0) // Gender: male
		{
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 0) { SkinDenstiy[itr_phantomIndex] = 1.089; SkinAverageDepth[itr_phantomIndex] = 1600. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 1) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 1250. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 2) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 990. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 3) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 850. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 4) { SkinDenstiy[itr_phantomIndex] = 1.099; SkinAverageDepth[itr_phantomIndex] = 660. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 5) { SkinDenstiy[itr_phantomIndex] = 1.099; SkinAverageDepth[itr_phantomIndex] = 660. / 10000.; } 
		}
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][1] == 1) // Gender: female
		{
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 0) { SkinDenstiy[itr_phantomIndex] = 1.088; SkinAverageDepth[itr_phantomIndex] = 1300. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 1) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 1200. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 2) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 990. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 3) { SkinDenstiy[itr_phantomIndex] = 1.098; SkinAverageDepth[itr_phantomIndex] = 850. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 4) { SkinDenstiy[itr_phantomIndex] = 1.099; SkinAverageDepth[itr_phantomIndex] = 660. / 10000.; } 
			if (pRt->m_Phantom_MainInfo[itr_phantomIndex][12] == 5) { SkinDenstiy[itr_phantomIndex] = 1.099; SkinAverageDepth[itr_phantomIndex] = 660. / 10000.; } 
		}

		// -------------------------------------------------------
		// 1. Load Original Polydata (Outer Skin Surface)
		// -------------------------------------------------------
		vtkSmartPointer<vtkPolyData> PhantomPolydataOriginal = vtkSmartPointer<vtkPolyData>::New();
		QString strPath;
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][2] == pRt->E_PHANTOMTYPE_IMPORTED)
		{
			std::vector<std::string> extract_organlist;
			extract_organlist.push_back("12200_Skin_surface");
			std::string outPath = ExtractPhantomOBJ(m_ImportedPhantomFilePath_NoExtention[itr_phantomIndex].toStdString(), PhantomFileTitle[itr_phantomIndex].toStdString(), extract_organlist);

			// Polydata로 등록
			vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
			reader->SetFileName(Util::Wcs_to_mbs(QString::fromStdString(outPath).toStdWString()).c_str());
			reader->Update();
			PhantomPolydataOriginal->DeepCopy(reader->GetOutput());

			// 삭제
			QString qOutPath = QString::fromStdString(outPath);
			if (QFile::exists(qOutPath)) {
				QFile::remove(qOutPath);
			}
		}
		else // 그 외 (MRCP, deformed)
		{
			vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
			strPath = "./data/phantom/skinLayer/" + PhantomFileTitle[itr_phantomIndex] + "_outer.obj";
			reader->SetFileName(Util::Wcs_to_mbs(strPath.toStdWString()).c_str());
			reader->Update();
			PhantomPolydataOriginal->DeepCopy(reader->GetOutput());
		}

		// -------------------------------------------------------
		// 2. Load 50um and 100um Polydata for Volume Calculation & 3D Grid
		// -------------------------------------------------------
		vtkSmartPointer<vtkPolyData> PhantomPolydataOriginal_50um = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPolyData> PhantomPolydataOriginal_100um = vtkSmartPointer<vtkPolyData>::New();

		{
			// Load 50um
			vtkSmartPointer<vtkOBJReader> reader50 = vtkSmartPointer<vtkOBJReader>::New();
			QString strPath50 = "./data/phantom/skinLayer/" + PhantomFileTitle[itr_phantomIndex] + "_50um.obj";
			reader50->SetFileName(Util::Wcs_to_mbs(strPath50.toStdWString()).c_str());
			reader50->Update();
			PhantomPolydataOriginal_50um->DeepCopy(reader50->GetOutput());

			// Load 100um
			vtkSmartPointer<vtkOBJReader> reader100 = vtkSmartPointer<vtkOBJReader>::New();
			QString strPath100 = "./data/phantom/skinLayer/" + PhantomFileTitle[itr_phantomIndex] + "_100um.obj";
			reader100->SetFileName(Util::Wcs_to_mbs(strPath100.toStdWString()).c_str());
			reader100->Update();
			PhantomPolydataOriginal_100um->DeepCopy(reader100->GetOutput());
		}

		// -------------------------------------------------------
		// 3. Transformation Setup
		// -------------------------------------------------------
		vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
		transform_p->PostMultiply(); 
		transform_p->Translate(-PhantomOrigianlPolyDataCenter[itr_phantomIndex][0], -PhantomOrigianlPolyDataCenter[itr_phantomIndex][1], -PhantomOrigianlPolyDataCenter[itr_phantomIndex][2]);
		
		vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		transform->PostMultiply(); 
		double xy_scale = 1;
		double z_scale = 1;
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_TRANSFORMED) 
		{
			xy_scale = PhantomPolyDataScaleFactor[itr_phantomIndex][0];
			z_scale = PhantomPolyDataScaleFactor[itr_phantomIndex][1];
		}

		double PosX = pRt->m_Phantom_MainInfo[itr_phantomIndex][6];
		double PosY = pRt->m_Phantom_MainInfo[itr_phantomIndex][7];
		double PosZ = pRt->m_Phantom_MainInfo[itr_phantomIndex][8];
		double RotX = pRt->m_Phantom_MainInfo[itr_phantomIndex][9];
		double RotY = pRt->m_Phantom_MainInfo[itr_phantomIndex][10];
		double RotZ = pRt->m_Phantom_MainInfo[itr_phantomIndex][11];
		const double PI = 3.141592 / 180; 

		Eigen::Matrix4f X, Y, Z, Scale;
		double RadianX = RotX * PI;
		double RadianY = RotY * PI;
		double RadianZ = RotZ * PI;

		X << 1, 0, 0, 0, 0, cos(RadianX), -sin(RadianX), 0, 0, sin(RadianX), cos(RadianX), 0, 0, 0, 0, 1;
		Y << cos(RadianY), 0, sin(RadianY), 0, 0, 1, 0, 0, -sin(RadianY), 0, cos(RadianY), 0, 0, 0, 0, 1;
		Z << cos(RadianZ), -sin(RadianZ), 0, 0, sin(RadianZ), cos(RadianZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
		Scale << xy_scale, 0, 0, 0, 0, xy_scale, 0, 0, 0, 0, z_scale, 0, 0, 0, 0, 1;

		Eigen::Matrix4f Result = Z * X * Y * Scale; 

		double elementsResult[16];
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				elementsResult[i * 4 + j] = Result(i, j);
			}
		}
		transform->SetMatrix(elementsResult);
		transform->Translate(PosX, PosY, PosZ);


		// -------------------------------------------------------
		// 4. Apply Transformation to All Layers
		// -------------------------------------------------------
		
		// (1) Surface Layer
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_p->SetInputData(PhantomPolydataOriginal);
		transformFilter_p->SetTransform(transform_p);
		transformFilter_p->Update();

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetInputData(transformFilter_p->GetOutput());
		transformFilter->SetTransform(transform);
		transformFilter->Update();
		vtkSmartPointer<vtkPolyData> polySkin = transformFilter->GetOutput(); 
		SkinLayer_PolyData[itr_phantomIndex] = polySkin;

		// (2) 50um Layer
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p_50 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_p_50->SetInputData(PhantomPolydataOriginal_50um);
		transformFilter_p_50->SetTransform(transform_p);
		transformFilter_p_50->Update();

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_50 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_50->SetInputData(transformFilter_p_50->GetOutput());
		transformFilter_50->SetTransform(transform);
		transformFilter_50->Update();
		vtkSmartPointer<vtkPolyData> polySkin_50um = transformFilter_50->GetOutput(); 

		// (3) 100um Layer
		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p_100 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_p_100->SetInputData(PhantomPolydataOriginal_100um);
		transformFilter_p_100->SetTransform(transform_p);
		transformFilter_p_100->Update();

		vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_100 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter_100->SetInputData(transformFilter_p_100->GetOutput());
		transformFilter_100->SetTransform(transform);
		transformFilter_100->Update();
		vtkSmartPointer<vtkPolyData> polySkin_100um = transformFilter_100->GetOutput(); 


		// -------------------------------------------------------
		// 5. Build Locators (2D & 3D) & Calculate Volume
		// -------------------------------------------------------
		
		// [2D Locator]
		SkinCellLocator[itr_phantomIndex] = vtkSmartPointer<vtkCellLocator>::New();
		SkinCellLocator[itr_phantomIndex]->SetDataSet(SkinLayer_PolyData[itr_phantomIndex]);
		SkinCellLocator[itr_phantomIndex]->CacheCellBoundsOn();
		SkinCellLocator[itr_phantomIndex]->BuildLocator();

		std::vector<std::array<double, 3>> centers;
		std::vector<std::array<double, 3>> normals;

		vtkCellArray* cells = SkinLayer_PolyData[itr_phantomIndex]->GetPolys(); // Surface
		vtkPoints* points = SkinLayer_PolyData[itr_phantomIndex]->GetPoints();  // Surface

		// Points for 50um and 100um
		vtkPoints* points_50 = polySkin_50um->GetPoints();
		vtkPoints* points_100 = polySkin_100um->GetPoints();

		// 3D UnstructuredGrid 준비
		vtkSmartPointer<vtkUnstructuredGrid> volumeGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		vtkSmartPointer<vtkPoints> volumePoints = vtkSmartPointer<vtkPoints>::New();
		
		// Grid에 포인트 추가: 50um 점들 먼저, 그 뒤에 100um 점들
		for (vtkIdType i = 0; i < points_50->GetNumberOfPoints(); ++i) {
			volumePoints->InsertNextPoint(points_50->GetPoint(i));
		}
		for (vtkIdType i = 0; i < points_100->GetNumberOfPoints(); ++i) {
			volumePoints->InsertNextPoint(points_100->GetPoint(i));
		}
		volumeGrid->SetPoints(volumePoints);
		
		// 100um 점들의 Index Offset (Wedge 생성 시 사용)
		vtkIdType offset100 = points_50->GetNumberOfPoints();

		cells->InitTraversal();
		vtkIdType npts;
		const vtkIdType* pts; 
		
		VertexToFacetMap[itr_phantomIndex].resize(points->GetNumberOfPoints());
		int cellID = 0;
		std::map<int, std::vector<int>> facet_vertexID_map;
		
		// [IMPORTANT] Index 8 is Volume
		std::vector<std::array<double, 9>> results; 
		while (cells->GetNextCell(npts, pts))
		{
			if (npts == 3) 
			{
				double p0[3], p1[3], p2[3];
				double center[3], normal[3];

				points->GetPoint(pts[0], p0);
				points->GetPoint(pts[1], p1);
				points->GetPoint(pts[2], p2);

				for (vtkIdType i = 0; i < npts; ++i)
				{
					VertexToFacetMap[itr_phantomIndex][pts[i]].push_back(cellID);
				}
				facet_vertexID_map[cellID].push_back(pts[0]);
				facet_vertexID_map[cellID].push_back(pts[1]);
				facet_vertexID_map[cellID].push_back(pts[2]);

				for (int i = 0; i < 3; ++i) center[i] = (p0[i] + p1[i] + p2[i]) / 3.0;
				
				double v0[3] = { p1[0] - p0[0], p1[1] - p0[1], p1[2] - p0[2] };
				double v1[3] = { p2[0] - p0[0], p2[1] - p0[1], p2[2] - p0[2] };
				vtkMath::Cross(v0, v1, normal);
				vtkMath::Normalize(normal);

				centers.push_back({ center[0], center[1], center[2] });
				normals.push_back({ normal[0], normal[1], normal[2] });

				vtkCell* cellA = SkinLayer_PolyData[itr_phantomIndex]->GetCell(cellID); 
				double areaA = -1;
				if (cellA->GetCellType() == VTK_TRIANGLE)
				{
					vtkTriangle* triangleA = dynamic_cast<vtkTriangle*>(cellA);
					areaA = triangleA->ComputeArea(); // cm^2
				}
				else
				{
					theApp.SetMessageBox("Not triangular polygon skin");
				}

				if (areaA <= 0.05)
				{
					for (auto itr : facet_vertexID_map[cellID]) 
					{
						for (auto itr2 : VertexToFacetMap[itr_phantomIndex][itr]) 
						{
							TinyFacetAdjacentFacetMap[itr_phantomIndex][cellID].insert(itr2); 
						}
					}
				}

				// ---------------------------------------------------
				// 3D Wedge Volume Calculation (50um ~ 100um)
				// ---------------------------------------------------
				double wedgeVolume = 0.0;

				double p50_0[3], p50_1[3], p50_2[3];
				points_50->GetPoint(pts[0], p50_0);
				points_50->GetPoint(pts[1], p50_1);
				points_50->GetPoint(pts[2], p50_2);

				double p100_0[3], p100_1[3], p100_2[3];
				points_100->GetPoint(pts[0], p100_0);
				points_100->GetPoint(pts[1], p100_1);
				points_100->GetPoint(pts[2], p100_2);

				// Decompose Wedge into 3 Tetrahedrons
				double vol1 = ComputeTetraVolume(p50_0, p50_1, p50_2, p100_0);
				double vol2 = ComputeTetraVolume(p50_1, p50_2, p100_0, p100_1);
				double vol3 = ComputeTetraVolume(p50_2, p100_0, p100_1, p100_2);

				wedgeVolume = vol1 + vol2 + vol3; // cm^3

				// ---------------------------------------------------
				// Create Wedge Cell for 3D Grid
				// ---------------------------------------------------
				
				vtkSmartPointer<vtkWedge> wedge = vtkSmartPointer<vtkWedge>::New();
				// Wedge Point IDs: 0,1,2 (Bottom/50um), 3,4,5 (Top/100um)
				// volumePoints 순서: [50um points ... ] [100um points ... ]
				wedge->GetPointIds()->SetId(0, pts[0]);
				wedge->GetPointIds()->SetId(1, pts[1]);
				wedge->GetPointIds()->SetId(2, pts[2]);
				wedge->GetPointIds()->SetId(3, pts[0] + offset100);
				wedge->GetPointIds()->SetId(4, pts[1] + offset100);
				wedge->GetPointIds()->SetId(5, pts[2] + offset100);
				
				volumeGrid->InsertNextCell(wedge->GetCellType(), wedge->GetPointIds());
			
				// Save result with Volume at index 8
				results.push_back({ 
					center[0], center[1], center[2], 
					normal[0], normal[1], normal[2], 
					SkinAverageDepth[itr_phantomIndex], 
					areaA, 
					wedgeVolume 
				});

				++cellID;
			}
		}
		FacetInfo[itr_phantomIndex] = results;

		// -------------------------------------------------------
		// Build 3D Locator from UnstructuredGrid
		// -------------------------------------------------------
		m_SkinVolumeLocator[itr_phantomIndex] = vtkSmartPointer<vtkCellLocator>::New();
		m_SkinVolumeLocator[itr_phantomIndex]->SetDataSet(volumeGrid);
		m_SkinVolumeLocator[itr_phantomIndex]->BuildLocator();


		for (auto itr : TinyFacetAdjacentFacetMap[itr_phantomIndex])
		{
			for (auto itr2 : TinyFacetAdjacentFacetMap[itr_phantomIndex][itr.first]) 
			{
				TinyFacetAdjacentFacetVolumeSum[itr_phantomIndex][itr.first] += (FacetInfo[itr_phantomIndex][itr2][7] * FacetInfo[itr_phantomIndex][itr2][6]);
			}
		}
	}

	// VisualizeVolumeRatio(0); 

	theApp.m_pVTKWidget->renderWindow()->Render();

}
void ETHuman3DApp::Generate_PSFdata() // called by (1) DataInitialization_Local
{
	std::ofstream ofp_phsp("./data/dbsend/PSFdata");
	for (auto itr_PSF_data : PSF_data)
	{
		ofp_phsp << std::get<0>(itr_PSF_data) << " "
			<< std::get<1>(itr_PSF_data) << " " << std::get<2>(itr_PSF_data) << " " << std::get<3>(itr_PSF_data) << " "
			<< std::get<4>(itr_PSF_data) << " " << std::get<5>(itr_PSF_data) << " " << std::get<6>(itr_PSF_data) << " "
			<< std::get<7>(itr_PSF_data) << " " << std::get<8>(itr_PSF_data) << endl;
	}
	ofp_phsp.close();

	// Read phspFile Size
	std::ifstream ifp_phsp_size("./data/dbsend/PSFdata", std::ios::binary); // Opening in binary mode
	// Seek to the end of the file
	ifp_phsp_size.seekg(0, std::ios::end);
	// Get the size in bytes
	std::streamsize size = ifp_phsp_size.tellg();
	// Convert bytes to megabytes (MB)
	pRt->PSF_FileSize = static_cast<double>(size);
	ifp_phsp_size.close();
}
void ETHuman3DApp::SaveData() // called by (1) FunctionPanelRight::DataInitialization_Local
{
	pRt->Write_TextResultFile_SimulationInfo("./data/dbsend/Simulation_info"); // Make SimulationInfo file	
	MakeFile_PhantomCollection();
	MakeFile_SourceMacro();
	MakeFile_ListCollection();
	MakeFile_CalcCollection();	

	/////////////////////////////// 팬텀/의복/선량계는 Geant4상에서 회전/이동함, Glasses tet.node 제작 및 회전/이동///////////////////////////////////////
	//for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector)
	//{
	//	// if 안경이 존재할때만
	//	GenerateGlassesTetFile(itr_phantomIndex);
	//	TranslateGlassesTetFile(itr_phantomIndex);
	//}	
}

void ETHuman3DApp::Make_mcsee_File_extdata_previous(QString filepath)
{
	// 결합된 파일 열기
	QFile outputFile(filepath);
	if (!outputFile.open(QIODevice::WriteOnly)) {
		qDebug() << "Failed to open output file for writing.";
		return;
	}
	int rearragned_phantomID = 0;
	// pRt->m_Phantom_SequenceVector 순회
	for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector) 
	{
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_TYPE] == pRt->E_PHANTOMTYPE_IMPORTED) 
		{
			// .ply 파일 열기 (이진 모드로 열기)
			QFile plyFile(m_ImportedPhantomFilePath[itr_phantomIndex]);
			if (plyFile.open(QIODevice::ReadOnly)) 
			{
				QByteArray plyData = plyFile.readAll();  // .ply 파일의 이진 데이터를 읽기
				plyFile.close();

				// -ipp 구분자 추가
				outputFile.write("-ipp ");

				// rearragned_phantomID 추가
				outputFile.write(QString::number(rearragned_phantomID).toUtf8());
				outputFile.write(" "); // 정수 뒤에 한 칸 띄우기

				// .ply 파일 데이터를 출력 파일에 쓰기
				outputFile.write(plyData);

				// 파일종료 구분자 추가
				outputFile.write("-end ");

			}
			else {
				theApp.SetMessageBox("Failed to open ply file");
				continue;
			}

			// .node 파일 열기 (이진 모드로 열기)
			QString nodeFilePath = m_ImportedPhantomFilePath[itr_phantomIndex];
			nodeFilePath.replace(".ply", ".node");  // .node 파일 경로 생성

			QFile nodeFile(nodeFilePath);
			if (nodeFile.open(QIODevice::ReadOnly)) {
				QByteArray nodeData = nodeFile.readAll();
				nodeFile.close();

				// -ipn 구분자 추가
				outputFile.write("-ipn ");

				// rearragned_phantomID 추가
				outputFile.write(QString::number(rearragned_phantomID).toUtf8());
				outputFile.write(" "); // 정수 뒤에 한 칸 띄우기				

				// .node 파일 데이터를 출력 파일에 쓰기
				outputFile.write(nodeData);

				// 파일종료 구분자 추가
				outputFile.write("-end ");
			}
			else 
			{
				theApp.SetMessageBox("Failed to open node file");
				continue;
			}
		}
		rearragned_phantomID++;
	}

	// 파일 닫기
	outputFile.close();
}
void ETHuman3DApp::MakeFile_PhantomCollection()
{
	//Phantom_collection 파일 생성, 
	std::string phantom_collection = "./data/dbsend/phantom_collection";
	std::ofstream ofpPhantom;
	ofpPhantom.precision(8);
	ofpPhantom.open(phantom_collection);

	ofpPhantom << pRt->m_Phantom_SequenceVector.size() << "\t" << pRt->maxX_PhantomBox << "\t" << pRt->maxY_PhantomBox << "\t" << pRt->maxZ_PhantomBox
		<< "\t" << pRt->minX_PhantomBox << "\t" << pRt->minY_PhantomBox << "\t" << pRt->minZ_PhantomBox << endl;

	for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector)
	{
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;
		// Dummy phantom
		int is_dummy = 0;
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES) is_dummy = 1;

		double pCenterX;
		double pCenterY;
		double pCenterZ;
		double ScaleX;
		double ScaleY;
		double ScaleZ;
		// Phantom geometry 정보
		if (is_dummy == 0)
		{
			pCenterX = PhantomOrigianlPolyDataCenter[itr_phantomIndex][0];
			pCenterY = PhantomOrigianlPolyDataCenter[itr_phantomIndex][1];
			pCenterZ = PhantomOrigianlPolyDataCenter[itr_phantomIndex][2];
			ScaleX = PhantomPolyDataScaleFactor[itr_phantomIndex][0];
			ScaleY = PhantomPolyDataScaleFactor[itr_phantomIndex][0];
			ScaleZ = PhantomPolyDataScaleFactor[itr_phantomIndex][1];
		}
		if (is_dummy == 1)
		{
			pCenterX = DummyPolyDataInfo[itr_phantomIndex][0];
			pCenterY = DummyPolyDataInfo[itr_phantomIndex][1];
			pCenterZ = DummyPolyDataInfo[itr_phantomIndex][2];
			ScaleX = DummyPolyDataInfo[itr_phantomIndex][3];
			ScaleY = DummyPolyDataInfo[itr_phantomIndex][4];
			ScaleZ = DummyPolyDataInfo[itr_phantomIndex][5];
		}

		double PosX = pRt->m_Phantom_MainInfo[itr_phantomIndex][6];
		double PosY = pRt->m_Phantom_MainInfo[itr_phantomIndex][7];
		double PosZ = pRt->m_Phantom_MainInfo[itr_phantomIndex][8];
		double RotX = pRt->m_Phantom_MainInfo[itr_phantomIndex][9];
		double RotY = pRt->m_Phantom_MainInfo[itr_phantomIndex][10];
		double RotZ = pRt->m_Phantom_MainInfo[itr_phantomIndex][11];
		const double PI = 3.141592 / 180; // 라디안 변환 
		double RadianX = RotX * PI;
		double RadianY = RotY * PI;
		double RadianZ = RotZ * PI;

		double* bounds = PhantomPanelActor[itr_phantomIndex]->GetBounds();

		int check_clothing = 0;
		int check_glasses = 0;
		int check_dosimeter = 0;

		//  clothing
		for (auto itr_clothingIndex : pRt->m_Clothing_SequenceVector[itr_phantomIndex]) // 의복 순환
		{
			check_clothing = 1; // 의복 인덱스 존재하면 의복 생성된 팬텀임
		}
		// glassses 개발필요

		// dosimeter 개발필요

		std::string pTitle;
		int is_imported = 0;
		QString Q_phantom_path = "-";
		std::string phantom_path = "-";
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][2] != pRt->E_PHANTOMTYPE_IMPORTED)
		{
			pTitle = PhantomFileTitle[itr_phantomIndex].toStdString();
		}
		if (pRt->m_Phantom_MainInfo[itr_phantomIndex][2] == pRt->E_PHANTOMTYPE_IMPORTED)
		{
			pTitle = m_ImportedPhantom_ParentPhantomFileName[itr_phantomIndex].toStdString();
			is_imported = 1;
			Q_phantom_path = m_ImportedPhantomFilePath_NoExtention[itr_phantomIndex];
			phantom_path = Q_phantom_path.toUtf8().constData();
		}

		ofpPhantom << pTitle << "\t" // phantom file title
			<< pCenterX << "\t" // pCenterX
			<< pCenterY << "\t" // pCenterY
			<< pCenterZ << "\t" // pCenterZ
			<< PosX << "\t" // PosX
			<< PosY << "\t" // PosY
			<< PosZ << "\t" // PosZ
			<< RotX << "\t" // RotX
			<< RotY << "\t" // RotY
			<< RotZ << "\t" // RotZ
			<< ScaleX << "\t" // ScaleX
			<< ScaleY << "\t" // ScaleY
			<< ScaleZ << "\t" // ScaleZ
			<< check_clothing << "\t" // Clothing 유무
			<< check_glasses << "\t" // Glasses 유무
			<< check_dosimeter << "\t" //Dosimeter 유무			
			<< is_dummy << "\t" // Dummyphantom 유무	
			<< is_imported << "\t" // imported 여부
			<< phantom_path << endl; // PhantomPath(imported 일 경우)
	}
	ofpPhantom.close();
}
void ETHuman3DApp::MakeFile_SourceMacro()
{
	std::ofstream ofp_source("./data/dbsend/source_macro");
	ofp_source << "/run/verbose 0" << endl;
	ofp_source << "/hadron/phys/thermalScattering true" << endl;
	ofp_source << "/run/initialize" << endl << endl;

	// Broad beam
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 0) 
	{
		if (pRt->m_comboBoxBeamdirection->currentText() == "User defined")
		{
			ofp_source << "/external/AziPol 1 " << QString::number(pRt->m_lineEditAzimuthalAngle->text().toDouble()).toStdString()
				<< " " << QString::number(pRt->m_lineEditPolarAngle->text().toDouble()).toStdString() << endl;
			ofp_source << "/gun/particle ";
			if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Photon") ofp_source << "gamma" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Electron") ofp_source << "e-" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Neutron") ofp_source << "neutron" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Proton") ofp_source << "proton" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Alpha") ofp_source << "alpha" << endl;
			ofp_source << "/gun/energy " << pRt->m_lineBroadBeamEnergy->text().toStdString() << " MeV" << endl;
		}
		else // AP PA LLAT RLAT ROT ISO 
		{
			ofp_source << "/external/dir " << pRt->m_comboBoxBeamdirection->currentText().toStdString() << endl;
			ofp_source << "/gun/particle ";
			if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Photon") ofp_source << "gamma" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Electron") ofp_source << "e-" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Neutron") ofp_source << "neutron" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Proton") ofp_source << "proton" << endl;
			else if (pRt->m_comboBoxBroadBeamParticleType->currentText() == "Alpha") ofp_source << "alpha" << endl;
			ofp_source << "/gun/energy " << pRt->m_lineBroadBeamEnergy->text().toStdString() << " MeV" << endl;
		}
	}

	// External point (EP)
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 1)
	{
		if (pRt->RI_Select_sourceEP_QRadioButton->isChecked())
		{
			bool is_first_RI = true;
			double first_RI_activity;
			bool is_first_radiation = true;
			double first_radiation_yield;
			double conversion_factor_sum_of_activity_yieldsum = 0;
			for (auto itr_info_RI : pRt->ListInfo_sourceEP) // 입력한 mulitple RI 마다 순환
			{
				std::string path = "./data/radioisotopes/" + itr_info_RI[4].toStdString() + ".txt";
				std::pair<std::vector<std::map<int, std::string>>, double> temp_pair = Read_RI_File(path);
				std::vector<std::map<int, std::string>> radionulclide_radiation_info = temp_pair.first;
				double this_RI_yieldsum = temp_pair.second;
				double this_RI_activity = itr_info_RI[5].toDouble();
				conversion_factor_sum_of_activity_yieldsum += this_RI_activity * this_RI_yieldsum; // activity concentration * area * yieldsum -> (NPS/s)
				if (is_first_RI) // 첫번째 RI 일때
				{
					first_RI_activity = this_RI_activity;
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						if (is_first_radiation) // 첫번째 RI, 첫번째 radiation 일때
						{
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Point" << endl;
							if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
							if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
							if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
							ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << itr_info_RI[1].toDouble() << " " << itr_info_RI[2].toDouble() << " " << itr_info_RI[3].toDouble() << " cm " << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
							first_radiation_yield = std::stod(itr_radiation[0]);
							is_first_radiation = false;
						}
						else // 첫번째 RI, 두번째 이상 radiation 일때
						{
							ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) << endl;
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Point" << endl;
							if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
							if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
							if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
							ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << itr_info_RI[1].toDouble() << " " << itr_info_RI[2].toDouble() << " " << itr_info_RI[3].toDouble() << " cm " << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
						}
					}
					is_first_RI = false;
				}
				else // 두번째 이상 RI
				{
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) * (this_RI_activity / first_RI_activity) << endl;
						ofp_source << "/gps/particle " << itr_radiation[2] << endl;
						ofp_source << "/gps/pos/type Point" << endl;
						if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
						if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
						if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
						ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << itr_info_RI[1].toDouble() << " " << itr_info_RI[2].toDouble() << " " << itr_info_RI[3].toDouble() << " cm " << endl;
						ofp_source << "/gps/ang/type iso" << endl;
						ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
					}
				}
			}
			DoseConversionFactor_inUImodule = conversion_factor_sum_of_activity_yieldsum;
		}
		else if (pRt->ES_Select_sourceEP_QRadioButton->isChecked())
		{
			// 전체 입자 중 첫 번째 입자인지를 판별하는 플래그
			bool is_first_particle_overall = true;
			// 기준이 될 첫 번째 입자의 절대적인 초당 방출량(#/s)을 저장할 변수
			double reference_absolute_intensity;
			// 모든 소스의 Intensity(#/s) 총합을 저장. 최종적으로 선량 변환 계수 계산에 사용됨.
			DoseConversionFactor_inUImodule = 0.0;
			
			for (auto itr_info_RI : pRt->ListInfo_sourceEP)
			{
				// 1. 현재 소스(파일)의 정보 파싱
				 // 위치 좌표 (비어있으면 "0"으로 처리)
				if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
				if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
				if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
				double posX = itr_info_RI[1].toDouble();
				double posY = itr_info_RI[2].toDouble();
				double posZ = itr_info_RI[3].toDouble();

				// 파일 경로와 해당 파일의 전체 Intensity
				QString filePath = itr_info_RI[4];
				double file_total_intensity = itr_info_RI[5].toDouble();

				// 총 방출량 계산
				DoseConversionFactor_inUImodule += file_total_intensity;

				// 2. 파일 파싱 준비: 파일 내용을 메모리에 저장하고 weight 총합 계산
				QFile inputFile(filePath);
				if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
					// 파일 열기 실패 시 이 파일은 건너뜀
					continue;
				}

				// 파일 내용을 임시로 저장할 리스트
				QList<QStringList> file_content;
				double total_weight_in_file = 0.0;

				QTextStream in(&inputFile);
				while (!in.atEnd()) {
					QString line = in.readLine();

					QStringList parts = line.simplified().split(' ');					

					if (parts.size() >= 3) {
						file_content.append(parts);
						// weight는 두 번째 열(parts[1])
						total_weight_in_file += parts[1].toDouble();
					}
				}
				inputFile.close();

				// 3. Geant4 GPS 명령어 생성
				// 메모리에 저장된 파일 내용을 한 줄씩 처리
				for (const auto& line_parts : file_content)
				{
					std::string particle_type = line_parts[0].toStdString();
					double weight = line_parts[1].toDouble();
					double energy = line_parts[2].toDouble();

					// 현재 입자의 절대적인 초당 방출량(#/s) 계산
					// = (파일 전체 방출량) * (파일 내 현재 입자의 가중치 / 파일 내 가중치 총합)
					double current_absolute_intensity = 0.0;
					if (total_weight_in_file > 0) {
						current_absolute_intensity = file_total_intensity * (weight / total_weight_in_file);
					}
					// 모든 소스를 통틀어 가장 첫 번째 입자인 경우
					if (is_first_particle_overall)
					{
						// 기준 방출량으로 설정
						reference_absolute_intensity = current_absolute_intensity;

						// /gps/source/add 없이 기본 명령어만 출력
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Point" << std::endl;
						ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << posX << " " << posY << " " << posZ << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;

						// 첫 번째 입자 처리가 끝났으므로 플래그를 false로 변경
						is_first_particle_overall = false;
					}
					// 두 번째 입자부터는 기준 입자 대비 상대적 비율을 계산하여 /gps/source/add 명령어 추가
					else
					{
						double relative_intensity = 0.0;
						// 기준 방출량이 0보다 클 때만 나누기 연산 수행 (오류 방지)
						if (reference_absolute_intensity > 0) {
							relative_intensity = current_absolute_intensity / reference_absolute_intensity;
						}

						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << relative_intensity << std::endl;
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Point" << std::endl;
						ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << posX << " " << posY << " " << posZ << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;
					}
				}
			}
		}		
	}

	// Floor disk (FD)
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 2)
	{
		if (pRt->RI_Select_sourceFD_QRadioButton->isChecked())
		{
			double center_x = pRt->PosX_SourceFD_QLineEdit->text().toDouble();
			double center_y = pRt->PosY_SourceFD_QLineEdit->text().toDouble();
			double center_z = pRt->PosZ_SourceFD_QLineEdit->text().toDouble();
			std::string center_x_str = pRt->PosX_SourceFD_QLineEdit->text().toStdString();
			std::string center_y_str = pRt->PosY_SourceFD_QLineEdit->text().toStdString();
			std::string center_z_str = pRt->PosZ_SourceFD_QLineEdit->text().toStdString();
			if (center_x_str == "") center_x = 0;
			if (center_y_str == "") center_y = 0;
			if (center_z_str == "") center_z = 0;
			double radius = pRt->Radius_sourceFD_QLineEdit->text().toDouble();

			bool is_first_RI = true;
			double first_RI_activity;
			bool is_first_radiation = true;
			double first_radiation_yield;
			double conversion_factor_sum_of_activity_yieldsum = 0;
			for (auto itr_info_RI : pRt->ListInfo_sourceFD) // 입력한 mulitple RI 마다 순환
			{
				std::string path = "./data/radioisotopes/" + itr_info_RI[1].toStdString() + ".txt";
				std::pair<std::vector<std::map<int, std::string>>, double> temp_pair = Read_RI_File(path);
				std::vector<std::map<int, std::string>> radionulclide_radiation_info = temp_pair.first;
				double this_RI_yieldsum = temp_pair.second;
				double this_RI_activity = itr_info_RI[2].toDouble();
				conversion_factor_sum_of_activity_yieldsum += this_RI_activity * radius * radius * M_PI * this_RI_yieldsum; // activity concentration * area * yieldsum -> (NPS/s)
				if (is_first_RI) // 첫번째 RI 일때
				{
					first_RI_activity = this_RI_activity;
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						if (is_first_radiation) // 첫번째 RI, 첫번째 radiation 일때
						{
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Plane" << endl;
							ofp_source << "/gps/pos/shape Circle" << endl;
							ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm " << endl;
							ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
							first_radiation_yield = std::stod(itr_radiation[0]);
							is_first_radiation = false;
						}
						else // 첫번째 RI, 두번째 이상 radiation 일때
						{
							ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) << endl;
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Plane" << endl;
							ofp_source << "/gps/pos/shape Circle" << endl;
							ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm " << endl;
							ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
						}
					}
					is_first_RI = false;
				}
				else // 두번째 이상 RI
				{
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) * (this_RI_activity / first_RI_activity) << endl;
						ofp_source << "/gps/particle " << itr_radiation[2] << endl;
						ofp_source << "/gps/pos/type Plane" << endl;
						ofp_source << "/gps/pos/shape Circle" << endl;
						ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm " << endl;
						ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << endl;
						ofp_source << "/gps/ang/type iso" << endl;
						ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
					}
				}
			}
			DoseConversionFactor_inUImodule = conversion_factor_sum_of_activity_yieldsum;
		}

		else if (pRt->ES_Select_sourceFD_QRadioButton->isChecked())
		{
			// 1. 선원의 기하학적 정보 파싱 (UI로부터)
			double center_x = pRt->PosX_SourceFD_QLineEdit->text().toDouble();
			double center_y = pRt->PosY_SourceFD_QLineEdit->text().toDouble();
			double center_z = pRt->PosZ_SourceFD_QLineEdit->text().toDouble();
			// 입력값이 비어있을 경우 0으로 처리
			if (pRt->PosX_SourceFD_QLineEdit->text().isEmpty()) center_x = 0;
			if (pRt->PosY_SourceFD_QLineEdit->text().isEmpty()) center_y = 0;
			if (pRt->PosZ_SourceFD_QLineEdit->text().isEmpty()) center_z = 0;
			double radius = pRt->Radius_sourceFD_QLineEdit->text().toDouble();

			// 2. Geant4 GPS 생성을 위한 변수 초기화
			// 전체 입자 중 첫 번째 입자인지를 판별하는 플래그
			bool is_first_particle_overall = true;
			// 기준이 될 첫 번째 입자의 절대적인 초당 방출량(#/s)을 저장할 변수
			double reference_absolute_intensity;
			// 모든 소스의 총 방출량(#/s) 합계
			DoseConversionFactor_inUImodule = 0.0;

			// 3. 사용자가 추가한 각 소스 정보(파일)를 순회
			for (auto itr_info_ES : pRt->ListInfo_sourceFD)
			{
				// itr_info_ES[1] : 파일 경로, itr_info_ES[2] : 면적당 Intensity (단위: #/s/cm2)
				QString filePath = itr_info_ES[1];
				double areal_intensity = itr_info_ES[2].toDouble();

				// 현재 파일(소스)의 총 방출량(#/s) = 면적당 Intensity * 면적
				double file_total_intensity = areal_intensity * radius * radius * M_PI;
				DoseConversionFactor_inUImodule += file_total_intensity;

				// 4. 파일 파싱 및 Geant4 명령어 생성 준비
				QFile inputFile(filePath);
				if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
					continue; // 파일 열기 실패 시 다음 소스로
				}

				// 파일 내용을 임시로 저장할 리스트
				QList<QStringList> file_content;
				double total_weight_in_file = 0.0;

				QTextStream in(&inputFile);
				while (!in.atEnd()) {
					QString line = in.readLine();

					QStringList parts = line.simplified().split(' ');

					if (parts.size() >= 3) {
						file_content.append(parts);
						// weight는 두 번째 열(parts[1])
						total_weight_in_file += parts[1].toDouble();
					}
				}
				inputFile.close();

				// 5. 메모리에 저장된 파일 내용을 기반으로 GPS 명령어 생성
				for (const auto& line_parts : file_content)
				{
					std::string particle_type = line_parts[0].toStdString();
					// 요청하신 대로 weight와 energy 순서 변경
					double weight = line_parts[1].toDouble();
					double energy = line_parts[2].toDouble();

					// 현재 입자의 절대적인 초당 방출량(#/s) 계산
					double current_absolute_intensity = 0.0;
					if (total_weight_in_file > 0) {
						current_absolute_intensity = file_total_intensity * (weight / total_weight_in_file);
					}

					// 모든 소스를 통틀어 가장 첫 번째 입자인 경우
					if (is_first_particle_overall)
					{
						reference_absolute_intensity = current_absolute_intensity;

						// 기본 명령어 출력 (원형 면선원 형태)
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Plane" << std::endl;
						ofp_source << "/gps/pos/shape Circle" << std::endl;
						ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm" << std::endl;
						ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;

						is_first_particle_overall = false;
					}
					// 두 번째 입자부터는 /gps/source/add 명령어 추가
					else
					{
						double relative_intensity = 0.0;
						if (reference_absolute_intensity > 0) {
							relative_intensity = current_absolute_intensity / reference_absolute_intensity;
						}

						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << relative_intensity << std::endl;
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Plane" << std::endl;
						ofp_source << "/gps/pos/shape Circle" << std::endl;
						ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm" << std::endl;
						ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;
					}
				}
			}
		}

		
	}

	// Hot particle (HP)
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 5)
	{
		if (pRt->RI_Select_sourceHP_QRadioButton->isChecked())
		{
			bool is_first_RI = true;
			double first_RI_activity;
			bool is_first_radiation = true;
			double first_radiation_yield;
			double conversion_factor_sum_of_activity_yieldsum = 0;
			for (auto itr_info_RI : pRt->ListInfo_sourceHP) // 입력한 mulitple RI 마다 순환
			{
				std::string path = "./data/radioisotopes/" + itr_info_RI[4].toStdString() + ".txt";
				std::pair<std::vector<std::map<int, std::string>>, double> temp_pair = Read_RI_File(path);
				std::vector<std::map<int, std::string>> radionulclide_radiation_info = temp_pair.first;
				double this_RI_yieldsum = temp_pair.second;
				double this_RI_activity = itr_info_RI[5].toDouble();
				conversion_factor_sum_of_activity_yieldsum += this_RI_activity * this_RI_yieldsum; // activity concentration * yieldsum -> (NPS/s)
				if (is_first_RI) // 첫번째 RI 일때
				{
					first_RI_activity = this_RI_activity;
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						if (is_first_radiation) // 첫번째 RI, 첫번째 radiation 일때
						{
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Point" << endl;
							if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
							if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
							if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
							ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << itr_info_RI[1].toDouble() << " " << itr_info_RI[2].toDouble() << " " << itr_info_RI[3].toDouble() << " cm " << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
							first_radiation_yield = std::stod(itr_radiation[0]);
							is_first_radiation = false;
						}
						else // 첫번째 RI, 두번째 이상 radiation 일때
						{
							ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) << endl;
							ofp_source << "/gps/particle " << itr_radiation[2] << endl;
							ofp_source << "/gps/pos/type Point" << endl;
							if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
							if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
							if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
							ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << itr_info_RI[1].toDouble() << " " << itr_info_RI[2].toDouble() << " " << itr_info_RI[3].toDouble() << " cm " << endl;
							ofp_source << "/gps/ang/type iso" << endl;
							ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
						}
					}
					is_first_RI = false;
				}
				else // 두번째 이상 RI
				{
					for (auto itr_radiation : radionulclide_radiation_info) // 각 RI의 radiation 마다 순환
					{
						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << (std::stod(itr_radiation[0]) / first_radiation_yield) * (this_RI_activity / first_RI_activity) << endl;
						ofp_source << "/gps/particle " << itr_radiation[2] << endl;
						ofp_source << "/gps/pos/type Point" << endl;
						if (itr_info_RI[1].toStdString() == "") itr_info_RI[1] = "0";
						if (itr_info_RI[2].toStdString() == "") itr_info_RI[2] = "0";
						if (itr_info_RI[3].toStdString() == "") itr_info_RI[3] = "0";
						ofp_source << "/gps/pos/centre " << itr_info_RI[1].toStdString() << " " << itr_info_RI[2].toStdString() << " " << itr_info_RI[3].toStdString() << " cm " << endl;
						ofp_source << "/gps/ang/type iso" << endl;
						ofp_source << "/gps/energy " << itr_radiation[1] << " MeV" << endl << endl;
					}
				}
			}
			DoseConversionFactor_inUImodule = conversion_factor_sum_of_activity_yieldsum;
		}
		else if (pRt->ES_Select_sourceHP_QRadioButton->isChecked())
		{
			// 전체 입자 중 첫 번째 입자인지를 판별하는 플래그
			bool is_first_particle_overall = true;
			// 기준이 될 첫 번째 입자의 절대적인 초당 방출량(#/s)을 저장할 변수
			double reference_absolute_intensity;
			// 모든 소스의 Intensity(#/s) 총합을 저장.
			DoseConversionFactor_inUImodule = 0.0;

			// 사용자가 UI에 추가한 각 소스 정보(파일)를 순회합니다.
			for (auto itr_info_ES : pRt->ListInfo_sourceHP)
			{
				// 1. 현재 소스(파일)의 정보 파싱
				// 위치 좌표 (비어있으면 "0"으로 처리)
				if (itr_info_ES[1].toStdString() == "") itr_info_ES[1] = "0";
				if (itr_info_ES[2].toStdString() == "") itr_info_ES[2] = "0";
				if (itr_info_ES[3].toStdString() == "") itr_info_ES[3] = "0";
				double posX = itr_info_ES[1].toDouble();
				double posY = itr_info_ES[2].toDouble();
				double posZ = itr_info_ES[3].toDouble();

				// 파일 경로와 해당 파일의 전체 Intensity
				QString filePath = itr_info_ES[4];
				double file_total_intensity = itr_info_ES[5].toDouble();

				// 총 방출량 계산
				DoseConversionFactor_inUImodule += file_total_intensity;

				// 2. 파일 파싱 준비
				QFile inputFile(filePath);
				if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
					// 파일 열기 실패 시 이 파일은 건너뜀
					continue;
				}

				// 파일 내용을 임시로 저장할 리스트
				QList<QStringList> file_content;
				double total_weight_in_file = 0.0;

				QTextStream in(&inputFile);
				while (!in.atEnd()) {
					QString line = in.readLine();

					QStringList parts = line.simplified().split(' ');

					if (parts.size() >= 3) {
						file_content.append(parts);
						// weight는 두 번째 열(parts[1])
						total_weight_in_file += parts[1].toDouble();
					}
				}
				inputFile.close();

				// 3. Geant4 GPS 명령어 생성
				// 메모리에 저장된 파일 내용을 한 줄씩 처리
				for (const auto& line_parts : file_content)
				{
					std::string particle_type = line_parts[0].toStdString();
					double weight = line_parts[1].toDouble();
					double energy = line_parts[2].toDouble();

					// 현재 입자의 절대적인 초당 방출량(#/s) 계산
					double current_absolute_intensity = 0.0;
					if (total_weight_in_file > 0) {
						current_absolute_intensity = file_total_intensity * (weight / total_weight_in_file);
					}

					// 모든 소스를 통틀어 가장 첫 번째 입자인 경우
					if (is_first_particle_overall)
					{
						reference_absolute_intensity = current_absolute_intensity;

						// /gps/source/add 없이 기본 명령어만 출력
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Point" << std::endl;
						ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << posX << " " << posY << " " << posZ << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;

						is_first_particle_overall = false;
					}
					// 두 번째 입자부터는 기준 입자 대비 상대적 비율을 계산하여 /gps/source/add 명령어 추가
					else
					{
						double relative_intensity = 0.0;
						if (reference_absolute_intensity > 0) {
							relative_intensity = current_absolute_intensity / reference_absolute_intensity;
						}

						ofp_source << "/gps/source/add " << std::scientific << std::setprecision(6) << relative_intensity << std::endl;
						ofp_source << "/gps/particle " << particle_type << std::endl;
						ofp_source << "/gps/pos/type Point" << std::endl;
						ofp_source << std::scientific << std::setprecision(4) << "/gps/pos/centre " << posX << " " << posY << " " << posZ << " cm" << std::endl;
						ofp_source << "/gps/ang/type iso" << std::endl;
						ofp_source << "/gps/energy " << energy << " MeV" << std::endl << std::endl;
					}
				}
			}
		}
	}

	// Parallel beam (PB)
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 9)
	{
		double center_x = pRt->m_lineEditParallelBeamPointX->text().toDouble();
		double center_y = pRt->m_lineEditParallelBeamPointY->text().toDouble();
		double center_z = pRt->m_lineEditParallelBeamPointZ->text().toDouble();
		std::string center_x_str = pRt->m_lineEditParallelBeamPointX->text().toStdString();
		std::string center_y_str = pRt->m_lineEditParallelBeamPointY->text().toStdString();
		std::string center_z_str = pRt->m_lineEditParallelBeamPointZ->text().toStdString();
		if (center_x_str == "") center_x = 0;
		if (center_y_str == "") center_y = 0;
		if (center_z_str == "") center_z = 0;
		double radius = pRt->m_lineEditParallelBeamRadius->text().toDouble();

		double theta_rad = pRt->m_lineEditParallelBeamDirectionTheta->text().toDouble() * vtkMath::Pi() / 180.0;
		double phi_rad = pRt->m_lineEditParallelBeamDirectionPhi->text().toDouble() * vtkMath::Pi() / 180.0;

		// 1) 빔 진행방향 n(정규화)
		double nx = std::sin(theta_rad) * std::cos(phi_rad);
		double ny = std::sin(theta_rad) * std::sin(phi_rad);
		double nz = std::cos(theta_rad);
		// 정규화(수치 안정)
		{
			double nlen = std::sqrt(nx*nx + ny * ny + nz * nz);
			if (nlen < 1e-12) { nx = 0; ny = 0; nz = 1; }
			else { nx /= nlen; ny /= nlen; nz /= nlen; }
		}

		// 2) 평행빔 원판 좌표축 u, v 구성  (u ⟂ n, v = n × u) -> GPS에서 필요
		// n과 거의 평행하지 않은 보조벡터 a 선택
		double ax = 0, ay = 0, az = 1;
		if (std::fabs(nz) > 0.99) { ax = 1; ay = 0; az = 0; }

		// u = normalize(a × n)
		double ux = ay * nz - az * ny;
		double uy = az * nx - ax * nz;
		double uz = ax * ny - ay * nx;
		double ulen = std::sqrt(ux*ux + uy * uy + uz * uz);
		if (ulen < 1e-12) {
			// 극단적 정렬일 때 대체 a로 재시도
			ax = 0; ay = 1; az = 0;
			ux = ay * nz - az * ny;
			uy = az * nx - ax * nz;
			uz = ax * ny - ay * nx;
			ulen = std::sqrt(ux*ux + uy * uy + uz * uz);
		}
		ux /= ulen; uy /= ulen; uz /= ulen;

		// v = normalize(n × u)  (이론상 이미 단위길이지만 안전상 정규화)
		double vx = ny * uz - nz * uy;
		double vy = nz * ux - nx * uz;
		double vz = nx * uy - ny * ux;
		double vlen = std::sqrt(vx*vx + vy * vy + vz * vz);
		vx /= vlen; vy /= vlen; vz /= vlen;
		
		if (pRt->sourcePB_MonoEnergy_radioButton->isChecked()) // Mono Energy 형태일 때
		{
			ofp_source << "/gps/particle ";
			if (pRt->m_comboBoxParallelBeamParticleType->currentText() == "Photon") ofp_source << "gamma" << endl;
			else if (pRt->m_comboBoxParallelBeamParticleType->currentText() == "Electron") ofp_source << "e-" << endl;
			else if (pRt->m_comboBoxParallelBeamParticleType->currentText() == "Neutron") ofp_source << "neutron" << endl;
			else if (pRt->m_comboBoxParallelBeamParticleType->currentText() == "Proton") ofp_source << "proton" << endl;
			else if (pRt->m_comboBoxParallelBeamParticleType->currentText() == "Alpha") ofp_source << "alpha" << endl;
			ofp_source << "/gps/pos/type Plane" << endl;
			ofp_source << "/gps/pos/shape Circle" << endl;
			ofp_source << "/gps/pos/centre " << std::scientific << std::setprecision(4) << center_x << " " << center_y << " " << center_z << " cm " << endl;
			ofp_source << "/gps/pos/radius " << std::scientific << std::setprecision(4) << radius << " cm" << endl;

			ofp_source << "/gps/pos/rot1 " << ux << " " << uy << " " << uz << std::endl; // u
			ofp_source << "/gps/pos/rot2 " << vx << " " << vy << " " << vz << std::endl; // v
			ofp_source << "/gps/ang/type beam2d" << endl;
			ofp_source << "/gps/direction " << std::to_string(nx) << " " << std::to_string(ny) << " " << std::to_string(nz) << endl;
			ofp_source << "/gps/ang/sigma_x 0 deg" << endl;
			ofp_source << "/gps/ang/sigma_y 0 deg" << endl;
			ofp_source << "/gps/energy " << pRt->m_lineEditParallelBeamEnergy->text().toStdString() << " MeV" << endl;
		}
		DoseConversionFactor_inUImodule = pRt->m_lineEditParallelBeamIntensity_MonoEnergy->text().toDouble();
	}

	// Object volume, phase-space file, room air contamination (OV, PS, RC) -> 별도 generator	   		
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 3 || pRt->m_comboBoxSourceGeometry->currentIndex() == 4 || pRt->m_comboBoxSourceGeometry->currentIndex() == 7)
	{
		// Nothing happend in source macro ...
	}
	ofp_source.close();
}
void ETHuman3DApp::MakeFile_ListCollection()
{
	//List_collection 파일 생성 (부가 정보들 -> Wearables/Glasses/Dosimeter layer 존재 여부, Geometry object 정보, etc(외부환경 정보, 선량당량 위치)..)
	std::string list_collection = "./data/dbsend/list_collection";
	std::ofstream ofpList;
	ofpList.open(list_collection);

	// Object 정보
	ofpList << "o" << endl;
	for (auto itr_objectIndex : pRt->m_Object_SequenceVector)
	{
		if (pRt->m_Object_MainInfo[itr_objectIndex][13] == 0) pRt->m_Object_MaterialInfo[itr_objectIndex] = ""; // If density is 0, make it void
		if (pRt->m_ObjectType[itr_objectIndex] == 0) // 해당 GeomID가 Box 일때
		{
			ofpList << pRt->m_Object_ButtonName[itr_objectIndex].toStdString() << " Box " << pRt->m_Object_MainInfo[itr_objectIndex][1] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][2] << " " << pRt->m_Object_MainInfo[itr_objectIndex][3] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][4] << " " << pRt->m_Object_MainInfo[itr_objectIndex][5] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][6] << " " << pRt->m_Object_MainInfo[itr_objectIndex][7] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][8] << " " << pRt->m_Object_MainInfo[itr_objectIndex][9] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][10] << " " << pRt->m_Object_MainInfo[itr_objectIndex][11] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][12] << " " << pRt->m_Object_ChargedParticleTransport[itr_objectIndex] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][13] << " " << pRt->m_Object_MaterialFractionType[itr_objectIndex] << endl
				<< pRt->m_Object_MaterialInfo[itr_objectIndex].toStdString() << endl;
		}
		if (pRt->m_ObjectType[itr_objectIndex] == 1) // 해당 GeomID가 sphere 일때(즉, 길이가 존재할때)
		{
			ofpList << pRt->m_Object_ButtonName[itr_objectIndex].toStdString() << " Sphere " << pRt->m_Object_MainInfo[itr_objectIndex][1] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][2] << " " << pRt->m_Object_MainInfo[itr_objectIndex][3] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][4] << " " << pRt->m_Object_MainInfo[itr_objectIndex][5] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][6] << " " << pRt->m_Object_MainInfo[itr_objectIndex][14] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][15] << " " << pRt->m_Object_MainInfo[itr_objectIndex][16] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][17] << " " << pRt->m_Object_MainInfo[itr_objectIndex][18] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][19] << " " << pRt->m_Object_ChargedParticleTransport[itr_objectIndex] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][13] << " " << pRt->m_Object_MaterialFractionType[itr_objectIndex] << endl
				<< pRt->m_Object_MaterialInfo[itr_objectIndex].toStdString() << endl;
		}
		if (pRt->m_ObjectType[itr_objectIndex] == 2) // 해당 GeomID가 cylinder 일때(즉, 길이가 존재할때)
		{
			ofpList << pRt->m_Object_ButtonName[itr_objectIndex].toStdString() << " Cylinder " << pRt->m_Object_MainInfo[itr_objectIndex][1] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][2] << " " << pRt->m_Object_MainInfo[itr_objectIndex][3] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][4] << " " << pRt->m_Object_MainInfo[itr_objectIndex][5] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][6] << " " << pRt->m_Object_MainInfo[itr_objectIndex][20] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][21] << " " << pRt->m_Object_MainInfo[itr_objectIndex][22] << " "
				<< pRt->m_Object_MainInfo[itr_objectIndex][23] << " " << pRt->m_Object_MainInfo[itr_objectIndex][24] << " "
				<< pRt->m_Object_ChargedParticleTransport[itr_objectIndex] << " " << pRt->m_Object_MainInfo[itr_objectIndex][13] << " "
				<< pRt->m_Object_MaterialFractionType[itr_objectIndex] << endl
				<< pRt->m_Object_MaterialInfo[itr_objectIndex].toStdString() << endl;
		}
		ofpList << "^" << endl;
	}
	ofpList << "END" << endl;

	// Environment 정보 -> 환경매질 설정
	ofpList << endl << "e" << endl;
	if (pRt->Geometry_EnvironmentType_QComboBox->currentIndex() == 0) ofpList << "Vacuum " << 0 << endl;
	if (pRt->Geometry_EnvironmentType_QComboBox->currentIndex() == 1) ofpList << "Air " << 1 << endl;
	if (pRt->Geometry_EnvironmentType_QComboBox->currentIndex() == 2) ofpList << "Water " << 2 << endl;
	ofpList << "END" << endl;

	// Source type 정보
	ofpList << endl << "s" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 0) ofpList << "0" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 1) ofpList << "1" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 2) ofpList << "2" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 3) ofpList << "3" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 4) ofpList << "4" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 5) ofpList << "5" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 6) ofpList << "6" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 7) ofpList << "7" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 8) ofpList << "8" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 9) ofpList << "9" << endl;
	ofpList << "END" << endl;
	
	// Conversion Factor 정보(Geant4 상에서 후처리 필요없는 선원의 경우)
	ofpList << endl << "c" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 1 || pRt->m_comboBoxSourceGeometry->currentIndex() == 2 || pRt->m_comboBoxSourceGeometry->currentIndex() == 5 || pRt->m_comboBoxSourceGeometry->currentIndex() == 6 || pRt->m_comboBoxSourceGeometry->currentIndex() == 9)
	{
		ofpList << DoseConversionFactor_inUImodule << endl;
	}
	ofpList << "END" << endl;

	// OV 정보
	ofpList << endl << "v" << endl;
	for (auto itr_sourceOV_index : pRt->m_sourceOV_objectSequenceVector)
	{
		auto it = std::find(pRt->m_Object_SequenceVector.begin(), pRt->m_Object_SequenceVector.end(), itr_sourceOV_index);
		int index = std::distance(pRt->m_Object_SequenceVector.begin(), it);
		ofpList << index << " ";
		ofpList << pRt->sourceOV_objectBound[itr_sourceOV_index][0] << " " << pRt->sourceOV_objectBound[itr_sourceOV_index][1] << " "
			<< pRt->sourceOV_objectBound[itr_sourceOV_index][2] << " " << pRt->sourceOV_objectBound[itr_sourceOV_index][3] << " "
			<< pRt->sourceOV_objectBound[itr_sourceOV_index][4] << " " << pRt->sourceOV_objectBound[itr_sourceOV_index][5] << endl;
		for (auto itr_ListInfoVector : pRt->ListInfo_sourceOV[itr_sourceOV_index])
		{
			ofpList << itr_ListInfoVector[1].toStdString() << " " << itr_ListInfoVector[2].toStdString() << endl;
		}
		ofpList << "^" << endl;
	}
	ofpList << "END" << endl;

	// RC 정보
	ofpList << endl << "r" << endl;
	if (pRt->m_comboBoxSourceGeometry->currentIndex() == 7) // sourceRC 일때
	{
		double* bounds = SourcePanelActor_Position[0]->GetBounds();
		ofpList << bounds[0] << " " << bounds[1] << " " << bounds[2] << " " << bounds[3] << " " << bounds[4] << " " << bounds[5] << endl;
		for (auto itr_RIvector : pRt->RI_Info_sourceRC)
		{
			ofpList << itr_RIvector[1].toStdString() << " " << itr_RIvector[2].toStdString() << endl;
		}
	}
	ofpList << "END" << endl;

	ofpList.close();
}
void ETHuman3DApp::MakeFile_CalcCollection()
{
	std::ofstream ofp_calc("./data/dbsend/calc_collection");
	ofp_calc << "e" << endl; // target Organ error
	for (int i = 0; i < pRt->OrganNameList.size(); i++)
	{
		if (pRt->m_CalculationSetting_TargetError[i].isEmpty())
		{
			ofp_calc << setw(20) << std::left << "\t" + pRt->OrganNameList[i].toStdString() << setw(20) << "1.0";
		}
		else
		{
			ofp_calc << setw(20) << std::left << "\t" + pRt->OrganNameList[i].toStdString() << setw(20) <<
				pRt->m_CalculationSetting_TargetError[i].toDouble() / 100;
			pRt->TargetOrganError_Map[pRt->OrganNameList[i].toStdString()] = pRt->m_CalculationSetting_TargetError[i].toDouble() / 100;
		}
		if (i == pRt->OrganNameList.size() - 1) continue;
		else ofp_calc << endl;
	}

	// Target NPS
	ofp_calc << endl << endl << "n ";
	if (pRt->m_CalculationSetting_TargetNPS.toStdString() != "") ofp_calc << pRt->m_CalculationSetting_TargetNPS.toStdString() << endl;
	if (pRt->m_CalculationSetting_TargetNPS.toStdString() == "") ofp_calc << "None" << endl;

	// Number of thread
	ofp_calc << endl << "t ";
	if (pRt->m_CalculationSetting_Thread.toStdString() != "") ofp_calc << pRt->m_CalculationSetting_Thread.toStdString() << endl;
	if (pRt->m_CalculationSetting_Thread.toStdString() == "") ofp_calc << "None" << endl;

	// Skin Depth mode 설정
	ofp_calc << endl << "s ";
	if (pRt->SettingSkinDepthModeInfo == 0) ofp_calc << "false" << endl;
	if (pRt->SettingSkinDepthModeInfo == 1) ofp_calc << "true" << endl;

	// visualization mode 설정
	ofp_calc << endl << "v ";
	if (pRt->SettingVisualizationModeInfo == 0) ofp_calc << "false" << endl;
	if (pRt->SettingVisualizationModeInfo == 1) ofp_calc << "true" << endl;
	
	ofp_calc << endl << "r " << pRt->m_CalculationSetting_ResultFileName.toStdString() << endl;

	ofp_calc.close();
}
void ETHuman3DApp::Generate_MaterialFile()
{
	// MATERIAL 파일 생성 -> tet.ele / tet.node 와 함께 있는 tet.material 파일 생성	
	for (auto phantomIdx : pRt->m_Phantom_SequenceVector)
	{		
		if (pRt->m_Clothing_SequenceVector[phantomIdx].size() != 0)
		{
			// 의복의 .material 파일 생성	
			QString materialFile = "./result/" + pRt->m_CalculationSetting_ResultFileName + "/tmpdata/clothing_" + QString::number(phantomIdx) + ".material";// +".material" 나중에추가			
			QFile* outFile = new QFile(materialFile); // node 파일은 Imported 팬텀으로 인해 한글 경로가 포함될 수 있으므로, 한글을 지원하는 QFile로 우회

			QIODeviceOStream ofpNode(outFile);
			ofpNode.precision(18);

			for (auto clothingIdx : pRt->m_Clothing_SequenceVector[phantomIdx]) // 의복 순환
			{
				for (auto layerIdx : pRt->m_ClothingLayer_SequenceVector[phantomIdx][clothingIdx]) // 레이어순환
				{
					std::string name = "Layer_" + std::to_string(layerIdx);
					double density = pRt->m_Clothing_MainInfo[phantomIdx][clothingIdx][layerIdx][1]; ;
					double thickness = pRt->m_Clothing_MainInfo[phantomIdx][clothingIdx][layerIdx][0];
					if (thickness == 0)
					{
					} //해당 의복의 해당 레이어가 정의 되어 있지않다면 break
					else
					{
						ofpNode << "C  " << name << "  " << density << " g/cm3" << endl;
						ofpNode << "m" << phantomIdx * 1000000 + 20000 + (clothingIdx * 100) + layerIdx << "    ";

						if (pRt->m_Clothing_MainInfo[phantomIdx][clothingIdx][layerIdx][pRt->E_CLOTHINGMAININFO_COMPOSITION] == pRt->E_CLOTHINGMAININFO_COMPOSITION_CLOTH) // Material: Cloth
						{
							ofpNode << " 5000     -0.056" << endl;
							ofpNode << "          8000     -0.472" << endl;
							ofpNode << "          9000     -0.007" << endl;
							ofpNode << "         13000     -0.008" << endl;
							ofpNode << "         14000     -0.254" << endl;
							ofpNode << "         19000     -0.116" << endl;
							ofpNode << "         26000     -0.019" << endl;
							ofpNode << "         30000     -0.028" << endl;
							ofpNode << "         33000     -0.003" << endl;
							ofpNode << "         40000     -0.037" << endl;
							ofpNode << "C" << endl;
						}
						if (pRt->m_Clothing_MainInfo[phantomIdx][clothingIdx][layerIdx][pRt->E_CLOTHINGMAININFO_COMPOSITION] == pRt->E_CLOTHINGMAININFO_COMPOSITION_LEAD) // Material: Lead
						{
							ofpNode << "82000     -1" << endl;
							ofpNode << "C" << endl;
						}
						if (pRt->m_Clothing_MainInfo[phantomIdx][clothingIdx][layerIdx][pRt->E_CLOTHINGMAININFO_COMPOSITION] == pRt->E_CLOTHINGMAININFO_COMPOSITION_AIR) // Material: Air
						{
							ofpNode << "7000     -0.8" << endl;
							ofpNode << "         8000     -0.2" << endl;
							ofpNode << "C" << endl;
						}
					}
				}
			}
			// 파일 끝의 빈 줄 제거
			outFile->seek(outFile->size() - 1);
			outFile->close();
		}		
	}
}
void ETHuman3DApp::TranslatePhantomTetFile(int reset_phantomID) // called by (1) DataInitialization_Local
{
	int phantomID = pRt->m_Phantom_SequenceVector[reset_phantomID];

	double pCenterX = PhantomOrigianlPolyDataCenter[phantomID][0];
	double pCenterY = PhantomOrigianlPolyDataCenter[phantomID][1];
	double pCenterZ = PhantomOrigianlPolyDataCenter[phantomID][2];
	double PosX = pRt->m_Phantom_MainInfo[phantomID][6];
	double PosY = pRt->m_Phantom_MainInfo[phantomID][7];
	double PosZ = pRt->m_Phantom_MainInfo[phantomID][8];
	double RotX = pRt->m_Phantom_MainInfo[phantomID][9];
	double RotY = pRt->m_Phantom_MainInfo[phantomID][10];
	double RotZ = pRt->m_Phantom_MainInfo[phantomID][11];
	const double PI = 3.141592 / 180; // 라디안 변환 
	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;
	double ScaleX = PhantomPolyDataScaleFactor[phantomID][0];
	double ScaleY = PhantomPolyDataScaleFactor[phantomID][0];
	double ScaleZ = PhantomPolyDataScaleFactor[phantomID][1];
	
}
void ETHuman3DApp::TranslateClothingTetFile(int reset_phantomID) // called by (1) DataInitialization_Local
{	
	int phantomID = pRt->m_Phantom_SequenceVector[reset_phantomID];
	if (pRt->m_Clothing_SequenceVector[phantomID].size() == 0) return; // Clothing 하나도 없으면 빠져 나오기

	double pCenterX = PhantomOrigianlPolyDataCenter[phantomID][0];
	double pCenterY = PhantomOrigianlPolyDataCenter[phantomID][1];
	double pCenterZ = PhantomOrigianlPolyDataCenter[phantomID][2];
	double PosX = pRt->m_Phantom_MainInfo[phantomID][6];
	double PosY = pRt->m_Phantom_MainInfo[phantomID][7];
	double PosZ = pRt->m_Phantom_MainInfo[phantomID][8];
	double RotX = pRt->m_Phantom_MainInfo[phantomID][9];
	double RotY = pRt->m_Phantom_MainInfo[phantomID][10];
	double RotZ = pRt->m_Phantom_MainInfo[phantomID][11];
	const double PI = 3.141592 / 180; // 라디안 변환 
	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;
	// 의복은 Scale X
	//double ScaleX = PhantomPolyDataScaleFactor[phantomID][0];
	//double ScaleY = PhantomPolyDataScaleFactor[phantomID][0];
	//double ScaleZ = PhantomPolyDataScaleFactor[phantomID][1];

	// NODE TRANSFORMATION

	// Define the translation vectors
	Eigen::Vector3d translation1(-pCenterX, -pCenterY, -pCenterZ); // 0, 0, 0으로 맞추기
	Eigen::Vector3d translation2(PosX, PosY, PosZ);

	// Create the rotation matrices for each axis
	Eigen::Matrix3d rotation_x;
	rotation_x = Eigen::AngleAxisd(RadianX, Eigen::Vector3d::UnitX());

	Eigen::Matrix3d rotation_y;
	rotation_y = Eigen::AngleAxisd(RadianY, Eigen::Vector3d::UnitY());

	Eigen::Matrix3d rotation_z;
	rotation_z = Eigen::AngleAxisd(RadianZ, Eigen::Vector3d::UnitZ());

	// Combine the rotation matrices
	Eigen::Matrix3d rotation = rotation_z * rotation_x * rotation_y; //Rotation Z -> X -> Y

	// Create the scaling matrix
	//Eigen::DiagonalMatrix<double, 3> scaling(ScaleX, ScaleY, ScaleZ);

	// Create the combined transformation matrix
	Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
	transform.block<3, 3>(0, 0) = rotation; // 의복은 trasnform 및 rotation만!
	
	QString ifpFileName; // input node파일 이름	
	ifpFileName = "./data/wearable/Clothing_forTetra/clothing_tetra_" + QString::number(phantomID) + ".node"; // Windows 폴더에 맞게 설정 필요

	QFile* inputFile = new QFile(ifpFileName); // node 파일은 Imported 팬텀으로 인해 한글 경로가 포함될 수 있으므로, 한글을 지원하는 QFile로 우회

	QString ofpFileName = "./result/" + pRt->m_CalculationSetting_ResultFileName + "/tmpdata/clothing_" + QString::number(reset_phantomID) + ".node";
	QFile* outFile = new QFile(ofpFileName); // node 파일은 Imported 팬텀으로 인해 한글 경로가 포함될 수 있으므로, 한글을 지원하는 QFile로 우회
		
	QIODeviceIStream ifpNode(inputFile);
	ifpNode.precision(18);
	QIODeviceOStream ofpNode(outFile);
	ofpNode.precision(18);

	int numVertex;
	int tempInt;

	double xPos, yPos, zPos;
	Eigen::Vector4d point;

	ifpNode >> numVertex >> tempInt >> tempInt >> tempInt;
	ofpNode << numVertex << "  3  0  0" << std::endl;

	for (int i = 0; i < numVertex; i++) // This could be faster using PRAGMA_OMP
	{
		ifpNode >> tempInt >> xPos >> yPos >> zPos;
		point = Eigen::Vector4d(xPos, yPos, zPos, 1.0);

		point.head<3>() += translation1.head<3>(); // translation1(pCenter 맞추기)
		point = transform * point; // rot -> scaling
		point.head<3>() += translation2.head<3>(); // translation2(Pos 이동)

		ofpNode << "   " << i << "    " << point.x() << "  " << point.y() << "  " << point.z() << std::endl;
	}
	inputFile->close(); // istream은 우회하기 위한 buffer 객체이고, QFile을 닫으면 됨.
	outFile->close();

	// ele 파일 단순 복사(변화없음)
	QString ifpFileName_ele; 
	ifpFileName_ele = "./data/wearable/Clothing_forTetra/clothing_tetra_" + QString::number(phantomID) + ".ele";
	QString ofpFileName_ele = "./result/" + pRt->m_CalculationSetting_ResultFileName + "/tmpdata/clothing_" + QString::number(reset_phantomID) + ".ele";

	// 대상 파일이 존재하면 삭제 후 복사
	if (QFile::exists(ofpFileName_ele)) {
		QFile::remove(ofpFileName_ele);
	}
	QFile::copy(ifpFileName_ele, ofpFileName_ele);
}
void ETHuman3DApp::WearableTetrahedralization() // called by (1) DataInitialization_Local
{
	////////////////////////File deletion in output folder//////////////////////////
	for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector)
	{
		std::string wearablePath = "./data/dbsend/wearable_send/";
		
		// 이전에 생성된 의복, 선량계, 안경 사면체 파일 제거
		const char* file_to_delete;
		std::string GlassesEle = wearablePath + "glasses_" + std::to_string(itr_phantomIndex) + ".ele";
		std::string GlassesMaterial = wearablePath + "glasses_" + std::to_string(itr_phantomIndex) + ".material";
		std::string GlassesNode = wearablePath + "glasses_" + std::to_string(itr_phantomIndex) + ".node";
		std::string LayerEle = wearablePath + "layer_" + std::to_string(itr_phantomIndex) + ".ele";
		std::string LayerMaterial = wearablePath + "layer_" + std::to_string(itr_phantomIndex) + ".material";
		std::string LayerNode = wearablePath + "layer_" + std::to_string(itr_phantomIndex) + ".node";
		std::string LayerDosimeterEle = wearablePath + "layer_dosimeter_" + std::to_string(itr_phantomIndex) + ".ele";
		std::string LayerDosimeterMaterial = wearablePath + "layer_dosimeter_" + std::to_string(itr_phantomIndex) + ".material";
		std::string LayerDosimeterNode = wearablePath + "layer_dosimeter_" + std::to_string(itr_phantomIndex) + ".node";
		std::string LayerDosimeterSideEle = wearablePath + "layer_dosimeter_side_" + std::to_string(itr_phantomIndex) + ".ele";
		std::string LayerDosimeterSideMaterial = wearablePath + "layer_dosimeter_side_" + std::to_string(itr_phantomIndex) + ".material";
		std::string LayerDosimeterSideNode = wearablePath + "layer_dosimeter_side_" + std::to_string(itr_phantomIndex) + ".node";
		file_to_delete = GlassesEle.c_str();	remove(file_to_delete);
		file_to_delete = GlassesMaterial.c_str();	remove(file_to_delete);
		file_to_delete = GlassesNode.c_str();	remove(file_to_delete);
		file_to_delete = LayerEle.c_str();	remove(file_to_delete);
		file_to_delete = LayerMaterial.c_str();	remove(file_to_delete);
		file_to_delete = LayerNode.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterEle.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterMaterial.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterNode.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterSideEle.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterSideMaterial.c_str();	remove(file_to_delete);
		file_to_delete = LayerDosimeterSideNode.c_str();	remove(file_to_delete);
	}

	for (auto itr_phantomIndex : pRt->m_Phantom_SequenceVector)
	{
		//사면체화에 사용될 points와 faces
		std::map<int, std::map<int, std::vector<Point_3>>> points_clothing;
		std::map<int, std::map<int, std::vector<Point_3>>> faces_clothing;
		std::map<int, std::map<int, std::vector<Point_3>>> points_dosimeter;
		std::map<int, std::map<int, std::vector<Point_3>>> faces_dosimeter;
		std::map<int, std::map<int, std::vector<Point_3>>> points_dosimeter_side;
		std::map<int, std::map<int, std::vector<Point_3>>> faces_dosimeter_side;

		// 단일 팬텀에 대하여, 사전제작 의복에 대하여
		std::map<int, std::ifstream*> BaseLayer_clothing; // BaseLayer[의복ID][layerIdx] = 각 의복의 base layer가 될 obj파일		
		std::map<int, std::ifstream*> BaseLayer_dosimeter; // BaseLayer[선량계ID] = 각 선량계 base layer가 될 obj파일
		std::map<int, std::ifstream*> BaseLayer_dosimeter_side; // BaseLayer[선량계ID] = 각 선량계 side의 base layer가 될 obj파일

		std::map<int, Polyhedron_SK> first_clothing; // 첫번째 레이어이자 그 다음 레이어들(update)의 base layer
		std::map<int, Polyhedron_SK> original_clothing; // 원본(첫번째 레이어의 base layer)
		std::map<int, Polyhedron_SK> update_clothing; // 첫번째 이후 레이어들(update)
		std::map<int, Polyhedron_SK> first_dosimeter; // 첫번째 레이어이자 그 다음 레이어들(update)의 base layer
		std::map<int, Polyhedron_SK> original_dosimeter; // 원본(첫번째 레이어의 base layer)
		std::map<int, Polyhedron_SK> update_dosimeter; // 첫번째 이후 레이어들(update)
		std::map<int, Polyhedron_SK> first_dosimeter_side; // 첫번째 레이어이자 그 다음 레이어들(update)의 base layer
		std::map<int, Polyhedron_SK> original_dosimeter_side; // 원본(첫번째 레이어의 base layer)
		std::map<int, Polyhedron_SK> update_dosimeter_side; // 첫번째 이후 레이어들(update)

		// 사면체화할 base polygon 생성		
		for (auto itr_clothingIndex : theApp.pRt->m_Clothing_SequenceVector[itr_phantomIndex]) // 의복 순환
		{
			BaseLayer_clothing[itr_clothingIndex] = new std::ifstream; // 기본 base layer 설정(0번째 layer)
			BaseLayer_clothing[itr_clothingIndex]->open("./data/wearable/Clothing_forTetra/" + std::to_string(itr_phantomIndex) + "_" + std::to_string(itr_clothingIndex) + ".obj"); // 기본으로 offset 하지 않은 original layer 사용			
		}

		////선량계
		//for (int dosim_idx = 0; dosim_idx < theApp.pRt->m_Clothing_MainInfo[itr_phantomIndex][999][0][0]; dosim_idx++) // dosimeter 순환
		//{
		//	vtkSmartPointer<vtkPolyData> PolydataOffsetDosimeter;
		//	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		//	QString inputDosimeterPath = "./data/wearable/Dosimeter/" + QString::number(itr_phantomIndex) + "_" + QString::number(dosim_idx) + ".obj"; // Separation distance + 0.0001 cm (1 um)만큼 떨어진 팬텀에서부터 만들어진 선량계
		//	reader->SetFileName(Util::Wcs_to_mbs(inputDosimeterPath.toStdWString()).c_str());
		//	reader->Update();
		//	PolydataOffsetDosimeter = Util::CreatePolyData(reader);
		//	CleaningPolyData(PolydataOffsetDosimeter, "./data/wearable/Dosimeter/" + std::to_string(itr_phantomIndex) + "_" + std::to_string(dosim_idx) + ".obj");
		//	QString qPath;
		//	qPath = "./data/wearable/Dosimeter/" + QString::number(itr_phantomIndex) + "_" + QString::number(dosim_idx) + ".obj";
		//	std::string stdPath = qPath.toStdString();
		//	BaseLayer_dosimeter[dosim_idx] = new ifstream;
		//	BaseLayer_dosimeter[dosim_idx]->open(stdPath);
		//}
		//for (int dosim_idx = 0; dosim_idx < theApp.pRt->m_Clothing_MainInfo[itr_phantomIndex][999][0][0]; dosim_idx++) // dosimeter side 순환
		//{	// 사면체화용 offset한(0.0001 cm + separation distance) 팬텀 polydata로부터 만든 선량계를 이용하여 side dosimeter 생성
		//	GenerateSideDosimeter("./data/wearable/Dosimeter/" + QString::number(itr_phantomIndex) + "_" + QString::number(dosim_idx) + ".obj", itr_phantomIndex, 0.0001 + theApp.pRt->m_Clothing_MainInfo[itr_phantomIndex][1000][dosim_idx][2], "./data/wearable/Dosimeter/" + QString::number(itr_phantomIndex) + "_" + QString::number(dosim_idx) + "_side.obj");
		//	QString qPath;
		//	qPath = "./data/wearable/Dosimeter/" + QString::number(itr_phantomIndex) + "_" + QString::number(dosim_idx) + "_side.obj";
		//	std::string stdPath = qPath.toStdString();
		//	BaseLayer_dosimeter_side[dosim_idx] = new ifstream;
		//	BaseLayer_dosimeter_side[dosim_idx]->open(stdPath);
		//}

		// 사면체화할 CGAL polygon 레이어별로 생성
		std::string Name;
		std::string Material;
		int Layer;
		double Distance;
		double Radius;
		double Thickness;
		double glass_thickness;
		double Density;
		std::string dump;
		int matID;

		CGAL_LayerDef* LAYERDEF = new CGAL_LayerDef;
		for (auto itr_clothingIndex : theApp.pRt->m_Clothing_SequenceVector[itr_phantomIndex]) // 의복 순환
		{
			int initial_selfintersections = 0;
			for (int LayerIdx = 0; LayerIdx < pRt->const_ClothingLayer_MaximumCount; LayerIdx++) // 레이어순환
			{
				Thickness = pRt->m_Clothing_MainInfo[itr_phantomIndex][itr_clothingIndex][LayerIdx][pRt->E_CLOTHINGMAININFO_THICKNESS];
				if (Thickness == 0)
				{
				}
				else // 두께 존재 할때
				{					
					matID = itr_phantomIndex * 1000000 + 20000 + (itr_clothingIndex * 100) + LayerIdx; // 2/01/00 -> 0번팬텀(첫번째 팬텀)의 1번 wearable의 0번(첫번째) layer(layerIdx=0)	
					if (LayerIdx == 0)
					{
						BaseLayer_clothing[itr_clothingIndex] = new std::ifstream("./data/wearable/Clothing_forTetra/" + std::to_string(itr_phantomIndex) + "_" + std::to_string(itr_clothingIndex) + ".obj"); // 기본 base layer 설정(0번째 layer)		
						LAYERDEF->OBJ_to_Polyhedron(true, first_clothing[itr_clothingIndex], original_clothing[itr_clothingIndex], update_clothing[itr_clothingIndex], *BaseLayer_clothing[itr_clothingIndex], Thickness, initial_selfintersections, false, 0);
						BaseLayer_clothing[itr_clothingIndex]->close();
						LAYERDEF->tetra_prepare(true, matID - LayerIdx, LayerIdx, points_clothing, faces_clothing, first_clothing[itr_clothingIndex], original_clothing[itr_clothingIndex], update_clothing[itr_clothingIndex]);
					}
					else
					{
						std::ifstream null_stream; // 0번째 레이어가 아니면 사용하지 않음(그냥 input parameter로 쓰는 용도)
						LAYERDEF->OBJ_to_Polyhedron(false, first_clothing[itr_clothingIndex], original_clothing[itr_clothingIndex], update_clothing[itr_clothingIndex], null_stream, Thickness, initial_selfintersections, false, 0);
						LAYERDEF->tetra_prepare(false, matID - LayerIdx, LayerIdx, points_clothing, faces_clothing, first_clothing[itr_clothingIndex], original_clothing[itr_clothingIndex], update_clothing[itr_clothingIndex]);
					}
				}
			}
		}

		//for (int DosimeterIdx = 0; DosimeterIdx < theApp.pRt->m_Clothing_MainInfo[itr_phantomIndex][999][0][0]; DosimeterIdx++) // dosimeter 순환
		//{
		//	if (theApp.pRt->m_Clothing_MainInfo[itr_phantomIndex][1000][DosimeterIdx][1] == 0) continue;
		//	// 사용자 지정 original dosimeter 사면체화
		//	int initial_selfintersections = 0;
		//	for (int DosimeterLayer = 0; DosimeterLayer < 7; DosimeterLayer++) // 총 7 개 레이어로 이루어짐
		//	{
		//		matID = itr_phantomIndex * 1000000 + 30000 + (DosimeterIdx * 100) + DosimeterLayer;
		//		// 3/01/00 -> 0: 1번 dosimeter의 soft tissue 부분, 1: Hp(10) 계측부, 2: soft tissue, 3: Hp(3) 계측부, 4: soft tissue, 5: Hp(0.07) 계측부, 6: soft tissue
		//		double Thickness[7];
		//		Thickness[0] = 0.4; Thickness[1] = 0.1; Thickness[2] = 0.695; Thickness[3] = 0.005; Thickness[4] = 0.292; Thickness[5] = 0.001; Thickness[6] = 0.007;
		//		// 0: 4mm, 1: 1mm, 2: 6.95 mm, 3: 0.05mm, 4: 2.92mm, 5: 0.01mm, 6: 0.07mm
		//		if (DosimeterLayer == 0)
		//		{
		//			LAYERDEF->OBJ_to_Polyhedron(true, first_dosimeter[DosimeterIdx], original_dosimeter[DosimeterIdx], update_dosimeter[DosimeterIdx], *BaseLayer_dosimeter[DosimeterIdx], Thickness[DosimeterLayer], initial_selfintersections, false, 0);
		//			LAYERDEF->tetra_prepare(true, matID - DosimeterLayer, DosimeterLayer, points_dosimeter, faces_dosimeter, first_dosimeter[DosimeterIdx], original_dosimeter[DosimeterIdx], update_dosimeter[DosimeterIdx]);
		//		}
		//		else
		//		{
		//			LAYERDEF->OBJ_to_Polyhedron(false, first_dosimeter[DosimeterIdx], original_dosimeter[DosimeterIdx], update_dosimeter[DosimeterIdx], *BaseLayer_dosimeter[DosimeterIdx], Thickness[DosimeterLayer], initial_selfintersections, false, 0);
		//			LAYERDEF->tetra_prepare(false, matID - DosimeterLayer, DosimeterLayer, points_dosimeter, faces_dosimeter, first_dosimeter[DosimeterIdx], original_dosimeter[DosimeterIdx], update_dosimeter[DosimeterIdx]);
		//		}
		//	}
		//	// 주변 side dosimeter 사면체화
		//	initial_selfintersections = 0;
		//	for (int DosimeterLayer = 0; DosimeterLayer < 1; DosimeterLayer++) // 1개 15 mm soft tissue layer
		//	{
		//		double Thickness[1];
		//		Thickness[0] = 1.5;

		//		matID = itr_phantomIndex * 1000000 + 30000 + (DosimeterIdx * 100) + 9 + DosimeterLayer; // 3/01/10 -> Side dosimeter
		//		LAYERDEF->OBJ_to_Polyhedron(true, first_dosimeter_side[DosimeterIdx], original_dosimeter_side[DosimeterIdx], update_dosimeter_side[DosimeterIdx], *BaseLayer_dosimeter_side[DosimeterIdx], Thickness[DosimeterLayer], initial_selfintersections, false, 0);
		//		LAYERDEF->tetra_prepare(true, matID - DosimeterLayer, DosimeterLayer, points_dosimeter_side, faces_dosimeter_side, first_dosimeter_side[DosimeterIdx], original_dosimeter_side[DosimeterIdx], update_dosimeter_side[DosimeterIdx]);
		//	}
		//}

		// CGAL polygon 층별로 이어서 사면체화 진행
		LAYERDEF->tetrahedralization(itr_phantomIndex, points_clothing, faces_clothing, 0); //tetraPrepare로 모든 의복 및 여러 겹의 레이어로 최종 생성된 points 및 faces 정보로 의복 (0) 사면체화 진행 
		//LAYERDEF->tetrahedralization(itr_phantomIndex, points_dosimeter, faces_dosimeter, 3); //tetraPrepare로 여러 겹의 레이어로 최종 생성된 points 및 faces 정보로 선량계 (1) 사면체화 진행
		//LAYERDEF->tetrahedralization(itr_phantomIndex, points_dosimeter_side, faces_dosimeter_side, 4); //tetraPrepare로 여러 겹의 레이어로 최종 생성된 points 및 faces 정보로 선량계_side (2) 사면체화 진행

		delete LAYERDEF;
	}
}

//// Sub-function (1)
void ETHuman3DApp::OffsetFromObj(QString inputPath, QString outputPath, double offset) // called by (1) WearableTetrahedralization
{
	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
	reader->SetFileName(Util::Wcs_to_mbs(inputPath.toStdWString()).c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> InputPolydata = vtkSmartPointer<vtkPolyData>::New();
	InputPolydata = Util::CreatePolyData(reader);

	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(InputPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	for (vtkIdType i = 0; i < InputPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		InputPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offset;
		p[1] = p[1] + n[1] * offset;
		p[2] = p[2] + n[2] * offset;
		InputPolydata->GetPoints()->SetPoint(i, p);
	}
	InputPolydata->Modified();

	vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
	writer->SetFileName(Util::Wcs_to_mbs(outputPath.toStdWString()).c_str());
	writer->SetInputData(InputPolydata);
	writer->Update();
}
void ETHuman3DApp::GenerateClothingLayerOverlappedWithDosimeter(int phantomIdx, int WearableIdx, int dosimeterIdx, std::map<int, std::map<int, std::vector<std::tuple<int, int, double>>>> &sorted_overlapped_dosimeter_info) // called by (1) WearableTetrahedralization
{
	vtkSmartPointer<vtkPolyData> PolydataClothing = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> uniquePoints_Clothing = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> uniqueCells_Clothing = vtkSmartPointer<vtkCellArray>::New();

	// Clothing과 Dosimeter의 겹치는 부분 제거하고 나머지만 남김(Clothing - Dosimeter)
	for (vtkIdType i = 0; i < OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetNumberOfCells(); i++) {
		bool isUnique = true;
		for (vtkIdType j = 0; j < OriginalDosimeterPolydata_forTetra[phantomIdx][dosimeterIdx]->GetNumberOfCells(); j++) {
			if (areCellsEquivalent(OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx], i, OriginalDosimeterPolydata_forTetra[phantomIdx][dosimeterIdx], j, 0.001)) { // 마지막 항은 tolerance
				isUnique = false; // matched 되는 동일한 cell 이 존재 -> unique 하지 않다
				break;
			}
		}
		if (isUnique) {
			vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
			OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetCellPoints(i, cellPointIds);
			vtkIdType newIds[3];
			for (int k = 0; k < 3; k++) {
				newIds[k] = uniquePoints_Clothing->InsertNextPoint(OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetPoint(cellPointIds->GetId(k)));
			}
			uniqueCells_Clothing->InsertNextCell(3, newIds);
		}
	}
	PolydataClothing->SetPoints(uniquePoints_Clothing);
	PolydataClothing->SetPolys(uniqueCells_Clothing); // 겹치지 않는 부분만 남긴 의복

	vtkSmartPointer<vtkPolyData> PolydataClothingOverlapped = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> uniquePoints_ClothingOverlapped = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> uniqueCells_ClothingOverlapped = vtkSmartPointer<vtkCellArray>::New();
	// Clothing과 Dosimeter의 겹치는 부분만 남김(Clothing 교집합 Dosimeter) 
	for (vtkIdType i = 0; i < OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetNumberOfCells(); i++) {
		bool isUnique = true;
		for (vtkIdType j = 0; j < OriginalDosimeterPolydata_forTetra[phantomIdx][dosimeterIdx]->GetNumberOfCells(); j++) {
			if (areCellsEquivalent(OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx], i, OriginalDosimeterPolydata_forTetra[phantomIdx][dosimeterIdx], j, 0.001)) { // 마지막 항은 tolerance
				isUnique = false; // matched 되는 동일한 cell 이 존재 -> unique 하지 않다
				break;
			}
		}
		if (!isUnique) {// unique하지 않은 겹치는 부분
			vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
			OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetCellPoints(i, cellPointIds);
			vtkIdType newIds[3];
			for (int k = 0; k < 3; k++) {
				newIds[k] = uniquePoints_ClothingOverlapped->InsertNextPoint(OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetPoint(cellPointIds->GetId(k)));
			}
			uniqueCells_ClothingOverlapped->InsertNextCell(3, newIds);
		}
	}
	PolydataClothingOverlapped->SetPoints(uniquePoints_ClothingOverlapped);
	PolydataClothingOverlapped->SetPolys(uniqueCells_ClothingOverlapped); // 겹치는 부분만 남긴 의복

	bool IsOverlapped = false;
	if (PolydataClothingOverlapped->GetPoints()->GetNumberOfPoints() != 0) // 선량계와 의복이 겹치는 것이 있음
	{
		theApp.SetMessageBox("Start!");
		double thickness = 0;
		for (int layerIdx = 0; layerIdx < theApp.pRt->const_ClothingLayer_MaximumCount; layerIdx++)
		{
			thickness += theApp.pRt->m_Clothing_MainInfo[phantomIdx][WearableIdx][layerIdx][0]; // 피부로부터 현재 의복레이어 표면의 깊이(즉, 레이어들의 누적 두께)
			theApp.SetMessageBox("Cummulated Thickness: " + QString::number(thickness));
			theApp.SetMessageBox("Layer Thickness: " + QString::number(theApp.pRt->m_Clothing_MainInfo[phantomIdx][WearableIdx][layerIdx][0]));
			theApp.SetMessageBox("Separation distance: " + QString::number(theApp.pRt->m_Clothing_MainInfo[phantomIdx][1000][dosimeterIdx][2]));
			if (thickness > theApp.pRt->m_Clothing_MainInfo[phantomIdx][1000][dosimeterIdx][2])
			{
				// 선량계가 처음으로 겹치는 깊이
				sorted_overlapped_dosimeter_info[phantomIdx][WearableIdx].push_back(std::make_tuple(dosimeterIdx, layerIdx, thickness - theApp.pRt->m_Clothing_MainInfo[phantomIdx][WearableIdx][layerIdx][0]));  // 피부로부터 가까운 순서대로 겹치는 선량계 인덱스 저장
				IsOverlapped = true;
				theApp.SetMessageBox(QString::number(WearableIdx) + "_clothing is overlapped with " + QString::number(dosimeterIdx) + "_dosimeter");
				break;
			}
		}
	}
	if (IsOverlapped == false) // 의복과 겹치는 선량계는 존재하지만, 해당 선량계와 의복의 깊이가 겹치지 않을 때
	{
		theApp.SetMessageBox(QString::number(WearableIdx) + "_clothing is not overlapped with any dosimeters");
		return; // 의복 영역은 겹치지만 선량계가 의복 위에 있으면 겹치지 않으므로 return
	}

	// Offset할 좌표 설정
	std::vector<vtkIdType> overlappingPointIds; // Original dosimeter와 겹치는 점들
	std::vector<vtkIdType> overlappingPointIds_side; // 완만하게 offset 하기 위한 side 부분의 점들
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputData(OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]);
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->Update();
	vtkSmartPointer<vtkPolyData> wearablePolyDataWithNormals = normalGenerator->GetOutput();
	// Create a KD-tree for fast spatial searching on PolydataClothingOverlapped
	vtkSmartPointer<vtkKdTreePointLocator> kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
	kdTree->SetDataSet(PolydataClothingOverlapped);
	kdTree->BuildLocator();
	// Tolerance for point coincidence
	double tolerance = 0.005;
	// Iterate through all points in OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx] and find corresponding points in PolydataClothingOverlapped
	for (vtkIdType i = 0; i < wearablePolyDataWithNormals->GetNumberOfPoints(); ++i) {
		double p[3];
		wearablePolyDataWithNormals->GetPoint(i, p);

		// Find the closest point in PolydataClothingOverlapped
		double dist;
		vtkIdType id = kdTree->FindClosestPointWithinRadius(tolerance, p, dist);

		if (id >= 0) {
			// Point is overlapping, add it to the list
			overlappingPointIds.push_back(i);
		}
	}

	// 선량계와 겹치는 의복의 각 점으로부터 전체 의복의 1겹의 마진 폴리데이터 생성
	vtkSmartPointer<vtkPoints> ClothingOverlappedPoints = PolydataClothingOverlapped->GetPoints();
	vtkSmartPointer<vtkPoints> OriginalClothingPoints = OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetPoints();
	vtkSmartPointer<vtkCellArray> CellsWithinMargin = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	for (int pointID = 0; pointID < ClothingOverlappedPoints->GetNumberOfPoints(); pointID++)
	{
		double PickedPoint[3]; // origianlDosimeterPoints의 모든 point를 돌면서 얻은 각 point
		ClothingOverlappedPoints->GetPoint(pointID, PickedPoint); // 3D Pick 및 location update 통해 받아오는 좌표 ID		
		for (vtkIdType i = 0; i < OriginalClothingPoints->GetNumberOfPoints(); i++)
		{
			// Check if the point is within radius of the specified point
			double dist = vtkMath::Distance2BetweenPoints(OriginalClothingPoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
			double radius = 0.001; // 1겹의 마진만 되도록 최소한의 길이
			if (dist <= radius * radius) // 
			{
				vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
				OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
				for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
				{
					vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
					OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx]->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
					CellsWithinMargin->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
				}
			}
		}
	}
	vtkSmartPointer<vtkPolyData> PolyDataWithinMargin = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	PolyDataWithinMargin->SetPoints(OriginalClothingPoints);
	PolyDataWithinMargin->SetPolys(CellsWithinMargin);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_SideIncluded;
	connectivityFilter_SideIncluded->SetInputData(PolyDataWithinMargin);
	connectivityFilter_SideIncluded->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_SideIncluded->Update();
	vtkSmartPointer<vtkPolyData> PolydataSideIncluded = vtkSmartPointer<vtkPolyData>::New();
	PolydataSideIncluded = connectivityFilter_SideIncluded->GetOutput(); // Side를 포함한 dosimeter
	vtkSmartPointer<vtkPolyData> PolydataSide = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> uniquePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> uniqueCells = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType i = 0; i < PolydataSideIncluded->GetNumberOfCells(); i++) {
		bool isUnique = true;
		for (vtkIdType j = 0; j < PolydataClothingOverlapped->GetNumberOfCells(); j++) {
			if (areCellsEquivalent(PolydataSideIncluded, i, PolydataClothingOverlapped, j, 0.001)) { // 마지막 항은 tolerance
				isUnique = false; // matched 되는 동일한 cell 이 존재 -> unique 하지 않다
				break;
			}
		}
		if (isUnique) {
			vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
			PolydataSideIncluded->GetCellPoints(i, cellPointIds);
			vtkIdType newIds[3];
			for (int k = 0; k < 3; k++) {
				newIds[k] = uniquePoints->InsertNextPoint(PolydataSideIncluded->GetPoint(cellPointIds->GetId(k)));
			}
			uniqueCells->InsertNextCell(3, newIds);
		}
	}
	PolydataSide->SetPoints(uniquePoints);
	PolydataSide->SetPolys(uniqueCells);//결과적으로 사용하는 선량계_side polydata 
	QString OutputPath = "./data/wearable/Clothing_forTetra/" + QString::number(phantomIdx) + "_" + QString::number(WearableIdx) + "_clothing_side.obj";
	CleaningPolyData(PolydataSide, OutputPath.toStdString()); // Side 부분만 있는 polydata 완성 -> 마진 vertex만 뽑기위함

	// Importing cleaned polydata
	vtkSmartPointer<vtkOBJReader> reader_side = vtkSmartPointer<vtkOBJReader>::New();
	QString sideQstr = "./data/wearable/Clothing_forTetra/" + QString::number(phantomIdx) + "_" + QString::number(WearableIdx) + "_clothing_side.obj";
	reader_side->SetFileName(Util::Wcs_to_mbs(sideQstr.toStdWString()).c_str());
	reader_side->Update();
	vtkSmartPointer<vtkPolyData> CleanedPolydataSide = vtkSmartPointer<vtkPolyData>::New(); // 해당 선량계는 polydata_base와 좌표 평행이동 위치가 맞음 (따라서 추후 사면체화 후, node 정렬할때 pCenter 원점이동 거침)
	CleanedPolydataSide = Util::CreatePolyData(reader_side);
	// Create a KD-tree for fast spatial searching on PolydataClothingOverlapped
	vtkSmartPointer<vtkKdTreePointLocator> kdTree_side = vtkSmartPointer<vtkKdTreePointLocator>::New();
	kdTree_side->SetDataSet(CleanedPolydataSide);
	kdTree_side->BuildLocator();
	// Iterate through all points in OriginalClothingPolydata_forTetra[phantomIdx][clothingIdx] and find corresponding points in PolydataClothingOverlapped
	for (vtkIdType i = 0; i < wearablePolyDataWithNormals->GetNumberOfPoints(); ++i) {
		double p[3]; // Origianl Clothing을 순환하는 점
		wearablePolyDataWithNormals->GetPoint(i, p);

		// Find the closest point in CleanedPolydataSide
		double dist;
		vtkIdType id = kdTree_side->FindClosestPointWithinRadius(tolerance, p, dist); // Origianl clothing의 점 p와 side polydata와 겹치는점 판단

		if (id >= 0) {
			// Point is overlapping
			if (kdTree->FindClosestPointWithinRadius(tolerance, p, dist) < 0) // 겹치는 해당 점이 overlapped polydata와 겹치는 점이 아닌 side margin 이기만 한 점만 추가 (음수로 나오면 겹치지 않는 다는 것)
			{
				overlappingPointIds_side.push_back(i);
			}
		}
	}
	int currentSortedDosimInfoIndex = sorted_overlapped_dosimeter_info[phantomIdx][WearableIdx].size() - 1;
	OffsetPointsByNormals(wearablePolyDataWithNormals, overlappingPointIds, 0.0001 + 1.5 - std::get<2>(SortedOverlappedDosimeterInfo[phantomIdx][WearableIdx][currentSortedDosimInfoIndex])); // 0.0001 (1 um 마진) + 1.5 (15 mm 선량계 두께)
	OffsetPointsByNormals(wearablePolyDataWithNormals, overlappingPointIds_side, (0.0001 + 1.5 - std::get<2>(SortedOverlappedDosimeterInfo[phantomIdx][WearableIdx][currentSortedDosimInfoIndex])) * 0.5); // 외곽의 점들은 절반만 offset
	CleaningPolyData(wearablePolyDataWithNormals, "./data/wearable/Clothing_forTetra/" + std::to_string(phantomIdx) + "_" + std::to_string(WearableIdx) + "_" + std::to_string(dosimeterIdx) + ".obj");

	OriginalClothingPolydata_forTetra[phantomIdx][WearableIdx] = wearablePolyDataWithNormals;
}
void ETHuman3DApp::GenerateSideDosimeter(QString inputOrigianlDosimeterPath, int phantomIdx, double offset, QString OutputPath) // called by (1) WearableTetrahedralization
{
	vtkSmartPointer<vtkPolyData> PolydataOriginalDosimeter;
	vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
	reader->SetFileName(Util::Wcs_to_mbs(inputOrigianlDosimeterPath.toStdWString()).c_str());
	reader->Update();
	vtkSmartPointer<vtkPolyData> inputPolyData = vtkSmartPointer<vtkPolyData>::New();
	PolydataOriginalDosimeter = Util::CreatePolyData(reader);
	// Offset한 팬텀 polydata 생성
	vtkSmartPointer<vtkPolyData> PhantomPolydata;
	// Move phantom base polydata to origianl position (즉, pCenter 0,0,0 맞추기 전으로 돌리기)
	vtkSmartPointer<vtkTransform> transform_p = vtkSmartPointer<vtkTransform>::New();
	transform_p->PostMultiply(); //this is the key line
	transform_p->Translate(m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[0], m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[1], m_3DHumanData_MultiplePhantom[phantomIdx].pCenterOriginal[2]);
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter_p = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter_p->SetInputData(m_3DHumanData_MultiplePhantom[phantomIdx].polydata_base);
	transformFilter_p->SetTransform(transform_p);
	transformFilter_p->Update();
	PhantomPolydata = transformFilter_p->GetOutput();
	// Phantom offset -> 팬텀 obj로부터 불러온 polydata(pCenter 0,0,0 안 맞춘 데이터)를 선량계 offset한 만큼 offset 진행
	vtkSmartPointer<vtkPolyDataNormals> normalsFilter = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalsFilter->SetInputData(PhantomPolydata);
	normalsFilter->ComputePointNormalsOn();
	normalsFilter->ComputeCellNormalsOff();
	normalsFilter->Update();
	vtkDataArray* normalData = normalsFilter->GetOutput()->GetPointData()->GetNormals();
	for (vtkIdType i = 0; i < PhantomPolydata->GetNumberOfPoints(); i++)
	{
		double p[3];
		double n[3];
		PhantomPolydata->GetPoint(i, p);
		normalData->GetTuple(i, n);
		p[0] = p[0] + n[0] * offset;
		p[1] = p[1] + n[1] * offset;
		p[2] = p[2] + n[2] * offset;
		PhantomPolydata->GetPoints()->SetPoint(i, p);
	}
	PhantomPolydata->Modified();
	// PolydataOriginalDosimeter의 각 점으로부터 팬텀 polydata와 0.4 mm 반경 내에 있는 폴리데이터 생성
	vtkSmartPointer<vtkPoints> OriginalDosimeterPoints = PolydataOriginalDosimeter->GetPoints();
	vtkSmartPointer<vtkPoints> PhantomPolydataPoints = PhantomPolydata->GetPoints();
	vtkSmartPointer<vtkCellArray> CellsWithinMargin = vtkSmartPointer<vtkCellArray>::New();// vtkCellArray = {1,2,3,  2,5,8,  3,8,9, ...} -> 1,2,3 pointIds로 하나의 cell(tri)를 만듦
	for (int pointID = 0; pointID < OriginalDosimeterPoints->GetNumberOfPoints(); pointID++)
	{
		double PickedPoint[3]; // origianlDosimeterPoints의 모든 point를 돌면서 얻은 각 point
		OriginalDosimeterPoints->GetPoint(pointID, PickedPoint); // 3D Pick 및 location update 통해 받아오는 좌표 ID		
		for (vtkIdType i = 0; i < PhantomPolydataPoints->GetNumberOfPoints(); i++)
		{
			// Check if the point is within radius of the specified point
			double dist = vtkMath::Distance2BetweenPoints(PhantomPolydataPoints->GetPoint(i), PickedPoint); // 거리 제곱을 반환
			double radius = 0.4; // Margin: 0.4 cm = 4 mm 
			if (dist <= radius * radius) // radius = 2 cm -> dist = 2^2 = 4
			{
				vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
				PhantomPolydata->GetPointCells(i, cellIds); //  dosim_idx번째 point가 포함된 cell id 배열을 반환 -> 즉 dosim_idx가 2라면 {1,2 ...}을 반환 (1번, 2번 cell에는 ID:2인 point가 포함됨)
				for (vtkIdType j = 0; j < cellIds->GetNumberOfIds(); j++) // 해당 cell id 개수만큼 반복
				{
					vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
					PhantomPolydata->GetCellPoints(cellIds->GetId(j), cellPointIds); // 해당 cell id에 해당하는 pointIds를 반환 -> 즉 cell id=2 라면 {2,5,8} 반환
					CellsWithinMargin->InsertNextCell(cellPointIds); // point ID 2,5,8을 하나의 cell이 되도록 1차원 배열로 계속 insert
				}
			}
		}
	}
	vtkSmartPointer<vtkPolyData> PolyDataWithinMargin = vtkSmartPointer<vtkPolyData>::New(); // 임시 데이터
	PolyDataWithinMargin->SetPoints(PhantomPolydataPoints);
	PolyDataWithinMargin->SetPolys(CellsWithinMargin);
	vtkNew<vtkPolyDataConnectivityFilter> connectivityFilter_SideIncluded;
	connectivityFilter_SideIncluded->SetInputData(PolyDataWithinMargin);
	connectivityFilter_SideIncluded->SetExtractionModeToLargestRegion(); // clean 및 island 제거
	connectivityFilter_SideIncluded->Update();
	vtkSmartPointer<vtkPolyData> PolydataSideIncludedDosimeter = vtkSmartPointer<vtkPolyData>::New();
	PolydataSideIncludedDosimeter = connectivityFilter_SideIncluded->GetOutput(); // Side를 포함한 dosimeter
	// PolydataSideIncludedDosimeter 에서 PolydataOriginalDosimeter를 뺀 polydata 생성
	vtkSmartPointer<vtkPolyData> PolydataSideDosimeter = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> uniquePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> uniqueCells = vtkSmartPointer<vtkCellArray>::New();
	// PolydataSideIncludedDosimeter와 PolydataOriginalDosimeter의 겹치는 부분 제거하고 나머지만 남김
	for (vtkIdType i = 0; i < PolydataSideIncludedDosimeter->GetNumberOfCells(); i++) {
		bool isUnique = true;
		for (vtkIdType j = 0; j < PolydataOriginalDosimeter->GetNumberOfCells(); j++) {
			if (areCellsEquivalent(PolydataSideIncludedDosimeter, i, PolydataOriginalDosimeter, j, 0.001)) { // 마지막 항은 tolerance
				isUnique = false; // matched 되는 동일한 cell 이 존재 -> unique 하지 않다
				break;
			}
		}
		if (isUnique) {
			vtkSmartPointer<vtkIdList> cellPointIds = vtkSmartPointer<vtkIdList>::New();
			PolydataSideIncludedDosimeter->GetCellPoints(i, cellPointIds);

			vtkIdType newIds[3];
			for (int k = 0; k < 3; k++) {
				newIds[k] = uniquePoints->InsertNextPoint(PolydataSideIncludedDosimeter->GetPoint(cellPointIds->GetId(k)));
			}
			uniqueCells->InsertNextCell(3, newIds);
		}
	}
	PolydataSideDosimeter->SetPoints(uniquePoints);
	PolydataSideDosimeter->SetPolys(uniqueCells);//결과적으로 사용하는 선량계_side polydata (마진 offset 하기 전)

	CleaningPolyData(PolydataSideDosimeter, OutputPath.toStdString());
}

//// Sub-function (2)
void ETHuman3DApp::OffsetPointsByNormals(vtkPolyData* polyData, std::vector<vtkIdType>& pointIds, double offset) // called by (1) GenerateClothingLayerOverlappedWithDosimeter
{
	vtkSmartPointer<vtkDataArray> normals = polyData->GetPointData()->GetNormals();
	if (!normals) {
		std::cerr << "Error: No normals were found. Exiting..." << std::endl;
		return;
	}

	for (vtkIdType id : pointIds) {
		double point[3], normal[3], offsetPoint[3];
		polyData->GetPoint(id, point);
		normals->GetTuple(id, normal);

		// Offset the point along its normal
		vtkMath::MultiplyScalar(normal, offset);
		vtkMath::Add(point, normal, offsetPoint);

		polyData->GetPoints()->SetPoint(id, offsetPoint);
	}

	polyData->GetPoints()->Modified();
}
bool ETHuman3DApp::areCellsEquivalent(vtkSmartPointer<vtkPolyData> polyData1, vtkIdType cellId1, vtkSmartPointer<vtkPolyData> polyData2, vtkIdType cellId2, double tolerance) // called by (1) GenerateClothingLayerOverlappedWithDosimeter, (2) GenerateSideDosimeter
{
	vtkSmartPointer<vtkIdList> cellPointIds1 = vtkSmartPointer<vtkIdList>::New();
	polyData1->GetCellPoints(cellId1, cellPointIds1);

	vtkSmartPointer<vtkIdList> cellPointIds2 = vtkSmartPointer<vtkIdList>::New();
	polyData2->GetCellPoints(cellId2, cellPointIds2);

	std::vector<bool> matchedPoints(3, false);

	for (int i = 0; i < 3; ++i) {
		double point1[3];
		polyData1->GetPoint(cellPointIds1->GetId(i), point1);

		for (int j = 0; j < 3; ++j) {
			if (!matchedPoints[j]) {
				double point2[3];
				polyData2->GetPoint(cellPointIds2->GetId(j), point2);

				if (arePointsEqual(point1, point2, tolerance)) {
					matchedPoints[j] = true;
					break;
				}
			}
		}
	}

	return std::all_of(matchedPoints.begin(), matchedPoints.end(), [](bool matched) { return matched; });
}

//// Sub-function (3)
bool ETHuman3DApp::arePointsEqual(double point1[3], double point2[3], double tolerance) // called by (1) areCellsEquivalent
{
	return std::fabs(point1[0] - point2[0]) < tolerance &&
		std::fabs(point1[1] - point2[1]) < tolerance &&
		std::fabs(point1[2] - point2[2]) < tolerance;
}

////////////////// Calculate_Common ////////////////// 

std::pair<std::vector<std::map<int, std::string>>, double> ETHuman3DApp::Read_RI_File(std::string RIpath)
{	
	std::ifstream ifp_RI(RIpath);
	double yield;
	double yieldSum = 0;
	std::string radionuclide_name;
	std::string dump, yield_str, energy, particle;
	std::vector<std::map<int, std::string>> radionuclide_yield_energy_particle;
	ifp_RI >> radionuclide_name;
	while (!ifp_RI.eof()) {
		ifp_RI >> dump >> yield_str >> energy >> particle;		
		if (ifp_RI.fail()) break; // 마지막 줄이 한번더 안읽혀도 이전 데이터로 실행되므로, 이를 방지하기 위함
		
		yield = std::stod(yield_str);		
		yieldSum += yield;
		std::map<int, std::string> temp_map;
		temp_map[0] = yield_str; temp_map[1] = energy;
		if (particle == "G") temp_map[2] = "gamma";
		else if (particle == "PG") temp_map[2] = "gamma";
		else if (particle == "DG") temp_map[2] = "gamma";
		else if (particle == "X") temp_map[2] = "gamma";
		else if (particle == "AQ") temp_map[2] = "gamma";
		else if (particle == "B+") temp_map[2] = "e+";
		else if (particle == "B-") temp_map[2] = "e-";
		else if (particle == "DB") temp_map[2] = "e-";
		else if (particle == "IE") temp_map[2] = "e-";
		else if (particle == "AE") temp_map[2] = "e-";
		else if (particle == "A") temp_map[2] = "alpha";
		else if (particle == "N") temp_map[2] = "neutron";
		else if (particle == "H") temp_map[2] = "proton";
		else temp_map[2] = "geantino"; // for non-supproted paritlce while considering the total yields
		radionuclide_yield_energy_particle.push_back(temp_map);
	}
	ifp_RI.close();
	
	return std::make_pair(radionuclide_yield_energy_particle, yieldSum); 
}
void ETHuman3DApp::dbsendFilesClear()
{
	// Remove exist data/dbsend files
	QFile file;
	file.setFileName("./data/dbsend/AbortSignal");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/activity");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/broad_beam_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/external_point_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/floor_disk_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/phase_space_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/hot_particle_point_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/object_volume_source");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/list_collection");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/phantom_collection");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/Simulation_info");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/err");
	if (file.exists())
	{
		file.remove();
	}
	file.setFileName("./data/dbsend/phspFile");
	if (file.exists())
	{
		file.remove();
	}
	QDir dir1("./data/dbsend/wearable_send");
	// Iterate through all files in the directory
	for (const QString &file : dir1.entryList(QDir::Files)) {
		// Remove each file
		dir1.remove(file);
	}
}
void ETHuman3DApp::GenerateGlassesTetFile(int PhantomIdx)
{
	struct Point {
		double x, y, z;
	};

	struct Tetrahedron {
		int p1, p2, p3, p4;
	};

	//////////////////////// Generate each(L or R) glasses Tetrahedron files///////////////////////
	for (int i = 0; i < 2; i++)
	{
		QString glassPart;
		if (i == 0) glassPart = "L";
		if (i == 1) glassPart = "R";
		// Read the .obj file
		vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
		QString qInputFile = "./data/wearable/Glasses/glasses" + glassPart + "_" + QString::number(PhantomIdx) + ".obj";
		reader->SetFileName(Util::Wcs_to_mbs(qInputFile.toStdWString()).c_str());
		reader->Update();

		// Apply surface filter to the loaded data
		vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
		surfaceFilter->SetInputConnection(reader->GetOutputPort());
		surfaceFilter->Update();

		// Unstable and degenerated tetrahedrons 방지
		vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
		cleanFilter->SetInputConnection(surfaceFilter->GetOutputPort());
		cleanFilter->SetTolerance(0.01); // Adjust this value based on the scale of your model
		cleanFilter->Update();

		//vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
		//smoother->SetInputConnection(cleanFilter->GetOutputPort());
		//smoother->SetNumberOfIterations(20); // Adjust this value based on your requirements
		//smoother->BoundarySmoothingOff();
		//smoother->FeatureEdgeSmoothingOff();
		//smoother->SetFeatureAngle(120);
		//smoother->SetPassBand(0.1);
		//smoother->NonManifoldSmoothingOn();
		//smoother->NormalizeCoordinatesOn();
		//smoother->Update();

		// Tetrahedralize the polygonal dataset
		vtkSmartPointer<vtkDelaunay3D> delaunay3D = vtkSmartPointer<vtkDelaunay3D>::New();
		delaunay3D->SetInputConnection(cleanFilter->GetOutputPort());
		delaunay3D->Update();
		vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = delaunay3D->GetOutput();

		std::ofstream eleFile("./data/dbsend/wearable_send/temp_glasses" + glassPart.toStdString() + "_" + std::to_string(PhantomIdx) + ".ele");
		std::ofstream nodeFile("./data/dbsend/wearable_send/temp_glasses" + glassPart.toStdString() + "_" + std::to_string(PhantomIdx) + ".node");

		// Write node file
		vtkSmartPointer<vtkPoints> points = unstructuredGrid->GetPoints();
		vtkIdType numPoints = points->GetNumberOfPoints();
		nodeFile << numPoints << " 3 0 0" << std::endl;

		for (vtkIdType i = 0; i < numPoints; ++i) {
			double point[3];
			points->GetPoint(i, point);
			nodeFile << i << " " << point[0] << " " << point[1] << " " << point[2] << std::endl;
		}

		// Write ele file
		vtkSmartPointer<vtkCellArray> cells = unstructuredGrid->GetCells();
		vtkIdType numCells = cells->GetNumberOfCells();
		eleFile << numCells << " 4 0" << std::endl;

		//vtkIdType npts, *pts;
		vtkIdType npts;
		const vtkIdType* pts;
		vtkIdType cellId = 0;
		for (cells->InitTraversal(); cells->GetNextCell(npts, pts); cellId++) {
			eleFile << cellId << " " << pts[0] << " " << pts[1] << " " << pts[2] << " " << pts[3] << std::endl;
		}

		eleFile.close();
		nodeFile.close();
	}

	//////////////////////// Combine two (L or R) glasses tet files///////////////////////
	std::vector<Point> pointsL, pointsR;
	std::vector<Tetrahedron> tetrahedraL, tetrahedraR;

	std::string nodeFileName_L = "./data/dbsend/wearable_send/temp_glassesL_" + std::to_string(PhantomIdx) + ".node";
	std::string nodeFileName_R = "./data/dbsend/wearable_send/temp_glassesR_" + std::to_string(PhantomIdx) + ".node";
	std::string eleFileName_L = "./data/dbsend/wearable_send/temp_glassesL_" + std::to_string(PhantomIdx) + ".ele";
	std::string eleFileName_R = "./data/dbsend/wearable_send/temp_glassesR_" + std::to_string(PhantomIdx) + ".ele";

	// read node files
	int numPoints, dim, attributes, boundaryMarkers;

	std::ifstream nodeFile_L(nodeFileName_L);
	nodeFile_L >> numPoints >> dim >> attributes >> boundaryMarkers;
	for (int i = 0; i < numPoints; ++i)
	{
		int id;
		Point point;
		nodeFile_L >> id >> point.x >> point.y >> point.z;
		pointsL.push_back(point);
	}
	nodeFile_L.close();

	std::ifstream nodeFile_R(nodeFileName_R);
	nodeFile_R >> numPoints >> dim >> attributes >> boundaryMarkers;
	for (int i = 0; i < numPoints; ++i)
	{
		int id;
		Point point;
		nodeFile_R >> id >> point.x >> point.y >> point.z;
		pointsR.push_back(point);
	}
	nodeFile_R.close();

	// read ele files
	int numTetrahedra, nodesPerTetrahedron, attributesTet;

	std::ifstream eleFile_L(eleFileName_L);
	eleFile_L >> numTetrahedra >> nodesPerTetrahedron >> attributesTet;

	for (int i = 0; i < numTetrahedra; ++i) {
		int id;
		Tetrahedron tetrahedron;
		eleFile_L >> id >> tetrahedron.p1 >> tetrahedron.p2 >> tetrahedron.p3 >> tetrahedron.p4;
		tetrahedraL.push_back(tetrahedron);
	}
	eleFile_L.close();

	std::ifstream eleFile_R(eleFileName_R);
	eleFile_R >> numTetrahedra >> nodesPerTetrahedron >> attributesTet;

	for (int i = 0; i < numTetrahedra; ++i) {
		int id;
		Tetrahedron tetrahedron;
		eleFile_R >> id >> tetrahedron.p1 >> tetrahedron.p2 >> tetrahedron.p3 >> tetrahedron.p4;
		tetrahedraR.push_back(tetrahedron);
	}
	eleFile_R.close();

	// Combining
	std::vector<Point> combinedPoints(pointsL);
	combinedPoints.insert(combinedPoints.end(), pointsR.begin(), pointsR.end());

	std::vector<Tetrahedron> combinedTetrahedra(tetrahedraL);
	for (const Tetrahedron& t : tetrahedraR)
	{
		Tetrahedron shiftedTetrahedron;
		shiftedTetrahedron.p1 = t.p1 + pointsL.size();
		shiftedTetrahedron.p2 = t.p2 + pointsL.size();
		shiftedTetrahedron.p3 = t.p3 + pointsL.size();
		shiftedTetrahedron.p4 = t.p4 + pointsL.size();
		combinedTetrahedra.push_back(shiftedTetrahedron);
	}

	// Writing combined tet
	std::string nodeFileName_combined = "./data/dbsend/wearable_send/glasses_" + std::to_string(PhantomIdx) + ".node";
	std::string eleFileName_combined = "./data/dbsend/wearable_send/glasses_" + std::to_string(PhantomIdx) + ".ele";

	std::ofstream nodeFile_combined(nodeFileName_combined);
	std::ofstream eleFile_combined(eleFileName_combined);

	nodeFile_combined << combinedPoints.size() << " 3 0 0" << std::endl;
	for (size_t i = 0; i < combinedPoints.size(); ++i)
	{
		nodeFile_combined << i << " " << combinedPoints[i].x << " " << combinedPoints[i].y << " " << combinedPoints[i].z << std::endl;
	}

	eleFile_combined << combinedTetrahedra.size() << " 4 0" << std::endl;
	int matID = PhantomIdx * 1000000 + 29900;
	for (size_t i = 0; i < combinedTetrahedra.size(); ++i) {
		eleFile_combined << i << " " << combinedTetrahedra[i].p1 << " " << combinedTetrahedra[i].p2
			<< " " << combinedTetrahedra[i].p3 << " " << combinedTetrahedra[i].p4 << " " << matID << std::endl;
	}

	// Remove temp ele/node files
	const char* file_to_delete;
	file_to_delete = nodeFileName_L.c_str();	remove(file_to_delete);
	file_to_delete = nodeFileName_R.c_str();	remove(file_to_delete);
	file_to_delete = eleFileName_L.c_str();		remove(file_to_delete);
	file_to_delete = eleFileName_R.c_str();		remove(file_to_delete);
}

void ETHuman3DApp::TranslateGlassesTetFile(int phantomIdx)
{
	// Phantom geometry 정보
	double pCenterX = PhantomOrigianlPolyDataCenter[phantomIdx][0];
	double pCenterY = PhantomOrigianlPolyDataCenter[phantomIdx][1];
	double pCenterZ = PhantomOrigianlPolyDataCenter[phantomIdx][2];
	double PosX = pRt->m_Phantom_MainInfo[phantomIdx][6];
	double PosY = pRt->m_Phantom_MainInfo[phantomIdx][7];
	double PosZ = pRt->m_Phantom_MainInfo[phantomIdx][8];
	double RotX = pRt->m_Phantom_MainInfo[phantomIdx][9];
	double RotY = pRt->m_Phantom_MainInfo[phantomIdx][10];
	double RotZ = pRt->m_Phantom_MainInfo[phantomIdx][11];
	const double PI = 3.141592 / 180; // 라디안 변환 
	double RadianX = RotX * PI;
	double RadianY = RotY * PI;
	double RadianZ = RotZ * PI;

	double ScaleX = PhantomPolyDataScaleFactor[phantomIdx][0];
	double ScaleY = PhantomPolyDataScaleFactor[phantomIdx][0];
	double ScaleZ = PhantomPolyDataScaleFactor[phantomIdx][1];

	Eigen::Vector3d translation1(-pCenterX, -pCenterY, -pCenterZ);
	Eigen::Vector3d translation2(PosX, PosY, PosZ);

	// Create the rotation matrices for each axis
	Eigen::Matrix3d rotation_x;
	rotation_x = Eigen::AngleAxisd(RadianX, Eigen::Vector3d::UnitX());

	Eigen::Matrix3d rotation_y;
	rotation_y = Eigen::AngleAxisd(RadianY, Eigen::Vector3d::UnitY());

	Eigen::Matrix3d rotation_z;
	rotation_z = Eigen::AngleAxisd(RadianZ, Eigen::Vector3d::UnitZ());

	// Combine the rotation matrices
	Eigen::Matrix3d rotation = rotation_z * rotation_x * rotation_y; //Rotation Z -> X -> Y

	// Create the scaling matrix
	Eigen::DiagonalMatrix<double, 3> scaling(ScaleX, ScaleY, ScaleZ);

	// Create the combined transformation matrix
	Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
	transform.block<3, 3>(0, 0) = rotation * scaling; // rotation -> scaling (실제로는 scaling 후 rotation이 됨, 매트릭스 역순)

	std::string FolderPath;

	int numVertex;
	int tempInt;

	double xPos, yPos, zPos;
	Eigen::Vector4d point;	

	FolderPath = "./data/dbsend/wearable_send/glasses_";

	std::string ifpFileName = FolderPath + std::to_string(phantomIdx) + ".node";
	std::ifstream ifpNode(ifpFileName);
	ifpNode.precision(20);
	std::string ofpFileName = FolderPath + std::to_string(phantomIdx) + "_temp.node";
	std::ofstream ofpNode(ofpFileName);
	ofpNode.precision(20);

	ifpNode >> numVertex >> tempInt >> tempInt >> tempInt;
	ofpNode << numVertex << "  3  0  0" << endl;


	for (int i = 0; i < numVertex; i++) // This could be faster using PRAGMA_OMP
	{
		ifpNode >> tempInt >> xPos >> yPos >> zPos;
		point = Eigen::Vector4d(xPos, yPos, zPos, 1.0);
		// glasses는 모두 사용자제작이므로 원점 이동 되어 있으므로(polydata_base에 맞음) 원점 이동(translation1)은 하지 않음	
		// Create the combined transformation matrix
		Eigen::Matrix4d transform_glasses = Eigen::Matrix4d::Identity();
		transform_glasses.block<3, 3>(0, 0) = rotation; // rotation (안경은 scaling은 하지 않음)
		point = transform_glasses * point;
		point.head<3>() += translation2.head<3>(); // translation2(Pos 위치 이동)
		ofpNode << "   " << i << "    " << point.x() << "  " << point.y() << "  " << point.z() << endl;

	}
	ifpNode.close();
	ofpNode.close();

	// Remove ofp_temp and rename to ifp
	const char* file_to_delete = ifpFileName.c_str();
	const char* old_file_name = ofpFileName.c_str();
	const char* new_file_name = ifpFileName.c_str();

	remove(file_to_delete);
	rename(old_file_name, new_file_name);
	
}
FILETIME ETHuman3DApp::getLastWriteTime(const std::string& path) {
	FILETIME lastWriteTime = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;

	if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fileInfo)) {
		lastWriteTime = fileInfo.ftLastWriteTime;
	}
	else {
	}
	return lastWriteTime;
}

// struct MappingDebugData {
// 	double p[3];        // Step Position
// 	vtkIdType oldFID;   // Old Algorithm Facet ID
// 	double oldC[3];     // Old Algorithm Facet Center
// 	double oldArea;     // Old Facet Area
// 	vtkIdType newFID;   // New Algorithm Facet ID
// 	double newC[3];     // New Algorithm Facet Center
// 	double newArea;     // New Facet Area
// 	bool isMatch;       // Match Status
// };

void ETHuman3DApp::ReadSkinDoseData(std::stringstream &ss)
{
	if (ss.str().empty()) {
		return;
	}

	double LogMaximum_VertexDoseAmongAllPhantoms_pGy = DBL_MIN;
	double LogMinimum_VertexDoseAmongAllPhantoms_pGy = DBL_MAX;
	double Maximum_VertexDoseAmongAllPhantoms_pGy = DBL_MIN;
	double Minimum_VertexDoseAmongAllPhantoms_pGy = DBL_MAX;
	double Maximum_FacetDose_pGy = DBL_MIN;

	// 휘발성 데이터 컨테이너 (현재 배치 처리용)
	std::map<int, std::vector<std::array<double, 3>>> SkinDEpoint;
	std::map<int, std::vector<double>> SkinDEvalue;

	int tmp_phantomID;
	double x, y, z, dE;
	std::uint64_t event_count;
	double unit_conversion_factor;

	// 헤더 정보 읽기
	ss >> event_count >> unit_conversion_factor;

	// ------------------------------------------------------------
	// [1] Data Parsing & Mapping Loop
	// ------------------------------------------------------------
	while (ss >> tmp_phantomID >> x >> y >> z >> dE)
	{       
		if (tmp_phantomID != -1) // 정상 선량 데이터
		{
			SkinDEpoint[tmp_phantomID].push_back({ x,y,z });
			SkinDEvalue[tmp_phantomID].push_back(dE * unit_conversion_factor); 
		}
		else // Sentinel (-1): 하나의 이벤트(History) 종료
		{
			for (auto phantomID : pRt->m_Phantom_SequenceVector)
			{
				if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES) continue;
				if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;

				for (int DEpointID = 0; DEpointID < SkinDEpoint[phantomID].size(); DEpointID++)
				{
					double p[3] = { SkinDEpoint[phantomID][DEpointID][0], SkinDEpoint[phantomID][DEpointID][1], SkinDEpoint[phantomID][DEpointID][2] };

					// ------------------------------------------------------------
					// [Old Algorithm: 2D Closest Point] - 주석처리
					// ------------------------------------------------------------
					// vtkIdType oldFacetID = -1;
					// double old_cp[3]; int subId; double d2;
					// SkinCellLocator[phantomID]->FindClosestPoint(p, old_cp, oldFacetID, subId, d2);

					// ------------------------------------------------------------
					// New Algorithm: 3D Wedge Volume Mapping
					// ------------------------------------------------------------
					vtkIdType targetFacetID = -1;
					if (m_SkinVolumeLocator.count(phantomID) && m_SkinVolumeLocator[phantomID] != nullptr)
					{
						targetFacetID = m_SkinVolumeLocator[phantomID]->FindCell(p);
					}

					// Fallback: 3D 매핑 실패 시 2D 결과 사용
					if (targetFacetID == -1)
					{
						double cp[3]; int subId; double d2;
						SkinCellLocator[phantomID]->FindClosestPoint(p, cp, targetFacetID, subId, d2);
					}

					// ------------------------------------------------------------
					// Dose Accumulation
					// ------------------------------------------------------------
					if (targetFacetID != -1)
					{
						if (FacetInfo[phantomID][targetFacetID][7] <= 0.05)
						{
							double neighborCount = TinyFacetAdjacentFacetMap[phantomID][targetFacetID].size();
							if (neighborCount > 0)
							{
								double distributed_dE = SkinDEvalue[phantomID][DEpointID] / neighborCount;
								for (auto itr : TinyFacetAdjacentFacetMap[phantomID][targetFacetID])
								{
									Facet_DE_DE2[phantomID][itr].first += distributed_dE;
									Facet_DE_DE2[phantomID][itr].second += distributed_dE * distributed_dE;
								}
							}
						}
						else
						{
							Facet_DE_DE2[phantomID][targetFacetID].first += SkinDEvalue[phantomID][DEpointID];
							Facet_DE_DE2[phantomID][targetFacetID].second += SkinDEvalue[phantomID][DEpointID] * SkinDEvalue[phantomID][DEpointID];
						}
					}
				}
			}
			SkinDEpoint.clear();
			SkinDEvalue.clear();
		}       
	}

	// ------------------------------------------------------------
	// 최종 선량(Dose) 계산 및 통계 처리
	// ------------------------------------------------------------
	for (auto phantomID : pRt->m_Phantom_SequenceVector)
	{
		double density_skin = SkinDenstiy[phantomID]; 

		// FacetDose 계산
		for (auto f_de : Facet_DE_DE2[phantomID])
		{
			double Mass = 0;
			double volume_cm3 = 0.0;

			if (FacetInfo[phantomID][f_de.first].size() >= 9) {
				volume_cm3 = FacetInfo[phantomID][f_de.first][8]; 
			} else {
				volume_cm3 = FacetInfo[phantomID][f_de.first][7] * (50.0 / 10000.0); 
			}

			Mass = volume_cm3 * density_skin / 1.0E+3; 
			if (Mass <= 0) Mass = 1.0e-15; 

			double de_sum = f_de.second.first;
			double de2_sum = f_de.second.second;
			double EX = de_sum / event_count;
			double EX2 = de2_sum / event_count;
			double variance = (EX2 - (EX*EX)) / (event_count - 1);
			
			double relativeE = 0.0;
			if (EX != 0) relativeE = sqrt(std::abs(variance)) / EX;

			FacetDose[phantomID][f_de.first] = EX / Mass; 
			FacetError[phantomID][f_de.first] = relativeE;

			if (FacetDose[phantomID][f_de.first] > Maximum_FacetDose_pGy)
			{
				Maximum_FacetDose_pGy = FacetDose[phantomID][f_de.first];
				MaximumFacetDoseID[phantomID] = f_de.first;
			}

			if (FacetDose[phantomID][f_de.first] > 10'000'000'000)
			{
				LOG_ERROR("=== FacetDose is much bigger than 1e10===");
				LOG_ERROR("FacetID: " + QString::number(f_de.first));
				LOG_ERROR("Mass: " + QString::number(Mass));
				LOG_ERROR("FacetDose: " + QString::number(FacetDose[phantomID][f_de.first]));
			}
			if (FacetDose[phantomID][f_de.first] > 0 && FacetDose[phantomID][f_de.first] < 1e-10)
			{
				LOG_ERROR("=== FacetDose is much smaller than 1e-10===");
				LOG_ERROR("FacetID: " + QString::number(f_de.first));
			}
		}

		// VertexDose 누적
		vtkSmartPointer<vtkPoints> originalPoints = SkinLayer_PolyData[phantomID]->GetPoints();
		for (vtkIdType i = 0; i < originalPoints->GetNumberOfPoints(); i++) 
		{
			double dE = 0;
			double Mass = 0;
			for (auto AdjacentFacetID : VertexToFacetMap[phantomID][i]) 
			{
				dE += Facet_DE_DE2[phantomID][AdjacentFacetID].first; 
				
				double vol_cm3 = 0.0;
				if (FacetInfo[phantomID][AdjacentFacetID].size() >= 9) {
					vol_cm3 = FacetInfo[phantomID][AdjacentFacetID][8];
				} else {
					vol_cm3 = FacetInfo[phantomID][AdjacentFacetID][7] * (50.0 / 10000.0);
				}
				
				Mass += vol_cm3 * density_skin / 1.0E+3; 
			}
			
			if (Mass > 0) VertexDose[phantomID][i] = dE / Mass / event_count;
			else VertexDose[phantomID][i] = 0;

			if (VertexDose[phantomID][i] > Maximum_VertexDoseAmongAllPhantoms_pGy) {
				Maximum_VertexDoseAmongAllPhantoms_pGy = VertexDose[phantomID][i];
			}
			if (VertexDose[phantomID][i] != 0 && (VertexDose[phantomID][i] < Minimum_VertexDoseAmongAllPhantoms_pGy)) {
				Minimum_VertexDoseAmongAllPhantoms_pGy = VertexDose[phantomID][i];
			}
		}
		
		LogMaximum_VertexDoseAmongAllPhantoms_pGy = log(Maximum_VertexDoseAmongAllPhantoms_pGy);
		LogMinimum_VertexDoseAmongAllPhantoms_pGy = log(Minimum_VertexDoseAmongAllPhantoms_pGy);

		// ------------------------------------------------------------
		// 3D Visualization Setup (LUT, ScalarBar, Labels)
		// ------------------------------------------------------------
		float SkinColor[3] = { 255, 192, 160 };
		
		vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
		lut->SetRange(0, 1); 
		lut->SetNumberOfTableValues(256); 

		float SkinColor_lut[3] = { 255.0f / 255.0f, 192.0f / 255.0f, 160.0f / 255.0f }; 
		float GreenColor_lut[3] = { 0.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
		float YellowColor_lut[3] = { 255.0f / 255.0f, 255.0f / 255.0f, 0.0f / 255.0f };
		float RedColor_lut[3] = { 255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f }; 

		for (int i = 0; i < 256; i++) {
			double value = static_cast<double>(i) / 255.0;
			double rgb[3];

			if (value <= 0.7) {
				for (int j = 0; j < 3; j++) {
					rgb[j] = GreenColor_lut[j] + value / 0.7 * (YellowColor_lut[j] - GreenColor_lut[j]);
				}
			}
			else {
				for (int j = 0; j < 3; j++) {
					rgb[j] = YellowColor_lut[j] + (value - 0.7) / 0.3 * (RedColor_lut[j] - YellowColor_lut[j]);
				}
			}
			lut->SetTableValue(i, rgb);
		}
		lut->Build();

		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_ScalarBar);
		SkinDoseVisualization_ScalarBar = nullptr;
		SkinDoseVisualization_ScalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
		SkinDoseVisualization_ScalarBar->SetLookupTable(lut);
		SkinDoseVisualization_ScalarBar->SetNumberOfLabels(0);
		SkinDoseVisualization_ScalarBar->SetPosition(0, 0.79); 
		SkinDoseVisualization_ScalarBar->SetPosition2(0.10, 0.20); 
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_ScalarBar);

		vtkSmartPointer<vtkTextProperty> textProperty = vtkSmartPointer<vtkTextProperty>::New();
		textProperty->SetFontSize(pRt->FontSizeScaling(18));
		textProperty->SetFontFamilyToArial(); 
		textProperty->BoldOn();
		textProperty->SetColor(100, 100, 100);

		double tmp_value;

		// Label 0%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label0percent);
		SkinDoseVisualization_Label0percent = nullptr;
		SkinDoseVisualization_Label0percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1. - 0) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 0);
		std::ostringstream oss0percent;
		oss0percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label0percent->SetInput(oss0percent.str().c_str());
		SkinDoseVisualization_Label0percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label0percent->SetPosition(0.04, 0.785); 
		SkinDoseVisualization_Label0percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label0percent);

		// Label 20%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label20percent);
		SkinDoseVisualization_Label20percent = nullptr;
		SkinDoseVisualization_Label20percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1. - 0.2) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 0.2);
		std::ostringstream oss20percent;
		oss20percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label20percent->SetInput(oss20percent.str().c_str());
		SkinDoseVisualization_Label20percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label20percent->SetPosition(0.04, 0.825); 
		SkinDoseVisualization_Label20percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label20percent);

		// Label 40%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label40percent);
		SkinDoseVisualization_Label40percent = nullptr;
		SkinDoseVisualization_Label40percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1. - 0.4) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 0.4);
		std::ostringstream oss40percent;
		oss40percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label40percent->SetInput(oss40percent.str().c_str());
		SkinDoseVisualization_Label40percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label40percent->SetPosition(0.04, 0.865); 
		SkinDoseVisualization_Label40percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label40percent);

		// Label 60%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label60percent);
		SkinDoseVisualization_Label60percent = nullptr;
		SkinDoseVisualization_Label60percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1. - 0.6) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 0.6);
		std::ostringstream oss60percent;
		oss60percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label60percent->SetInput(oss60percent.str().c_str());
		SkinDoseVisualization_Label60percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label60percent->SetPosition(0.04, 0.905); 
		SkinDoseVisualization_Label60percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label60percent);

		// Label 80%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label80percent);
		SkinDoseVisualization_Label80percent = nullptr;
		SkinDoseVisualization_Label80percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1. - 0.8) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 0.8);
		std::ostringstream oss80percent;
		oss80percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label80percent->SetInput(oss80percent.str().c_str());
		SkinDoseVisualization_Label80percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label80percent->SetPosition(0.04, 0.945); 
		SkinDoseVisualization_Label80percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label80percent);

		// Label 100%
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinDoseVisualization_Label100percent);
		SkinDoseVisualization_Label100percent = nullptr;
		SkinDoseVisualization_Label100percent = vtkSmartPointer<vtkTextActor>::New();
		tmp_value = std::pow(Minimum_VertexDoseAmongAllPhantoms_pGy, 1.) * std::pow(Maximum_VertexDoseAmongAllPhantoms_pGy, 1.);
		std::ostringstream oss100percent;
		oss100percent << std::scientific << std::setprecision(4) << tmp_value << " pGy" << pRt->DoseUnit_QString.toStdString();
		SkinDoseVisualization_Label100percent->SetInput(oss100percent.str().c_str());
		SkinDoseVisualization_Label100percent->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
		SkinDoseVisualization_Label100percent->SetPosition(0.04, 0.98); 
		SkinDoseVisualization_Label100percent->SetTextProperty(textProperty);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinDoseVisualization_Label100percent);

		// Apply Mapper and Actor
		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetScalarVisibility(true);
		mapper->SetInputData(SkinLayer_PolyData[phantomID]); 

		vtkNew<vtkUnsignedCharArray> Colors; 
		Colors->SetNumberOfComponents(3);
		Colors->SetNumberOfTuples(SkinLayer_PolyData[phantomID]->GetNumberOfPoints());
		
		for (vtkIdType i = 0; i < originalPoints->GetNumberOfPoints(); i++) 
		{
			double colorValue[3];
			if (VertexDose[phantomID][i] == 0) {
				Colors->SetTuple(i, SkinColor);
			}
			else {
				lut->GetColor((log(VertexDose[phantomID][i]) - LogMinimum_VertexDoseAmongAllPhantoms_pGy) / (LogMaximum_VertexDoseAmongAllPhantoms_pGy - LogMinimum_VertexDoseAmongAllPhantoms_pGy), colorValue); 

				float mappedColor[3] = { static_cast<float>(colorValue[0] * 255.0),
										static_cast<float>(colorValue[1] * 255.0),
										static_cast<float>(colorValue[2] * 255.0) };
				Colors->SetTuple(i, mappedColor);
			}
		}

		SkinLayer_PolyData[phantomID]->GetPointData()->SetScalars(Colors);
		mapper->Update();
		
		theApp.m_pVTKWidget->GetSceneRenderer()->RemoveActor(SkinPhantomActor[phantomID]);
		SkinPhantomActor[phantomID] = nullptr;
		SkinPhantomActor[phantomID] = vtkSmartPointer<vtkActor>::New();
		SkinPhantomActor[phantomID]->SetMapper(mapper);
		theApp.m_pVTKWidget->GetSceneRenderer()->AddActor(SkinPhantomActor[phantomID]);
		
		SkinPhantomActor[phantomID]->VisibilityOff();
		SkinDoseVisualization_ScalarBar->VisibilityOff();
		SkinDoseVisualization_Label0percent->VisibilityOff();
		SkinDoseVisualization_Label20percent->VisibilityOff();
		SkinDoseVisualization_Label40percent->VisibilityOff();
		SkinDoseVisualization_Label60percent->VisibilityOff();
		SkinDoseVisualization_Label80percent->VisibilityOff();
		SkinDoseVisualization_Label100percent->VisibilityOff();
	}

	// Visualization Toggle Check
	if (pRt->m_skinDoseVisualizationButton->isChecked()) 
	{
		for (auto phantomID : pRt->m_Phantom_SequenceVector)
		{
			if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES) continue;

			PhantomPanelActor[phantomID]->VisibilityOff();
			SkinPhantomActor[phantomID]->VisibilityOn();
			SkinDoseVisualization_ScalarBar->VisibilityOn();
			SkinDoseVisualization_Label0percent->VisibilityOn();
			SkinDoseVisualization_Label20percent->VisibilityOn();
			SkinDoseVisualization_Label40percent->VisibilityOn();
			SkinDoseVisualization_Label60percent->VisibilityOn();
			SkinDoseVisualization_Label80percent->VisibilityOn();
			SkinDoseVisualization_Label100percent->VisibilityOn();
		}
	}
	theApp.m_pVTKWidget->renderWindow()->Render();
}

vtkSmartPointer<vtkPolyData> ETHuman3DApp::ExportGeodesicROIForPhantom(int phantomID, double targetAreaCm2, double refineFactor, vtkSmartPointer<vtkPolyData>* roiBeforeSubdivision)
{
	// 0) 입력 메쉬/seed 준비 (클래스 멤버 존재 가정)
	vtkPolyData* skinRaw = SkinLayer_PolyData[phantomID];
	vtkIdType seed = MaximumFacetDoseID[phantomID];

	// 1) 삼각형화 & 클린
	auto triClean = HP_SkinDoseCalculate::ensure_tri_clean(skinRaw);

	// 2) 지오데식 확장으로 ROI cell 집합 구하기
	double achieved = 0.0;
	std::vector<vtkIdType> roiCells =
		HP_SkinDoseCalculate::grow_area_by_geodesic(triClean, seed, targetAreaCm2, achieved);

	// 3) ROI만 복사하여 PolyData 구성 (세분화 "전" ROI)
	vtkSmartPointer<vtkPolyData> roiPoly =
		HP_SkinDoseCalculate::extract_cells_as_polydata(triClean, roiCells);

	// (옵션) 외부에서 세분화 전 ROI를 쓰고 싶다면 out 파라미터로 전달
	if (roiBeforeSubdivision) {
		*roiBeforeSubdivision = roiPoly;
	}

	// 4) refineFactor에 맞춰 반복 횟수 선택 (예: 50 → n=3)
	const int nIters = HP_SkinDoseCalculate::choose_linear_subdiv_iterations(refineFactor);

	// 5) 선형 세분화 실행 → 세분화된 ROI PolyData (리턴값)
	vtkSmartPointer<vtkPolyData> roiSubdiv =
		HP_SkinDoseCalculate::linear_subdivide(roiPoly, nIters);

	return roiSubdiv;
}

void ETHuman3DApp::ReadSkinDoseData_1_10cm2(const std::vector<SkinDoseData>& totalDoseData,
	qint64 event_count,
	double unit_conversion_factor)
{
	// vector가 비었는지 확인 (기존 ss.str().empty() 대체)
	if (totalDoseData.empty()) {
		return;
	}

	std::map<int, std::vector<std::array<double, 3>>> SkinDEpoint; // SkinDEpoint[phantomID][DEpointID] = {x(cm), y(cm), z(cm)} *휘발성
	std::map<int, std::vector<double>> SkinDEvalue; // SkinDEvalue[phantomID][DEpointID] = dE(Joule) *휘발성
		
	// ===== 각 phantomID 별로: ROI 생성→세분화→roiLocator 구성 =====
	const double targetAreaCm2 = 30.0; // 30 cm2이 ROI임(1, 10cm2 구하기 위한 여유 있는 ROI)
	const double refineFactor = 50.0; // 하나의 삼각형을 50개의 삼각형으로 subdivide -> 필요시 조정/입력화
	std::map<int, vtkSmartPointer<vtkCellLocator>> fullLocator;
	std::map<int, vtkSmartPointer<vtkCellLocator>> roiLocator;
	std::map<int, std::unordered_set<vtkIdType>> roi30_full_set;
	std::map<int, std::vector<vtkIdType>> patchIDs_1cm2, patchIDs_10cm2;
	std::map<int, std::map<vtkIdType, double>> cellAreas;
	std::map<int, std::array<double, 3>> seedCenter;
	std::map<int, vtkDataArray*> cellNormalsFull;

	for (auto phantomID : pRt->m_Phantom_SequenceVector)
	{
		if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES ||
			pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;

		vtkIdType seedRaw = MaximumFacetDoseID[phantomID]; // 최대 선량 지점의 facetID
		vtkPolyData* skinRaw = SkinLayer_PolyData[phantomID];

		// --- [A] 전신(outer skin) 클린 메쉬 & roiLocator 준비 ---
		auto fullTriClean = HP_SkinDoseCalculate::ensure_tri_clean(skinRaw);

		// 전신 기하의 "일관된 셀 노멀" 생성 + 이것으로 fullLocator 구성 -> ROI 내 dEpoint인지 판단 + depth는 원본 면으로부터 판단
		auto fullNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
		fullNormals->SetInputData(fullTriClean);
		fullNormals->ConsistencyOn();       // 인접 셀 노멀 일관화
		fullNormals->AutoOrientNormalsOn(); // 자동 방향 정렬
		fullNormals->SplittingOff();        // 경계에서 노멀 쪼개지 않음(원하면 On)
		fullNormals->ComputePointNormalsOff();
		fullNormals->ComputeCellNormalsOn();
		fullNormals->Update();

		vtkPolyData* fullWithNormals = fullNormals->GetOutput();
		cellNormalsFull[phantomID] = fullWithNormals->GetCellData()->GetNormals();
		if (!cellNormalsFull[phantomID]) {
			theApp.SetMessageBox("No cell normals on full mesh");
			continue;
		}
		fullLocator[phantomID] = vtkSmartPointer<vtkCellLocator>::New();
		fullLocator[phantomID]->SetDataSet(fullWithNormals); // 노멀 포함된 동일 토폴로지
		fullLocator[phantomID]->CacheCellBoundsOn();
		fullLocator[phantomID]->BuildLocator();

		// --- [B] 전신 메쉬에서 30 cm² ROI (전신 인덱스 기준) 셋 구하기 ---
		seedCenter[phantomID] = HP_SkinDoseCalculate::cell_center(skinRaw, seedRaw); // seedCenter는 재배열을 무시하고 최대선량지점(xyz 좌표)임
		vtkIdType seedOnFull = HP_SkinDoseCalculate::closest_cell_id_for_point(fullLocator[phantomID], seedCenter[phantomID].data());

		double achieved30 = 0.0;
		std::vector<vtkIdType> roi30_full_ids =
			HP_SkinDoseCalculate::grow_area_by_geodesic(fullTriClean, seedOnFull, /*targetAreaCm2*/30.0, achieved30); // full Locator에서의 roi 30 cm2 내의 facetID들의 집합
		roi30_full_set[phantomID].insert(roi30_full_ids.begin(), roi30_full_ids.end());

		// 세분화된 ROI polydata 생성
		vtkSmartPointer<vtkPolyData> roiBefore; // roi인데 subdivide 전(사용하지는 않음)		
		auto roiSubdiv = HP_SkinDoseCalculate::ExportGeodesicROIForPhantom(
			phantomID,
			skinRaw,
			seedRaw,
			/*targetAreaCm2=*/30.0,
			/*refineFactor=*/50.0,
			/*roiBeforeSubdivision=*/&roiBefore,
			&cellAreas[phantomID]
		);

		// 세분화된 ROI Loacator 할당
		roiLocator[phantomID] = vtkSmartPointer<vtkCellLocator>::New();
		roiLocator[phantomID]->SetDataSet(roiSubdiv);
		roiLocator[phantomID]->CacheCellBoundsOn();
		roiLocator[phantomID]->BuildLocator();

		// 세분화 roi에서 seed 검증용
		vtkIdType seedOnSubdiv =
			HP_SkinDoseCalculate::closest_cell_id_for_point(roiLocator[phantomID], seedCenter[phantomID].data());
		if (seedOnSubdiv < 0) {
			theApp.SetMessageBox("[HP] seed on subdiv not found. skip phantom " + QString::number(phantomID));
			continue;
		}

		// 지오데식으로 1 cm² / 10 cm² facetID 집합 얻기		
		double achieved1 = 0.0, achieved10 = 0.0;

		HP_SkinDoseCalculate::geodesic_dual_area_region(
			roiSubdiv, seedOnSubdiv,
			/*areaTarget1*/ 1.0,
			/*areaTarget2*/ 10.0,
			/*outIDs1*/ patchIDs_1cm2[phantomID],
			/*outIDs2*/ patchIDs_10cm2[phantomID],
			/*achieved1*/ &achieved1,
			/*achieved2*/ &achieved10
		);
	}

	for (const auto& data : totalDoseData)
	{
		if (data.phantomId != -1) // 정상 선량
		{
			// vector의 원소(data)에서 직접 값을 사용합니다.
			SkinDEpoint[data.phantomId].push_back({ data.x, data.y, data.z });
			SkinDEvalue[data.phantomId].push_back(data.dE * unit_conversion_factor);
		}
		else // Sentinel (이벤트 경계) 처리
		{			
			// "Find closet point w.r.t DEpoint"
			for (auto phantomID : pRt->m_Phantom_SequenceVector)
			{
				for (int DEpointID = 0; DEpointID < SkinDEpoint[phantomID].size(); DEpointID++)
				{
					const double p[3] = { SkinDEpoint[phantomID][DEpointID][0], SkinDEpoint[phantomID][DEpointID][1], SkinDEpoint[phantomID][DEpointID][2] };
					const double dE = SkinDEvalue[phantomID][DEpointID];
					
					// 1) 전신 기준 최근접 facet -> 전신을 기준으로 매핑을 먼저 해야, ROI에 이상한 점이 매핑되지 않음
					double fullClosest[3]; vtkIdType fullFacetId = -1; int subId = -1; double dist2 = 0.0;
					fullLocator[phantomID]->FindClosestPoint(p, fullClosest, fullFacetId, subId, dist2);

					// 2) 전신 30 cm² ROI 집합에 속하지 않으면 스킵 → ROI 밖 점 제거
					if (roi30_full_set[phantomID].find(fullFacetId) == roi30_full_set[phantomID].end()) continue;
				
					// 세분화 ROI에서 facetID 매핑
					double closest_subdiv[3]; vtkIdType facetId = -1; int subId_sub = -1; double dist2_sub = 0.0;
					roiLocator[phantomID]->FindClosestPoint(p, closest_subdiv, facetId, subId_sub, dist2_sub);			

					// 누적: ROI(1 cm2, 10 cm2 subdiv) 누적			

					// 1cm2 패치에 속하는지 확인(std::find 사용)
					// 벡터의 시작부터 끝까지 facetId를 검색해서, 끝까지 도달하지 않았으면(찾았으면) 참.
					if (std::find(patchIDs_1cm2[phantomID].begin(), patchIDs_1cm2[phantomID].end(), facetId) != patchIDs_1cm2[phantomID].end())
					{
						FacetDE_DE2_1cm2[phantomID].first += dE;
						FacetDE_DE2_1cm2[phantomID].second += dE * dE;
					}

					// 10cm2 패치에 속하는지 확인 (std::find 사용)
					if (std::find(patchIDs_10cm2[phantomID].begin(), patchIDs_10cm2[phantomID].end(), facetId) != patchIDs_10cm2[phantomID].end())
					{
						FacetDE_DE2_10cm2[phantomID].first += dE;
						FacetDE_DE2_10cm2[phantomID].second += dE * dE;
					}
				}						
			}
			SkinDEpoint.clear();
			SkinDEvalue.clear();
		}		
	}

	for (auto phantomID : pRt->m_Phantom_SequenceVector)
	{
		// === 1cm2 계산 ===
		{
			// 면적
			double sumArea_1cm2 = 0.0;
			for (auto cid : patchIDs_1cm2[phantomID])
			{
				auto itArea = cellAreas[phantomID].find(cid);
				if (itArea != cellAreas[phantomID].end())             sumArea_1cm2 += itArea->second;
			}

			// 선량, 에러
			double Mass = sumArea_1cm2 * (50. / 10000) * SkinDenstiy[phantomID] / 1.0E+3;
			double de_sum = FacetDE_DE2_1cm2[phantomID].first;
			double de2_sum = FacetDE_DE2_1cm2[phantomID].second;
			double EX = de_sum / event_count;
			double EX2 = de2_sum / event_count;
			double variance = (EX2 - (EX*EX)) / (event_count - 1);
			double relativeE = sqrt(variance) / EX;

			// 1cm2, 10cm2 최대선량지점 및 선량값 계산 - 그래프제작용
			HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][0]);
			HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][1]);
			HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][2]);
			HP_1cm2_MaximumDose_GraphInfo[phantomID] = EX / Mass; //
			HP_1cm2_RelativeError_GraphInfo[phantomID] = relativeE;
		}		

		// === 10cm2 계산 ===
		{
			// 면적
			double sumArea_10cm2 = 0.0;
			for (auto cid : patchIDs_10cm2[phantomID]) {
				auto itArea = cellAreas[phantomID].find(cid);
				if (itArea != cellAreas[phantomID].end())             sumArea_10cm2 += itArea->second;
			}

			// 선량, 에러
			double Mass = sumArea_10cm2 * (50. / 10000) * SkinDenstiy[phantomID] / 1.0E+3;
			double de_sum = FacetDE_DE2_10cm2[phantomID].first;
			double de2_sum = FacetDE_DE2_10cm2[phantomID].second;
			double EX = de_sum / event_count;
			double EX2 = de2_sum / event_count;
			double variance = (EX2 - (EX*EX)) / (event_count - 1);
			double relativeE = sqrt(variance) / EX;

			// 1cm2, 10cm2 최대선량지점 및 선량값 계산 - 그래프제작용
			HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][0]);
			HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][1]);
			HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[phantomID][2]);
			HP_10cm2_MaximumDose_GraphInfo[phantomID] = EX / Mass; //
			HP_10cm2_RelativeError_GraphInfo[phantomID] = relativeE;
		}
	}	
}

void ETHuman3DApp::ReadSkinDoseData_HP(std::stringstream &ss)
{
	if (ss.str().empty()) {
		theApp.SetMessageBox("No data");
		return;
	}

	std::map<int, std::vector<std::array<double, 3>>> HP_SkinDEpoint; // SkinDEpoint[phantomID][DEpointID] = {x(cm), y(cm), z(cm)} *휘발성
	std::map<int, std::vector<double>> HP_SkinDEvalue; // SkinDEvalue[phantomID][DEpointID] = dE(Joule) *휘발성

	int tmp_phantomID;
	double x, y, z, dE;
	std::uint64_t event_count;
	double unit_conversion_factor;
	ss >> event_count >> unit_conversion_factor;
	while (ss >> tmp_phantomID >> x >> y >> z >> dE)
	{
		HP_SkinDEpoint[tmp_phantomID].push_back({ x,y,z });
		HP_SkinDEvalue[tmp_phantomID].push_back(dE * unit_conversion_factor); // pJoule * 선원종류에 따른 단위(/hr, *cm2 등) -> 질량만 계산 안되어 있음(추후 나눔)
	}

	// ===== 각 phantomID 별로: ROI 생성→세분화→roiLocator 구성 =====
	const double targetAreaCm2 = 30.0; // 30 cm2이 ROI임(1, 10cm2 구하기 위한 여유 있는 ROI)
	const double refineFactor = 50.0; // 하나의 삼각형을 50개의 삼각형으로 subdivide -> 필요시 조정/입력화

	for (auto phantomID : pRt->m_Phantom_SequenceVector)
	{
		if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES ||
			pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;

		vtkIdType seedRaw = MaximumFacetDoseID[phantomID]; // 최대 선량 지점의 facetID
		vtkPolyData* skinRaw = SkinLayer_PolyData[phantomID];

		// --- [A] 전신(outer skin) 클린 메쉬 & roiLocator 준비 ---
		auto fullTriClean = HP_SkinDoseCalculate::ensure_tri_clean(skinRaw);

		// 전신 기하의 "일관된 셀 노멀" 생성 + 이것으로 fullLocator 구성 -> ROI 내 dEpoint인지 판단 + depth는 원본 면으로부터 판단
		auto fullNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
		fullNormals->SetInputData(fullTriClean);
		fullNormals->ConsistencyOn();       // 인접 셀 노멀 일관화
		fullNormals->AutoOrientNormalsOn(); // 자동 방향 정렬
		fullNormals->SplittingOff();        // 경계에서 노멀 쪼개지 않음(원하면 On)
		fullNormals->ComputePointNormalsOff();
		fullNormals->ComputeCellNormalsOn();
		fullNormals->Update();

		vtkPolyData* fullWithNormals = fullNormals->GetOutput();
		vtkDataArray* cellNormalsFull = fullWithNormals->GetCellData()->GetNormals();
		if (!cellNormalsFull) {
			theApp.SetMessageBox("No cell normals on full mesh");
			continue;
		}
		auto fullLocator = vtkSmartPointer<vtkCellLocator>::New();
		fullLocator->SetDataSet(fullWithNormals); // 노멀 포함된 동일 토폴로지
		fullLocator->CacheCellBoundsOn();
		fullLocator->BuildLocator();

		// --- [B] 전신 메쉬에서 30 cm² ROI (전신 인덱스 기준) 셋 구하기 ---
		std::array<double, 3> seedCenter = HP_SkinDoseCalculate::cell_center(skinRaw, seedRaw); // seedCenter는 재배열을 무시하고 최대선량지점(xyz 좌표)임
		vtkIdType seedOnFull = HP_SkinDoseCalculate::closest_cell_id_for_point(fullLocator, seedCenter.data());

		double achieved30 = 0.0;
		std::vector<vtkIdType> roi30_full_ids =
			HP_SkinDoseCalculate::grow_area_by_geodesic(fullTriClean, seedOnFull, /*targetAreaCm2*/30.0, achieved30); // full Locator에서의 roi 30 cm2 내의 facetID들의 집합
		std::unordered_set<vtkIdType> roi30_full_set(roi30_full_ids.begin(), roi30_full_ids.end()); // 실제 사용을 위한 unordered set

		// 세분화된 ROI polydata 생성
		vtkSmartPointer<vtkPolyData> roiBefore; // roi인데 subdivide 전(사용하지는 않음)
		std::map<vtkIdType, double> cellAreas; // subdivide된 roi의 각 facet area들
		auto roiSubdiv = HP_SkinDoseCalculate::ExportGeodesicROIForPhantom(
			phantomID,
			skinRaw,
			seedRaw,
			/*targetAreaCm2=*/30.0,
			/*refineFactor=*/50.0,
			/*roiBeforeSubdivision=*/&roiBefore,
			&cellAreas
		);

		// 세분화된 ROI Loacator 할당
		auto roiLocator = vtkSmartPointer<vtkCellLocator>::New();
		roiLocator->SetDataSet(roiSubdiv);
		roiLocator->CacheCellBoundsOn();
		roiLocator->BuildLocator();

		// 세분화 roi에서 seed 검증용
		vtkIdType seedOnSubdiv =
			HP_SkinDoseCalculate::closest_cell_id_for_point(roiLocator, seedCenter.data());
		if (seedOnSubdiv < 0) {
			theApp.SetMessageBox("[HP] seed on subdiv not found. skip phantom " + QString::number(phantomID));
			continue;
		}

		// 지오데식으로 1 cm² / 10 cm² facetID 집합 얻기
		std::vector<vtkIdType> patchIDs_1cm2, patchIDs_10cm2;
		double achieved1 = 0.0, achieved10 = 0.0;

		HP_SkinDoseCalculate::geodesic_dual_area_region(
			roiSubdiv, seedOnSubdiv,
			/*areaTarget1*/ 1.0,
			/*areaTarget2*/ 10.0,
			/*outIDs1*/ patchIDs_1cm2,
			/*outIDs2*/ patchIDs_10cm2,
			/*achieved1*/ &achieved1,
			/*achieved2*/ &achieved10
		);
		
		// 빠른 전처리: ROI AABB를 피부 두께(+마진) 패딩해서 안일 때만 전신 locator를 태우자
		double roiBounds[6];
		roiSubdiv->GetBounds(roiBounds);
		const double thickness_cm = SkinAverageDepth[phantomID]; // 약 0.05~0.20 cm (500 um ~ 2000um)
		const double safety_cm = 0.5;  // 여유(5000 um)
		const double pad = thickness_cm * 1.5 + safety_cm; // 1.5배 마진 + 여유
		auto insideROIPaddedBox = [&](const double p[3])->bool {
			return (p[0] >= roiBounds[0] - pad && p[0] <= roiBounds[1] + pad &&
				p[1] >= roiBounds[2] - pad && p[1] <= roiBounds[3] + pad &&
				p[2] >= roiBounds[4] - pad && p[2] <= roiBounds[5] + pad);
		};
		
		for (int DEpointID = 0; DEpointID < HP_SkinDEpoint[phantomID].size(); DEpointID++)
		{
			const double p[3] = { HP_SkinDEpoint[phantomID][DEpointID][0], HP_SkinDEpoint[phantomID][DEpointID][1], HP_SkinDEpoint[phantomID][DEpointID][2] };
			const double dE = HP_SkinDEvalue[phantomID][DEpointID];

			// (성능) ROI 박스 바깥 점은 스킵
			if (!insideROIPaddedBox(p)) continue;
			//ofsInAABB << p[0] << ' ' << p[1] << ' ' << p[2] << '\n'; // 디버그

			// 1) 전신 기준 최근접 facet
			double fullClosest[3]; vtkIdType fullFacetId = -1; int subId = -1; double dist2 = 0.0;
			fullLocator->FindClosestPoint(p, fullClosest, fullFacetId, subId, dist2);

			// 2) 전신 30 cm² ROI 집합에 속하지 않으면 스킵 → ROI 밖 점 제거
			if (roi30_full_set.find(fullFacetId) == roi30_full_set.end()) continue;
			//ofsInROI << p[0] << ' ' << p[1] << ' ' << p[2] << ' ' << fullFacetId << '\n'; // 디버그 

			//// 3) 세분화 ROI에서 facet/깊이 계산 (정밀 매핑) -> 이걸로 할지 깊이 계산을 full phantom으롭부터 할지...
			//vtkIdType facetId = -1; double depth_cm = 0.0; double closest[3] = { 0,0,0 };
			//HP_SkinDoseCalculate::find_closest_facet_and_depth(
			//	roiLocator, roiSubdiv, p,
			//	/*outCellId*/ facetId,
			//	/*outDepth*/  depth_cm,
			//	/*outClosestPoint*/ closest,
			//	/*signedDepth*/ false // 부호 깊이 필요시 true + 노멀 방향 정의
			//);

			// 세분화 ROI에서 facetID 매핑
			double closest_subdiv[3]; vtkIdType facetId = -1; int subId_sub = -1; double dist2_sub = 0.0;
			roiLocator->FindClosestPoint(p, closest_subdiv, facetId, subId_sub, dist2_sub);

			// 깊이 측정은 전신 기하(Full) 기준으로 부호/절대값 계산
			double n_full[3]; cellNormalsFull->GetTuple(fullFacetId, n_full);
			// (VTK 노멀은 보통 정규화되어 있지만, 안전하게 재정규화 가능)
			double nn = std::sqrt(n_full[0] * n_full[0] + n_full[1] * n_full[1] + n_full[2] * n_full[2]);
			if (nn > 0) { n_full[0] /= nn; n_full[1] /= nn; n_full[2] /= nn; }

			double v_to_surface[3] = { p[0] - fullClosest[0], p[1] - fullClosest[1], p[2] - fullClosest[2] };
			double depth_signed_cm = v_to_surface[0] * n_full[0] + v_to_surface[1] * n_full[1] + v_to_surface[2] * n_full[2];
			double depth_cm = std::fabs(depth_signed_cm); // binning/누적에는 절대값 사용

			// 누적: 전체 ROI(30 cm² subdiv) 기준 facet 누적
			int depth_grid_ID = HP_SkinDepth_to_Index(phantomID, depth_cm);
			HP_FacetDE[phantomID][facetId] += dE;
			HP_FacetDE_Depth[phantomID][facetId][depth_grid_ID] += dE;
			if (depth_cm >= 0.005 && depth_cm <= 0.010) { // 50–100 µm (0.005–0.010 cm)
				HP_FacetDE_50_100um[phantomID][facetId] += dE;
			}

		}

		// === 1cm² / 10cm² 패치의 총 dE와 총 면적 계산 ===
		double sumDE_1cm2 = 0.0;
		double sumArea_1cm2 = 0.0;
		for (auto cid : patchIDs_1cm2) {
			auto itDE = HP_FacetDE_50_100um[phantomID].find(cid);
			auto itArea = cellAreas.find(cid);

			if (itDE != HP_FacetDE_50_100um[phantomID].end()) sumDE_1cm2 += itDE->second;
			if (itArea != cellAreas.end())             sumArea_1cm2 += itArea->second;
		}

		double sumDE_10cm2 = 0.0;
		double sumArea_10cm2 = 0.0;
		for (auto cid : patchIDs_10cm2) {
			auto itDE = HP_FacetDE_50_100um[phantomID].find(cid);
			auto itArea = cellAreas.find(cid);

			if (itDE != HP_FacetDE_50_100um[phantomID].end()) sumDE_10cm2 += itDE->second;
			if (itArea != cellAreas.end())             sumArea_10cm2 += itArea->second;
		}

		// 1cm2, 10cm2 최대선량지점 및 선량값 계산 - 그래프제작용 -> 현재 HP가 아니어도 처리됨
		//HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[0]);
		//HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[1]);
		//HP_1cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[2]);
		//HP_1cm2_MaximumDose_GraphInfo[phantomID] = sumDE_1cm2 / (sumArea_1cm2 * (50. / 10000) * SkinDenstiy[phantomID] / 1.0E+3) / event_count; // dE / mass(area * 50 um * density)
		//// pGy per 선원종류에 따른 단위로 환산됨
		//HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[0]);
		//HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[1]);
		//HP_10cm2_MaximumLocation_GraphInfo[phantomID].push_back(seedCenter[2]);
		//HP_10cm2_MaximumDose_GraphInfo[phantomID] = sumDE_10cm2 / (sumArea_10cm2 * (50. / 10000) * SkinDenstiy[phantomID] / 1.0E+3) / event_count; // dE / mass(area * 50 um * density)
		//// pGy per 선원종류에 따른 단위로 환산됨

		// 1cm2 깊이분포 계산
		std::map<int, double> map_DE_cumulation_1cm2; // 1cm2에 대하여 임시 map에 누적		
		for (int i = HP_SkinDepth_Interval / 2; i <= static_cast<int>(SkinAverageDepth[phantomID] * 10000); i += HP_SkinDepth_Interval) // map_DE_cumulation_1cm2 초기화, *10000->cm to um, 		
		{
			map_DE_cumulation_1cm2[i] = 0.0;
		}
		for (auto itr_FacetID : patchIDs_1cm2) // 1cm2 최대 선량을 갖는 facet ID들에 대해서 순환
		{
			for (auto itr_facetDE_depth : HP_FacetDE_Depth[phantomID][itr_FacetID]) // itr_facetDE_depth.first = depth_grid_id, itr_facetDE_depth.second = 누적 DE
			{
				map_DE_cumulation_1cm2[itr_facetDE_depth.first] += itr_facetDE_depth.second; // 모든 depth grid ID마다 해당하는 깊이의 facetID별 detph dose를 누적해서 더함
			}
		}
		for (auto itr : map_DE_cumulation_1cm2) // 누적한 임시 dE map에서 
		{
			HP_1cm2_Depth_GraphInfo[phantomID].push_back({ itr.first, itr.second / (sumArea_1cm2 * (HP_SkinDepth_Interval / 10000.) * SkinDenstiy[phantomID] / 1.0E+3) / event_count }); //질량 나누기 1000 -> g to kg / 깊이 나누기 10000->um to cm / 단위: pGy / nps * 선원종류에 따른 단위 (/hr, *cm2 등)
		}

		// 10cm2 깊이분포 계산
		std::map<int, double> map_DE_cumulation_10cm2; // 10cm2에 대하여 임시 map에 누적
		for (int i = HP_SkinDepth_Interval / 2; i <= static_cast<int>(SkinAverageDepth[phantomID] * 10000); i += HP_SkinDepth_Interval) // map_DE_cumulation_10cm2 초기화, *10000-> cm to um, 		
		{
			map_DE_cumulation_10cm2[i] = 0.0;
		}
		for (auto itr_FacetID : patchIDs_10cm2) // 10cm2 최대 선량을 갖는 facet ID들에 대해서 순환
		{
			for (auto itr_facetDE_depth : HP_FacetDE_Depth[phantomID][itr_FacetID]) // itr_facetDE_depth.first = depth_grid_id, itr_facetDE_depth.second = 누적 DE
			{
				map_DE_cumulation_10cm2[itr_facetDE_depth.first] += itr_facetDE_depth.second;
			}
		}
		for (auto itr : map_DE_cumulation_10cm2) // 누적한 임시 dE map에서 
		{
			HP_10cm2_Depth_GraphInfo[phantomID].push_back({ itr.first, itr.second / (sumArea_10cm2 * (HP_SkinDepth_Interval / 10000.) * SkinDenstiy[phantomID] / 1.0E+3) / event_count }); //질량 나누기 1000 -> g to kg / 깊이 나누기 10000->um to cm / 단위: pGy / nps * 선원종류에 따른 단위 (/hr, *cm2 등)
		}
	}
}

int  ETHuman3DApp::HP_SkinDepth_to_Index(int phantomID, double input_depth)
{
	int skin_total_depth = SkinAverageDepth[phantomID] * 10000; // to um
	double dE_depth = input_depth * 10000; // to um

	int index = static_cast<int>(dE_depth) / HP_SkinDepth_Interval;
	int changed_index = HP_SkinDepth_Interval * 0.5 + index * HP_SkinDepth_Interval;

	//while (changed_index > skin_total_depth) changed_index -= HP_SkinDepth_Interval;// 자기자신이 grid이면(즉, interval 20 기준으로 850, 870 등) 해당값은 사용, 해당 값을 0.1이라도 넘으면 last grid로 합산 (일단은 피부두께 지키기)

	return changed_index;
}
void ETHuman3DApp::ResultLoad_OrganDose_OutputPanel(std::stringstream &ss)
{
	std::string dump, organ, dose, err;
	int cnt(0);
		for (auto phantomID : pRt->m_Phantom_SequenceVector)
		{
			if (pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_DUMMY] == pRt->E_PHANTOMDUMMY_YES ||
				pRt->m_Phantom_MainInfo[phantomID][pRt->E_PHANTOMMAININFO_CATEGORY] == pRt->E_PHANTOMCATEGORY_AIR) continue;
			while (ss >> dump)
			{
				if (dump == "Relative_error") 
				{
					for (int i = 0; i < theApp.pRt->OrganNameList.size(); i++)
					{
						ss >> organ >> dose >> err;
						OutputDose_data[phantomID][i][0] = QString::fromStdString(organ); // 0 -> organ name
						OutputDose_data[phantomID][i][1] = QString::fromStdString(formatToThreeDecimalPlaces(dose)); // 1-> dose
					}
					break;
				}
			}
		}
		
		//  선택되어 있는 팬텀에 대해서 저장한 데이터 output 출력 진행
		for (int i = 0; i < theApp.pRt->OrganNameList.size(); i++)
		{
			theApp.pRt->OutputPanel_DoseResult_QLineEdit[OutputDose_data[pRt->OutputPhantomSelectedIndex][i][0]]->setText(OutputDose_data[theApp.pRt->OutputPhantomSelectedIndex][i][1]);
		}
		UpdatePhantom_ActorHighlighted(theApp.pRt->OutputPhantomSelectedIndex);
			
	theApp.pRt->b_IsOutputResultLoaded = true;
}
void ETHuman3DApp::LoadOutputData_AirKerma(std::stringstream &ss)
{

}	
SIZE_T ETHuman3DApp::GetProcessMemoryUsage()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	HANDLE hProcess = GetCurrentProcess();
	if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc))) {
		return pmc.WorkingSetSize / 1024 / 1024; // Returns memory usage in MB
	}
	return 0; // Return 0 if unable to retrieve memory usage
}

// Not used
void ETHuman3DApp::combineTetrahedronFiles(const std::string& nodeFile1, const std::string& nodeFile2, const std::string& eleFile1, const std::string& eleFile2, const std::string& eleFileOutput, const std::string& nodeFileOutput)
{
	struct Point {
		double x, y, z;
	};

	struct Tetrahedron {
		int p1, p2, p3, p4;
	};

	// Vectors to store points and tetrahedra
	std::vector<Point> points1, points2;
	std::vector<Tetrahedron> tetrahedra1, tetrahedra2;
	std::vector<int> Mat1, Mat2;

	// Read node files for L and R
	int numPoints, dim, attributes, boundaryMarkers;

	std::ifstream nodeFile_1(nodeFile1);
	nodeFile_1 >> numPoints >> dim >> attributes >> boundaryMarkers;
	for (int i = 0; i < numPoints; ++i)
	{
		int id;
		Point point;
		nodeFile_1 >> id >> point.x >> point.y >> point.z;
		points1.push_back(point);
	}
	nodeFile_1.close();

	std::ifstream nodeFile_2(nodeFile2);
	nodeFile_2 >> numPoints >> dim >> attributes >> boundaryMarkers;
	for (int i = 0; i < numPoints; ++i)
	{
		int id;
		Point point;
		nodeFile_2 >> id >> point.x >> point.y >> point.z;
		points2.push_back(point);
	}
	nodeFile_2.close();

	// Read ele files for L and R
	int numTetrahedra, nodesPerTetrahedron, attributesTet;

	std::ifstream eleFile_1(eleFile1);
	eleFile_1 >> numTetrahedra >> nodesPerTetrahedron >> attributesTet;

	for (int i = 0; i < numTetrahedra; ++i) {
		int id;
		Tetrahedron tetrahedron;
		int MatID;
		eleFile_1 >> id >> tetrahedron.p1 >> tetrahedron.p2 >> tetrahedron.p3 >> tetrahedron.p4 >> MatID;
		tetrahedra1.push_back(tetrahedron);
		Mat1.push_back(MatID);
	}
	eleFile_1.close();

	std::ifstream eleFile_2(eleFile2);
	eleFile_2 >> numTetrahedra >> nodesPerTetrahedron >> attributesTet;

	for (int i = 0; i < numTetrahedra; ++i) {
		int id;
		Tetrahedron tetrahedron;
		int MatID;
		eleFile_2 >> id >> tetrahedron.p1 >> tetrahedron.p2 >> tetrahedron.p3 >> tetrahedron.p4 >> MatID;
		tetrahedra2.push_back(tetrahedron);
		Mat2.push_back(MatID);
	}
	eleFile_2.close();

	// Combining points and tetrahedra
	std::vector<Point> combinedPoints(points1);
	combinedPoints.insert(combinedPoints.end(), points2.begin(), points2.end());

	std::vector<Tetrahedron> combinedTetrahedra(tetrahedra1);
	std::vector<int> combinedMat(Mat1);
	int index = 0;
	for (const Tetrahedron& t : tetrahedra2) {
		Tetrahedron shiftedTetrahedron;
		shiftedTetrahedron.p1 = t.p1 + points1.size();
		shiftedTetrahedron.p2 = t.p2 + points1.size();
		shiftedTetrahedron.p3 = t.p3 + points1.size();
		shiftedTetrahedron.p4 = t.p4 + points1.size();
		combinedTetrahedra.push_back(shiftedTetrahedron);
		combinedMat.push_back(Mat2[index] * 10); // 10배 곱해서 달라지게 하기
		index++;
	}

	// Writing combined tet files
	std::string nodeFileName_combined = nodeFileOutput;
	std::string eleFileName_combined = eleFileOutput;

	std::ofstream nodeFile_combined(nodeFileName_combined);
	std::ofstream eleFile_combined(eleFileName_combined);

	nodeFile_combined << combinedPoints.size() << " 3 0 0" << std::endl;
	for (size_t i = 0; i < combinedPoints.size(); ++i)
	{
		nodeFile_combined << i << " " << combinedPoints[i].x << " " << combinedPoints[i].y << " " << combinedPoints[i].z << std::endl;
	}

	eleFile_combined << combinedTetrahedra.size() << " 4 0" << std::endl;
	for (size_t i = 0; i < combinedTetrahedra.size(); ++i) {
		eleFile_combined << i << " " << combinedTetrahedra[i].p1 << " " << combinedTetrahedra[i].p2
			<< " " << combinedTetrahedra[i].p3 << " " << combinedTetrahedra[i].p4 << " " << combinedMat[i] << std::endl;
	}
}

//************************************** Reconstruction Process **************************************//
void ETHuman3DApp::WriteSaveFile(std::string path)
{
	pRt->Write_TextResultFile_SimulationInfo(path);
}
void ETHuman3DApp::WriteExtdataFile(std::string path)
{
	Make_mcsee_File_extdata_previous(QString::fromStdString(path));
}
void ETHuman3DApp::Read_mcsee_File_previous(QString filepath)
{
	// 결합된 파일 열기
	QFile inputFile(filepath);
	if (!inputFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open input file for reading.";
		return;
	}

	QByteArray fileData = inputFile.readAll();
	inputFile.close();

	QByteArray delimiter_ipp = "-ipp ";
	QByteArray delimiter_ipn = "-ipn ";
	int start = 0;
	int end = 0;
	QString folderPath = QFileInfo(filepath).absolutePath();  // 파일이 위치하는 폴더 경로

	// 파일 분리 시작
	while (start < fileData.size())
	{
		// -ipp 구분자를 찾기
		if (fileData.indexOf(delimiter_ipp, start) != -1) {
			int delimiterPos = fileData.indexOf(delimiter_ipp, start);
			start = delimiterPos + delimiter_ipp.size();

			// 정수형 값 추출 (구분자 뒤에 있는 정수)
			int numStart = start;
			int numEnd = fileData.indexOf(" ", start);
			QByteArray intValue = fileData.mid(numStart, numEnd - numStart);
			QString phantomID = intValue;  // 파일명에 정수형 추가

			// 파일 이름과 확장자 설정
			QString newFileName = folderPath + "/ImportedPhantom_" + phantomID + ".ply";  // .ply 파일

			// 해당 데이터의 구간을 찾고 저장
			start = numEnd + 1;
			end = fileData.indexOf("-end ", start);

			QByteArray fileDataSegment = fileData.mid(start, end - start);

			QFile outputFile(newFileName);
			if (outputFile.open(QIODevice::WriteOnly)) {
				outputFile.write(fileDataSegment);  // 해당 데이터를 파일에 기록
				outputFile.close();
				qDebug() << newFileName << " saved successfully.";
			}
			else {
				qDebug() << "Failed to save file: " << newFileName;
			}

			// 다음 파일을 위해 위치 이동
			start = end + 5;  // `-end ` 이후의 시작 지점으로 이동
		}

		// -ipn 구분자를 찾기
		if (fileData.indexOf(delimiter_ipn, start) != -1) {
			int delimiterPos = fileData.indexOf(delimiter_ipn, start);
			start = delimiterPos + delimiter_ipn.size();

			// 정수형 값 추출 (구분자 뒤에 있는 정수)
			int numStart = start;
			int numEnd = fileData.indexOf(" ", start);
			QByteArray intValue = fileData.mid(numStart, numEnd - numStart);
			QString phantomID = intValue;  // 파일명에 정수형 추가

			// 파일 이름과 확장자 설정
			QString newFileName = folderPath + "/ImportedPhantom_" + phantomID + ".node";  // .node 파일

			// 해당 데이터의 구간을 찾고 저장
			start = numEnd + 1;
			end = fileData.indexOf("-end ", start);

			// 파일 데이터 추출
			QByteArray fileDataSegment = fileData.mid(start, end - start);		

			// 파일을 저장
			QFile outputFile(newFileName);
			if (outputFile.open(QIODevice::WriteOnly)) {
				outputFile.write(fileDataSegment);  // 해당 데이터를 파일에 기록
				outputFile.close();
				qDebug() << newFileName << " saved successfully.";
			}
			else {
				qDebug() << "Failed to save file: " << newFileName;
			}

			// 다음 파일을 위해 위치 이동
			start = end + 5;  // `-end ` 이후의 시작 지점으로 이동
		}

		// 더 이상 구분자가 없으면 종료
		if (fileData.indexOf(delimiter_ipp, start) == -1 && fileData.indexOf(delimiter_ipn, start) == -1) {
			break;  // 더 이상 구분자가 없다면 종료
		}
	}
}
//************************************** Actor Assembly Process **************************************//
void ETHuman3DApp::SetAssemblyOpacity(vtkAssembly* assembly, double value)
{
	vtkProp3DCollection* props = assembly->GetParts();
	props->InitTraversal();
	for (vtkIdType i = 0; i < props->GetNumberOfItems(); ++i) {
		vtkProp3D* prop = props->GetNextProp3D();
		vtkActor* actor = vtkActor::SafeDownCast(prop);
		if (actor) {
			// Set the color of the actor
			actor->GetProperty()->SetOpacity(actor->GetProperty()->GetOpacity()*value);			
		}
		// If the prop is another assembly, recursively set its actors' colors
		else if (vtkAssembly* subAssembly = vtkAssembly::SafeDownCast(prop)) {
			SetAssemblyOpacity(subAssembly, value);
		}
	}
}
//*************************************Memory Usage Expectation**************************************//
double ETHuman3DApp::GetTotalSystemMemoryGB()
{
#ifdef _WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return static_cast<double>(memInfo.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
	//return memInfo.ullTotalPhys / (1000ULL * 1000 * 1000);
#elif __linux__
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	return static_cast<double>(memInfo.totalram) * memInfo.mem_unit / (1024.0 * 1024.0 * 1024.0);
	//return (memInfo.totalram * memInfo.mem_unit) / (1000ULL * 1000 * 1000);
#else
	return 8; // default 8GB
#endif
}

bool ETHuman3DApp::isLowSpecMode()
{
	TotalSystemMemoryGB = GetTotalSystemMemoryGB();
	if (TotalSystemMemoryGB < 17.0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHuman3DApp::OldComputer()
{
	TotalSystemMemoryGB = GetTotalSystemMemoryGB();
	if (TotalSystemMemoryGB < 9.0)
	{
		return true;
	}
	else return false;
}

bool ETHuman3DApp::isLowMemoryState() const
{
	return LowMemoryState;
}

void ETHuman3DApp::setLowMemoryState(bool LowMemoryState_)
{
	LowMemoryState = LowMemoryState_;
}


// ETHuman3DApp theApp;  // <--- 이 줄을 삭제하거나 주석 처리하세요.