#pragma once

#include<string>

#include"Renderer/Core/Handles.h"

namespace zRender {
    enum PipelineStateType {
        PipelineStateType_PBR_Opaque, PipelineStateType_Skybox, PipelineStateType_PBR_Transparent, PipelineStateType_ShadowMapping, PipelineStateType_Wireframe
    };
    enum PrimitiveTopology { PrimitiveTopology_Triangelist };
    class PipelineStateContainer {
    public:
        std::string name;

        ShaderHandle shaderHandle;

        RasterizerHandle rasterizerHandle;
        DepthStateHandle depthStencilHandle;
        uint32_t blendHandle;

        PrimitiveTopology topology;
    };

}