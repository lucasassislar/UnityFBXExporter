#include "ExportData.h"

bool ExportData::bInitialized = false;
FbxManager* ExportData::Manager = nullptr;
FbxExporter* ExportData::Exporter = nullptr;
FbxScene* ExportData::Scene = nullptr;
FbxMesh* ExportData::Mesh = nullptr;
