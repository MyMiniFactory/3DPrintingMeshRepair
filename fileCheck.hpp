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


bool loadMesh(MyMesh & mesh, const std::string filepath);

void Boundary(MyMesh & mesh, float* boundary);
bool NoDegenratedFaces(MyMesh & mesh, int & numDegeneratedFaces);
bool NoDuplicateFaces(MyMesh & mesh, int & numDuplicateFaces);
bool NoIntersectingFaces(MyMesh & mesh, int & numIntersectingFaces);
bool IsWaterTight(MyMesh & mesh);
bool IsCoherentlyOrientedMesh(MyMesh & mesh);
bool IsPositiveVolume(MyMesh & mesh);
bool IsSingleShell(MyMesh & mesh, int & numConnectedComponents);

void file_check(MyMesh & m, int* results, float* boundary);

extern "C" {
    void file_check(const std::string filepath, int* results, float* boundary);
}

bool DoesFlipNormalOutside(MyMesh & mesh,
        bool isWaterTight, bool isCoherentlyOriented, bool isPositiveVolume);
bool DoesMakeCoherentlyOriented(MyMesh & mesh,
        bool isWaterTight, bool isCoherentlyOriented);
std::vector<std::vector<vcg::Point3<float>>> CountHoles(MyMesh & m);

int file_repair(MyMesh & mesh, int* results, int* repair_record,
        const std::string repaired_path);

#endif
