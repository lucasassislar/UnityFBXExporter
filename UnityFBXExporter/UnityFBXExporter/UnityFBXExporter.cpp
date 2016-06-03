#pragma once
#include "ExportData.h"
#include "Vector3.h"
#include "Vector2.h"

extern "C"
{
	__declspec(dllexport) void Initialize(char* SceneName)
	{
		if (ExportData::bInitialized)
		{
			ExportData::Scene->Clear();
			ExportData::Scene->SetName(SceneName);
		}
		else
		{
			ExportData::bInitialized = true;
			ExportData::Manager = FbxManager::Create();
			ExportData::Exporter = FbxExporter::Create(ExportData::Manager, "Exporter");
			ExportData::Scene = FbxScene::Create(ExportData::Manager, SceneName);
		}
	}

	__declspec(dllexport) void SetFBXCompatibility(int CompatibilityVersion)
	{
		const char* FBX = FBX_2016_00_COMPATIBLE;
		switch (CompatibilityVersion)
		{
			case 0:
				FBX = FBX_2010_00_COMPATIBLE;
				break;
			case 1:
				FBX = FBX_2011_00_COMPATIBLE;
				break;
			case 2:
				FBX = FBX_2012_00_COMPATIBLE;
				break;
			case 3:
				FBX = FBX_2013_00_COMPATIBLE;
				break;
			case 4:
				FBX = FBX_2014_00_COMPATIBLE;
				break;
			case 5:
				FBX = FBX_2016_00_COMPATIBLE;
				break;
		}

		ExportData::Exporter->SetFileExportVersion(FBX);
	}

	__declspec(dllexport) void AddMesh(char* MeshName)
	{
		FbxNode* Node = FbxNode::Create(ExportData::Scene, MeshName);
		ExportData::Scene->GetRootNode()->AddChild(Node);
		ExportData::Mesh = FbxMesh::Create(ExportData::Scene, MeshName);

		Node->SetNodeAttribute(ExportData::Mesh);
	}

	__declspec(dllexport) void AddIndices(int Triangles[], int Count, int Material)
	{
		for (int i = 0; i < Count - 2; i += 3)
		{
			ExportData::Mesh->BeginPolygon(Material);
			ExportData::Mesh->AddPolygon(Triangles[i]);
			ExportData::Mesh->AddPolygon(Triangles[i + 1]);
			ExportData::Mesh->AddPolygon(Triangles[i + 2]);
			ExportData::Mesh->EndPolygon();
		}
	}

	__declspec(dllexport) void AddVertices(Vector3 Vertices[], int Count)
	{
		ExportData::Mesh->InitControlPoints(Count);

		FbxVector4* ControlPoints = ExportData::Mesh->GetControlPoints();
		for (int i = 0; i < Count; i++)
		{
			Vector3 Vec = Vertices[i];
			ControlPoints[i] = FbxVector4(Vec.X, Vec.Y, Vec.Z);
		}
	}

	__declspec(dllexport) void AddNormals(Vector3 Normals[], int Count)
	{
		FbxLayer* Layer = ExportData::Mesh->GetLayer(0);

		if (Layer == nullptr)
		{
			ExportData::Mesh->CreateLayer();
			Layer = ExportData::Mesh->GetLayer(0);
		}

		FbxLayerElementNormal* LayerElementNormal = FbxLayerElementNormal::Create(ExportData::Mesh, "");
		LayerElementNormal->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		LayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

		for (int i = 0; i < Count; i++)
		{
			Vector3 Vec = Normals[i];
			LayerElementNormal->GetDirectArray().Add(FbxVector4(Vec.X, Vec.Y, Vec.Z));
		}
		Layer->SetNormals(LayerElementNormal);
	}

	__declspec(dllexport) void AddTexCoords(Vector2 TexCoords[], int Count, int UVLayer, char* ChannelName)
	{
		FbxLayer* Layer = ExportData::Mesh->GetLayer(UVLayer);

		if (Layer == nullptr)
		{
			int layers = ExportData::Mesh->GetLayerCount();
			int toCreate = UVLayer - layers;
			for (int i = 0; i <= toCreate; i++)
			{
				ExportData::Mesh->CreateLayer();
			}
			Layer = ExportData::Mesh->GetLayer(UVLayer);
		}

		FbxLayerElementUV* UVsLayer = FbxLayerElementUV::Create(ExportData::Mesh, ChannelName);
		UVsLayer->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		UVsLayer->SetReferenceMode(FbxLayerElement::eDirect);//eIndexToDirect

		for (int i = 0; i < Count; i++)
		{
			Vector2 Vec = TexCoords[i];
			UVsLayer->GetDirectArray().Add(FbxVector2(Vec.X, Vec.Y));
		}
		Layer->SetUVs(UVsLayer);
	}

	__declspec(dllexport) void AddMaterial(Vector3 DiffuseColor)
	{
		FbxLayer* Layer = ExportData::Mesh->GetLayer(0);

		if (Layer == nullptr)
		{
			ExportData::Mesh->CreateLayer();
			Layer = ExportData::Mesh->GetLayer(0);
		}

		FbxLayerElementMaterial* MatLayer = FbxLayerElementMaterial::Create(ExportData::Mesh, "");
		MatLayer->SetMappingMode(FbxLayerElement::eByPolygon);
		MatLayer->SetReferenceMode(FbxLayerElement::eIndexToDirect);
		Layer->SetMaterials(MatLayer);

		FbxSurfaceMaterial* FbxMaterial = FbxSurfaceLambert::Create(ExportData::Scene, "Default Material");
		((FbxSurfaceLambert*)FbxMaterial)->Diffuse.Set(FbxDouble3(DiffuseColor.X, DiffuseColor.Y, DiffuseColor.Z));
		ExportData::Scene->AddMaterial(FbxMaterial);
	}

	__declspec(dllexport) void Export(char* FileName)
	{
		ExportData::Exporter->Initialize(FileName);
		ExportData::Exporter->Export(ExportData::Scene);
	}
}

