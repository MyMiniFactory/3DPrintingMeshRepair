#include <helloworld.hpp>

bool NoDegenratedFaces(MyMesh & mesh, int & numDuplicateFaces) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    bool RemoveDegenerateFlag=true;
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    numDuplicateFaces = beforeNumFaces - afterNumFaces;

    return (beforeNumFaces == afterNumFaces);
}

bool NoDuplicateFaces(MyMesh & mesh, int & numDuplicateFaces) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    vcg::tri::Clean<MyMesh>::RemoveDuplicateFace(mesh); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    numDuplicateFaces = beforeNumFaces - afterNumFaces;

    return (beforeNumFaces == afterNumFaces);
}

bool NoIntersectingFaces(MyMesh & mesh, int & numIntersectingFaces) { // change mesh in-place

    std::vector<MyFace *> IntersectingFaces;
    vcg::tri::Clean<MyMesh>::SelfIntersections(mesh, IntersectingFaces);

    numIntersectingFaces = static_cast<int>(IntersectingFaces.size());

    return (numIntersectingFaces > 0) ? false : true;
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

    int file_check(const std::string filepath, float merge_vertice=0) {

        int result = 0;

        printf("reading file  %s\n",filepath.c_str());

        MyMesh m;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(m, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }
        printf( "Mesh has %i vert and %i faces\n", m.VN(), m.FN() );

        if (merge_vertice > 0) {
            printf("mesh vertices before merge %i\n", m.VN());
            vcg::tri::Clean<MyMesh>::MergeCloseVertex(m, merge_vertice);
            printf("mesh vertices after merge %i\n", m.VN());
        }

        int numDegeneratedFaces;
        bool RemoveDegenerateFace = NoDegenratedFaces(m, numDegeneratedFaces);
        printf( "after remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );
        result++;

        int NumDuplicateFaces;
        NoDuplicateFaces(m, NumDuplicateFaces);

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

        std::vector<MyFace *> IntersectingFaces;
        vcg::tri::Clean<MyMesh>::SelfIntersections(m, IntersectingFaces);
        printf("Number of self intersection faces %i\n", static_cast<int>(IntersectingFaces.size()));

        printf("Good? %s %i \n", ( result == 5 )? "true" : "false", result);

        return result;
    }
}

#ifndef FILECHECK_TEST
int main( int argc, char **argv )
{
    printf( "start in main\n" );
    // std::string filepath = "./Bishop.stl";
    // std::string filepath = "/home/mmf159/Documents/vcg_learning/unittest/meshes/duplicateFaces.stl";
    // std::string filepath = "/home/mmf159/Downloads/wholething.stl";
    std::string filepath = argv[1];

    printf("----------------- file check -------------------\n");
    file_check(filepath.c_str());

    printf("----------------- file check 0.00001 -------------------\n");
    file_check(filepath.c_str(), 0.00001);

    printf("----------------- file check 0.0001 -------------------\n");
    file_check(filepath.c_str(), 0.0001);

    printf("----------------- file check 0.001 -------------------\n");
    file_check(filepath.c_str(), 0.001);

    // MyMesh mesh;
    // loadMesh(mesh, filepath);
    // NoDuplicateFaces(mesh);


    //printf("tiger\n");

    //std::vector<MyFace *> IntersectingFaces;
    //vcg::tri::Clean<MyMesh>::SelfIntersections(mesh, IntersectingFaces);
    //printf("Number of self intersection faces %lu\n", IntersectingFaces.size());

     //int counter = 1;
    //for (auto const& face: IntersectingFaces) {
        //auto v0 = face->cV(0)->cP();
        //auto v1 = face->cV(1)->cP();
        //auto v2 = face->cV(2)->cP();
        //printf("v %f %f %f \n", v0[0], v0[1], v0[2]);
        //printf("v %f %f %f \n", v1[0], v1[1], v1[2]);
        //printf("v %f %f %f \n", v2[0], v2[1], v2[2]);
        //printf("f %i %i %i \n", counter, counter+1, counter+2);
        //printf("\n");
        //counter += 3;
    //}

    return 0;
}
#endif
