#pragma once

#include "Core/Timestep.h"
#include "Core/Events/Event.h"

namespace GLMV {

    class UI
    {
        public:
            UI() = default;
            virtual ~UI() = default;

            virtual void OnUpdate(Timestep ts) {}
            virtual void Render() {}
            virtual void OnEvent(Event& event) {}
    };

}
