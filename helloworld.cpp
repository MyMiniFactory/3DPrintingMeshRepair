#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/clean.h>
#include <wrap/io_trimesh/import_stl.h>
#include <vcg/complex/algorithms/inertia.h>

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
    vcg::face::BitFlags > {};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> > {};

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


  bool RemoveDegenerateFlag=true;
  vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge
  printf( "after remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );

  bool NumDuplicateFaces = vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
  printf( "removed %i duplicate faces\n", NumDuplicateFaces );
  printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );

  vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
  // int edgeNonManifNum = vcg::tri::Clean<MyMesh>::CountNonManifoldEdgeFF(m,true);
  // printf( "number of NonManif edges %i \n", edgeNonManifNum );

  printf( "IsWaterTight %s \n", vcg::tri::Clean<MyMesh>::IsWaterTight(m) ? "True" : "False");
  printf( "IsCoherentlyOrientedMesh %s \n", vcg::tri::Clean<MyMesh>::IsCoherentlyOrientedMesh(m) ? "True" : "False");

  vcg::tri::Inertia<MyMesh> Ib(m);

  printf( "Volume %f \n",Ib.Mass());

  std::vector<MyFace *> IntersFaces;
  vcg::tri::Clean<MyMesh>::SelfIntersections(m, IntersFaces);

  return 0;
}
