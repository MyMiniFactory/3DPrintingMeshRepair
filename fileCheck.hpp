#ifndef FILECHECK_HPP
#define FILECHECK_HPP

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>
#include <vcg/complex/algorithms/inertia.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include <stdio.h>

class MyVertex; class MyFace;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
                            vcg::Use<MyFace>     ::AsFaceType>{};
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
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> > {};

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

int file_repair(MyMesh & mesh, int* results, int* repair_record);

#endif
