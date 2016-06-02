#pragma once
#include <fbxsdk.h>

class ExportData
{
public:
	static bool bInitialized;
	static FbxManager* Manager;
	static FbxExporter* Exporter;
	static FbxScene* Scene;
	static FbxMesh* Mesh;
};