#pragma once

#include <algorithm>
#include <ranges>
#include <imgui.h>
#include <string>
#include <iterator>

// ============================================================================
// Semantic Colors — universal across all themes
// ============================================================================
namespace SemanticColors {
    // State indicators
    inline constexpr ImVec4 MODIFIED     { 1.00f, 0.80f, 0.00f, 1.00f };
    inline constexpr ImVec4 SAVED        { 0.20f, 0.80f, 0.20f, 1.00f };
    inline constexpr ImVec4 DANGER       { 0.90f, 0.30f, 0.30f, 1.00f };
    inline constexpr ImVec4 WARNING      { 0.90f, 0.70f, 0.20f, 1.00f };
    inline constexpr ImVec4 INFO         { 0.20f, 0.50f, 0.90f, 1.00f };
    inline constexpr ImVec4 GOLD         { 1.00f, 0.80f, 0.00f, 1.00f };

    // Text roles
    inline constexpr ImVec4 LABEL        { 0.55f, 0.58f, 0.62f, 1.00f };
    inline constexpr ImVec4 VALUE        { 0.95f, 0.95f, 0.95f, 1.00f };
    inline constexpr ImVec4 EMPTY        { 0.40f, 0.42f, 0.45f, 1.00f };
    inline constexpr ImVec4 HEADER_TEXT  { 0.85f, 0.88f, 0.92f, 1.00f };
    inline constexpr ImVec4 MUTED        { 0.67f, 0.70f, 0.75f, 1.00f };

    // Pulse/Glow base
    inline constexpr ImVec4 PULSE_BASE   { 0.60f, 0.50f, 0.00f, 1.00f };

    // Misc
    inline constexpr float DISABLED_ALPHA = 0.5f;

    // Theme-adaptive text colors (samples from current ImGui style)
    inline ImVec4 TextPrimary() { return ImGui::GetStyleColorVec4(ImGuiCol_Text); }
    inline ImVec4 TextDim()     { return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled); }

    // Helpers for deriving hover/active variants
    constexpr ImVec4 Lighten(const ImVec4& c, float amount = 0.12f) {
        return ImVec4(std::clamp(c.x + amount, 0.0f, 1.0f),
                      std::clamp(c.y + amount, 0.0f, 1.0f),
                      std::clamp(c.z + amount, 0.0f, 1.0f), c.w);
    }
    constexpr ImVec4 Darken(const ImVec4& c, float amount = 0.12f) {
        return ImVec4(std::clamp(c.x - amount, 0.0f, 1.0f),
                      std::clamp(c.y - amount, 0.0f, 1.0f),
                      std::clamp(c.z - amount, 0.0f, 1.0f), c.w);
    }
} // namespace SemanticColors

// ============================================================================
// Theme selection
// ============================================================================
enum class ThemeType {
    DocumentLight = 0,
    DocumentDark,
    DocumentMidnight
};

struct ThemeInfo {
    ThemeType type;
    const char* name;
    const char* description;
    ImVec4 preview_color;
};

constexpr ThemeInfo AVAILABLE_THEMES[] = {
    { ThemeType::DocumentLight, "Document Light", "Warm parchment and ink", {0.96f, 0.94f, 0.89f, 1.0f} },
    { ThemeType::DocumentDark, "Document Dark", "Inkwell and candlelight", {0.11f, 0.10f, 0.08f, 1.0f} },
    { ThemeType::DocumentMidnight, "Document Midnight", "Warm charcoal and power-tool orange", {0.08f, 0.07f, 0.04f, 1.0f} },
};

constexpr size_t THEME_COUNT = std::size(AVAILABLE_THEMES);

inline const char* GetThemeName(ThemeType type) {
    const auto it = std::ranges::find(AVAILABLE_THEMES, type, &ThemeInfo::type);
    if (it != std::end(AVAILABLE_THEMES)) {
        return it->name;
    }
    return "Unknown";
}

// ============================================================================
// Document Midnight Theme
// ============================================================================
inline void ApplyDocumentMidnight() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 4.0f;
    style.FrameRounding     = 3.0f;
    style.GrabRounding      = 3.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 3.0f;
    style.TabRounding       = 3.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 0.0f;
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style.Colors;

    ImVec4 bg_deep      = ImVec4(0.078f, 0.071f, 0.043f, 1.00f);
    ImVec4 bg_medium    = ImVec4(0.118f, 0.110f, 0.075f, 1.00f);
    ImVec4 bg_light     = ImVec4(0.153f, 0.145f, 0.106f, 1.00f);
    ImVec4 bg_lighter   = ImVec4(0.196f, 0.188f, 0.157f, 1.00f);
    ImVec4 accent       = ImVec4(0.784f, 0.447f, 0.180f, 1.00f);
    ImVec4 accent_hover = ImVec4(0.847f, 0.514f, 0.290f, 1.00f);
    ImVec4 accent_active= ImVec4(0.647f, 0.361f, 0.133f, 1.00f);
    ImVec4 text_main    = ImVec4(0.969f, 0.969f, 0.957f, 1.00f);
    ImVec4 text_dim     = ImVec4(0.533f, 0.514f, 0.478f, 1.00f);

    colors[ImGuiCol_Text]                  = text_main;
    colors[ImGuiCol_TextDisabled]          = text_dim;
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.784f, 0.447f, 0.180f, 0.28f);
    colors[ImGuiCol_WindowBg]              = bg_deep;
    colors[ImGuiCol_ChildBg]               = bg_deep;
    colors[ImGuiCol_PopupBg]               = bg_medium;
    colors[ImGuiCol_Border]                = ImVec4(0.55f, 0.52f, 0.40f, 0.15f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TitleBg]               = bg_medium;
    colors[ImGuiCol_TitleBgActive]         = bg_light;
    colors[ImGuiCol_TitleBgCollapsed]      = bg_medium;
    colors[ImGuiCol_FrameBg]               = bg_medium;
    colors[ImGuiCol_FrameBgHovered]        = bg_light;
    colors[ImGuiCol_FrameBgActive]         = bg_lighter;
    colors[ImGuiCol_Button]                = bg_medium;
    colors[ImGuiCol_ButtonHovered]         = bg_light;
    colors[ImGuiCol_ButtonActive]          = bg_lighter;
    colors[ImGuiCol_ScrollbarBg]           = bg_deep;
    colors[ImGuiCol_ScrollbarGrab]         = bg_light;
    colors[ImGuiCol_ScrollbarGrabHovered]  = bg_lighter;
    colors[ImGuiCol_ScrollbarGrabActive]   = accent;
    colors[ImGuiCol_CheckMark]             = accent;
    colors[ImGuiCol_SliderGrab]            = accent;
    colors[ImGuiCol_SliderGrabActive]      = accent_active;
    colors[ImGuiCol_Separator]             = ImVec4(0.55f, 0.52f, 0.40f, 0.12f);
    colors[ImGuiCol_SeparatorHovered]      = accent_hover;
    colors[ImGuiCol_SeparatorActive]       = accent_active;
    colors[ImGuiCol_ResizeGrip]            = accent;
    colors[ImGuiCol_ResizeGripHovered]     = accent_hover;
    colors[ImGuiCol_ResizeGripActive]      = accent_active;
    colors[ImGuiCol_Tab]                   = bg_medium;
    colors[ImGuiCol_TabHovered]            = bg_light;
    colors[ImGuiCol_TabActive]             = bg_lighter;
    colors[ImGuiCol_TabUnfocused]          = bg_medium;
    colors[ImGuiCol_TabUnfocusedActive]    = bg_light;
    colors[ImGuiCol_MenuBarBg]             = bg_light;
    colors[ImGuiCol_Header]                = bg_medium;
    colors[ImGuiCol_HeaderHovered]         = bg_light;
    colors[ImGuiCol_HeaderActive]          = bg_lighter;
    colors[ImGuiCol_TableHeaderBg]         = bg_light;
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.55f, 0.52f, 0.40f, 0.22f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.40f, 0.38f, 0.28f, 0.10f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 0.95f, 0.85f, 0.03f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.784f, 0.447f, 0.180f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = accent;
    colors[ImGuiCol_NavWindowingHighlight] = accent_hover;
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.08f, 0.07f, 0.04f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.04f, 0.03f, 0.02f, 0.50f);
}

// ============================================================================
// Document Dark Theme
// ============================================================================
inline void ApplyDocumentDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 4.0f;
    style.GrabRounding      = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding       = 4.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style.Colors;

    ImVec4 bg_deep          = ImVec4(0.11f, 0.10f, 0.08f, 1.00f);
    ImVec4 bg_medium        = ImVec4(0.14f, 0.13f, 0.09f, 1.00f);
    ImVec4 bg_light         = ImVec4(0.18f, 0.16f, 0.13f, 1.00f);
    ImVec4 bg_lighter       = ImVec4(0.21f, 0.20f, 0.16f, 1.00f);
    ImVec4 accent_ink       = ImVec4(0.36f, 0.51f, 0.80f, 1.00f);
    ImVec4 accent_hover     = ImVec4(0.48f, 0.63f, 0.88f, 1.00f);
    ImVec4 accent_active    = ImVec4(0.25f, 0.40f, 0.68f, 1.00f);
    ImVec4 text_cream       = ImVec4(0.91f, 0.89f, 0.83f, 1.00f);
    ImVec4 text_dim         = ImVec4(0.54f, 0.52f, 0.47f, 1.00f);

    colors[ImGuiCol_Text]                  = text_cream;
    colors[ImGuiCol_TextDisabled]          = text_dim;
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.36f, 0.51f, 0.80f, 0.30f);
    colors[ImGuiCol_WindowBg]              = bg_deep;
    colors[ImGuiCol_ChildBg]               = bg_deep;
    colors[ImGuiCol_PopupBg]               = bg_medium;
    colors[ImGuiCol_Border]                = ImVec4(0.55f, 0.53f, 0.47f, 0.25f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TitleBg]               = bg_medium;
    colors[ImGuiCol_TitleBgActive]         = bg_light;
    colors[ImGuiCol_TitleBgCollapsed]      = bg_medium;
    colors[ImGuiCol_FrameBg]               = bg_medium;
    colors[ImGuiCol_FrameBgHovered]        = bg_light;
    colors[ImGuiCol_FrameBgActive]         = bg_lighter;
    colors[ImGuiCol_Button]                = bg_medium;
    colors[ImGuiCol_ButtonHovered]         = bg_light;
    colors[ImGuiCol_ButtonActive]          = bg_lighter;
    colors[ImGuiCol_ScrollbarBg]           = bg_deep;
    colors[ImGuiCol_ScrollbarGrab]         = bg_light;
    colors[ImGuiCol_ScrollbarGrabHovered]  = bg_lighter;
    colors[ImGuiCol_ScrollbarGrabActive]   = accent_ink;
    colors[ImGuiCol_CheckMark]             = accent_ink;
    colors[ImGuiCol_SliderGrab]            = accent_ink;
    colors[ImGuiCol_SliderGrabActive]      = accent_active;
    colors[ImGuiCol_Separator]             = ImVec4(0.55f, 0.53f, 0.47f, 0.22f);
    colors[ImGuiCol_SeparatorHovered]      = accent_hover;
    colors[ImGuiCol_SeparatorActive]       = accent_active;
    colors[ImGuiCol_ResizeGrip]            = accent_ink;
    colors[ImGuiCol_ResizeGripHovered]     = accent_hover;
    colors[ImGuiCol_ResizeGripActive]      = accent_active;
    colors[ImGuiCol_Tab]                   = bg_medium;
    colors[ImGuiCol_TabHovered]            = bg_light;
    colors[ImGuiCol_TabActive]             = bg_lighter;
    colors[ImGuiCol_TabUnfocused]          = bg_medium;
    colors[ImGuiCol_TabUnfocusedActive]    = bg_light;
    colors[ImGuiCol_MenuBarBg]             = bg_medium;
    colors[ImGuiCol_Header]                = bg_medium;
    colors[ImGuiCol_HeaderHovered]         = bg_light;
    colors[ImGuiCol_HeaderActive]          = bg_lighter;
    colors[ImGuiCol_TableHeaderBg]         = bg_medium;
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.55f, 0.53f, 0.47f, 0.40f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.40f, 0.38f, 0.34f, 0.25f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 0.90f, 0.04f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.36f, 0.51f, 0.80f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = accent_ink;
    colors[ImGuiCol_NavWindowingHighlight] = accent_hover;
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.10f, 0.10f, 0.10f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.05f, 0.05f, 0.05f, 0.45f);
}

// ============================================================================
// Document Light Theme
// ============================================================================
inline void ApplyDocumentLightTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 4.0f;
    style.GrabRounding      = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding       = 4.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(6.0f, 4.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style.Colors;

    ImVec4 bg_paper         = ImVec4(0.96f, 0.94f, 0.89f, 1.00f);
    ImVec4 bg_medium        = ImVec4(0.90f, 0.88f, 0.83f, 1.00f);
    ImVec4 bg_light         = ImVec4(0.84f, 0.82f, 0.77f, 1.00f);
    ImVec4 bg_lighter       = ImVec4(0.77f, 0.75f, 0.70f, 1.00f);
    ImVec4 accent_ink       = ImVec4(0.18f, 0.32f, 0.62f, 1.00f);
    ImVec4 accent_hover     = ImVec4(0.24f, 0.42f, 0.78f, 1.00f);
    ImVec4 accent_active    = ImVec4(0.12f, 0.22f, 0.50f, 1.00f);
    ImVec4 text_ink         = ImVec4(0.10f, 0.10f, 0.13f, 1.00f);
    ImVec4 text_dim         = ImVec4(0.42f, 0.42f, 0.46f, 1.00f);

    colors[ImGuiCol_Text]                  = text_ink;
    colors[ImGuiCol_TextDisabled]          = text_dim;
    colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.18f, 0.32f, 0.62f, 0.25f);
    colors[ImGuiCol_WindowBg]              = bg_paper;
    colors[ImGuiCol_ChildBg]               = bg_paper;
    colors[ImGuiCol_PopupBg]               = bg_medium;
    colors[ImGuiCol_Border]                = ImVec4(0.62f, 0.60f, 0.55f, 0.80f);
    colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TitleBg]               = bg_medium;
    colors[ImGuiCol_TitleBgActive]         = bg_light;
    colors[ImGuiCol_TitleBgCollapsed]      = bg_medium;
    colors[ImGuiCol_FrameBg]               = bg_medium;
    colors[ImGuiCol_FrameBgHovered]        = bg_light;
    colors[ImGuiCol_FrameBgActive]         = bg_lighter;
    colors[ImGuiCol_Button]                = bg_medium;
    colors[ImGuiCol_ButtonHovered]         = bg_light;
    colors[ImGuiCol_ButtonActive]          = bg_lighter;
    colors[ImGuiCol_ScrollbarBg]           = bg_paper;
    colors[ImGuiCol_ScrollbarGrab]         = bg_light;
    colors[ImGuiCol_ScrollbarGrabHovered]  = bg_lighter;
    colors[ImGuiCol_ScrollbarGrabActive]   = accent_ink;
    colors[ImGuiCol_CheckMark]             = accent_ink;
    colors[ImGuiCol_SliderGrab]            = accent_ink;
    colors[ImGuiCol_SliderGrabActive]      = accent_active;
    colors[ImGuiCol_Separator]             = ImVec4(0.62f, 0.60f, 0.55f, 0.60f);
    colors[ImGuiCol_SeparatorHovered]      = accent_hover;
    colors[ImGuiCol_SeparatorActive]       = accent_active;
    colors[ImGuiCol_ResizeGrip]            = accent_ink;
    colors[ImGuiCol_ResizeGripHovered]     = accent_hover;
    colors[ImGuiCol_ResizeGripActive]      = accent_active;
    colors[ImGuiCol_Tab]                   = bg_medium;
    colors[ImGuiCol_TabHovered]            = bg_light;
    colors[ImGuiCol_TabActive]             = bg_lighter;
    colors[ImGuiCol_TabUnfocused]          = bg_medium;
    colors[ImGuiCol_TabUnfocusedActive]    = bg_light;
    colors[ImGuiCol_MenuBarBg]             = bg_medium;
    colors[ImGuiCol_Header]                = bg_medium;
    colors[ImGuiCol_HeaderHovered]         = bg_light;
    colors[ImGuiCol_HeaderActive]          = bg_lighter;
    colors[ImGuiCol_TableHeaderBg]         = bg_medium;
    colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.62f, 0.60f, 0.55f, 0.80f);
    colors[ImGuiCol_TableBorderLight]      = ImVec4(0.75f, 0.73f, 0.68f, 0.50f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.60f, 0.58f, 0.53f, 0.12f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.18f, 0.32f, 0.62f, 0.90f);
    colors[ImGuiCol_NavHighlight]          = accent_ink;
    colors[ImGuiCol_NavWindowingHighlight] = accent_hover;
    colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.70f, 0.70f, 0.70f, 0.35f);
}

inline void ApplyTheme(ThemeType type) {
    switch (type) {
        case ThemeType::DocumentDark: ApplyDocumentDarkTheme(); break;
        case ThemeType::DocumentMidnight: ApplyDocumentMidnight(); break;
        default: ApplyDocumentLightTheme(); break;
    }
}
