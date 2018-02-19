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

TEST_CASE( "test NoDengeratedFaces", "[file_check]" ) {
    MyMesh noDengeratedFacesMesh;
    loadMesh(noDengeratedFacesMesh, "./meshes/perfect.stl");

    REQUIRE( NoDengeratedFaces(noDengeratedFacesMesh) == true );
}

TEST_CASE( "test DengeratedFaces", "[file_check]" ) {
    MyMesh DengeratedFacesMesh;
    loadMesh(DengeratedFacesMesh, "./meshes/degeneratedFaces.stl");

    REQUIRE( NoDengeratedFaces(DengeratedFacesMesh) == false );
}

TEST_CASE( "test NoDuplicateFaces", "[file_check]" ) {
    MyMesh noDuplicateFacesMesh;
    loadMesh(noDuplicateFacesMesh, "./meshes/perfect.stl");

    REQUIRE( NoDuplicateFaces(noDuplicateFacesMesh) == true );
}

TEST_CASE( "test DuplicateFaces", "[file_check]" ) {
    MyMesh DuplicateFacesMesh;
    loadMesh(DuplicateFacesMesh, "./meshes/duplicateFaces.stl");

    REQUIRE( NoDuplicateFaces(DuplicateFacesMesh) == false );
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

    REQUIRE( NoIntersectingFaces(noIntersectingFacesMesh) == true );
}

TEST_CASE( "test intersecting faces", "[file_check]" ) {
    MyMesh IntersectingFacesMesh;
    loadMesh(IntersectingFacesMesh, "./meshes/intersectingFaces.stl");

    REQUIRE( NoIntersectingFaces(IntersectingFacesMesh) == false );
}
