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
