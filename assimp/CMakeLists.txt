cmake_minimum_required(VERSION 3.10)
project(assimp)

find_package(ZLIB REQUIRED)

add_library(assimp STATIC
    assimp/contrib/unzip/unzip.c
    assimp/contrib/unzip/ioapi.c
    assimp/contrib/zip/src/zip.c
    assimp/contrib/irrXML/irrXML.cpp
    assimp/code/CApi/AssimpCExport.cpp
    assimp/code/CApi/CInterfaceIOWrapper.cpp
    assimp/code/Material/MaterialSystem.cpp
    assimp/code/Common/VertexTriangleAdjacency.cpp
    assimp/code/Common/Bitmap.cpp
    assimp/code/Common/Importer.cpp
    assimp/code/Common/Subdivision.cpp
    assimp/code/Common/simd.cpp
    assimp/code/Common/DefaultLogger.cpp
    assimp/code/Common/ZipArchiveIOSystem.cpp
    assimp/code/Common/SpatialSort.cpp
    assimp/code/Common/SplitByBoneCountProcess.cpp
    assimp/code/Common/Exporter.cpp
    assimp/code/Common/BaseProcess.cpp
    assimp/code/Common/BaseImporter.cpp
    assimp/code/Common/scene.cpp
    assimp/code/Common/CreateAnimMesh.cpp
    assimp/code/Common/ImporterRegistry.cpp
    assimp/code/Common/SceneCombiner.cpp
    assimp/code/Common/Assimp.cpp
    assimp/code/Common/DefaultIOStream.cpp
    assimp/code/Common/Version.cpp
    assimp/code/Common/SkeletonMeshBuilder.cpp
    assimp/code/Common/TargetAnimation.cpp
    assimp/code/Common/PostStepRegistry.cpp
    assimp/code/Common/RemoveComments.cpp
    assimp/code/Common/StandardShapes.cpp
    assimp/code/Common/ScenePreprocessor.cpp
    assimp/code/Common/SGSpatialSort.cpp
    assimp/code/Common/DefaultIOSystem.cpp
    assimp/code/Obj/ObjFileMtlImporter.cpp
    assimp/code/Obj/ObjExporter.cpp
    assimp/code/Obj/ObjFileParser.cpp
    assimp/code/Obj/ObjFileImporter.cpp
    assimp/code/PostProcessing/EmbedTexturesProcess.cpp
    assimp/code/PostProcessing/GenBoundingBoxesProcess.cpp
    assimp/code/PostProcessing/OptimizeMeshes.cpp
    assimp/code/PostProcessing/ValidateDataStructure.cpp
    assimp/code/PostProcessing/FixNormalsStep.cpp
    assimp/code/PostProcessing/ProcessHelper.cpp
    assimp/code/PostProcessing/DropFaceNormalsProcess.cpp
    assimp/code/PostProcessing/SortByPTypeProcess.cpp
    assimp/code/PostProcessing/TriangulateProcess.cpp
    assimp/code/PostProcessing/ImproveCacheLocality.cpp
    assimp/code/PostProcessing/MakeVerboseFormat.cpp
    assimp/code/PostProcessing/SplitLargeMeshes.cpp
    assimp/code/PostProcessing/ArmaturePopulate.cpp
    assimp/code/PostProcessing/CalcTangentsProcess.cpp
    assimp/code/PostProcessing/PretransformVertices.cpp
    assimp/code/PostProcessing/FindInvalidDataProcess.cpp
    assimp/code/PostProcessing/FindDegenerates.cpp
    assimp/code/PostProcessing/ConvertToLHProcess.cpp
    assimp/code/PostProcessing/GenVertexNormalsProcess.cpp
    assimp/code/PostProcessing/RemoveRedundantMaterials.cpp
    assimp/code/PostProcessing/TextureTransform.cpp
    assimp/code/PostProcessing/ScaleProcess.cpp
    assimp/code/PostProcessing/LimitBoneWeightsProcess.cpp
    assimp/code/PostProcessing/OptimizeGraph.cpp
    assimp/code/PostProcessing/RemoveVCProcess.cpp
    assimp/code/PostProcessing/ComputeUVMappingProcess.cpp
    assimp/code/PostProcessing/FindInstancesProcess.cpp
    assimp/code/PostProcessing/JoinVerticesProcess.cpp
    assimp/code/PostProcessing/DeboneProcess.cpp
    assimp/code/PostProcessing/GenFaceNormalsProcess.cpp
    assimp/code/Collada/ColladaExporter.cpp
    assimp/code/Collada/ColladaParser.cpp
    assimp/code/Collada/ColladaLoader.cpp
)

target_include_directories(assimp PRIVATE
    assimp/contrib
    assimp/contrib/unzip
    assimp/contrib/zip/src
    assimp/contrib/irrXML
    assimp/contrib/utf8cpp/source
    assimp/code
)

target_link_libraries(assimp PRIVATE ZLIB::ZLIB)

target_include_directories(assimp PUBLIC include/ assimp/include)

target_compile_definitions(assimp PRIVATE
    ASSIMP_BUILD_NO_OWN_ZLIB

    ASSIMP_BUILD_NO_3D_IMPORTER
    ASSIMP_BUILD_NO_3DS_EXPORTER
    ASSIMP_BUILD_NO_3DS_IMPORTER
    ASSIMP_BUILD_NO_3MF_EXPORTER
    ASSIMP_BUILD_NO_3MF_EXPORTERobjnomtl
    ASSIMP_BUILD_NO_3MF_IMPORTER
    ASSIMP_BUILD_NO_AC_IMPORTER
    ASSIMP_BUILD_NO_AMF_IMPORTER
    ASSIMP_BUILD_NO_ASE_IMPORTER
    ASSIMP_BUILD_NO_ASSBIN_EXPORTER
    ASSIMP_BUILD_NO_ASSBIN_IMPORTER
    ASSIMP_BUILD_NO_ASSJSON_EXPORTER
    ASSIMP_BUILD_NO_ASSXML_EXPORTER
    ASSIMP_BUILD_NO_B3D_IMPORTER
#     ASSIMP_BUILD_NO_BASE_IMPORTER
    ASSIMP_BUILD_NO_BLEND_IMPORTER
    ASSIMP_BUILD_NO_BVH_IMPORTER
    ASSIMP_BUILD_NO_C4D_IMPORTER
    ASSIMP_BUILD_NO_COB_IMPORTER
#     ASSIMP_BUILD_NO_COLLADA_EXPORTER
#     ASSIMP_BUILD_NO_COLLADA_IMPORTER
    ASSIMP_BUILD_NO_COMPRESSED_BLEND
    ASSIMP_BUILD_NO_COMPRESSED_IFC
    ASSIMP_BUILD_NO_COMPRESSED_X
    ASSIMP_BUILD_NO_COMPRESSED_XGL
    ASSIMP_BUILD_NO_CSM_IMPORTER
#     ASSIMP_BUILD_NO_DAE_IMPORTER
    ASSIMP_BUILD_NO_DXF_IMPORTER
    ASSIMP_BUILD_NO_FBX_EXPORTER
    ASSIMP_BUILD_NO_FBX_IMPORTER
    ASSIMP_BUILD_NO_GLTF_EXPORTER
    ASSIMP_BUILD_NO_GLTF_IMPORTER
    ASSIMP_BUILD_NO_HMP_IMPORTER
    ASSIMP_BUILD_NO_IFC_IMPORTER
    ASSIMP_BUILD_NO_IRR_IMPORTER
    ASSIMP_BUILD_NO_IRRMESH_IMPORTER
    ASSIMP_BUILD_NO_LWO_IMPORTER
    ASSIMP_BUILD_NO_LWS_IMPORTER
    ASSIMP_BUILD_NO_M3D_EXPORTER
    ASSIMP_BUILD_NO_M3D_IMPORTER
    ASSIMP_BUILD_NO_MD2_IMPORTER
    ASSIMP_BUILD_NO_MD3_IMPORTER
    ASSIMP_BUILD_NO_MD5_IMPORTER
    ASSIMP_BUILD_NO_MDC_IMPORTER
    ASSIMP_BUILD_NO_MDL_IMPORTER
    ASSIMP_BUILD_NO_MMD_IMPORTER
    ASSIMP_BUILD_NO_MS3D_IMPORTER
    ASSIMP_BUILD_NO_NDO_IMPORTER
    ASSIMP_BUILD_NO_NFF_IMPORTER
#     ASSIMP_BUILD_NO_OBJ_EXPORTER
#     ASSIMP_BUILD_NO_OBJ_IMPORTER
    ASSIMP_BUILD_NO_OFF_IMPORTER
    ASSIMP_BUILD_NO_OGRE_IMPORTER
    ASSIMP_BUILD_NO_OPENGEX_EXPORTER
    ASSIMP_BUILD_NO_OPENGEX_IMPORTER
    ASSIMP_BUILD_NO_PLY_EXPORTER
    ASSIMP_BUILD_NO_PLY_IMPORTER
    ASSIMP_BUILD_NO_Q3BSP_IMPORTER
    ASSIMP_BUILD_NO_Q3D_IMPORTER
    ASSIMP_BUILD_NO_RAW_IMPORTER
    ASSIMP_BUILD_NO_SIB_IMPORTER
    ASSIMP_BUILD_NO_SMD_IMPORTER
    ASSIMP_BUILD_NO_STEP_EXPORTER
    ASSIMP_BUILD_NO_STEP_IMPORTER
    ASSIMP_BUILD_NO_STL_EXPORTER
    ASSIMP_BUILD_NO_STL_IMPORTER
    ASSIMP_BUILD_NO_TERRAGEN_IMPORTER
    ASSIMP_BUILD_NO_X3D_EXPORTER
    ASSIMP_BUILD_NO_X3D_IMPORTER
    ASSIMP_BUILD_NO_X_EXPORTER
    ASSIMP_BUILD_NO_XGL_IMPORTER
    ASSIMP_BUILD_NO_X_IMPORTER


#     ASSIMP_BUILD_NO_REMOVEVC_PROCESS
#     ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS
#     ASSIMP_BUILD_NO_ARMATUREPOPULATE_PROCESS
#     ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS
#     ASSIMP_BUILD_NO_DEBONE_PROCESS
#     ASSIMP_BUILD_NO_DROPFACENORMALS_PROCESS
#     ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS
#     ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS
#     ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS
#     ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS
#     ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS
#     ASSIMP_BUILD_NO_FLIPUVS_PROCESS
#     ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS
#     ASSIMP_BUILD_NO_FLIPWINDING_PROCESS
#     ASSIMP_BUILD_NO_GENBOUNDINGBOXES_PROCESS
#     ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS
#     ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS
#     ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS
#     ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS
#     ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS
#     ASSIMP_BUILD_NO_JOINVERTICES_PROCESS
#     ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS
#     ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS
#     ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS
#     ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS
#     ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS
#     ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS
#     ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS
#     ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS
#     ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS
#     ASSIMP_BUILD_NO_TRIANGULATE_PROCESS
#     ASSIMP_BUILD_NO_VALIDATEDS_PROCESS
)
