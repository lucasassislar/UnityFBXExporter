#pragma once
#include "ExportData.h"

bool ExportData::bInitialized = false;
FbxManager* ExportData::Manager = nullptr;
FbxScene* ExportData::Scene = nullptr;
FbxMesh* ExportData::Mesh = nullptr;
FbxSurfacePhong* ExportData::Material = nullptr;
FbxIOSettings* ExportData::IOSettings = nullptr;
char* ExportData::MeshName = nullptr;
int ExportData::FbxCompatibility = 4;//2014 by default
