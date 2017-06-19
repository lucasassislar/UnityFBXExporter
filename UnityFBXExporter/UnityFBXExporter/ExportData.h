#pragma once
#include <fbxsdk.h>

class ExportData
{
public:
	static bool bInitialized;
	static char* MeshName;
	static FbxManager* Manager;
	static FbxScene* Scene;
	static FbxMesh* Mesh;
	static FbxSurfacePhong* Material;
	static FbxIOSettings* IOSettings;
	static int FbxCompatibility;
};