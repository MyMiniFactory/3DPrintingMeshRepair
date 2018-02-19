#include <helloworld.hpp>

bool NoDengeratedFaces(MyMesh & mesh) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    bool RemoveDegenerateFlag=true;
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    return (beforeNumFaces == afterNumFaces);
}

bool NoDuplicateFaces(MyMesh & mesh) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(mesh); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    return (beforeNumFaces == afterNumFaces);
}

bool IsWaterTight(MyMesh & mesh) {
    return vcg::tri::Clean<MyMesh>::IsWaterTight(mesh);
}


bool IsCoherentlyOrientedMesh(MyMesh & mesh) {
    return vcg::tri::Clean<MyMesh>::IsCoherentlyOrientedMesh(mesh);
}

bool IsPositiveVolume(MyMesh & mesh) {
    vcg::tri::Inertia<MyMesh> Ib(mesh);
    return Ib.Mass() > 0. ;
}

bool loadMesh(MyMesh & mesh, const std::string filepath) {

    int a = 2; // TODO: understand what this is
    if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
    {
        // printf("Error reading file  %s\n", filepath.c_str());
        return false;
    }

    bool RemoveDegenerateFlag=false;
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag);

    return true;
}

extern "C" {

    //void print_file(const std::string filepath) {
        //printf("print_file function reading file  %s\n",filepath.c_str());

        //std::ifstream f(filepath.c_str());
        //if (f.is_open())
            //std::cout << f.rdbuf() << std::endl;
        //else
            //std::cout << "file not open\n";

        //std::cout << "finish\n";
    //}

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


        // bool RemoveDegenerateFlag=true;
        // vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

        bool RemoveDegenerateFace = NoDengeratedFaces(m);
        printf( "after remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );
        result++;
        printf("%i", result);

        bool NumDuplicateFaces = vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(m);
        printf( "removed %i duplicate faces\n", NumDuplicateFaces );
        printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );
        result++;

        vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);

        bool isWaterTight = IsWaterTight(m);
        printf( "IsWaterTight %s \n", isWaterTight ? "True" : "False");
        if (isWaterTight)
            result+=1;
        else
            return result;

        bool isCoherentlyOrientedMesh = IsCoherentlyOrientedMesh(m);
        printf( "IsCoherentlyOrientedMesh %s \n", isCoherentlyOrientedMesh ? "True" : "False");

        if (isCoherentlyOrientedMesh)
            result+=1;
        else
            return result;


        bool isPositiveVolume = IsPositiveVolume(m);

        if (isPositiveVolume)
            result+=1;
        else
            return result;

        // std::vector<MyFace *> IntersFaces;
        // vcg::tri::Clean<MyMesh>::SelfIntersections(m, IntersFaces);

        return result;
    }
}

#ifndef FILECHECK_TEST
int main( int argc, char **argv )
{
    printf( "start in main\n" );
    // std::string filepath = "./Bishop.stl";
    std::string filepath = "/home/mmf159/Documents/vcg_learning/unittest/meshes/duplicateFaces.stl";
    file_check(filepath.c_str());
    // print_file(filepath);

    return 0;
}
#endif
