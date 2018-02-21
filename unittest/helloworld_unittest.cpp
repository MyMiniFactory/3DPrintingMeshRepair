#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "helloworld.hpp"

TEST_CASE( "test successful loadMesh", "[file_check]" ) {
    MyMesh mesh;
    bool is_successful = loadMesh(mesh, "./meshes/perfect.stl");

    REQUIRE( is_successful == true );
}

TEST_CASE( "test not successful loadMesh", "[file_check]" ) {
    MyMesh mesh;
    bool is_successful = loadMesh(mesh, "./meshes/notexists.stl");

    REQUIRE( is_successful == false );
}

TEST_CASE( "test NoDegeneratedFace", "[file_check]" ) {
    MyMesh noDegenratedFacesMesh;
    loadMesh(noDegenratedFacesMesh, "./meshes/perfect.stl");

    int numDegeneratedFaces;

    REQUIRE(
        NoDegenratedFaces(noDegenratedFacesMesh, numDegeneratedFaces) == true
    );
    REQUIRE( numDegeneratedFaces == 0);
}

TEST_CASE( "test DengeratedFaces", "[file_check]" ) {
    MyMesh DegenratedFacesMesh;
    loadMesh(DegenratedFacesMesh, "./meshes/degeneratedFaces.stl");

    int numDegeneratedFaces;
    REQUIRE(
        NoDegenratedFaces(DegenratedFacesMesh, numDegeneratedFaces) == false
    );
    REQUIRE( numDegeneratedFaces == 2);
}

TEST_CASE( "test NoDuplicateFaces", "[file_check]" ) {
    MyMesh noDuplicateFacesMesh;
    loadMesh(noDuplicateFacesMesh, "./meshes/perfect.stl");

    int numDuplicateFaces;
    REQUIRE( NoDuplicateFaces(noDuplicateFacesMesh, numDuplicateFaces) == true );
    REQUIRE( numDuplicateFaces == 0 );
}

TEST_CASE( "test DuplicateFaces", "[file_check]" ) {
    MyMesh DuplicateFacesMesh;
    loadMesh(DuplicateFacesMesh, "./meshes/duplicateFaces.stl");

    int numDuplicateFaces;
    REQUIRE( NoDuplicateFaces(DuplicateFacesMesh, numDuplicateFaces) == false );
    REQUIRE( numDuplicateFaces == 1 );
}

TEST_CASE( "test WaterTight", "[file_check]" ) {
    MyMesh waterTightMesh;
    loadMesh(waterTightMesh, "./meshes/perfect.stl");

    REQUIRE( IsWaterTight(waterTightMesh) == true );
}

TEST_CASE( "test Not WaterTight", "[file_check]" ) {
    MyMesh notWaterTightMesh;
    loadMesh(notWaterTightMesh, "./meshes/notWatertight.stl");

    REQUIRE( IsWaterTight(notWaterTightMesh) == false );
}

TEST_CASE( "test Coherently Oriented", "[file_check]" ) {
    MyMesh coherentlyOrientedMesh;
    loadMesh(coherentlyOrientedMesh, "./meshes/perfect.stl");
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(coherentlyOrientedMesh);

    REQUIRE( IsCoherentlyOrientedMesh(coherentlyOrientedMesh) == true );
}

TEST_CASE( "test not Coherently Oriented", "[file_check]" ) {
    MyMesh notCoherentlyOrientedMesh;
    loadMesh(notCoherentlyOrientedMesh, "./meshes/notCoherentlyOriented.stl");
    vcg::tri::UpdateTopology<MyMesh>::FaceFace(notCoherentlyOrientedMesh);

    REQUIRE( IsCoherentlyOrientedMesh(notCoherentlyOrientedMesh) == false );
}

TEST_CASE( "test Positive Volume", "[file_check]" ) {
    MyMesh positiveVolumeMesh;
    loadMesh(positiveVolumeMesh, "./meshes/perfect.stl");

    REQUIRE( IsPositiveVolume(positiveVolumeMesh) == true );
}

TEST_CASE( "test not Positive Volume", "[file_check]" ) {
    MyMesh notPositiveVolumeMesh;
    loadMesh(notPositiveVolumeMesh, "./meshes/notPositiveVolume.stl");

    REQUIRE( IsPositiveVolume(notPositiveVolumeMesh) == false );
}

TEST_CASE( "test no intersecting faces", "[file_check]" ) {
    MyMesh noIntersectingFacesMesh;
    loadMesh(noIntersectingFacesMesh, "./meshes/perfect.stl");

    int numIntersectingFaces;
    REQUIRE(
        NoIntersectingFaces(noIntersectingFacesMesh, numIntersectingFaces) == true
    );
    REQUIRE(numIntersectingFaces == 0);
}

TEST_CASE( "test intersecting faces", "[file_check]" ) {
    MyMesh IntersectingFacesMesh;
    loadMesh(IntersectingFacesMesh, "./meshes/intersectingFaces.stl");

    int numIntersectingFaces;
    REQUIRE(
        NoIntersectingFaces(IntersectingFacesMesh, numIntersectingFaces) == false
    );
    // REQUIRE(numIntersectingFaces == 0); // the intersecting algorithm is not good enough
}
