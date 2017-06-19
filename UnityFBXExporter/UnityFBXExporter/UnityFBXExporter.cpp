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
			//
			ExportData::bInitialized = true;
			ExportData::Manager = FbxManager::Create();
			ExportData::IOSettings = FbxIOSettings::Create(ExportData::Manager, IOSROOT);
			ExportData::Manager->SetIOSettings(ExportData::IOSettings);

			ExportData::Scene = FbxScene::Create(ExportData::Manager, SceneName);
		}
	}

	__declspec(dllexport) void SetFBXCompatibility(int CompatibilityVersion)
	{
		ExportData::FbxCompatibility = CompatibilityVersion;
	}

	__declspec(dllexport) void BeginMesh(char* MeshName)
	{
		ExportData::Mesh = FbxMesh::Create(ExportData::Scene, MeshName);
		ExportData::MeshName = strdup(MeshName);
	}

	__declspec(dllexport) void EndMesh()
	{
		FbxNode* MeshNode = FbxNode::Create(ExportData::Scene, ExportData::MeshName);
		MeshNode->SetNodeAttribute(ExportData::Mesh);
		MeshNode->SetShadingMode(FbxNode::eTextureShading);
		if (ExportData::Material != nullptr)
		{
			MeshNode->AddMaterial(ExportData::Material);
		}
		ExportData::Scene->GetRootNode()->AddChild(MeshNode);

		// free data
		free(ExportData::MeshName);
		ExportData::Material = nullptr;
		ExportData::Mesh = nullptr;
	}

	__declspec(dllexport) void EnableDefaultMaterial(char* materialName)
	{
		FbxSurfacePhong* Material = FbxSurfacePhong::Create(ExportData::Scene, materialName);
		ExportData::Material = Material;

		FbxString lShadingName = "Phong";
		FbxDouble3 lEmissive(0.0, 0.0, 0.0);
		FbxDouble3 lAmbient(1.0, 1.0, 1.0);
		FbxDouble3 lDiffuseColor(1.0, 1.0, 1.0);
		FbxDouble3 lSpecular(0, 0, 0);
		double specular = 0;

		Material->Emissive.Set(lEmissive);
		Material->Ambient.Set(lAmbient);
		Material->AmbientFactor.Set(1);
		Material->DiffuseFactor.Set(1);
		Material->TransparencyFactor.Set(1);
		Material->ShadingModel.Set(lShadingName);
		Material->Shininess.Set(0.5);
		Material->Specular.Set(lSpecular);
		Material->SpecularFactor.Set(1.0);
	}

	__declspec(dllexport) void SetMaterial(char* materialName, Vector3 Emissive, Vector3 Ambient, Vector3 Diffuse, Vector3 Specular, Vector3 Reflection, double Shininess)
	{
		FbxSurfacePhong* Material = ExportData::Material;
		if (Material == nullptr)
		{
			EnableDefaultMaterial(materialName);
		}

		Material->AmbientFactor.Set(1);
		Material->DiffuseFactor.Set(1);
		Material->SpecularFactor.Set(1);
		Material->TransparencyFactor.Set(1);

		Material->Emissive.Set(FbxDouble3(Emissive.X, Emissive.Y, Emissive.Z));
		Material->Ambient.Set(FbxDouble3(Ambient.X, Ambient.Y, Ambient.Z));
		Material->Diffuse.Set(FbxDouble3(Diffuse.X, Diffuse.Y, Diffuse.Z));
		Material->Specular.Set(FbxDouble3(Specular.X, Specular.Y, Specular.Z));
		Material->Reflection.Set(FbxDouble3(Reflection.X, Reflection.Y, Reflection.Z));
		Material->Shininess.Set(Shininess);
	}

	__declspec(dllexport) void AddIndices(int Triangles[], int Count, int Material)
	{
		for (int i = 0; i < Count - 2; i += 3)
		{
			ExportData::Mesh->BeginPolygon(Material);
			for (int j = 0; j < 3; j++)
			{
				ExportData::Mesh->AddPolygon(Triangles[i + j]);
			}
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

		/*FbxGeometryElementNormal* lGeometryElementNormal = ExportData::Mesh->CreateElementNormal();
		lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

		for (int i = 0; i < Count; i++)
		{
			Vector3 Vec = Normals[i];
			lGeometryElementNormal->GetDirectArray().Add(FbxVector4(Vec.X, Vec.Y, Vec.Z));
		}*/

		FbxLayerElementNormal* LayerElementNormal = FbxLayerElementNormal::Create(ExportData::Mesh, "");
		LayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
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

		// Create UV for Diffuse channel
		/*FbxGeometryElementUV* lUVDiffuseElement = ExportData::Mesh->CreateElementUV(ChannelName);
		FBX_ASSERT(lUVDiffuseElement != NULL);
		lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
		lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

		for (int i = 0; i < Count; i++)
		{
			Vector2 Vec = TexCoords[i];
			lUVDiffuseElement->GetDirectArray().Add(FbxVector2(Vec.X, Vec.Y));
		}*/

		FbxLayerElementUV* UVsLayer = FbxLayerElementUV::Create(ExportData::Mesh, ChannelName);
		UVsLayer->SetMappingMode(FbxLayerElement::eByControlPoint);
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
		const char* FBX = FBX_2016_00_COMPATIBLE;
		switch (ExportData::FbxCompatibility)
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

		FbxExporter* exporter = FbxExporter::Create(ExportData::Manager, "Exporter");
		exporter->SetFileExportVersion(FBX);

		exporter->Initialize(FileName);
		exporter->Export(ExportData::Scene);

		exporter->Destroy(true);
	}
}

