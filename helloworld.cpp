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

extern "C" {
    void file_check(const char* filepath) {

        MyMesh m;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(m, filepath,  a))
        {
            printf("Error reading file  %s\n",filepath);
            exit(0);
        }
        printf( "Mesh has %i vert and %i faces\n", m.VN(), m.FN() );


        bool RemoveDegenerateFlag=true;
        vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge
        printf( "after remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );

        bool NumDuplicateFaces = vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
        printf( "removed %i duplicate faces\n", NumDuplicateFaces );
        printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );

        vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);

        printf( "IsWaterTight %s \n", vcg::tri::Clean<MyMesh>::IsWaterTight(m) ? "True" : "False");
        printf( "IsCoherentlyOrientedMesh %s \n", vcg::tri::Clean<MyMesh>::IsCoherentlyOrientedMesh(m) ? "True" : "False");

        vcg::tri::Inertia<MyMesh> Ib(m);

        printf( "Volume %f \n",Ib.Mass());

        // std::vector<MyFace *> IntersFaces;
        // vcg::tri::Clean<MyMesh>::SelfIntersections(m, IntersFaces);
    }
}

int main( int argc, char **argv )
{
    printf( "start in main\n" );
    std::string filepath = "./20150803-usb-duck-1.stl";
    file_check(filepath.c_str());

    return 0;
}
