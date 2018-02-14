#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import_stl.h>

class MyVertex; class MyEdge; class MyFace;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
                            vcg::Use<MyEdge>     ::AsEdgeType,
                            vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags  >{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj,  vcg::face::VertexRef, vcg::face::BitFlags > {};
class MyEdge    : public vcg::Edge<   MyUsedTypes> {};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};

int main( int argc, char **argv )
{
  if(argc<2)
  {
      printf("Usage trimesh_base <meshfilename.stl>\n");
      return -1;
    }
  MyMesh m;
  int a = 2;
  if(vcg::tri::io::ImporterSTL<MyMesh>::Open(m,argv[1], a))
  {
      printf("Error reading file  %s\n",argv[1]);
      exit(0);
    }
  // vcg::tri::RequirePerVertexNormal(m);
  // vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(m);
  // printf("Input mesh  vn:%i fn:%i\n",m.VN(),m.FN());
  printf( "Mesh has %i vert and %i faces\n", m.VN(), m.FN() );

  vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
  printf( "after remove Duplicate Mesh has %i vert and %i faces\n", m.VN(), m.FN() );

  // int edgeManifNum = vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m,true);
  // printf( "Nonmanifold edges number %i vert and %i faces\n", m.VN(), m.FN() );

  return 0;
}
