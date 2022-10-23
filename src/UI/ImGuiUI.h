#pragma once

#include "UI.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

namespace GLMV {

    class ImGuiUI : public UI
    {
        public:
            ImGuiUI();
            ~ImGuiUI();

            virtual void OnEvent(Event& e) override;
            virtual void Render() override;

            void Begin();
            void End();

            void BlockEvents(bool block) { m_BlockEvents = block; }
        private:
            bool m_BlockEvents = true;
            float m_Time = 0.0f;
    };
}
