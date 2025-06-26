/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES.
 * All rights reserved.
 * SPDX-License-Identifier: LicenseRef-NvidiaProprietary
 *
 * NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
 * property and proprietary rights in and to this material, related
 * documentation and any modifications thereto. Any use, reproduction,
 * disclosure or distribution of this material and related documentation
 * without an express license agreement from NVIDIA CORPORATION or
 * its affiliates is strictly prohibited.
 */

#define CARB_EXPORTS

#include <carb/PluginUtils.h>
#include <carb/logging/Log.h>
#include <carb/eventdispatcher/IEventDispatcher.h>
#include <carb/events/IEvents.h>
#include <carb/events/EventsUtils.h>

#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>

#include <memory>
#include <carb/input/IInput.h>

#include <memory>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/cube.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdPhysics/collisionAPI.h>
#include <pxr/usd/usdPhysics/massAPI.h>
#include <pxr/usd/usdPhysics/rigidBodyAPI.h>
#include <pxr/usd/usdPhysics/scene.h>
#include <pxr/usd/usdUtils/stageCache.h>
#include <pxr/usd/usd/primRange.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/shader.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/sdf/types.h>
#include <pxr/usd/usd/primRange.h>

#include <chrono>

#define EXTENSION_NAME "a1den.cpp_extension.plugin"

using namespace carb;
using namespace pxr;

// Plugin Implementation Descriptor
const struct carb::PluginImplDesc kPluginImpl = {
    EXTENSION_NAME,  // Name of the plugin (e.g. "carb.dictionary.plugin"). Must be globally unique.
    "Example of a native plugin extension.",  // Helpful text describing the plugin.  Used for debugging/tools.
    "NVIDIA",  // Author
    carb::PluginHotReload::eEnabled,  // If hot reloading is supported by the plugin.  (Note: hot reloading is deprecated)
    "dev"  // Build version of the plugin.
};

// List dependencies for this plugin
CARB_PLUGIN_IMPL_DEPS(omni::kit::IApp, carb::logging::ILogging)

bool MyInputCallback(const carb::input::InputEvent& event, void* userData)
{
    if (event.deviceType == carb::input::DeviceType::eKeyboard)
    {
        if (event.keyboardEvent.type == carb::input::KeyboardEventType::eKeyPress && event.keyboardEvent.key == carb::input::KeyboardInput::eNumpad0)
        {
            // Handle 'A' key pressed
            printf("A key was pressed!\n");

            // Start timing
            auto start = std::chrono::high_resolution_clock::now();

            int counter = 0;
            const std::vector<pxr::UsdStageRefPtr> allStages = pxr::UsdUtilsStageCache::Get().GetAllStages();
            if (allStages.empty())
            {
                printf("No stages found in the cache.\n");
            }
            else
            {
                printf("Found %zu stages in the cache.\n", allStages.size());
            }

            pxr::UsdStageRefPtr stage = allStages[0];
            pxr::UsdPrimRange primRange = stage->Traverse();
            for (auto itr = primRange.begin(); itr != primRange.end(); ++itr)
            {
                pxr::UsdPrim prim = *itr;
                if (prim.IsValid())
                {
                    // std::cout << "Found prim: " << prim.GetPath() << std::endl;

                    // Example: filter geometry
                    if (prim.GetTypeName() == "Mesh")
                    {
                        // std::cout << "  It's a Mesh prim!" << std::endl;
                        counter++;
                    }
                }
            }

            // End timing
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            printf("Execution time: %lld ms\n", duration);

            printf("Found %d Mesh prims in the stage.\n", counter);
        }

        if (event.keyboardEvent.type == carb::input::KeyboardEventType::eKeyPress && event.keyboardEvent.key == carb::input::KeyboardInput::eM)
        {

        }
    }

    return true;
}

class NativeExtensionExample : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        printf(EXTENSION_NAME ": in onStartup\n");
        // Get app interface using Carbonite Framework
        omni::kit::IApp* app = carb::getFramework()->acquireInterface<omni::kit::IApp>();

        printf("Handle Input\n");
        carb::input::IInput* input = carb::getFramework()->acquireInterface<carb::input::IInput>();
        input = carb::getFramework()->acquireInterface<carb::input::IInput>();
        if (input)
        {
            printf("subscribeToInputEvents\n");
            input->subscribeToInputEvents(nullptr, -1, MyInputCallback, nullptr);
        }
    }

    void onShutdown() override
    {
        // Unsubscribes from the event stream
        m_holder = nullptr;
    }

private:
    int m_counter = 0;
    carb::ObjectPtr<carb::events::ISubscription> m_holder;
};

// Generate boilerplate code
CARB_PLUGIN_IMPL(kPluginImpl, NativeExtensionExample)

// There must be a fillInterface(InterfaceType&) function for each interface type that is exported by this plugin.
void fillInterface(NativeExtensionExample& iface)
{
}
