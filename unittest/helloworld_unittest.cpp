#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "helloworld.hpp"

TEST_CASE( "test NoDengeratedFaces", "[file_check]" ) {
    MyMesh noDengeratedFacesMesh = loadMesh("./meshes/perfect.stl")
    REQUIRE( NoDengeratedFaces(noDengeratedFacesMesh) == true );
}

TEST_CASE( "test DengeratedFaces", "[file_check]" ) {
    MyMesh DengeratedFacesMesh = loadMesh("./meshes/degeneratedFaces.stl")
    REQUIRE( NoDengeratedFaces(DengeratedFacesMesh) == false );
}

TEST_CASE( "test NoDuplicateFaces", "[file_check]" ) {
    MyMesh noDuplicateFacesMesh = loadMesh("./meshes/perfect.stl")
    REQUIRE( NoDuplicateFaces(noDuplicateFacesMesh) == true );
}

TEST_CASE( "test DuplicateFaces", "[file_check]" ) {
    MyMesh DuplicateFacesMesh = loadMesh("./meshes/duplicateFaces.stl")
    REQUIRE( NoDuplicateFaces(noDuplicateFacesMesh) == true );
}

TEST_CASE( "test WaterTight", "[file_check]" ) {
    MyMesh waterTightMesh = loadMesh("./meshes/perfect.stl")
    REQUIRE( IsWaterTight(WaterTightMesh) == true );
}

TEST_CASE( "test Not WaterTight", "[file_check]" ) {
    MyMesh notWaterTightMesh = loadMesh("./meshes/notWatertight.stl")
    REQUIRE( IsWaterTight(notWaterTightMesh) == false );
}

TEST_CASE( "test Coherently Oriented", "[file_check]" ) {
    MyMesh coherentlyOrientedMesh = loadMesh("./meshes/perfect.stl")
    REQUIRE( IsCoherentlyOrientedMesh(coherentlyOrientedMesh) == true );
}

TEST_CASE( "test not Coherently Oriented", "[file_check]" ) {
    MyMesh notCoherentlyOrientedMesh = loadMesh("./meshes/notCoherentlyOriented.stl")
    REQUIRE( IsCoherentlyOrientedMesh(notCoherentlyOrientedMesh) == false );
}

TEST_CASE( "test Positive Volume", "[file_check]" ) {
    MyMesh positiveVolumeMesh = loadMesh("./meshes/perfect.stl")
    REQUIRE( IsPositiveVolume(positiveVolumeMesh) == true );
}

TEST_CASE( "test not Positive Volume", "[file_check]" ) {
    MyMesh notPositiveVolumeMesh = loadMesh("./meshes/notPositiveVolume.stl")
    REQUIRE( IsPositiveVolume(notPositiveVolumeMesh) == false );
}
