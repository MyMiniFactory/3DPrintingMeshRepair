#ifndef FILECHECK_HPP
#define FILECHECK_HPP

// #include <vcg/complex/complex.h>
// #include <vcg/complex/algorithms/closest.h>
// #include <vcg/space/index/grid_static_ptr.h>
// #include <vcg/space/index/spatial_hashing.h>
// #include <vcg/complex/algorithms/update/normal.h>
#include <vcg/space/triangle3.h>

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/export_stl.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/hole.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <stdio.h>
#include <stdlib.h>

class MyVertex; class MyFace; class MyEdge;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
                            vcg::Use<MyFace>     ::AsFaceType,
                            vcg::Use<MyEdge>     ::AsEdgeType>{};

class MyVertex  : public vcg::Vertex< MyUsedTypes,
    vcg::vertex::Coord3f,
    vcg::vertex::Normal3f,
    vcg::vertex::Mark,
    vcg::vertex::BitFlags  >{};

class MyFace    : public vcg::Face< MyUsedTypes,
    vcg::face::FFAdj,
    vcg::face::Normal3f,
    vcg::face::VertexRef,
    vcg::face::Mark,
    vcg::face::BitFlags > {};

class MyEdge: public vcg::Edge< MyUsedTypes,
    vcg::edge::VertexRef > {};

class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge> > {};

typedef vcg::tri::Clean<MyMesh> Clean;


bool loadMesh(MyMesh & mesh, const std::string filepath);

float Volume(MyMesh & mesh);
float Area(MyMesh & mesh);

unsigned int NumDegenratedFaces(MyMesh & mesh);
unsigned int NumDuplicateFaces(MyMesh & mesh);
unsigned int NumIntersectingFaces(MyMesh & mesh);

bool IsWaterTight(MyMesh & mesh);
bool IsCoherentlyOrientedMesh(MyMesh & mesh);
bool IsPositiveVolume(MyMesh & mesh);
unsigned int NumShell(MyMesh & mesh);
bool IsGoodMesh(int* results);

class checkResult_t {

    public:

    unsigned int version; // 0 version number
    unsigned int n_faces; // 1 face number
    unsigned int n_vertices; // 2 vertices number
    unsigned int n_degen_faces; // 3 number of degenerated faces
    unsigned int n_duplicate_faces; // 4 number of duplicate faces
    bool is_watertight; // 5 is watertight
    bool is_coherently_oriented; // 6 is coherently oriented
    bool is_positive_volume; // 7 is positive volume
    unsigned int n_intersecting_faces; // 8 number of intersecting faces
    unsigned int n_shells; // 9 number of connected components
    unsigned int n_non_manifold_edges; //10 number of non manifold edges
    unsigned int n_holes; //11 number of holes
    bool is_good_mesh; //11 good or bad

    // file stat
    float xmin; float xmax;
    float ymin; float ymax;
    float zmin; float zmax;
    float area; float volume;

    void output_report(FILE* report) const {
        printf("---------------writing report\n");

        assert(version == 4);

        std::fprintf(report, "%d num_version\n",                    version);
        std::fprintf(report, "%d num_face\n",                       n_faces);
        std::fprintf(report, "%d num_vertices\n",                   n_vertices);
        std::fprintf(report, "%d num_degenerated_faces_removed\n",  n_degen_faces);
        std::fprintf(report, "%d num_duplicated_faces_removed\n",   n_duplicate_faces);
        std::fprintf(report, "%d is_watertight\n",                  is_watertight);
        std::fprintf(report, "%d is_coherently_oriented\n",         is_coherently_oriented);
        std::fprintf(report, "%d is_positive_volume\n",             is_positive_volume);
        std::fprintf(report, "%d num_intersecting_faces\n",         n_intersecting_faces);
        std::fprintf(report, "%d num_shells\n",                     n_shells);
        std::fprintf(report, "%d num_non_manifold_edges\n",         n_non_manifold_edges);
        std::fprintf(report, "%d num_holes\n",                      n_holes);
        std::fprintf(report, "%d is_good_mesh\n",                   is_good_mesh);
        std::fprintf(report, "%f min_x\n",                          xmin);
        std::fprintf(report, "%f max_x\n",                          xmax);
        std::fprintf(report, "%f min_y\n",                          ymin);
        std::fprintf(report, "%f max_y\n",                          ymax);
        std::fprintf(report, "%f min_z\n",                          zmin);
        std::fprintf(report, "%f max_z\n",                          zmax);
        std::fprintf(report, "%f area\n",                           area);
        std::fprintf(report, "%f volume\n",                         volume);
    }

};

void Boundary(MyMesh & mesh, checkResult_t& boundary);

class repairRecord_t {

    public:

    unsigned int version; // 0 repair version
    bool does_fix_coherently_oriented; // 1 fix CoherentlyOriented
    bool does_fix_positive_volume; // 2 fix not Positive Volume
    bool does_union = false; // 3 attempt to fix the union, require human check
    unsigned int n_non_manif_f_removed = 0; // 4 remove non manifold faces
    bool does_fix_hole = false; // 5 fix hole
    bool is_good_repair = false; // 6 is good repair

    void output_report(FILE* report) const {
        assert(version == 1);
        std::fprintf(report, "%d repair_version\n",            version);
        std::fprintf(report, "%d does_make_coherent_orient\n", does_fix_coherently_oriented);
        std::fprintf(report, "%d does_flip_normal_outside\n",  does_fix_positive_volume);
        std::fprintf(report, "%d does_union\n",                does_union);
        std::fprintf(report, "%d does_rm_non_manif_faces\n",   n_non_manif_f_removed);
        std::fprintf(report, "%d does_hole_fix\n",             does_fix_hole);
        std::fprintf(report, "%d is_good_repair\n",            is_good_repair);
    }
};

checkResult_t file_check(MyMesh & m);

extern "C" {
    void file_check(const std::string filepath, int* results);
}

bool DoesFlipNormalOutside(MyMesh & mesh,
    bool isWaterTight, bool isCoherentlyOriented, bool isPositiveVolume);
bool DoesMakeCoherentlyOriented(MyMesh & mesh,
    bool isWaterTight, bool isCoherentlyOriented);
std::vector<std::vector<vcg::Point3<float>>> CountHoles(MyMesh & m);
void repair_hole(
    MyMesh & mesh, std::vector<std::vector<vcg::Point3<float>>> vpss
);

repairRecord_t file_repair(
    MyMesh & mesh, const checkResult_t check_r, const std::string repaired_path
);

repairRecord_t file_repair_then_check(
    MyMesh & mesh, const checkResult_t check_r, const std::string repaired_path,
    FILE* report = stdout
);
#endif
