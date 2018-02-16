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
    vcg::face::BitFlags > {};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> > {};

extern "C" {
    void print_file(const std::string filepath) {
        printf("print_file function reading file  %s\n",filepath.c_str());

        std::ifstream f(filepath.c_str());
        if (f.is_open())
            std::cout << f.rdbuf() << std::endl;
        else
            std::cout << "file not open\n";

        std::cout << "finish\n";
    }

    int file_check(const std::string filepath) {

        int result;

        printf("reading file  %s\n",filepath.c_str());

        MyMesh m;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(m, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }
        printf( "Mesh has %i vert and %i faces\n", m.VN(), m.FN() );


        bool RemoveDegenerateFlag=true;
        vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge
        printf( "after remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );
        result++;
        printf("%i", result);

        bool NumDuplicateFaces = vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
        printf( "removed %i duplicate faces\n", NumDuplicateFaces );
        printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );
        result++;

        vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);

        bool isWaterTight = vcg::tri::Clean<MyMesh>::IsWaterTight(m);
        printf( "IsWaterTight %s \n", isWaterTight ? "True" : "False");
        if (isWaterTight)
            result+=1;
        else
            return result;

        bool isCoherentlyOrientedMesh = vcg::tri::Clean<MyMesh>::IsCoherentlyOrientedMesh(m);
        printf( "IsCoherentlyOrientedMesh %s \n", isCoherentlyOrientedMesh ? "True" : "False");

        if (isCoherentlyOrientedMesh)
            result+=1;
        else
            return result;


        vcg::tri::Inertia<MyMesh> Ib(m);
        printf( "Volume %f \n",Ib.Mass());
        bool isVolumeBiggerThanZero = Ib.Mass() > 0. ;

        if (isVolumeBiggerThanZero)
            result+=1;
        else
            return result;

        // std::vector<MyFace *> IntersFaces;
        // vcg::tri::Clean<MyMesh>::SelfIntersections(m, IntersFaces);

        return result;
    }
}

int main( int argc, char **argv )
{
    printf( "start in main\n" );
    std::string filepath = "./test.txt";
    file_check(filepath.c_str());
    // print_file(filepath);

    return 0;
}
