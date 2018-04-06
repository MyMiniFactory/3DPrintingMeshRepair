#include <fileCheck.hpp>

void Boundary(MyMesh & mesh, checkResult_t& r) {
    vcg::tri::UpdateBounding<MyMesh>::Box(mesh);
    r.xmin = mesh.bbox.min.X();
    r.xmax = mesh.bbox.max.X();
    r.ymin = mesh.bbox.min.Y();
    r.ymax = mesh.bbox.max.Y();
    r.zmin = mesh.bbox.min.Z();
    r.zmax = mesh.bbox.max.Z();
}

unsigned int NoDegenratedFaces(MyMesh & mesh) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    bool RemoveDegenerateFlag=true;
    Clean::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    return beforeNumFaces - afterNumFaces;
}

unsigned int NoDuplicateFaces(MyMesh & mesh) { // change mesh in-place
    const int beforeNumFaces = mesh.FN();

    Clean::RemoveDuplicateFace(mesh); // remove degenerateFace, removeDegenerateEdge, RemoveDuplicateEdge

    const int afterNumFaces = mesh.FN();

    return beforeNumFaces - afterNumFaces;
}

unsigned int NumIntersectingFaces(MyMesh & mesh) { // change mesh in-place

    std::vector<MyFace *> IntersectingFaces;
    Clean::SelfIntersections(mesh, IntersectingFaces);

    // return static_cast<int>(IntersectingFaces.size());
    return IntersectingFaces.size();

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
}

bool IsWaterTight(MyMesh & mesh) {
    return Clean::IsWaterTight(mesh);
}


bool IsCoherentlyOrientedMesh(MyMesh & mesh) {
    return Clean::IsCoherentlyOrientedMesh(mesh);
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

unsigned int NumShell(MyMesh & mesh) {
    return Clean::CountConnectedComponents(mesh);
}

bool IsGoodMesh(checkResult_t r) {
    assert(r.version == 4);
    // 5 is watertight
    // 6 is coherently oriented
    // 7 is positive volume

    bool isWaterTight = r.is_watertight;
    bool isCoherentlyOriented = r.is_coherently_oriented;
    bool isPositiveVolume = r.is_positive_volume;

    if (isWaterTight and isCoherentlyOriented and isPositiveVolume) {
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
    Clean::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag);

    return true;
}

checkResult_t file_check(MyMesh & m) {
    checkResult_t r;

    r.version = 4; // set version number

    r.n_degen_faces = NoDegenratedFaces(m);
    r.n_duplicate_faces = NoDuplicateFaces(m);

    r.n_faces = m.FN();
    r.n_vertices = m.VN();

    Boundary(m, r);
    r.area = Area(m);
    r.volume = Volume(m);

    vcg::tri::UpdateTopology<MyMesh>::FaceFace(m); // require for isWaterTight

    r.is_watertight = IsWaterTight(m);

    r.is_coherently_oriented = IsCoherentlyOrientedMesh(m);

    r.is_positive_volume = IsPositiveVolume(m);

    r.n_intersecting_faces = NumIntersectingFaces(m);

    r.n_shells = NumShell(m);

    // non manifold edges in a mesh, e.g. the edges where there are more than 2 incident faces
    r.n_non_manifold_edges = Clean::CountNonManifoldEdgeFF(m);

    if (r.n_non_manifold_edges == 0) {
        auto vpss = CountHoles(m);
        auto numHoles = vpss.size();
        r.n_holes = numHoles;
    }

    r.is_good_mesh = IsGoodMesh(r);

    return r;
}

repairRecord_t file_repair(
        MyMesh & mesh, checkResult_t check_r, const std::string repaired_path
    ) {

    repairRecord_t r;

    r.version = 1; // version 1 of file repair

    assert(check_r.version == 4); // version number needs to be 1

    printf("----------------- file repair-------------------\n");

    // TODO: result sanity check
    // if (check_r.is_watertight == -1) {
        // printf("isWaterTight is not init\n");
        // return 1;
    // }

    // if (check_r.is_coherently_oriented == -1) {
        // printf("isCoherentlyOriented is not init\n");
    // }

    bool isWaterTight = check_r.is_watertight;
    const int numNonManifoldEdge = check_r.n_non_manifold_edges;
    bool isCoherentlyOriented = check_r.is_coherently_oriented;

    if (!isWaterTight and numNonManifoldEdge > 0) {
        r.n_non_manif_f_removed = Clean::RemoveNonManifoldFace(mesh);
        printf("number non manifold faces removed %d \n", r.n_non_manif_f_removed);

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
        r.does_fix_hole = 0;
    }

    if (!isWaterTight) {
        auto vpps = CountHoles(mesh);
        const int numHoles = vpps.size();
        repair_hole(mesh, vpps);
        printf("fix num of holes %d \n", numHoles);
        if (numHoles > 0) {
            r.does_fix_hole = 1;
            Clean::RemoveDuplicateVertex(mesh, true);

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
        r.does_fix_hole = 0;
    }

    bool doesMakeCoherentlyOriented = DoesMakeCoherentlyOriented(mesh, isWaterTight, isCoherentlyOriented);
    r.fix_coherently_oriented = doesMakeCoherentlyOriented;

    isCoherentlyOriented = IsCoherentlyOrientedMesh(mesh);
    bool isPositiveVolume = IsPositiveVolume(mesh);

    if (doesMakeCoherentlyOriented) { // update volume because makeCoherentlyOriented will change the volume
        isPositiveVolume = IsPositiveVolume(mesh);
        printf("make coherently oriented\n");
    }

    bool doesFlipNormalOutside = DoesFlipNormalOutside(mesh, isWaterTight, isCoherentlyOriented, isPositiveVolume);
    r.does_fix_positive_volume = doesFlipNormalOutside;
    if (doesFlipNormalOutside)
        printf("flip normal outsite\n");

    r.does_union = 0;

    return r;
}

bool IsGoodRepair(checkResult_t results, checkResult_t repair_results) {
    assert(results.version == 4); // correct version
    if (not repair_results.is_good_mesh) // if it is not good mesh
        return false;
    if (results.n_shells != repair_results.n_shells) // require same number of shells
        return false;
    if (results.n_intersecting_faces != repair_results.n_intersecting_faces) // require same number of intersecting faces
        return false;
    return true;
}

void file_repair_then_check(
        MyMesh & mesh, checkResult_t results, const std::string repaired_path,
        FILE* report
    ) {
    auto repair_record = file_repair(mesh, results, repaired_path);
    assert(repair_record.version == 1);

    if (not repaired_path.empty())
        vcg::tri::io::ExporterSTL<MyMesh>::Save(mesh, repaired_path.c_str());

    loadMesh(mesh, repaired_path);

    auto repair_results = file_check(mesh); // TODO: repair boundary

    repair_record.is_good_repair = IsGoodRepair(results, repair_results);

    repair_record.output_report(report);
    repair_results.output_report(report);
}

void output_report(FILE* report, int* results, float* boundary, int* repair_record) {
    printf("---------------writing report\n");

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
    std::fprintf(report, "%d is_good_repair\n",            repair_record[6]);
}

void file_check(const std::string filepath) {
    printf("reading file  %s\n",filepath.c_str());

    MyMesh mesh;
    int a = 2;
    if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(),  a))
    {
        printf("Error reading file  %s\n",filepath.c_str());
        exit(0);
    }
    file_check(mesh);
}

int check_repair_main(const std::string filepath) {
    checkResult_t results, repair_results;
    repairRecord_t repair_record;

    MyMesh mesh;
    bool successfulLoadMesh = loadMesh(mesh, filepath);

    if (not successfulLoadMesh) {
        return 1;
    }
    file_check(mesh);
}

extern "C" {

    /*
    int js_check_repair(const std::string filepath) {
        int results[13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
        float boundary[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
        int repair_record[6] = { -1, -1, -1, -1, -1, -1 };

        int repair_results[13] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

        MyMesh mesh;
        // bool successfulLoadMesh = loadMesh(mesh, filepath);
        int load_mask = 2;
        if(vcg::tri::io::ImporterSTL<MyMesh>::Open(mesh, filepath.c_str(), load_mask))
        {
            printf("Error reading file  %s\n",filepath.c_str());
            exit(0);
        }

        bool RemoveDegenerateFlag=false;
        Clean::RemoveDuplicateVertex(mesh, RemoveDegenerateFlag);

        printf("---------------- file check -------------------\n");
        file_check(mesh, results, boundary);

        FILE * report;
        report = std::fopen("report.txt", "w");
        output_report(report, results, boundary, repair_record);
        std::fclose(report);

        if not IsGoodMesh(mesh) {
            std::string repaired_path = "repaired.stl";
            printf("---------------- file repair -------------------\n");
            file_repair_then_check(mesh, results, repair_results, boundary, repaired_path, repair_record);

            printf("---------------- file check for repair -------------------\n");
            report = std::fopen("repair_report.txt", "w");
            output_report(report, repair_results, boundary, repair_record);
            std::fclose(report);
        } else {
            printf("Good Mesh already, no need to repair");
        }

        return 0;
    }
    */
}

bool DoesFlipNormalOutside(MyMesh & mesh, bool isWaterTight, bool isCoherentlyOriented, bool isPositiveVolume) {
    if (isWaterTight && isCoherentlyOriented && not isPositiveVolume) {
        Clean::FlipMesh(mesh);
        return true;
    } else {
        return false;
    }
}

bool DoesMakeCoherentlyOriented(MyMesh & mesh, bool isWaterTight, bool isCoherentlyOriented) {
    if (isWaterTight && not isCoherentlyOriented) {
        bool isOriented = true;
        bool isOrientable = true;
        Clean::OrientCoherentlyMesh(mesh, isOriented, isOrientable);
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
        printf("path to stl file not provided use default %s\n", filepath.c_str());
    } else {
        filepath = argv[1];
    }

    std::string repaired_path = "./out/repaired_perfect.stl";
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
        printf("repaired path is given writing to %s\n", repaired_path.c_str());
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
    MyMesh mesh;
    bool successfulLoadMesh = loadMesh(mesh, filepath);

    if (not successfulLoadMesh) {
        return 1;
    }
    auto results = file_check(mesh);

    FILE * report;
    if (report_path.empty())
        report = stdout;
    else
        report = std::fopen(report_path.c_str(), "w");

    results.output_report(report);

    FILE * repair_report;
    if (repaired_report_path.empty())
        repair_report = stdout;
    else
        repair_report = std::fopen(repaired_report_path.c_str(), "w");

    file_repair_then_check(mesh, results, repaired_path, repair_report);



    // output_report(repair_report, repair_results, boundary, repair_record);
    // std::fclose(repair_report);

    return 0;
}
#endif
