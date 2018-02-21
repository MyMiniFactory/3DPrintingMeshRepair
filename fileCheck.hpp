#ifndef HELLOWORLD_HPP
#define HELLOWORLD_HPP

#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import_stl.h>
#include <vcg/complex/algorithms/inertia.h>
#include <iostream>
#include <fstream>

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

bool NoDegenratedFaces(MyMesh & mesh, int & numDegeneratedFaces);
bool NoDuplicateFaces(MyMesh & mesh, int & numDuplicateFaces);
bool NoIntersectingFaces(MyMesh & mesh, int & numIntersectingFaces);
bool IsWaterTight(MyMesh & mesh);
bool IsCoherentlyOrientedMesh(MyMesh & mesh);
bool IsPositiveVolume(MyMesh & mesh);

#endif
