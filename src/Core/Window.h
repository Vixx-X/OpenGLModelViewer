#pragma once

#include "Base.h"

#include <GLFW/glfw3.h>
#include "Core/Core.h"
#include "Core/Events/Event.h"

namespace GLMV {

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(const std::string& title = "GL Model Viewer",
                uint32_t width = 1280,
                uint32_t height = 720)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    // Interface representing a desktop system based Window
    class Window
    {
        public:
            using EventCallbackFn = std::function<void(Event&)>;

            ~Window() = default;

            void OnUpdate();

            inline uint32_t GetWidth() const { return m_Data.Width; }
            inline uint32_t GetHeight() const { return m_Data.Height; }

            // Window attributes
            void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
            void SetVSync(bool enabled);
            bool IsVSync() const;

            void* GetNativeWindow() const { return m_Window; }

            static Window* Create(const WindowProps& props = WindowProps());

        private:
            void Init(const WindowProps& props);
            void Shutdown();

            GLFWwindow* m_Window;

            struct WindowData
            {
                std::string Title;
                uint32_t Width, Height;
                bool VSync;

                EventCallbackFn EventCallback;
            };

            WindowData m_Data;
    };

}
