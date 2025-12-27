#pragma once

#include<string>

#include"ResourceHandles.h"

namespace zRender {
    enum PipelineStateType {
        PipelineStateType_PBR_Opaque, PipelineStateType_Skybox, PipelineStateType_PBR_Transparent, PipelineStateType_ShadowMapping, PipelineStateType_Wireframe
    };
    enum PrimitiveTopology { PrimitiveTopology_Triangelist };
    class PipelineStateContainer {
    public:
        std::string name;

        Handle shaderHandle;

        Handle rasterizerHandle;
        Handle depthStencilHandle;
        Handle blendHandle;

        PrimitiveTopology topology;
    };

}