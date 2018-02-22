#include <fileCheck.hpp>
#include <array>

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

    void file_check(const std::string filepath, array<int, 7> & results,  float merge_vertice=0) {

        std::ostringstream stringStream;

        printf("reading file  %s\n",filepath.c_str());
        printf("tiger test delete me\n");

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
        results[1] = numDegeneratedFaces;
        stringStream << "numDegeneratedFaces" << std::to_string(numDegeneratedFaces) << ";";
        // std::cout << stringStream <<"\n test";

        int numDuplicateFaces;
        NoDuplicateFaces(m, numDuplicateFaces);

        printf( "removed %i duplicate faces\n", numDuplicateFaces );
        printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );
        results[2] = numDuplicateFaces;

        vcg::tri::UpdateTopology<MyMesh>::FaceFace(m); // require for isWaterTight

        bool isWaterTight = IsWaterTight(m);
        printf( "Is WaterTight %s \n", isWaterTight ? "True" : "False");
        results[3] = isWaterTight;
        // if (not isWaterTight) return stringStream;

        bool isCoherentlyOriented = IsCoherentlyOrientedMesh(m);
        printf( "Is Coherently OrientedMesh %s \n", isCoherentlyOriented ? "True" : "False");
        results[4] = isCoherentlyOriented;
        // if (not isCoherentlyOriented) return stringStream;

        bool isPositiveVolume = IsPositiveVolume(m);
        printf( "Is Positive Volume %s \n", isPositiveVolume ? "True" : "False");
        results[5] = isPositiveVolume;
        // if (not isPositiveVolume) return stringStream;

        int numIntersectingFaces;
        NoIntersectingFaces(m, numIntersectingFaces);
        printf("Number of self intersection faces %i\n", numIntersectingFaces);
        results[6] = numIntersectingFaces;

        printf("Good\n");

        // return stringStream.str().c_str();
    }
}

#ifndef FILECHECK_TEST
int main( int argc, char *argv[] )
{
    printf( "start in main\n" );
    // std::string filepath = "./Bishop.stl";
    // std::string filepath = "/home/mmf159/Documents/vcg_learning/unittest/meshes/duplicateFaces.stl";
    // std::string filepath = "/home/mmf159/Downloads/wholething.stl";
    if (argc < 2) {
        printf("please provide path to stl file\n");
        return 1;
    }

    std::string filepath = argv[1];

    printf("----------------- file check -------------------\n");
    std::array<int, 7>results = {
        0, // version number
        -1, // number of degenerated faces
        -1, // number of duplicate faces
        -1, // is watertight
        -1, // is coherently oriented
        -1, // is positive volume
        -1 // number of intersecting faces
    };

    file_check(filepath.c_str(), results);

    printf("%i version number\n", results[0]);
    printf("%i num of degen faces\n", results[1]);
    printf("%i num of dup faces\n", results[2]);
    printf("%i is watertight\n", results[3]);
    printf("%i is coherent oriented\n", results[4]);
    printf("%i is positive volume\n", results[5]);
    printf("%i num of intersecting faces\n", results[6]);

    // printf("----------------- file check 0.00001 -------------------\n");
    // file_check(filepath.c_str(), 0.00001);

    // printf("----------------- file check 0.0001 -------------------\n");
    // file_check(filepath.c_str(), 0.0001);

    // printf("----------------- file check 0.001 -------------------\n");
    // file_check(filepath.c_str(), 0.001);

     //MyMesh mesh;
     //loadMesh(mesh, filepath);

    //std::vector<MyFace *> IntersectingFaces;
    //vcg::tri::Clean<MyMesh>::SelfIntersections(mesh, IntersectingFaces);
    //printf("Number of self intersection faces %lu\n", IntersectingFaces.size());

    //printf("tiger\n");
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
