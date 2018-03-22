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

    // FILE * fp;
    // int counter = 1;
    // fp = fopen("./intersecting.obj", "w+");
    // for (auto const& face: IntersectingFaces) {
        // auto v0 = face->cV(0)->cP();
        // auto v1 = face->cV(1)->cP();
        // auto v2 = face->cV(2)->cP();
        // fprintf(fp, "v %f %f %f \n", v0[0], v0[1], v0[2]);
        // fprintf(fp, "v %f %f %f \n", v1[0], v1[1], v1[2]);
        // fprintf(fp, "v %f %f %f \n", v2[0], v2[1], v2[2]);
        // fprintf(fp, "f %i %i %i \n", counter, counter+1, counter+2);
        // // fprintf(fp, "\n");
        // counter += 3;
    // }
    return (numIntersectingFaces > 0) ? false : true;
}

bool IsWaterTight(MyMesh & mesh) {
    return vcg::tri::Clean<MyMesh>::IsWaterTight(mesh);
}


bool IsCoherentlyOrientedMesh(MyMesh & mesh) {
    return vcg::tri::Clean<MyMesh>::IsCoherentlyOrientedMesh(mesh);
}

float Volume(MyMesh & mesh) {
    vcg::tri::Inertia<MyMesh> Ib(mesh);
    return Ib.Mass();
}

bool IsPositiveVolume(MyMesh & mesh) {
    return Volume(mesh) > 0. ;
}

float Area(MyMesh & mesh) {
    float area = 0;
    for(auto fi = mesh.face.begin(); fi!=mesh.face.end();++fi)
        if(!fi->IsD())
            area += DoubleArea(*fi)/2;
    return area;
}

bool IsSingleShell(MyMesh & mesh, int & numConnectedComponents) {

    numConnectedComponents = vcg::tri::Clean<MyMesh>::CountConnectedComponents(mesh);
    return (numConnectedComponents == 1) ? true : false;
}

bool IsGoodMesh(int* results) {
    assert(results[0] == 4);
    // 5 is watertight
    // 6 is coherently oriented
    // 7 is positive volume

    bool isWaterTight = results[5];
    bool isCoherentlyOriented = results[6];
    bool isPositiveVolume = results[7];

    if (isWaterTight and isCoherentlyOriented and isPositiveVolume) {
        printf("Good mesh\n");
        return true;
    } else {
        return false;
    }

    // bool isWaterTightR = repair_results[5];
    // bool isCoherentlyOrientedR = repair_results[6];
    // bool isPositiveVolumeR = repair_results[7];
    // int numIntersectingFacesR = repair_results[8];
    // int numConnectedComponentsR = repair_results[9];

    // if (isWaterTightR and isCoherentlyOrientedR and isPositiveVolumeR
            // // make sure no added intersecting faces
            // and numIntersectingFacesR == numIntersectingFaces
            // // make sure no added connected components
            // and numConnectedComponentsR == numConnectedComponents
    // ) {
        // printf("Good mesh\n");
        // return true;
    // }

    // printf("Bad mesh\n");
    // return false;
}


std::vector<std::vector<vcg::Point3<float>>> CountHoles(MyMesh & m)
{
    vcg::tri::UpdateFlags<MyMesh>::FaceClearV(m);
    std::vector<std::vector<vcg::Point3<float>>> vpss;

    int loopNum=0;
    for(auto fi=m.face.begin(); fi!=m.face.end();++fi) if(!fi->IsD())
    {
        for(int j=0;j<3;++j)
        {
            if(!fi->IsV() && vcg::face::IsBorder(*fi,j))
            {
                vcg::face::Pos<MyFace> startPos(&*fi,j);
                vcg::face::Pos<MyFace> curPos=startPos;

                std::vector<vcg::Point3<float>> vps;

                do
                {
                    auto curFace = curPos.F();
                    curPos.NextB();
                    curPos.F()->SetV();
                    auto face = curPos.F();

                    auto edgeIndex = curPos.E();
                    if (edgeIndex == 0) {
                        vps.push_back(face->cV(0)->cP());
                        vps.push_back(face->cV(1)->cP());
                    } else if (edgeIndex == 1) {
                        vps.push_back(face->cV(1)->cP());
                        vps.push_back(face->cV(2)->cP());
                    } else {
                        assert(edgeIndex == 2);
                        vps.push_back(face->cV(2)->cP());
                        vps.push_back(face->cV(0)->cP());
                    }

                }
                while(curPos!=startPos);
                vpss.push_back(vps);
                ++loopNum;
            }
        }
    }
    // return loopNum;
    return vpss;
}

void repair_hole(
        MyMesh & mesh, std::vector<std::vector<vcg::Point3<float>>> vpss
    ) {
    std::cout<<"in repair hole " << vpss.size() <<std::endl;
    for (auto& vps : vpss) {
        if (vps.size() >= 6) {

            const int num_edges = vps.size()/2;

            vcg::Point3<float> center(0, 0, 0);
            for (auto& n : vps) center += n;

            center[0] /= vps.size();
            center[1] /= vps.size();
            center[2] /= vps.size();

            for (int count=0;count<num_edges;count++) {
                vcg::tri::Allocator<MyMesh>::AddFace(
                    mesh, vps[count*2 + 1], vps[count*2], center);
                std::cout<<"add faces"<<std::endl;
            }
        }
    }
}


bool loadMesh(MyMesh & mesh, const std::string filepath) {

    int a = 2; // TODO: understand what this is

    std::string extension;
    extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "stl") {
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n", filepath.c_str());
            return false;
        }
    } else if (extension == "obj") {
        typedef vcg::tri::io::ImporterOBJ<MyMesh> ImporterOBJ;

        auto error_code = ImporterOBJ::Open(mesh, filepath.c_str(),  a);
        auto error_message = ImporterOBJ::ErrorMsg(error_code);
        auto error_critical = ImporterOBJ::ErrorCritical(error_code);

        if (error_code!=0 && !error_critical) { // even error code critical error
            printf("Reading file  %s with Non Critical Error %s\n", filepath.c_str(), error_message);
        } else if (error_critical) { // odd error code critical error
            printf("Error reading file  %s with Critical Error %s\n", filepath.c_str(), error_message);
            return false;
        }
    } else {
        return false;
    }

    bool RemoveDegenerateFlag=false;
    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag);

    return true;
}

void file_check(MyMesh & m, int* results, float* boundary) {

    results[0] = 4; // set version number

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

    boundary[6] = Area(m);
    boundary[7] = Volume(m);

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m); // require for isWaterTight
    printf( "Update topology \n");

    bool isWaterTight = IsWaterTight(m);
    printf( "Is WaterTight %s \n", isWaterTight ? "True" : "False");
    results[5] = isWaterTight;
    // if (not isWaterTight) return;

    bool isCoherentlyOriented = IsCoherentlyOrientedMesh(m);
    printf( "Is Coherently OrientedMesh %s \n", isCoherentlyOriented ? "True" : "False");
    results[6] = isCoherentlyOriented;
    // if (not isCoherentlyOriented) return;

    bool isPositiveVolume = IsPositiveVolume(m);
    printf( "Is Positive Volume %s \n", isPositiveVolume ? "True" : "False");
    results[7] = isPositiveVolume;

    int numIntersectingFaces;
    NoIntersectingFaces(m, numIntersectingFaces);
    printf("Number of self intersection faces %i\n", numIntersectingFaces);
    results[8] = numIntersectingFaces;

    int numConnectedComponents;
    IsSingleShell(m, numConnectedComponents);
    printf("number of connected components %i\n", numConnectedComponents);
    results[9] = numConnectedComponents;

    // non manifold edges in a mesh, e.g. the edges where there are more than 2 incident faces
    int numNonManifoldEdge;
    numNonManifoldEdge = vcg::tri::Clean<MyMesh>::CountNonManifoldEdgeFF(m);
    printf("removed number of non manifold faces %i\n", numNonManifoldEdge);
    results[10] = numNonManifoldEdge;

    if (numNonManifoldEdge == 0) {
        auto vpss = CountHoles(m);
        auto numHoles = vpss.size();
        printf("number of holes %li\n", numHoles);
        results[11] = numHoles;
    }

    results[12] = IsGoodMesh(results);

    return;
}

int file_repair(
        MyMesh & mesh, int* results, int* repair_record,
        const std::string repaired_path
    ) {

    repair_record[0] = 1; // version 1 of file repair

    assert(results[0] == 4); // version number needs to be 1

    printf("----------------- file repair-------------------\n");

    // TODO: result sanity check
    if (results[5] == -1) {
        printf("isWaterTight is not init\n");
        return 1;
    }

    if (results[6] == -1) {
        printf("isCoherentlyOriented is not init\n");
    }

    bool isWaterTight = results[5] == 1 ? true: false;
    const int numNonManifoldEdge = results[10];
    bool isCoherentlyOriented = results[6] == 1 ? true: false;


    // tiger test not necessary
    {
        int numHoles = results[11];
        if (numNonManifoldEdge == 0 and numHoles == 0 and not isWaterTight) {
            printf("isWaterTight is because of vertex ?? num nonmanif vertex%d",
                    vcg::tri::Clean<MyMesh>::CountNonManifoldVertexFF(mesh));
        }
    }
    // tiger test not necessary

    printf("-----is water tight %d \n", isWaterTight);

    if (!isWaterTight and numNonManifoldEdge > 0) {
        int numNonManifoldFacesRemoved = vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(mesh);
        repair_record[4] = 1;
        printf("number non manifold faces removed %d \n", numNonManifoldFacesRemoved);

        // reload mesh
        vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());
        MyMesh repaired_mesh;
        loadMesh(mesh, repaired_path);
        vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh); // require for isWaterTight

        isWaterTight = IsWaterTight(mesh);
        isCoherentlyOriented = IsCoherentlyOrientedMesh(mesh);
        printf("-----new is water tight %d \n", isWaterTight);
        printf("-----new is coherently oriented %d \n", isCoherentlyOriented);
    } else {
        repair_record[4] = 0;
    }

    if (!isWaterTight) {
        auto vpps = CountHoles(mesh);
        const int numHoles = vpps.size();
        repair_hole(mesh, vpps);
        printf("fix num of holes %d \n", numHoles);
        if (numHoles > 0) {
            repair_record[5] = 1;
            vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh, true);

            // reload mesh
            vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());
            MyMesh repaired_mesh;
            loadMesh(mesh, repaired_path);
            vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh); // require for isWaterTight

            isWaterTight = IsWaterTight(mesh);
            isCoherentlyOriented = IsCoherentlyOrientedMesh(mesh);
            printf("-----new is water tight %d \n", isWaterTight);
            printf("-----new is coherently oriented %d \n", isCoherentlyOriented);
        }
    } else {
        repair_record[5] = 0;
    }

    bool doesMakeCoherentlyOriented = DoesMakeCoherentlyOriented(mesh, isWaterTight, isCoherentlyOriented);
    repair_record[1] = doesMakeCoherentlyOriented;

    isCoherentlyOriented = IsCoherentlyOrientedMesh(mesh);
    bool isPositiveVolume = IsPositiveVolume(mesh);

    if (doesMakeCoherentlyOriented) { // update volume because makeCoherentlyOriented will change the volume
        isPositiveVolume = IsPositiveVolume(mesh);
        printf("make coherently oriented\n");
    }

    bool doesFlipNormalOutside = DoesFlipNormalOutside(mesh, isWaterTight, isCoherentlyOriented, isPositiveVolume);
    repair_record[2] = doesFlipNormalOutside;
    if (doesFlipNormalOutside)
        printf("flip normal outsite\n");

    repair_record[3] = 0;

    return 0;
}

void output_report(FILE* report, int* results, float* boundary, int* repair_record) {

    assert(results[0] == 4 && repair_record[0] == 1);

    std::fprintf(report, "%d num_version\n",                     results[0]);
    std::fprintf(report, "%d num_face\n",                        results[1]);
    std::fprintf(report, "%d num_vertices\n",                    results[2]);
    std::fprintf(report, "%d num_degenerated_faces_removed\n",   results[3]);
    std::fprintf(report, "%d num_duplicated_faces_removed\n",    results[4]);
    std::fprintf(report, "%d is_watertight\n",                   results[5]);
    std::fprintf(report, "%d is_coherently_oriented\n",          results[6]);
    std::fprintf(report, "%d is_positive_volume\n",              results[7]);
    std::fprintf(report, "%d num_intersecting_faces\n",          results[8]);
    std::fprintf(report, "%d num_shells\n",                      results[9]);
    std::fprintf(report, "%d num_non_manifold_edges\n",          results[10]);
    std::fprintf(report, "%d num_holes\n",                       results[11]);
    std::fprintf(report, "%d is_good_mesh\n",                    results[12]);
    std::fprintf(report, "%f min_x\n",                          boundary[0]);
    std::fprintf(report, "%f max_x\n",                          boundary[1]);
    std::fprintf(report, "%f min_y\n",                          boundary[2]);
    std::fprintf(report, "%f max_y\n",                          boundary[3]);
    std::fprintf(report, "%f min_z\n",                          boundary[4]);
    std::fprintf(report, "%f max_z\n",                          boundary[5]);
    std::fprintf(report, "%f area\n",                           boundary[6]);
    std::fprintf(report, "%f volume\n",                         boundary[7]);
    std::fprintf(report, "%d repair_version\n",            repair_record[0]);
    std::fprintf(report, "%d does_make_coherent_orient\n", repair_record[1]);
    std::fprintf(report, "%d does_flip_normal_outside\n",  repair_record[2]);
    std::fprintf(report, "%d does_union\n",                repair_record[3]);
    std::fprintf(report, "%d does_rm_non_manif_faces\n",   repair_record[4]);
    std::fprintf(report, "%d does_hole_fix\n",             repair_record[5]);
}

void file_repair_complex(const std::string repaired_path, MyMesh & mesh,
        int* results, int* repair_record, std::string union_py_path) {
    // put repair function here which requires backend

    // int numIntersectingFaces = results[8];
    // int numConnectedComponents = results[9];

    repair_record[2] = 0; // record we don't do the union
    // if (numConnectedComponents > 1 and numIntersectingFaces > 0) {
        // // do union
        // printf("Doing union\n");
        // auto command = "python3 " + union_py_path + " "+ repaired_path + " " + repaired_path;
        // int result = system(command.c_str());
        // printf("result from calling union %i\n", result);
        // if (result == 0) { // command line call without error
            // repair_record[2] = 1; // record we do the union
        // }
    // }
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

    void file_check_repair(const std::string filepath, int* results, float* boundary, int* repair_record, const std::string repaired_path) {
        printf("reading file  %s\n",filepath.c_str());

        MyMesh mesh;
        int a = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }

        file_check(mesh, results, boundary);

        file_repair(mesh, results, repair_record, repaired_path);

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

// TODO: write test for this function
#ifndef FILECHECK_TEST
int main( int argc, char *argv[] )
{
    std::string filepath = "./unittest/meshes/perfect.stl";
    if (argc < 2) {
        printf("path to stl file not provided use default\n");
    } else {
        filepath = argv[1];
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
    } else {
        printf("repaired path is required");
        return 1;
    }

    std::string report_path;
    if (argc >= 4) {
        report_path = argv[3];
    } else {
        printf("report path is not given, writing to stdout\n");
    }


    std::string repaired_report_path;
    if (argc >= 5) {
        repaired_report_path = argv[4];
    } else {
        printf("repaired report path is not given, writing to stdout\n");
    }

    std::string union_py_path;
    if (argc >= 6) {
        union_py_path = argv[5];
    } else {
        printf("Union python script is not given, not doing union\n");
    }

    printf("----------------- file check -------------------\n");
    int results[13] = {
        -1, // 0 version number
        -1, // 1 face number
        -1, // 2 vertices number
        -1, // 3 number of degenerated faces
        -1, // 4 number of duplicate faces
        -1, // 5 is watertight
        -1, // 6 is coherently oriented
        -1, // 7 is positive volume
        -1, // 8 number of intersecting faces
        -1, // 9 number of connected components
        -1, //10 number of non manifold edges
        -1, //11 number of holes
        -1  //11 good or bad
    };

    int repair_results[13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
    float boundary[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

    MyMesh mesh;
    bool successfulLoadMesh = loadMesh(mesh, filepath);

    //////////////////////////////// tiger test
    /*
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh); // require for isWaterTight
    printf("is watertight %d\n", IsWaterTight(mesh));
    // int numNonManifoldFacesRemoved = vcg::tri::Clean<MyMesh>::RemoveNonManifoldFace(mesh);
    // printf("number non manifold faces removed %d \n", numNonManifoldFacesRemoved);
    int numHoles = CountHoles(mesh);
    printf("number hole %d \n", numHoles);
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh); // require for isWaterTight

    // vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());
    // loadMesh(mesh, repaired_path);
    // vcg::tri::UpdateTopology<MyMesh>::FaceFace(mesh); // require for isWaterTight

    vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(mesh);
    printf("is watertight %d\n", IsWaterTight(mesh));
    return 1;
    */
    /////////////////////////////// tiger test end

    if (not successfulLoadMesh) {
        return 1;
    }
    file_check(mesh, results, boundary);

    int repair_record[6] = {
        -1, // 0 repair version
        -1, // 1 fix CoherentlyOriented
        -1, // 2 fix not Positive Volume
        -1, // 3 attempt to fix the union, require human check
        -1, // 4 remove non manifold faces
        -1  // 5 fix hole
    };

    file_repair(mesh, results, repair_record, repaired_path);

    if (not repaired_path.empty())
        vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());

    // if (not repaired_path.empty() and not union_py_path.empty()) {
        // std::printf("writing to path %s\n", repaired_path.c_str());
        // file_repair_complex(repaired_path, mesh, results, repair_record, union_py_path); // writing to repaired path
        // vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());
    // } else {
        // std::printf("repaired path is empty, didn't do repair complex %s\n", repaired_path.c_str());
    // }

    if (report_path.empty()) {
        std::printf("report_path is not provided, write to stdout\n");
    }

    FILE * report;
    if (report_path.empty())
        report = stdout;
    else
        report = std::fopen(report_path.c_str(), "w");

    output_report(report, results, boundary, repair_record);
    std::fclose(report);

    loadMesh(mesh, repaired_path);
    std::printf("-------------------------- check for repair ----------\n");
    file_check(mesh, repair_results, boundary);

    FILE * repair_report;
    if (repaired_report_path.empty())
        repair_report = stdout;
    else
        repair_report = std::fopen(repaired_report_path.c_str(), "w");

    output_report(repair_report, repair_results, boundary, repair_record);
    std::fclose(repair_report);

    exit(0);

    return 0;
}
#endif
