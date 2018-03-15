#include <fileCheck.hpp>

void Boundary(MyMesh & mesh, float* boundary) {
    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
    boundary[0] = mesh.bbox.min.X();
    boundary[1] = mesh.bbox.max.X();
    boundary[2] = mesh.bbox.min.Y();
    boundary[3] = mesh.bbox.max.Y();
    boundary[4] = mesh.bbox.min.Z();
    boundary[5] = mesh.bbox.max.Z();
}

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

    FILE * fp;
    int counter = 1;
    fp = fopen("./intersecting.obj", "w+");
    for (auto const& face: IntersectingFaces) {
        auto v0 = face->cV(0)->cP();
        auto v1 = face->cV(1)->cP();
        auto v2 = face->cV(2)->cP();
        fprintf(fp, "v %f %f %f \n", v0[0], v0[1], v0[2]);
        fprintf(fp, "v %f %f %f \n", v1[0], v1[1], v1[2]);
        fprintf(fp, "v %f %f %f \n", v2[0], v2[1], v2[2]);
        fprintf(fp, "f %i %i %i \n", counter, counter+1, counter+2);
        // fprintf(fp, "\n");
        counter += 3;
    }


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
        printf("Error reading file  %s\n", filepath.c_str());
        return false;
    }

    bool RemoveDegenerateFlag=false;
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag);

    return true;
}

void file_check(MyMesh & m, int* results, float* boundary) {

    results[0] = 1; // set version number

    float merge_vertice = 0.; // not used
    if (merge_vertice > 0) {
        printf("mesh vertices before merge %i\n", m.VN());
        vcg::tri::Clean<MyMesh>::MergeCloseVertex(m, merge_vertice);
        printf("mesh vertices after merge %i\n", m.VN());
    }

    int numDegeneratedFaces;
    bool RemoveDegenerateFace = NoDegenratedFaces(m, numDegeneratedFaces);
    // printf( "before remove Duplicate Vertex has %i vert and %i faces\n", m.VN(), m.FN() );
    results[3] = numDegeneratedFaces;
    if (numDegeneratedFaces>0)
        printf("removed %i numDegeneratedFaces\n", numDegeneratedFaces);
    // std::cout << stringStream <<"\n test";

    int numDuplicateFaces;
    NoDuplicateFaces(m, numDuplicateFaces);

    if (numDuplicateFaces>0)
        printf("removed %i duplicate faces\n", numDuplicateFaces);

    // printf( "after remove Duplicate faces has %i vert and %i faces\n", m.VN(), m.FN() );
    results[4] = numDuplicateFaces;

    results[1] = m.FN();
    results[2] = m.VN();

    Boundary(m, boundary);
    printf( "xmin %f xmax %f \n", boundary[0], boundary[1]);
    printf( "ymin %f ymax %f \n", boundary[2], boundary[3]);
    printf( "zmin %f zmax %f \n", boundary[4], boundary[5]);

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m); // require for isWaterTight

    bool isWaterTight = IsWaterTight(m);
    printf( "Is WaterTight %s \n", isWaterTight ? "True" : "False");
    results[5] = isWaterTight;
    //if (not isWaterTight) return;

    bool isCoherentlyOriented = IsCoherentlyOrientedMesh(m);
    printf( "Is Coherently OrientedMesh %s \n", isCoherentlyOriented ? "True" : "False");
    results[6] = isCoherentlyOriented;
    //if (not isCoherentlyOriented) return;

    bool isPositiveVolume = IsPositiveVolume(m);
    printf( "Is Positive Volume %s \n", isPositiveVolume ? "True" : "False");
    results[7] = isPositiveVolume;
    //if (not isPositiveVolume) return;

    int numIntersectingFaces;
    NoIntersectingFaces(m, numIntersectingFaces);
    printf("Number of self intersection faces %i\n", numIntersectingFaces);
    results[8] = numIntersectingFaces;

    printf("Good\n");

    return;
}

int file_repair(MyMesh & mesh, int* results, int* repair_record) {
    assert(results[0] == 1); // version number needs to be 1

    printf("----------------- file repair-------------------\n");

    // TODO: result sanity check
    if (results[5] == -1) {
        printf("isWaterTight is not init\n");
        return 1;
    }

    if (results[6] == -1) {
        printf("isCoherentlyOriented is not init\n");
        return 1;
    }

    bool isWaterTight = results[5] == 1 ? true: false;

    if (not isWaterTight) {
        printf("is not watertight don't know how to repair\n");
        return 1;
    }
    bool isCoherentlyOriented = results[6] == 1 ? true: false;

    bool doesMakeCoherentlyOriented = DoesMakeCoherentlyOriented(mesh, isWaterTight, isCoherentlyOriented);
    repair_record[0] = doesMakeCoherentlyOriented;

    isCoherentlyOriented = IsCoherentlyOrientedMesh(mesh);
    bool isPositiveVolume = IsPositiveVolume(mesh);

    if (doesMakeCoherentlyOriented) { // update volume because makeCoherentlyOriented will change the volume
        isPositiveVolume = IsPositiveVolume(mesh);
        printf("make coherently oriented\n");
    }

    bool doesFlipNormalOutside = DoesFlipNormalOutside(mesh, isWaterTight, isCoherentlyOriented, isPositiveVolume);
    repair_record[1] = doesFlipNormalOutside;
    if (doesFlipNormalOutside)
        printf("flip normal outsite\n");

    return 0;
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

    void file_check(const std::string filepath, int* results, float* boundary) {
        printf("reading file  %s\n",filepath.c_str());

        MyMesh mesh;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }
        file_check(mesh, results, boundary);
    }

    void file_check_repair(const std::string filepath, int* results, float* boundary, int* repair_record) {
        printf("reading file  %s\n",filepath.c_str());

        MyMesh mesh;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }

        file_check(mesh, results, boundary);

        file_repair(mesh, results, repair_record);

        std::printf("writing to path %s\n", "repaired.stl");
        vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, "repaired.stl");
    }
}

bool DoesFlipNormalOutside(MyMesh & mesh, bool isWaterTight, bool isCoherentlyOriented, bool isPositiveVolume) {
    if (isWaterTight && isCoherentlyOriented && not isPositiveVolume) {
        vcg::tri::Clean<MyMesh>::FlipMesh(mesh);
        return true;
    } else {
        return false;
    }
}

bool DoesMakeCoherentlyOriented(MyMesh & mesh, bool isWaterTight, bool isCoherentlyOriented) {
    if (isWaterTight && not isCoherentlyOriented) {
        bool isOriented = true;
        bool isOrientable = true;
        vcg::tri::Clean<MyMesh>::OrientCoherentlyMesh(mesh, isOriented, isOrientable);
        return true;
    } else {
        return false;
    }
}

#ifndef FILECHECK_TEST
int main( int argc, char *argv[] )
{
    // std::string filepath = "./Bishop.stl";
    // std::string filepath = "/home/mmf159/Documents/vcg_learning/unittest/meshes/duplicateFaces.stl";
    // std::string filepath = "/home/mmf159/Downloads/wholething.stl";
    if (argc < 2) {
        printf("please provide path to stl file\n");
        return 1;
    }

    std::string repaired_path;
    if (argc >= 3) {
        std::string filepath_0 = argv[1];
        std::string filepath_1 = argv[2];
        if (filepath_0 == filepath_1) {
            printf("DANGER!export filepath is the same with original filepath!\n");
            printf("file path %s repaired file path %s\n",
                   filepath_0.c_str(), filepath_1.c_str());
            return 1;
        }

        repaired_path = argv[2];
    }

    std::string report_path;
    if (argc == 4) {
        report_path = argv[3];
    }

    std::string filepath = argv[1];

    printf("----------------- file check -------------------\n");
    int results[9] = {
        -1, // 0 version number
        -1, // 1 face number
        -1, // 2 vertices number
        -1, // 3 number of degenerated faces
        -1, // 4 number of duplicate faces
        -1, // 5 is watertight
        -1, // 6 is coherently oriented
        -1, // 7 is positive volume
        -1  // 8 number of intersecting faces
    };

    float boundary[6] = {-1, -1, -1, -1, -1, -1};

    MyMesh mesh;
    bool successfulLoadMesh = loadMesh(mesh, filepath);
    if (not successfulLoadMesh) {
        return 1;
    }
    file_check(mesh, results, boundary);

    //printf("%i version number\n", results[0]);
    //printf("%i face number\n", results[1]);
    //printf("%i vertices number\n", results[2]);
    //printf("%i num of degen faces\n", results[3]);
    //printf("%i num of dup faces\n", results[4]);
    //printf("%i is watertight\n", results[5]);
    //printf("%i is coherent oriented\n", results[6]);
    //printf("%i is positive volume\n", results[7]);
    //printf("%i num of intersecting faces\n", results[8]);

    //printf("xmin %f xmax %f \n", boundary[0], boundary[1]);
    //printf("ymin %f ymax %f \n", boundary[2], boundary[3]);
    //printf("zmin %f zmax %f \n", boundary[4], boundary[5]);

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

    int repair_record[2] = {
        -1, // fix CoherentlyOriented
        -1  // fix not Positive Volume
    };
    file_repair(mesh, results, repair_record);

    // bool doesMakeCoherentlyOriented = DoesMakeCoherentlyOriented(mesh, true, true);
    // printf("doesMakeCoherentlyOriented %i\n", doesMakeCoherentlyOriented ? true : false);

    // IsPositiveVolume(mesh);

    if (not repaired_path.empty()) {
        std::printf("writing to path %s\n", repaired_path.c_str());
        vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());
    }

    assert(results[0] == 1); // make sure the version is 1

    if (report_path.empty()) {
        std::printf("report_path is not provided, write to stdout\n");
    }

    FILE * report;
    if (report_path.empty())
        report = stdout;
    else
        report = std::fopen (report_path.c_str(), "w");

    std::fprintf(report, "%d num_version\n",                     results[0]);
    std::fprintf(report, "%d num_face\n",                        results[1]);
    std::fprintf(report, "%d num_vertices\n",                    results[2]);
    std::fprintf(report, "%d num_degenerated_faces_removed\n",   results[3]);
    std::fprintf(report, "%d num_duplicated_faces_removed\n",    results[4]);
    std::fprintf(report, "%d is_watertight\n",                   results[5]);
    std::fprintf(report, "%d is_coherently_oriented\n",          results[6]);
    std::fprintf(report, "%d is_positive_volume\n",              results[7]);
    std::fprintf(report, "%d num_intersecting_faces\n",          results[8]);
    std::fprintf(report, "%f min_x\n",                          boundary[0]);
    std::fprintf(report, "%f max_x\n",                          boundary[1]);
    std::fprintf(report, "%f min_y\n",                          boundary[2]);
    std::fprintf(report, "%f max_y\n",                          boundary[3]);
    std::fprintf(report, "%f min_z\n",                          boundary[4]);
    std::fprintf(report, "%f max_z\n",                          boundary[5]);
    std::fprintf(report, "%d does_make_coherent_orient\n", repair_record[0]);
    std::fprintf(report, "%d does_flip_normal_outside\n",  repair_record[1]);


    return 0;
}
#endif
