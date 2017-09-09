// dear imgui, v1.50 WIP
// (drawing and font code)

// Contains implementation for
// - ImDrawList
// - ImDrawData
// - ImFontAtlas
// - ImFont
// - Default font data

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include "../Settings/Vars.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui_internal.h"

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca)
#ifdef _WIN32
#include <malloc.h>     // alloca
#elif (defined(__FreeBSD__) || defined(FreeBSD_kernel) || defined(__DragonFly__)) && !defined(__GLIBC__)
#include <stdlib.h>     // alloca. FreeBSD uses stdlib.h unless GLIBC
#else
#include <alloca.h>     // alloca
#endif
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"      // warning : macro name is a reserved identifier                //
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wdouble-promotion"         // warning: implicit conversion from 'float' to 'double' when passing argument to function
#pragma GCC diagnostic ignored "-Wconversion"               // warning: conversion to 'xxxx' from 'xxxx' may alter its value
#pragma GCC diagnostic ignored "-Wcast-qual"                // warning: cast from type 'xxxx' to type 'xxxx' casts away qualifiers
#endif

//-------------------------------------------------------------------------
// STB libraries implementation
//-------------------------------------------------------------------------

//#define IMGUI_STB_NAMESPACE     ImGuiStb
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456)                             // declaration of 'xx' hides previous local declaration
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"              // warning: comparison is always true due to limited range of data type [-Wtype-limits]
#endif

#define STBRP_ASSERT(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#include "stb_rect_pack.h"

#define STBTT_malloc(x,u)  ((void)(u), ImGui::MemAlloc(x))
#define STBTT_free(x,u)    ((void)(u), ImGui::MemFree(x))
#define STBTT_assert(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#include "stb_truetype.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImGuiStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

static const ImVec4 GNullClipRect(-8192.0f, -8192.0f, +8192.0f, +8192.0f); // Large values that are easy to encode in a few bits+shift

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    // NB: Do not clear channels so our allocations are re-used after the first frame.
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

// Use macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug mode
#define GetCurrentClipRect()    (_ClipRectStack.Size ? _ClipRectStack.Data[_ClipRectStack.Size-1]  : GNullClipRect)
#define GetCurrentTextureId()   (_TextureIdStack.Size ? _TextureIdStack.Data[_TextureIdStack.Size-1] : NULL)

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const ImVec4 curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size-1] : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) != 0) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (curr_cmd->ElemCount == 0 && prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}

void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(ImVec4)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}

#undef GetCurrentClipRect
#undef GetCurrentTextureId

// Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(ImVec2 cr_min, ImVec2 cr_max, bool intersect_with_current_clip_rect)
{
    ImVec4 cr(cr_min.x, cr_min.y, cr_max.x, cr_max.y);
    if (intersect_with_current_clip_rect && _ClipRectStack.Size)
    {
        ImVec4 current = _ClipRectStack.Data[_ClipRectStack.Size-1];
        if (cr.x < current.x) cr.x = current.x;
        if (cr.y < current.y) cr.y = current.y;
        if (cr.z > current.z) cr.z = current.z;
        if (cr.w > current.w) cr.w = current.w;
    }
    cr.z = ImMax(cr.x, cr.z);
    cr.w = ImMax(cr.y, cr.w);

    _ClipRectStack.push_back(cr);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(ImVec2(GNullClipRect.x, GNullClipRect.y), ImVec2(GNullClipRect.z, GNullClipRect.w));
    //PushClipRect(GetVisibleRect());   // FIXME-OPT: This would be more correct but we're not supposed to access ImGuiContext from here?
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::PushTextureID(const ImTextureID& texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

void ImDrawList::ChannelsSplit(int channels_count)
{
    IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _ChannelsCount = channels_count;

    // _Channels[] (24 bytes each) hold storage that we'll swap with this->_CmdBuffer/_IdxBuffer
    // The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        }
        else
        {
            _Channels[i].CmdBuffer.resize(0);
            _Channels[i].IdxBuffer.resize(0);
        }
        if (_Channels[i].CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = _ClipRectStack.back();
            draw_cmd.TextureId = _TextureIdStack.back();
            _Channels[i].CmdBuffer.push_back(draw_cmd);
        }
    }
}

void ImDrawList::ChannelsMerge()
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_ChannelsCount <= 1)
        return;

    ChannelsSetCurrent(0);
    if (CmdBuffer.Size && CmdBuffer.back().ElemCount == 0)
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd* cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    AddDrawCmd();
    _ChannelsCount = 1;
}

void ImDrawList::ChannelsSetCurrent(int idx)
{
    IM_ASSERT(idx < _ChannelsCount);
    if (_ChannelsCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
    memcpy(&_Channels.Data[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelsCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}

// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size-1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_size;

    int idx_buffer_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_size;
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(GImGui->FontTexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness, bool anti_aliased)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedLines;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    int count = points_count;
    if (!closed)
        count = points_count-1;

    const bool thick_line = thickness > 1.0f;
    if (anti_aliased)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & IM_COL32(255,255,255,0);

        const int idx_count = thick_line ? count*18 : count*12;
        const int vtx_count = thick_line ? points_count*4 : points_count*3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2));
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            ImVec2 diff = points[i2] - points[i1];
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i1].x = diff.y;
            temp_normals[i1].y = -diff.x;
        }
        if (!closed)
            temp_normals[points_count-1] = temp_normals[points_count-2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count-1)*2+0] = points[points_count-1] + temp_normals[points_count-1] * AA_SIZE;
                temp_points[(points_count-1)*2+1] = points[points_count-1] - temp_normals[points_count-1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+3;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                dm *= AA_SIZE;
                temp_points[i2*2+0] = points[i2] + dm;
                temp_points[i2*2+1] = points[i2] - dm;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2+0); _IdxWritePtr[1] = (ImDrawIdx)(idx1+0); _IdxWritePtr[2] = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1+2); _IdxWritePtr[4] = (ImDrawIdx)(idx2+2); _IdxWritePtr[5] = (ImDrawIdx)(idx2+0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2+1); _IdxWritePtr[7] = (ImDrawIdx)(idx1+1); _IdxWritePtr[8] = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1+0); _IdxWritePtr[10]= (ImDrawIdx)(idx2+0); _IdxWritePtr[11]= (ImDrawIdx)(idx2+1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i*2+0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i*2+1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        }
        else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+0] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+1] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+2] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+3] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+4;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                ImVec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
                ImVec2 dm_in = dm * half_inner_thickness;
                temp_points[i2*4+0] = points[i2] + dm_out;
                temp_points[i2*4+1] = points[i2] + dm_in;
                temp_points[i2*4+2] = points[i2] - dm_in;
                temp_points[i2*4+3] = points[i2] - dm_out;

                // Add indexes
                _IdxWritePtr[0]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[1]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[2]  = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3]  = (ImDrawIdx)(idx1+2); _IdxWritePtr[4]  = (ImDrawIdx)(idx2+2); _IdxWritePtr[5]  = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[6]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[7]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[8]  = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9]  = (ImDrawIdx)(idx1+0); _IdxWritePtr[10] = (ImDrawIdx)(idx2+0); _IdxWritePtr[11] = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2+2); _IdxWritePtr[13] = (ImDrawIdx)(idx1+2); _IdxWritePtr[14] = (ImDrawIdx)(idx1+3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1+3); _IdxWritePtr[16] = (ImDrawIdx)(idx2+3); _IdxWritePtr[17] = (ImDrawIdx)(idx2+2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i*4+0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i*4+1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i*4+2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i*4+3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count*6;
        const int vtx_count = count*4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];
            ImVec2 diff = p2 - p1;
            diff *= ImInvLength(diff, 1.0f);

            const float dx = diff.x * (thickness * 0.5f);
            const float dy = diff.y * (thickness * 0.5f);
            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col, bool anti_aliased)
{
    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedShapes;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    if (anti_aliased)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & IM_COL32(255,255,255,0);
        const int idx_count = (points_count-2)*3 + points_count*6;
        const int vtx_count = (points_count*2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx+1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+((i-1)<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx+(i<<1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 diff = p1 - p0;
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i0].x = diff.y;
            temp_normals[i0].y = -diff.x;
        }

        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            ImVec2 dm = (n0 + n1) * 0.5f;
            float dmr2 = dm.x*dm.x + dm.y*dm.y;
            if (dmr2 > 0.000001f)
            {
                float scale = 1.0f / dmr2;
                if (scale > 100.0f) scale = 100.0f;
                dm *= scale;
            }
            dm *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx+(i1<<1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+(i0<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx+(i0<<1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx+(i0<<1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx+(i1<<1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx+(i1<<1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count-2)*3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+i-1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2& centre, float radius, int amin, int amax)
{
    static ImVec2 circle_vtx[12];
    static bool circle_vtx_builds = false;
    const int circle_vtx_count = IM_ARRAYSIZE(circle_vtx);
    if (!circle_vtx_builds)
    {
        for (int i = 0; i < circle_vtx_count; i++)
        {
            const float a = ((float)i / (float)circle_vtx_count) * 2*IM_PI;
            circle_vtx[i].x = cosf(a);
            circle_vtx[i].y = sinf(a);
        }
        circle_vtx_builds = true;
    }

    if (amin > amax) return;
    if (radius == 0.0f)
    {
        _Path.push_back(centre);
    }
    else
    {
        _Path.reserve(_Path.Size + (amax - amin + 1));
        for (int a = amin; a <= amax; a++)
        {
            const ImVec2& c = circle_vtx[a % circle_vtx_count];
            _Path.push_back(ImVec2(centre.x + c.x * radius, centre.y + c.y * radius));
        }
    }
}

void ImDrawList::PathArcTo(const ImVec2& centre, float radius, float amin, float amax, int num_segments)
{
    if (radius == 0.0f)
        _Path.push_back(centre);
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
        _Path.push_back(ImVec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
    }
}

static void PathBezierToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2+d3) * (d2+d3) < tess_tol * (dx*dx + dy*dy))
    {
        path->push_back(ImVec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1+x2)*0.5f,       y12 = (y1+y2)*0.5f;
        float x23 = (x2+x3)*0.5f,       y23 = (y2+y3)*0.5f;
        float x34 = (x3+x4)*0.5f,       y34 = (y3+y4)*0.5f;
        float x123 = (x12+x23)*0.5f,    y123 = (y12+y23)*0.5f;
        float x234 = (x23+x34)*0.5f,    y234 = (y23+y34)*0.5f;
        float x1234 = (x123+x234)*0.5f, y1234 = (y123+y234)*0.5f;

        PathBezierToCasteljau(path, x1,y1,        x12,y12,    x123,y123,  x1234,y1234, tess_tol, level+1);
        PathBezierToCasteljau(path, x1234,y1234,  x234,y234,  x34,y34,    x4,y4,       tess_tol, level+1);
    }
}

void ImDrawList::PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        // Auto-tessellated
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, GImGui->Style.CurveTessellationTol, 0);
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
        {
            float t = t_step * i_step;
            float u = 1.0f - t;
            float w1 = u*u*u;
            float w2 = 3*u*u*t;
            float w3 = 3*u*t*t;
            float w4 = t*t*t;
            _Path.push_back(ImVec2(w1*p1.x + w2*p2.x + w3*p3.x + w4*p4.x, w1*p1.y + w2*p2.y + w3*p3.y + w4*p4.y));
        }
    }
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, int rounding_corners)
{
    float r = rounding;
    r = ImMin(r, fabsf(b.x-a.x) * ( ((rounding_corners&(1|2))==(1|2)) || ((rounding_corners&(4|8))==(4|8)) ? 0.5f : 1.0f ) - 1.0f);
    r = ImMin(r, fabsf(b.y-a.y) * ( ((rounding_corners&(1|8))==(1|8)) || ((rounding_corners&(2|4))==(2|4)) ? 0.5f : 1.0f ) - 1.0f);

    if (r <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x,a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x,b.y));
    }
    else
    {
        const float r0 = (rounding_corners & 1) ? r : 0.0f;
        const float r1 = (rounding_corners & 2) ? r : 0.0f;
        const float r2 = (rounding_corners & 4) ? r : 0.0f;
        const float r3 = (rounding_corners & 8) ? r : 0.0f;
        PathArcToFast(ImVec2(a.x+r0,a.y+r0), r0, 6, 9);
        PathArcToFast(ImVec2(b.x-r1,a.y+r1), r1, 9, 12);
        PathArcToFast(ImVec2(b.x-r2,b.y-r2), r2, 0, 3);
        PathArcToFast(ImVec2(a.x+r3,b.y-r3), r3, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathLineTo(a + ImVec2(0.5f,0.5f));
    PathLineTo(b + ImVec2(0.5f,0.5f));
    PathStroke(col, false, thickness);
}

// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
void ImDrawList::AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners_flags, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    PathRect(a + ImVec2(0.5f,0.5f), b - ImVec2(0.5f,0.5f), rounding, rounding_corners_flags);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners_flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(a, b, rounding, rounding_corners_flags);
        PathFill(col);
    }
    else
    {
        PrimReserve(6, 4);
        PrimRect(a, b, col);
    }
}

void ImDrawList::AddRectFilledMultiColor(const ImVec2& a, const ImVec2& c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(ImVec2(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(ImVec2(a.x, c.y), uv, col_bot_left);
}

void ImDrawList::AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathLineTo(d);
    PathFill(col);
}

void ImDrawList::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFill(col);
}

void ImDrawList::AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius-0.5f, 0.0f, a_max, num_segments);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments);
    PathFill(col);
}

void ImDrawList::AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    PathLineTo(pos0);
    PathBezierCurveTo(cp0, cp1, pos1, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect, bool scaled)
{
	if ((col & IM_COL32_A_MASK) == 0)
		return;

	if (text_end == NULL)
		text_end = text_begin + strlen(text_begin);
	if (text_begin == text_end)
		return;

	// Note: This is one of the few instance of breaking the encapsulation of ImDrawList, as we pull this from ImGui state, but it is just SO useful.
	// Might just move Font/FontSize to ImDrawList?
	if (font == NULL)
		font = GImGui->Font;
	if (font_size == 0.0f)
		font_size = GImGui->FontSize;

	IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

	ImVec4 clip_rect = _ClipRectStack.back();
	if (cpu_fine_clip_rect)
	{
		clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
		clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
		clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
		clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
	}
	font->RenderText(this, font_size, pos, col, clip_rect, text_begin, text_end, wrap_width, cpu_fine_clip_rect != NULL, scaled);
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
	AddText(GImGui->Font, GImGui->FontSize, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv0, const ImVec2& uv1, ImU32 col)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    // FIXME-OPT: This is wasting draw calls.
    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(a, b, uv0, uv1, col);

    if (push_texture_id)
        PopTextureID();
}

//-----------------------------------------------------------------------------
// ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than ImGui expects, or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2& scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * scale.x, cmd->ClipRect.y * scale.y, cmd->ClipRect.z * scale.x, cmd->ClipRect.w * scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// ImFontAtlas
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
	FontData = NULL;
	FontDataSize = 0;
	FontDataOwnedByAtlas = true;
	FontNo = 0;
	SizePixels = 0.0f;
	OversampleH = 0;
	OversampleV = 0;
	PixelSnapH = false;
	GlyphExtraSpacing = ImVec2(0.0f, 0.0f);
	GlyphRanges = NULL;
	MergeMode = false;
	MergeGlyphCenterV = false;
	DstFont = NULL;
	memset(Name, 0, sizeof(Name));
}

ImFontAtlas::ImFontAtlas()
{
	TexID = NULL;
	TexPixelsAlpha8 = NULL;
	TexPixelsRGBA32 = NULL;
	TexWidth = TexHeight = TexDesiredWidth = 0;
	TexUvWhitePixel = ImVec2(0, 0);
}

ImFontAtlas::~ImFontAtlas()
{
	Clear();
}

void    ImFontAtlas::ClearInputData()
{
	for (int i = 0; i < ConfigData.Size; i++)
		if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
		{
			ImGui::MemFree(ConfigData[i].FontData);
			ConfigData[i].FontData = NULL;
		}

	// When clearing this we lose access to the font name and other information used to build the font.
	for (int i = 0; i < Fonts.Size; i++)
		if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
		{
			Fonts[i]->ConfigData = NULL;
			Fonts[i]->ConfigDataCount = 0;
		}
	ConfigData.clear();
}

void    ImFontAtlas::ClearTexData()
{
	if (TexPixelsAlpha8)
		ImGui::MemFree(TexPixelsAlpha8);
	if (TexPixelsRGBA32)
		ImGui::MemFree(TexPixelsRGBA32);
	TexPixelsAlpha8 = NULL;
	TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
	for (int i = 0; i < Fonts.Size; i++)
	{
		Fonts[i]->~ImFont();
		ImGui::MemFree(Fonts[i]);
	}
	Fonts.clear();
}

void    ImFontAtlas::Clear()
{
	ClearInputData();
	ClearTexData();
	ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
	// Build atlas on demand
	if (TexPixelsAlpha8 == NULL)
	{
		if (ConfigData.empty())
			AddFontDefault();
		Build();
	}

	*out_pixels = TexPixelsAlpha8;
	if (out_width) *out_width = TexWidth;
	if (out_height) *out_height = TexHeight;
	if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
	// Convert to RGBA32 format on demand
	// Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
	if (!TexPixelsRGBA32)
	{
		unsigned char* pixels;
		GetTexDataAsAlpha8(&pixels, NULL, NULL);
		TexPixelsRGBA32 = (unsigned int*)ImGui::MemAlloc((size_t)(TexWidth * TexHeight * 4));
		const unsigned char* src = pixels;
		unsigned int* dst = TexPixelsRGBA32;
		for (int n = TexWidth * TexHeight; n > 0; n--)
			*dst++ = ((unsigned int)(*src++) << 24) | 0x00FFFFFF;
	}

	*out_pixels = (unsigned char*)TexPixelsRGBA32;
	if (out_width) *out_width = TexWidth;
	if (out_height) *out_height = TexHeight;
	if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
	IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
	IM_ASSERT(font_cfg->SizePixels > 0.0f);

	// Create new font
	if (!font_cfg->MergeMode)
	{
		ImFont* font = (ImFont*)ImGui::MemAlloc(sizeof(ImFont));
		IM_PLACEMENT_NEW(font) ImFont();
		Fonts.push_back(font);
	}

	ConfigData.push_back(*font_cfg);
	ImFontConfig& new_font_cfg = ConfigData.back();
	new_font_cfg.DstFont = Fonts.back();
	if (!new_font_cfg.FontDataOwnedByAtlas)
	{
		new_font_cfg.FontData = ImGui::MemAlloc(new_font_cfg.FontDataSize);
		new_font_cfg.FontDataOwnedByAtlas = true;
		memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
	}

	// Invalidate texture
	ClearTexData();
	return Fonts.back();
}

// Default font TTF is compressed with stb_compress then base85 encoded (see extra_fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(unsigned char *input);
static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length);
static const char*  GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c) { return c >= '\\' ? c - 36 : c - 35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
	while (*src)
	{
		unsigned int tmp = Decode85Byte(src[0]) + 85 * (Decode85Byte(src[1]) + 85 * (Decode85Byte(src[2]) + 85 * (Decode85Byte(src[3]) + 85 * Decode85Byte(src[4]))));
		dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianess.
		src += 5;
		dst += 4;
	}
}

static const char MenuFOnt_compressed_data_base85[31340 + 1] =
"7])#######xAO$0'/###I),##c'ChLYqH##7HJf=_gub2fG:;$1g>11`]n42;pxb'Y->>#D'g<6l%lj;Yu%rHo'6PJqPUV$6PV=B]?c%M=@$)*WqO41=1XGHH6;G#/+8>,5V+41$Jk7D"
"(#:'HInV]+0Vus/m`(*HRG.%#eIwA-5c5&5+-0%J_Xf.$E)8>,h1Rc;C?K0FKH0A?TXKs0>jFJ(E-d<B:865B]^mA#xGU[-TT$=(I5lq$(lkA#wO4U%I3n0Fd=we-;%)##-;###8YaUC"
"ZS`K:Ea&##9x%##JuthF_Mv,stS###mJJwK<8W=BQ8k1pE<oY-o^$F7O4^=BG>a=>?_*##/Q(##[NIiFuoCaf(f&##<Tr92%TBfGVQq),7Y+##ujt92K<A>#1bZ_#+>Ew;Pfss'o?)&+"
"<#RfLbM'IA89FF%iprSol4mo%qJV]X0@_#$U1FcMrwSfL)501vJBBY$nF4v--<fNM=b@fM3A([Moc;R#`:F&#7;OGMZL$##/5YY#AmL7#=duH2dEmV#dBP##9T_n#X$###uk_&4r:%)3"
"*l6<$41@<$1(eS%W3[0#=ae--)K8L;MNv)#OesY-Bgx9v*K#lLZKs*M-g3vL2_Zb#G(X>-7R2j1Hd*Dux't.LokAJLnZWM0V2)g(0X2/:,$:loU/2,):#6k(C>mu>RN]lS.F0:)(1/F%"
"ql<R*w2>8%fB/F%+[Gk4%i0^#QM#<-@sG<-1N#<->6T;-=WjfL&rJfL28RA-KbOkL[xSfL;FUP83t]`XN#sEIAh1^#w=RA-=xS,M.1omM=n=onims%cVejfMwIF]upYdfrtUX-ZK]w-$"
"i&jwBWXG^#?F6IMh'd.#gdn#MACEY.qTot##3RA-FPP8.o?.@#%i0^#RK&n.c`#r#TJwA-1r.>-w-T,M2[L`NN&5R*1JR;7aJ5d3%MM_&r^4oLlI6.N#hb`<>Ax92,xkA#N(/>-nF*b."
"sGI[#8<Mt-SWRFM^#$/C)V@cVs3nsIj-:)NN5;lof9*vms0,,Mi0B-#JHmq7#vFR*SaHG)-jdc)iN.F.00k-$Q/Tw9^b)M^g^d8/OkE_&H&s&#IpKB#8Pq2(lh]fL.cHI1PD(7#)ka1#"
"=<&=#R<Mt-NHBsL99#&#i&o/NK2N'8&IPF%%DOGM.Nl]#ffO)N5n=ont:4F%B==onu)HdFDJuD%L[T/D24uo70duF%rC@v76.el+D7vS&)96GsHn<<)Ghv[tfTUS]bm)^#kx'F@vd`E["
"7OC^#`#iaM=Trt-+Og+MXS;R#8=w0#pc-,2LNOkMb<EMMCbYHM>A^;--o1ENE8xiLheW#.:Xs?Q@hs'6r>nA#t3'oLu8(@-_IfZQUF?hLFVi.MGSrhL(LC.&9Y:v#:4@W$.V:v#/PuY#"
"5x$W$_WNh#Sl:Z#/:_>$*.$Z$vR0^#HmbZ-l(w<(3_dv$=4Is$Z>t9)RVMq2%ug#$61eS%M<D_&U+Is$(_d--h[J2'67IW$4cU;$1`L;$=CIW$H?@Z$dO4gLd/K;-.DNfLZ-/W$W9n0#"
"CV;UD9XCC&'>ofLSjnFN;S#<-'YGs-_1ffLsCQN'+Ggv$:%Vv#u_^>$8gEZ$eeB)N3^uk-,8A'f4;+^#$3&W%Rjw<(*OV8&&uT#$j%nw'X7Rs$(/8'fW=*p%wx8;-0:V9.?=Rs$6GLQ'"
"9Ins$1#jE-1EuK.?Les$i9#OFYR+?.<C>HMxwo$McMK;-IaOZ-.*RP+B%Cv$XIpQa7=1hLYC]s$f&g*%VcCv#64e8%`a<1#SlqV$`C+gLC&`1.b7ffL,h:dMIFD=-<+TgLFu19.=@e8%"
"Xgr#$9C3p%xx8;-;I5-MU/RI/EOa5&d=ffL+<#gL^kkp%Wuh3=A5t;%Ehp-6-itGM&0gfLnZ4DPUBSW$<eZuPhqVv#6bZ;%%[qQak'+=$;L*9%v]E#$EcfQa:t4Rah[Y9VZO<T%O/1[-"
"saoQa][Np%@4Oh#o:hT&?7.<$<L<T%E5h0,?[d--Uu_v#bHn0#cBrB8/lXgLhk)Z#BI7s$>WYO-9jUL.B:%<$rxOs%lh6w^,`X,M*3@aNKUIJO?m1HM-xne/6PG##CsH-)>8p8&]?n0#"
"Wx$W$84.<$/^2I$Vr$W$>_ap%W^nI-EurqLrgt5&tC<=(0rbgLVvo$Mmd4T%@UET%6x_v#@=pgLAF5-Mt-Nv#DO7s$EF@<$w@Yw$?qAQ&@4@s$Dq&6&%0*L>2_<tqsp:nWEe,J$.`F,M"
"PEAY-rX]q).wqQarQkX$e_OgLiNx^.;FRW$24IM05+7W$F*96&k_FgLdM8<$H*BQ&mbFgL`vbp%N6'q%<1@W$mNO0P^Uns$02VNt2oFgLw?,gL`/RN'ER[W$E-O=$MEgm&74Rs$=ONp%"
"G(/R3ff9T&mhb,M'XPgL(n(-M32/<$%@V8&ZLK+%0iO,M>er=-DnGhL-_uGMp5(6&12###Oa'Y$Ansp%#ss>$hd-[B[_s5&.Q,PM^Ea;$@[Wp%K$tp%Gq&6&gL=gLA#:6&8=;X-pAg*%"
"X:IW$AO5hLkl:1.o6qdMiSOp%G3.$$LtwS%w'lgLGO=T%0(r5'nU=,MP0_Q&1Np4MA':6&n[=gLA2P[MQ#TGM`utT%M>#9&gXOgLanCdM-tKb*3v+T&3QgoLo8AW$qUdL-utie.FL@<$"
"7a:1.rwXgL/ekp%1XvV%MjsS&HZss%TgPn&%kfQaN(QT&I'96&EWkV.E-pM''5pr-lwB)N'U8s$WEu/(D@.W$>@IW$PjuN'=;gs%PQ5n&laPh#&Un0#*j<hY<5#T&Q2AC&f-'6&g=o7j"
"3J8U)u0KQ&22###V/_u$F9gQ&G:.W$5VX?/Bh3t$v6CHMZH&W$EwAQ&R?K6&O?pm&Bt@Y--Ccn*G$*I-tapQ&DLns$=gXq&M9^Q&E_eW$HBP/(&#9;-0%vW-7*e--j:[8%[TCK(HF.W$"
"AIRW$V2`0(m[qZ^mgY3'u.Y8&>L7m'%_@nW92je$q4CT&RBKQ&ZJV6j@CKI$awAm&0kr8&nU+gLLVj[,D0#3'QwnW$?[Wp%>[a5&PH5n&.MPS.>U39%Eb2u0SKpQ&MqwW$Vv:0(MJ1R'"
"V^G3'a/d3'FRws$1wR5'e$3q&HhnW$M^:g(LR$W%63]5'`+=,3UoFZ,R)24'GLEp%EbNp%+[ChL=H?3'%=ugL-$VdMx<Eq-aBn0#Q%XI$J.%N(9l/gLA-RAO8>qQ&M6I*.v3(-MR.,Y-"
"HHcj'W<Ft$Cn&6&Bn/Q&Vg(O':Raf1AeWT%Z5Mk'[pc3'&C(hL*&&h(*U_HM+otE*X<k9%9s9[0#e>n&VWGN'M9>N'#LugL$RGgLj:Fm'i8Dk'Y:LW-ZS.l'L_j5&Jws5&hSVO'PjlN'"
"`#Qn&vJ,F%#]?m'7U/N-0V/GMOjsfLTdvN')4ugL4er=-9@cDNw7In&T#vN']#)O'*####u^+9%co<e)G;*+0GkWp%NH9U%5aDE-A6u%4Dw&q%bY@L(b/2O'W?Xt$d`nH),Jw<(7?`0("
"ru.1(Q'tT%eGi0(a;i0(UHt9%XDwD*@kC_&iY20(qr.1(Lba5&wfA'+Vww8%M'X9%kLTF*UZlN'h`IL(oewK><b*N(YokKPvXU3XwUQ2(8LCC&4=tH?e25O+H_ge$2oO,Mn+&L((bfQa"
"So8t%XG.1(Zt<T%HtN9%T/7h(a]Ih(+OD<%LEpm&3kLhL&2Ek'[Np2'o+F.)w&Oh#<d[h(xlRh(TsCK(kY7L(_v>R&U'Xp%ei*M(,sr20Z,MK(v7oh(Pn&Q&&#gB+Z'x8%Q9'U%qh,c*"
"Xj:k'n(4.)gV%1(vsw0#%NMk'x$Z;%+QsH?Qr<I$-nw0#Cb:-M9er=-BeudMk(ak'CL_`.j`IL(MX$u3-0YT%p[p'+Pnjp%N3BQ&Vs57&8aDE-:1ei0JBpQ&oI'f)mcIL(SOv=%pFBb*"
"t7be)puw-)*`O.)YHg6&H<Kk0mr*.)_mTU%bxA^+-l`8'joEI))Abe)mgXK1lX0/)Y6#3'VT,3'(it.)36YK.roe-)0O/g)t#9tq'9;nW_T+F72NgJ)@J.a+A(gGMmm(eMa/:U%7Q0N("
"-1PgLYW4j1WG*e)Aa>RawJwH)j:be)%-Oh#f?>N'*$pQaBkUhL$V_U%rUg'+S7a-*r+=I)-lkI)]Qp6&gb*L)iM;4'_HBQ&oCBJ)-$+6'm(ke)xKI;7optg1nhTJ)[?>N'X^GN',%CJ)"
"ao3I)u+]L(r%x-)F%@j(,#9;-##9;--Xx(Prex1(PAl3+BIl7*;>7@'+8XI)nUcu.$OGN'..-,*MH53'Q39q%n%OE*=<7.MkDPT%dc<e)*VkI)xIBF*lxAb*,.qF*NpdY540X:.vSvR&"
"nFpb*#8+i(u1X**b;nd)9HihL1UY***$=u-#m`4'u-Gu%6q`@,SvCk'flsE*+aQ7&6^Ma-&G)R<1cpb*;MnA,xmn0#$On0#O*)FIte3[9;X7x,5hS,*Jer=-En:*NdAUU%me]/*qSmR&"
"MIDM02t:0(:hM)+Q^YN'VHg6&xOTB+.9+1#ogCK(#Ci@,RH#L1.C)D+qhTf)=R)D+Aiw<(gsk^,xr[H)'O2D+-%6(+)xY(+[5d7&8*o0#(7_'+7j#8//Mx1(*nU;&C^=Y-W>7L(pC,C+"
"623S&,=vC+54QC+3C2`+<]W>-l]U,*%#9;-i'uoMi9^?.8UHg)H)l3+52R[':g>g)')d'&<Cm(+%vwh%t`FT'x?))+$'63']v57&pk1%,.h`%,?$&&,h,.-)+nex,<bV%,9P@['::QG*"
"U[rX-UT*I-pY6[-);F**.t%&,6I)`+1FM`+aM;S&GBwf6'%H_+QaA&,im1K(ZEcO0x&:6&n]V4'H];T/.rqZ^LHx],djxjk9<x0#EU?c*&IrS&.moE@<QF1#t%X8/:?1a+Oer=-FeudM"
"'X?G*FL_`.;h``+V'<75due3'D8HS/ipuN'fA`K(uX=2(GKx],xIVS%tMwd)3Etu-F3J],?<o],+k`D+PHf],H3/&,_4<=.3c9b*b@oN1A$8],<$f],ilio&D9o0#1'a[,L4+j1A71j("
"=vEt&YoZS/f.te)'+`[,JxA5'=6f],I-8],FE=>-ER(T/0-o0#'Xn0#wT_-6@Y<#-NwhP0-RZ-?c:V4+?F;p&Gi@a+Hq(hLImLE4La*e)YG_?-bV[-)g224'9h7X-O#(Z-?o]5'pI'+*"
"V;:Z-D-s%,Hl%*+YVQ<.?TOh#i#Cv-j5:v-'@Vw,RjX>->X-g),miK(UV^0MqXJ=-Ub9g2J[h/)DJ9:'fX8m0-e=-*/UnX-R7T5'PgmZ-SZFY-OsK;.Hn$Q043o0#([n0#?pvY-`JV6j"
"K-uH?D'4O+?@KI$9J&5'^+FZ->4P2(-pQ`5WvN**dxdw-fi*I)k;;4'A6+:.ZS6W.'E?T.k=tI)Tovs.Rd=^,@45Q'W+821`cm8/X5U;.vRR<.1DFM(XEg01Q&Lv-7+,Q'AA.e2Z)NV6"
"><+u-tFI<.(g***+,p(4;,wN'+V42(hWHh29(%[^`,5B,W,1v-C8pr-f#]iL)qugLBc'KMFC^;-21B$.pCUkL-RgoLUeudM<&#B,DAh;.W>hv-*vwh%W5QQ)IoDt.LGJh(xFx1(=,D50"
"YF&60teS603]Y$,VXkj1p@s50Or@['ccHw-)fJ60q&+I-J#k22T<A=-^e]60hx;p/ax%q/*113(]Wo0#APQo/vltZ6i#9e*b<89(5^<*4&Or%,Epu40uW`N(dFA60s:a50oUom0Qm+d3"
"GR/N-8DNfLOAHHNd0B$..1IlLB8&q/DmO2(@FTI$N-2hL[sdW.cWVhL'[/LEEiEJ2q#/O(1n[x,-kw9.kL=R01]CkLsdvO(76]x,5*ik1tOWT/^H]t-8KEM24wqk1.bq02Q'i02ID)l0"
"/Fc31icuB,O=Lb*vgm51IRKC,q'_02N-.L2&d,GEwusm1_;Gh3F*/x,N>+Q1[qXN13CB:/*.,n0N8pr-0#YjL,-;hLwGvD-[SNfL:%c31f5q*%EvkM(?@KI$]*MN(-O1O1k8'I*?3Uh#"
"$k/.$v5_'#)(MT.?ww%#-4ip.cY#s$)$CX(F]h@krTBX(4`4R*^qH##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#jdV[%66TY,cL5;-gelr-k'MS.o?.5/"
"sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4A1OY5EI0;6Ibgr6M$HS7Q<)58UT`l8Ym@M9^/x.:bGXf:f`9G;jxp(<n:Q`<rR2A=vkix=$.JY>(F+;?,_br?0wBS@49$5A"
"Z(`s6[%Y6#(;w0#=.92#O_,3#Re53#Wk>3#$(Z3#`-d3#d?)4#hE24#lWM4#qji4#spr4#xv%5#(3A5#Y2.#MTXV/#2gNrL5:SqL;%r2#c5C/#]G_/#H/;t-bwiuL'J,s->V5pM6:`5/"
"k,85#BMlLO7c><-?F(a3srH0#k$S-#OH4.#I6o-#D*]-#F(xU.)If5#Sr-^OSr/-#H-.b3%A*1#%G31#-SE1#5`W1#6rs1#wqP11A@T2#H:K2#8uI-#Pf(eMppr/#/Fc>#&raN0VPj)#"
"0Sl##D_psLQW)xLG#pxZ4Gg1^K<Txbdr*Pf,q7`j;#h:mNI=ipmGJxtwxFuuTVU]=T6FSIwTe%k<b`D+Ns9v-6l:>Zg:Q:v@2W?-;7AcVrZYxXf3'C-o2'C-(W>[.gh<9#=3'C-HW>[."
"Q)a<#dQ^$.[j?QM:=_R-96f=-QR<_6(lxOfhl%Se],HucSHk@b4[)##B:;f_`L1cihkI_&]P^1^i0bw'#xFfL.,ca=_2lIUZu>r>Uec'#gVc769&E(;m[u.:Gq,/$i9Px'ZL8`&gD4;-"
"t+9;-=Gp;-WOpgLao>f_^GXO_I(h%F@RLo[B88JC>2SfC@jZlA)=,F#K?U'#^^%/1*l%v#)d7Q#pj68_ctv<_>)-M_)Guu#r%O3_qPp?-6*rw8cX_pL)w8/1VBrP0aMjv5d7dP9B1:Q9"
"J&0Z?#gKDO&`vI-Sf&=BbI<p.I#Pn*@f/f?#Ug5#f?_qLPW&D?94;^?I7;9.vV%d5g5a>$M%l63*WQDOaV3B--/ImL7IMX-LwIb.)6T/1Bqc2$CZ8/=FNF$9tvTs8/h'Z-O8&sa=s&sa"
"RbqY?DX4?-3RQxL34kpD@hAN-,e]j-Jve--l#4@9]7:_8d3&Z?evMk=qnTA.@ISUO+O3uLTOD6Md>vQ#8Uv<-sUr9.Z9H5/e;&$$GSwQ#]<a-6GFp;.=38#ME5:Z-+:aBSeccmL=X#01"
"`LG;92blZ-TV$##]T`i0P6Km/b=Hf_'5>>##S,<-Dv8K1^ke%#F(V$#`-Sj0&B;R*PLk=(]t`Y#Gj=v-e*ofLx5Kj07Td>-J_Mi/7-CG);,8VZ$o9'#vTa%=FW/,;rlMcs6I:^#(/Le$"
"lwC`s=[Rm'>c>$MK4$##^X&v.DTY6#hR#C8+L;s%._Vs%9Fl+ME_wajvdHB#pv52')ZX$0]pdh26$nO(#mqB#cc``3Qh%i),5CQBI+1?/tcEigvaRl/`pYT/7Jx&#3-sW#?aO_.EXI%#"
"rbEB-nZgC-,5T;-1C*j0s8eh2TFq8.Je75/n76m'CE/=%vu_2'26w`*m(wo)2'>`&[R]i'i[V^kipP@NGojf/C8SY,3YfiBZx4$$[e3]b))niLE9xiL>2oiLCaYgLCaYgL647ajaxJ_&"
"OCt9)DGD_&uQj-$DGD_&3Ds9)#w+F%)3,F%-?,F%5dPF%5W,F%4/[w'SN3.-9d,F%f[N)+_+MP-*[gC-65T;-&G5s-DV>$MlSH##9J>$MX*^fLZ5Z6#su6A-KDC@-&M#<-.Y#<-<xS,M"
"@qugL$(^fLB'2hL9QrhL8W[*N@2O+NSU?ofd5DD3N=`8.kwdLMS[-lL.PP8.)w@+4AVVh):JeC#&d%F#^)?\?:Ojq0#k_P9T%l0B#dR+DDccbPT=N1+%]5>##,[Ms#N*x%#rpB'#AR/u_"
"0u68%p^`;%]4G,M:W%iLDA,gLH2O+N@&]iL:[`=-T4m<-:*wT-aK-T-v^d5.,AXGMK5#&#CfYs-/lBHMu_:)N/rtjL,XPgLppIiL9QrhLHVOjLA,fiL?vRiLI]XjL.ecgLMu'kLd)@K)"
"]AqB#:31qie]d8/YekD#U%V%6c,8J0PXd,*aej=.*?k-$vP(m&5AP,4kj874TZ9w#425,4RRv54W_nl/]n>%Y+>a;/-7H[]nDxk(Z4480/@dw]*^+_5$G4/iA%KfLg%pwu_M<O8RkP<."
"0);D3Eho6#g)V1.;^/KMTlw*Nv6$##:.m<-pWkV.Wr+@5a$EU/BhFJ()c?f_ao/^#]q0Z--P?F%`-GJ(JL,W.rl,F%Ub`Y#+:iJ2GHo;-8O6p-+quv./[_F*E3lD#P4vr-3mDF3<^7T*"
"89(B#Hc-C<PnJ:TdltN+T6rF*.P@'4^RXG)xf+G#,T/G#rmBP9chNE,>xnx'e#V*M3=$##odHiLHikjLr5T%#trB;_,#`3_v`k-$9?a>n0w(hLJWBcj88)b[PFt9);Vg'`P=Mm'Z1j^o"
"c,mi'?pK/)=-<s%<8M)+Kx]7.he9HMGgA%#&fYs-spvIM@29S8:<d;%6YN#.s@XGMdXPgLF+$ofr)/IM5?l_-N.OF3d&+Z6R9#;/TiC%6rS6&4/sId)01V00LF;R8=I73OFHu`#=O<5;"
"$H?_#nskU.0+DRP(WKL(KQ-Sa5xk$/gU?>#2K?ipHm7wgxSW]+u;$IM;7Z6#I1C%/$=e._1ukZgM2KkLE@Bj01e'E#]3&C%GiUC,keS/Lnq5g)vL4I)-(ZG4-jO%t/BJe*+Onx6Dk6)4"
"0uGp1Z>3cr#,IJ(<Lo6#b.xfL&+Zj)lm+C&?_n-$O9o-$`jo-$pDp-$*vp-$:Pq-$J+r-$Z[r-$k6s-$FjZ#8^4+gL9KihLDVOjLkMB>GIhp%4inF59w[0^#+3-L-m1-L-LXJe%DDn$$"
"OM>c4%hhc)oYRD*pAOi(Y*xYQ4pig(NVeb6aB7G6?d:+RuTk,FCsN*#0:BB,X2nFr/p>wT=R@$Mmo(-M;TQ##Y<dD-''HpL-B]qLGKCsLRV*uLcbgvLsmMxL-#5$M=.r%MM9X'M^D?)M"
"nO&+Mtt2M-,S,<-75T;-K_Fc/F)Y/(w^2bjj'L&#rp%W%:FS1)n`WI)<<Nv6gG7l1aC^A(7eHaN-9cS7^QVoW7ojOoDLIVHA3uQWaOo6#qe2i-/9:I?CbYB$)5kmf3(V5'8+6@$cU)##"
"4O/2':DEl_c]C(&0gC(&3pC(&6#D(&7v1(&'&fh2jH3Q/Bbp=%5Qaa4c`0i)D%RP/cLhBHSq<(?uCvo;d:1a48.m6D6_3)5HN0iFnQ&##B-4&#ZpZg#Hf1$#qh`Y#X-ZjrAtd29$I;s%"
"od%W%h--^#q)xS&@C8I-KAU0*,_YgLxYwlfQ'svRNS0i)0YxAAwun5VQx:oA;^rvA08F#vDUw8#lQc2V0-bV$iVTI*GHp>-X'Lh$_BF#7>Y-lLl'7./Z9UV6adW<_ArYi9HXK-QqF3]-"
"BwnXl<?]c;[9]jLkLS8#oJ6p-(l.C&NMuu#v>:@-qJ&R.<#gb4N[@<-H*9k$e:A<]SR?>#l2'PS6B.p7:B72_xN8j-/F_E[OaCD3RjT`EA,puYni2X:g%6uudh>`sImcH*eiZ6##^Z6#"
"8>pV-nMQn3;;Vd*R>MJ(QXjfL=..ajC`J&#`g]29FiYL26K8+4?xrB#*)4I)AbSR/Lo@['QLw[-f$VcDt/0#0#wB1N&ma4OlAbL#30?;-`bjUmnuLi#t0nYOg#b>eK=?>#]a&_/ZX3i_"
"6C35&evD2_$r2eHx^HF:gH;s%0J.<-mDsI35Hrc)I1'C4[Y]e,>2YtZw@xIaX%r_,ps'j'll<j.B-4&#@YR2.i,-.OK/;hL6tJfLR:`P-%M#<-=5`P-6aDE-:X`=-%M#<-HePhL<mneM"
"O$TfLV*>$MuKAZ$_6(,)s$qdmFotM(qJ))3dG`=.xNL,3-7UhLGDd;%&YjDu#Fpc0WeOQ#DC2lb#k;Xa)Euq2TBYkQx+S$6]MtJ-iN;i/I=cn_GkRS%Z%(W%pNAj9d=ti_,R5R*2,Sh("
"CFl+MK/x.M(QQ##2]Hs-$:$IM9/E$#.S#<-<M#<-`AxjLpR7IMQZ>5Ob'V:%Pn]M'FtcN#9>;^uM;co/J#a1D_Y[`?V5$Sa8%eZZI/WHaK/jt'/p=J:fmVG<qWZ^#tRBP86iME-nbmL-"
"v[mL-i:O$%-kGq96Reaj8s9W-*iW#RZ5Z6#IBcw$'-,F%3GXmffG%&4..Y)4XaSX-J#G:.I2fI#JlhF`Vt5q7K_:tgE&0B^hkqnu1u>&v:FUP8^;L0)+i[6#u]Z6#4>pV-;0K_&vIqP'"
"IqrP'0I?v$.Vq;-(V&0%:A[w')q7Zn[Dm6#?WF?eJgAk9cMF&#:ZR#$#X*+.`xko7$P_s-60fX-(T^wMwdtI#qDM^uPYt]u]28?N_Qg_11p*Mg5Peo7FsbP9h#bT&6wr+;'sZiLc-fiL"
";vRiL]xVf:<Rsp'(@,gLe%`i:U-?v$@Pr)+Y2l6#AFoV%e+vu#$J^]+SXev7Sa:x$aJ))33r`BSLeUN(LLI*RB)pA,7s&i)a#;wHqG,?0;*N7n36+,Mv8wR[o<NEE3WnK)_U43NteDnQ"
"t2ioK36Td=#)doLDZZ##U)83%NPuu#NNou%jhYV-(Qvu>G:xR#K]%l#(>CJuRBxIaXaV#$O#>0lB/uQj9UAJ1[N^6##AFR-M5T;-^Kck-0ql?pm//32P7LkLWikjL+:#gLg)wT-WL.U."
"rOs6#/:^?.6r+@5gV=X(N4=X(Fn*20tRP'?Y9W$#/W&7.+StGMOF?##8S#<-Pr8gLxAm6#V+29.V4GI)UE8m8N&.`&,=<:.V`%01rfUC,WVe)*H@56KGA,s-t&4kLZ;_B#iubLpaY7)?"
"ms5,)_Y[`?C>kn.aRb31S8vQ0O6^EFpY=e?&Q9_I4-:5%q$.kto;5=-`XkR-H/o3%6ZnD_m8_3_k1[w'iiM2iEv3+NOYwaNM+oVPfJdd38%&H*ku<WS:2Cv-;I9mhtIFA+36+,M(%w%="
"SP0_EEAOk%VZxs7+s(RWnDnS#`h1$#S/uT8X1FwT_ucER*>49#5N;781.@w%NpAO9hFkM(+C/O9m,UB#&.5BG9Qg:H,.C'#%/5##>lr4#^<C`.W9F&#1Tjo:`6`D+`&><-2QGP&I:9v-"
"%8Hs-)sZiLm/gfL/l[IMkM#**FdS9^QlBgM(gko&?,AW94M'_,n`v.LV+ZV$&*`?%?8H,2YOCF*WoAF*^>D7rWv'5J3*D8%)k5pMpH_=u_k1]t-l68%FD>f_%ug>$g:6L-<0]Y-KE8w@"
"O/pk9^w0^u&BXA#fkf.:3HuRE0n^3_l`xQE$DdP/eC4D#6K&SMf)(`.$NN,Qu2u_.LpT^#Z(6-.8je4;L1T;.w)iv-Np52'lB$j93LF&#&]F&#3'S5'7Jq;-+fG<-N=%q.*ia6#$.e^d"
"_Dm6#VBvp0]Hj$#rDN-ZnT.dbD]?<.(f8W(?m;>522J&4>v`e6IZeLgdr+1P(DWT/`pYT/Jm>6LTUZY#A($##4>]-Z0o=PA=L#lL)^-lLL1CkLc_.iLsv,i;AYlS/N@X>-X^*j1MOT;."
"LQd;.AJm-.B?<jL<Im6#Dw)JMfv]%#%V-N9B-4m',8k#$v1'&+Z2=W%MAhh;J'[#6bg,V/paO+4d5%&4QsGA#jx`=.7M]L(KB7g)GUFT%-fn8%9)MB#$*m7(r,0Ejwlm0_a4sK2,UfT/"
"xA&8.?Lmr?^e-8@KI0c#+][3'Bt82'S-1-)7'DP#HYi^uoso;:;[k]u5-W]=LU.<-rBSj/5(V$#362/(URrM(.*O2(PA:<-S]O4%?is9))_)'m$anlf5t@;$OjEW%pv52'XW%j_CKh@."
"cPA+4&>0N(q$=g3Q5Ki)u2VW#X/GpZk2.+OgE:Guotv_u<d6bQU]KA9JwbP9P[_;%k7s&,?O]6#;6ZD-<5T;-OloF-#L4,%Aos9)Rehv$Yd`Y#&+n;-B@Rm/jOs6#U_R%#l_9`$=@p&v"
"Wlb0&Vfuu#r;Hp/AhFJ(%Weh2]Eof$H<e*NhcrG)i^j39;^?C#2ZTdWhgTdW%n[hukDZB9Vh3ngE`6kCK:>k(v.@CP2LtZ.</RI/B-4&#PfR+Mk>W$#6L'=(9p=nEi=4kiCOC).hXbgL"
"bgdh2pT[J&P^Bv-0C+C#^Kj7O/Gk^uVF:g0OOY%kHpe'#?l1e$,Scp'iX2u7=IvV%B;l;-B&Y@&k*sQL^F:p$WxUV$f&7XA6QR*N1jS@#bAtJ/2EVS.YKMUMJ_)Y$@0/t#$i1$#iZ95&"
"2Ih#$gT95&->0Z$1L+];/DAFeT+RS%q2b80l06g)LT/i)(3lA#BJHC#KMCLfncB^#?8.bZmxJ.?vIHP/fYmQ/_nd._k>?k_@b(^#wZdi_[[=`?#*Q%.PJXfMpH]YMl8YCj=o-<-^?c-("
",s13r$anlffAO,2HGc;-Ct,1%9'(kbHwd--$$,F%`ts'McAn68MI;s%SSl;-q(e.%#2tM(_9.W6;h?g)/xWD#G`>;-Uh#JC*U6JU'hK31g]Hig0*[2V&M:%k*u'S-$HtV$(ZrE#-klgL"
"ltK]=^d7p&46?FR2As9)<m.S:+[c,M[P`]$SO)##t7vM9R%tY-iU)W-@q3L#F'#BZn$UfeK]'^uMSJtugS]s7P-C'#?YJ=#I(@h_.CVt-,XGcMqW5,20(X=.6NllZN%+,Nmk'AX1eMs%"
"an8NiZ5Z6#ff^g-lD6R366?k_>6rB8Js&kL;EsI3,CoHHPB=^,6:(8Ot7$j'vLk7NeFYcMlrt8&dBc<<##_>$fD];%:HiA#AX:2_A]@Q-tT^C-vAi=/0r+@5mgT)N_sdh2W,-$$-hcp3"
"N[fC#)k,5hu#P]_ZMCnukQLV?e=Hh-Bj7:DS[A(M]SQ##FGSb-+.Z9iq(@k=ss:RWF=TfLgwlA#Ddo]Z42=`s-a;R*kG4;-Rse+iaYKp&`47w7B$7q73$S5'DiBgL2gh'`4+78%F,Rh("
"*F1x$_a/fb)L>gL-_YgL^G6##ZRtGM9sq`N2'i)N_sdh2&efw#q*.#3ul)`#Ptv2W8PBQbN<?8.1I%.$;@-&lB.'Fe,m$qL:vv##1U95&59I2_C[kh8N0-?$[v@;$@oq;-g:6L-8;=J-"
"lO0%%I)###6&f3O/^1(&dmeD#Fl*F3kF*`#r:dIhl^MJ1c=Tiu6RJtu%Up2#uKE$9njBN(m+[6#u@jG9Oc'^#+09j(<uq;-<X`=-412X-wuA3tBB?k_4uUV$bmpj_sLY^?dp9-%)Y4wp"
"'gUC,];(B#2ZTdWMW?6OPgxj.$-)bZM3D`sLqF0#JjiEI-Ujh#<XJ=#Hke%#.D^]+AWC&,(&?<-YO:d-%c+x./..ajI'S2(9AJ&#6Nt;-h/37'Du&(Qk4+E*o6tG<Yo.E+8lWdm:./U."
")6DOD=g9`$T1WQ/B_R%#X2'&+hidD+2NgJ)cu/^#7&D)+sJ><-8(m<-r+XvLZ;_&=6S0j_MpCG)VtIs-gB=gLh2M-2MW%,8ogt&#2cuu#jHo>-dVmb9o(1B#0-;o0rdKj0^QT:%9#)q/"
"CfZGZo$a8.-_L)$t?Ap.wsN.1UYNN`0g,5&bdK]=%gHiLl<KS8SI@&,_E#*<#;Z;%&_L&#vsA<-Z4dd$Z-]V$A9Os-v9$IMWx]%#[KxD<(p-m2tp#i;v#HK)Qh%i)c#fb>r`@ab%o=PS"
"2-(R0Fp&dRJ:Q01b2JSaf(v-N$),##95YY#;WJ=#:WXP9jt-La]c+0&aqM^24uv9.XL3]-W9qFZ7tUZ#$9wo#`17@MoxsJ%E(Zi9J0$Z$@g130bCa6#k]Z6#@YwE@&0Gb%q7[n99/(kb"
"=7+T%Y>Cv-X$nO(*$WcD.NBhusAuc/toF5&@5Pi#r&uIu%RKfLrwSfLJZZY#*F/=#0n%S8*dbA#rc8Z$s%<p^2nM8.vi5A#[f0fTZp]&J$cVl&'%AM'hBVe-I>uu#n^W#.@uC.Nb_da$"
"Bk)29u=ti_$<f'/MW+<-&`5<-6m;a-h?(CA7ug$$i,'u;m^tAa>>1`AXZ4C.$f0]OfKEg2v)Z+MplHi$pU1xLkx2$#]l+G-tc&V->R_]O:YlGM0SGgLT+MT.Cj2,)JmVE-./4i%BR=]b"
"(wqOOakX(`I.-R#$><L#$<AX#oD_fu%GP##$,Guuq7YY#(9x#/gdpj_e3jT`%',F%)3,F%l^^dO):#gL?F5,2h<4gL2Kq-%(%w9.@YkZ/TO&RZXiH<LrGrT8$.'#5f[C(j49]w^JU?`s"
"?\?(,)f=Qe$H>uu#$&Bv$0C+<-Yc`B#ZxF)4&>YV-BslXc$ttXcpq)c`&b-I#u)2eu:%Tv.m0E8%^^S-HGp=$M8,WJ:RSbA#Snr;-BKD2%ODjc)JH/i)8+[xn's<+$;G=[0WkFJ(2U:Gj"
"r@)<->ro$2vL=F3^wOt?Jmr)$;t9PMg]TcNMNO/MHurV?&TDs%d)W#M4bAN-JBRe-jJ*F7^_Y)4S9=^,294#?w%J,Eq6;[9K),##/mYj#6'Wm.7(V$##>(-%Xt@;$-/<u.&Ff8%V+Sqg"
"5VUM#gpJEr#ENn3i5SY,kXkiB?QM&#3$qj0JF3]-B6]F`U0&)I[/sGj$sPiTb9O4rSbG)1faO&u1g&1$Lm9K<lR`s-$1JnL^B,&#)/b`*[W%T@H@Md*ahuu,5Dh;-6N:*)x*'u.`Zbr$"
"eJ))3B8C%6msAP)s^Mh)7^9Z-GI5GMNOQ`+]36iTT8N]uHjU.P40b(Nu5R+*s@F.)Ib]`XVmRVOp5ju$%Gu@t=-GJ(j7@3D+S:`j_a%DZ+QG,2,-239Y-?v$'fbA#AZxP'DQ*7%jnMmL"
"l>hh$fnat$Vf*F3Mnt[$$w(^6(PD^#4[WhuX6.xN#4&#1eE:nutLx@tIg`H1Lm#?I+g.Pf*8ii'N,m6#*####b7?f_W7lo$1HvE@/&w<(]pdh2jtw[-EaoA,uwWD#[3Q)4I1rl/-YZ)k"
"6h@M#Kxv]1p*puM;id_fR1(`f^%iK*)L>gL2*^fLQM.IMm7Z6#1=%q.cCa6#Zhp^f>K?k_/m$8@ZOV8&nGN#$%:CpM,cKu.vf%&4UQBv-FPWn0M')k#6wHlu%'L>#2kaf/sd)c[ZmN1#"
"BqQ2i;;rj_J#@D*`Z93_%P:kFBMZ&M,+n'=(B%a+oB`,M$V3L=5IHZ$#jic)$0#t-$)E/8(`HC#g+LS-8p&VQ>0L>#)O;MKbsBm/It]`XqSnFODQB%%T*%Dsgb#T/0MBs$;f-W-ZF`Kl"
"Y<XKlZk;qixi8gLC[#w-BT/i)6o0N((V'v5V:$L0OLX'[X+%`,]f;U0_ra.C8;3,38erxL/S6##im^^#$+AnLbkJ%#Zrq1&v^db:K#)d*Kd=<-h(N[>ZOF&#E,L/)--tM(mu6d;ciM<%"
":%vu#[:SC#lvo8.Y$x[-C-LT%bf)T/eC%#1w2uLT9R1AOK-2]Oj:.)kLh1JUeD)C#,qa:HHPvf)Zwm*M66_=#(vD5_o>_3_*De8^wqJfL#4cH;Gad;%oG#)%OmX$099?k_.PYY#vM]dt"
"Vr0i)[4pL(O5QX1?l5Sk]GfP##&_.C^/P5Pfqv?0o($##?XC`s=wfi'>c>$MRmlgL93Z6#*[A(H?LVX-^PGWAj:,1)8a)F3&,J#%XQ1m#C/102I9#G2$ccL^N1/REmdBB#[Cq[/*W2l_"
"BH(,)?@;p&k13REjm44kFDdG*UrY58NEHXhq^gH;DYkA#_3Hl$0P8,6f<e&/KZ4m,jTHd=h,#]#%c($#owH#)hu%3i61GD'aEl34tre+Mjvdh2,;2T/nVUHmo[fC#1U%RHv/$8i$)'fh"
"#L'Ds'e?/@)*0;e?xTeOpP`=%#j#<-HfHt.Qjjf:5m*9'V*E$#9fh;-X?Ad;txuQW?K?k_;.;;$Fl42_$lx^$#]e[-OGMT/0q3L#kjN78Xdk,2soq_,rJq`3YorW@[P'_#>d5H3g]O;-"
"8_A@#kP-YuOV/_0rID`aXW[g)XoC(,OAI^,V?kc2lr-h)eiZ6#w]Z6#55T;-%a^s$F0AkVBev.:lE;s%>i2K:Bc>;^+R1N(r?\?GMQ[lc24p6[/EioA#H<w&/$[8E#r?dCJq3n0#h8SY,"
"w<4_JsM#N3^ukv%n<PT%:BT@#;GDRB^.5875pu=P8WT`Nmlb#18JeX.5SNL#vao1##=+_J0v7'$]Ej$#:'@u%@_w;-t0*N&D4[WAd9W$#l]Z.%iFd;-)6iX%4jic)PawiLVSdS1F($,D"
":0eP#+JOD9Uk9^#v-C6sVnds%&.Zi9uj_-HZ,#<-XwhH-BFuG-4LE/1_Ca6#h]Z6#ATY6#3oAK%Ng$8@VNV8&p?'N0(_iI3X_Y)4fVFjL$&HA#iR6DO:U4SkPMU*Q_V:M#;Av7%sAmA="
"[6m,E&pll&#t.0W^i^._f/?k___3T.e^ot-+x>x%Y*j;-st&<&ec9[9A3o;-@@g?.Qqn%#6Z9<-GFJv$&2(L32'@K)iF3]-G4&Z67%mI7L(OF3GwOG2i8_J#sw)oNH1b=YJifOO;h9UD"
"UK'RK*r.Q/s<$DsWUHP/m5:Q/JA(0([Adlfj(<9KWpQl$2TNI3E^a0C(eVL)xmEXZtAC4O/7Ih#o56GfA=si-RTwVoBT$+#]oq:89^O&#65_^=C,oP'g98W-Rqh9D;9_3_j`9pp90@K)"
"x&9Z-MR^fLA.;hL_JhkLUj0^#iR#H3b5]Y,>2YtZ4#_?ZxM/u.P[p,3XWUnu]a%C#e>9K/,5YY#+I7O;Fp(GFC*mf^lP2,;$Q4,;$j^`<5kFJ(Zb#&O4t*($?x)Y$_^TP&fqK&#w_B#$"
"x&6a<aT]>e7_7:.tf_;.P:]j0e<VfSFXCpa<IJr^w['B#P/6&M4fUU##Os-$_<s;-#$u_.Ud0'#ToV2%39E]OSuF<%PuVE,'d#[-BUr8..0fI*=7wY#a_VqC+QLBiF7Uah1Wvo.2:9wU"
"SmE>#IL%/1`iZ-?fpv%+<wH,Ms4pfLQYoG;rM:v%,Ik%$u7dY#Sufi[4^kdJ@,B*u'.dP/$Wbc;t9'mBndF'mr%9Hu`&;E1CF?(j[R#I#ofbdu-ixG4viPN#g`s)#)L&u_,JYY#=&>t7"
":oYlLn/BnLjG9>G8oLcDGD#sIRq+)OcV45Ts<=AY-#FM_=_NYdMDWfi^*arnnfi(tBoxg4#(^fLkB6##XR7U.R0p6#0iu8.1r+@5Axh;-NH)2%6Z,F%noC0_`s-+:a<kM(uBIT&8%;;$"
"uUNj9#i9C4*n.J3YvT:%^7^F*YjQD*s-g:/'(%K#D9E@%f<36/f_@[#D=YJ6UfaiBb8(s$QucIh'Qv3'&nw=-wqpQ;<E='#xhBB#.,ClS_K&kkdvV]+_jX<-aFSg5b]Z6#_8eh2:$nO("
"i>4+$w%;,#41%wu>lOoL:hA%#M$Z6axe%j05SPfCF/'vHOU/,N`;88Spw@DX*^IP^:CR]cJ)[ihZedumkJm+s<,u;-)S,<-45T;-2>tiB5m9^#Lqa$'$gBr_5.78%$^x2)8A%n&ZtFJ("
"2FNM2c4n8%>IuD4eu+G4#/<9/rhGH3&c^F*rngs&Rs8#0#d$c;J'Gt7X^kA#)pP9%K:I&#gPUV$$s:$#+Ja)#o$),#@N=.#pxQ0#X9v3#6J:7#n$X9#T%>Y#Okn@#n8$C#=>WD#`OGF#"
"u*;G#1h@H#GNFI#_iM`%E1_`<&>uu#lxGi^6sg8._;3VZ/.(B#%SrMM$;88#tS#<-QJg4MD)doL*dYs-<ebOMGV.(#^K4v-ZO>LM/2(2#ShYs-SviuLPJxn$,<nD_e,?k_tDIwB'VXh("
"*aRG)mh#2Bb[kA#Q92I&+QC_&5dN&`1c4VZ^R)@0'T_pLMU4rjev]vf6LC_&t*F_&0elp^bMm6#(RPF#5wX?-N2RA-Clb_.rfvQ#xPmp%)lkA#Cdq@-mcq@-P3S>-P3S>-k3S>-L:#-M"
"[^.iL;^.iL`C_kLHd7iLi`e&MV=DPM>`e&M<^.ijWSk&#%,ClMOKx>-wcQX.3^Z6#MhG<-HhG<-UYjfLJ4%&MTotjLRlw&MN%1kLZ%1kL0)doLT1CkLXUZ-N%>UkLV=UkLH@7&MdC_kL"
"[hv-Nib6lLb[,oLhtQlLi'n1MFgG<-afG<-o:#-MrB3mL0;6L-o@;=-uZYO-jfG<-nrG<-lfG<-V=9C-(r-A-FQF0MIVY0ND6KnL,6KnL.B^nLYE:sLIE:sLs;TnLo;TnLrMpnL)44uf"
"t8UhL,YVpLRP#<-Ah(P-)*m<->N#<-QeF?-QeF?-F)m<-9gG<-Hm+G-b4`T.(vs6#/UZI%JeQPTkLC;IUb'@0lsx?0x4j34*Z#@0RsG_&Zs0F%RZ0F%D00F%d81F%hD1F%,>2F%H;,F."
"iG1F%iG1F%5WQV[N9jiUtRhiUlYmV7EgpHWh)HMB->s=Y82e4_pJ?X(663L,J4aX1&DI_&&DI_&8n:R*7xI_&,>2F%6]2F%Bt#:)OkJ_&1INq;@Ubw'FOJ_&>+WF%Ue3F%n=l--;l2F%"
";l2F%T/Sk+@=J_&LI3F%F73F%D13F%PUuD__XD0_af:-mDt-#,Ovc?K?n+Vm:pLucxlPk+Y-,)MBBD28*P]ooi^]ooM,4m0Hr;Mq2ZQcsWVw.i%VF]u%ck]ud[2##8A[A,VBQ&#f$?D-"
"4LM=-jM#<-%sM]$3ChhL>v2*MC^.iL@KihL=j@iLb0=fM$@B>,Cc[A,P-iP0Xkk^,4E@8.#/-/1%$Ej_mjGrd$WYk=U0Mk+pr>_/pr>_/>6G_&Y1E_&E%et7J>4I#X.'sLODetL`OKvL"
"pZ2xL*go#M:rU%MJ'='MZ2$)Mk=a*M^nbp@roqr$9pG,*DFP8/(5;^m(S,<-l(.m/O$^6#B^uQ#k*HD-ws`E-`fG<-$gG<-&#'k$sjOoL&sPoLiV#oL4)doL9D8#M(A2pL4A2pLGij,#"
"HJnYM+QM=-8MM=-Xdq@-8Af>-NqX?-wN#<-<A;=-/N#<-1N#<-poWB-ofG<-6gG<-6gG<-5,D9.wr&R#C*qfDB*qfDAwTJDC36,E8U9/D8nPGEjj'pA:*2)F/r[PB<<i`F?V[A,E&F^G"
"f2l6#iuRX^Ut)$#XdX4M&FPGMsq49#Inu8.@OGS76Jb2_tBE0_54b05Q@U6;9F;=-@@;=-=Dvt$Ajnmi<v3+N=j@iL)t-A-v%.A-DM#<-$fnBM/WOjLP%h,N61ngD*&.m0&afM1^gE/2"
"nUm;-UM#<-a/jE-q'/>-s'/>-_M#<-VF*nLWYWuPPQ-58Is)29XCJpf82e4_8V+eZ-2Ge-c4.F%'En3+TwK?pX5/4+$tZJ;d[Ne-ttRF%KtE@0-LSF%?8]R**C<pp-fipL.fipLfZ_@-"
".8NM-HA;=-V6MP-DgG<-HsG<-4@3U-GN#<-UC5O-OgG<-r_./Mp3ItLLX[&MEE9vLfhP8N_uc8N/,v8N2>;9NrZi9N.h%:N)$A:N9@=,8]@28]*E28]L^25^q7KM_=SFM_#(n;-;hG<-"
";hG<-Xn+G-=hG<-K<#-M_/R=N<;e=N>Gw=NU-'?NW99?NCGK?NZKT?N]Wg?Nwd#@N/x>@NYjK(MgF$D2o5H;#?<1K(;=v,4`D?)MpD?)M;5pfL3kv)Mliv)M-F5gLwU/+M%%GCN#&bV$"
"aKto%DhiY#4?.^#gU)W-cSC_&`n#.6(VXgL4$VdM:kpi'594m')I*W-]g9XU8n2+N>#+fMR$FA+Ro(W-W&.90lbYV-tmCZ-1C@8.:Kq3FVeTq)j`5W-u<e&6vgji0EgrA#02@A-TY#<-"
"p3S>-7-=J-`fG<-drG<-efG<-r1^gL^9158i[J88'OcP9m*cP9+t$j:OH%g;d4>,<+O*W-rh(F.1&S^$J86>>s6Ap.[KQ&`1V9db]J2R#H_Ij%Hk&NrqpCoe61Fsoq5@lf$3t;-cpko$"
"5P*rLEcB%8.ZkA#D>%q.x9eh2e=[l3Y^Z)4M/6<.LSEF3*lOF37uj-$('lA#'sUv-='xU.BTIg)^K9o/8vjI3QNCD3]`,<-HR`X-TV[w'S/Qs.->g+4ZDl-$w+TF*KLg;.:qlBA@NAU)"
")CKn3l^D.3OnR)477X-?A=-[$+A@<.`&YA#?ai?B/Gl/2<$(]$(J-.6P<m'&0Z]F4DcGg)VYbJMutV..;CTfL(NO/M@./b-//xTMj$8nNaBQt1DCWg)jJ@d)*#dE4OCi8.?iC%68fq*%"
"pf^I*'5-dN;Q`.M1+p+Mx;w[-h>P/)s):H*u`$w-Rs+H3%/qkL?\?eA4=8b6N%(@lL9I8f3^7Ph>*:?Z$$3&.M:2I./c84c4sf6X1^YLa4w^3Z6^b%T%t`0i)c^om(U,Ki^Dt5L2[Bo@#"
"%>;GD/Fm`E%=Xp%r_:<-Mk,.)@>l-$b65A-4iDA-p**`<I$BQ&'4HQ8'%tDuIM930U:06Af$HZ%v9Kb*MO#G4H&/J/1D%n&/;]T&J%R9.OKYK+@i+6'u5aH*TGG:vOErJ)=nCR'Z0s?#"
"pQ)D+*S7cEU6[:73bfR/#18iT)N(l0(R//1e9Sw#l2?7&grRL(,&QiT.#n*3>W)0(kkgF*Ir($u@k3?6lhAQTKRRs$T/[d)8=H<e;]f77d@9+*?T[9*()d_Fc=9+*KN-D*R9#R&,a_qI"
"LF]Yc78*p@xUZ&k.Ys/217/s.MvIO;pBsT+1?JX#9t8lLmuF:IZclJ*BVKY6GifS:Ymk8M$ESi)+4gM'mi?URK?2W))p-P;+ade2@0ir&7%qh(W@UO;FpOI)SC*6/?7ue)TiCW-J]+aI"
"6s9gMa<u4iAZ1E@^BjG>'=dbWM,Rg`dYZ8/4&M)S`BVB#.5Ml0iMWa*@lt.)g`@L(;W8i)g5Pb5Q2#b+Be&s`mD%s-JY1W-Z1pF*3aA>Htnb6/JTGo/nJ=T'%9w3:J`*SI68;&/Wjw=)"
"u,/IMqgp-<T_AG2#&>uuA3OxuDUw8#<OY##@eIq.M_`+`11]w'2Ii*.=Iu&#>iX.#Im@u-YClwL0&2'#V9xNMHqugLRO-##Tn:?#Nv?0#ZmSs6e;F,2gGX,2$rT_4FI3Q/7=Kn:n4``5"
"Bc*R:r[_]?tX=&u(rVP&27K('Ku0F_UTj-$PsKEYV-oo%CJD)+?#qJ)Ac-E&DAY3F0npj_n8Iij'@,gLK]BnfjTXgL5x#ofXIkN0=d*P(lB7g)i@QLM.NKJ17qcr/SwZ4O_UMHU]`)cQ"
"D(iwMV-F7a_]vNak5;^,wveQjc#IG)N,m6#@4i$#`PZ6#]'tY$Ub`Y#chvP'>WTE4K1-o'u7aF3uc-T.xQ*c4UDbB'V6#kGG.1^#kIvD-SK@2*gcsOso-hxX6;t^#*;ZiTSC]`WO4(60"
"#P[6#<CBC-Rnda$-=FT.pha6#f5`P-DV>W-XqsSL;fI.2qU[s-hHFgL?cm(F$L#<.i4=1)Q/`;7A*aX-EuhOtvfV]'nbdmO^fOCP_^ZY#,Gv.UUOx%Xr#mS8Ei6j(#BxS&HkhP0'x]>-"
"'x]>-hcsS/DVLQ'/Yuu#cKgN-Ab,w(O')W)2KVY5LRp=_2/j_T]T-##UJFI#.`H(#@FFf%^F>L2VIuM(p%bpu8$_ZD0[Ud=ps(SeYMWS7jl@n/;EsI3=Cwjef':W.omHK#B)Nm0kF3]-"
"BV.`Wp36mHWb+T%1X9p%-GB>P2P###UBuD#[a<c4vHff1Rt%q+xn431rR0,3$:1k1OTG,,V^GR&^T'j*OEKU%25YY#iVguGp7Z8%>BCG);Io6#2_IU.b]m6#.0F7%xp#K+2-;hL0SSL("
"'&[6##RWR%0n^3_3hVg=2,D-2<ou$')?nD_H'4q(/1r5/hujh2v8ApAB0G)4`kMe$uV5A#csVDDEx*A#w[QW0@Qp+>wW#juAOeAGP1C=One+M#R%sW#JYX.#D;SY,Hb:p/;aN'%h7B3E"
"wJ%a+u[r;-;f=$%%FPX]G6FbjRnuu,r$g+M#kv-N1ilrHHg<#-P&Bp&F`gFB_pW>-GI5s.[#'g2M*.m0wThA#[N$9I[oYs.#jic)Wr[/1'BqB#kUFb3S$]I*=c%T%ec,YuhDO`+Cjp(2"
"t=aT/1o8kXX49oe<bR=-q[ihc>-fx,Jk8GMkDI)*7E,Aer$;&VmhhV*R/<;6]RsBO#&>uu5-r`$w6*h#ZiPlL][Z##Wln*%P/5##oTt&#l)x%#n';p$7%Is-at[fL`A;,D65=2_LEBN*"
"MHxaj2XD>#j?=8%2cuu#<#gb4H(b'4MrDi-2q'kbrG:4hS05T.H>6C#XjvO%L)>>#/jPN#eNDmL^u0*#`PE#.>(#)M)d-##=,>>#nYJ=#>%>n_CHEO<Ocir?MF0j(6E=6'Y2l6#x3UhL"
"pHm6#f_0HM'MA^$gP.,)r%m;-Fs@3%Sc#gLfeTW$w.Vg)1BGA#@dI#5YR?f=mW@RQKKJHljU7@Db_-SaI26>DKBoV%(<n38ukjo9db18.3mlGYBs^Y#KD6m&sgTMKEj?D*j.)6)@&=+N"
"QU#,2jeg`j5SPfCJaXrHOU/,N`;88Spw@DX*^IP^:CR]cJ)[ihZedumkJm+s'_F42(S,<-45T;-t3SD<76Gb%dul-$tSb:_oWT*@F`0F_Mn418kE%@.6q%v#Wtr;-b%O$%+9,F%jQN^2"
"'*fF4_R(f)-<Tv-ln8`&Ig8d*B0<d*%+)TNCk?29@'wG*1-@k=k76uuOsL[fxQIP/wOn6#je14BbY0j_QcSY,&DDW-3=$RNg8INsFJXJMpC?##I$f;-ww#&%C`tw5J>,T8c,,F%%6GVH"
"H,U#$8E:@o[/LB#A1oUj[cs6Z7=p4N+%_4Og3eL#av._uf<MB#3.b)@+,Y>Jdi</I`&1Bu+B,SnAP'>,H2qKGUv4SRr7:w^w,[`*xA>G2UBp6#@w4VEw.wW_i<,UVcwdTiHAn/1dOs6#"
"0####:r+@59N*N0kFAnfF)Y/(vGqhL;^.iLUR-1MNlu&#uC*#.IvSKMmJ9^F%oB#$;?f5'Rh=ohaJ8f36tvp-b27PN1g^R/0i)<-&s9(%S8,%P'o78)tUS.8$Y._#Q<q*<UI?hu<f&V-"
"Lm&V-#7Do%L5*T)[NJ[8H6cp%iv'c(FBo6g7VNL#7V1AO,/DC#xYi^u]d$##Mrjp#FG,/$'h1$#:vB)Llx7p&KoGp&NO,M8&WX&#g7bTrO9o-$`jo-$pDp-$*vp-$:Pq-$J+r-$Z[r-$"
"k6s-$hTs;-`p)6.wJNjLWJ=4%5>#295e?_#o%VV-Wl*)*j2xr$l&Gp.t6p*#3wa]@QXZ##KmP<-R6T;-c6T;-s6T;--7T;-=7T;-M7T;-^7T;-QVw>%,BGb%7Ek-$Gvk-$h+(/#x-'p7"
"7^Tp7=Yu##^,eh2>]WF3J29f3?;Rv$IH:a#S,m]#]q.[#C]R_#l6hF*>7hF*@:qB+:e3:.v@pB+:_wt-A:vi9PH%1,SwPr)G*2'#TOp6$+5^#CaIa%$bXD>#`:SC#4C4D#qh2/VihN=8"
"r?=_/8.@P/,Y1,;<o@X-Us#73WO)##gm%v#;d9N(Mgpl&/X;s%Udx;-$wG`-o.xT0Z5Z6#UYiM1[ujh2Ll*F31(XD#po-F%tgfX-T%%N$-[XB#lrV(E%^t]u=PE20MDEiB)dKwK'3lo%"
"h;[]+lGQ]4bnRfLQu`H%dmWMa.w_)NFwEmf`;IuLv=HNBgjB3(,7pK2)g0i)[s4W-Y5DdOk6b/8AS57K[rGnAtpxkC%6Tt-/H;&.pjQiLa=M^#VL7%#L+,7_3d,naAdk-$(4w;-&fG<-"
"`S(A&N-$XLKX'*#5?VhLcT..2N`;<-Wf1*%[(;;$P7#B#a:(F$J@w[-HqlN+Wp3J-^8l@&9$kvMbYrB#YBiQNh&jB#A60W-dB8VU&?v;d`NN]/C.ai-X-Y'AYYfiBAbD5/vWq&#jxES7"
"1(G&#e`B;Ie@;5K^?^PBu&DAPNHQGEfIVPKV;+#H41P&#JHj>0[YJ=#;L7%#N(VT:i^2kB^iG,*Y'=<-i.P'%`L)qK0w(hLNsSk9:Ro^-TR6##b]m6#r'ChLY2Z6#>Fhj*NsSk9.q^>$"
",#aF&f,g_2XMi=.r_/AA)Vs+&NmZZK0'?i#qx6,RP^1[Ebt^B#%Y#r#1<?G`I*(K1A8]Y,+>,YuBP0s-A19'fmpToI%JT[%:9J9$OY?T-SIr9.I1tckifK;I(w`=-$[U[-Z)9S*bOpR*"
"jFsvN<rHxkPxO507t2<%SW7M9l%3/%c&eYO)[%,v0``=-RaOZ-LK:_A]KpPBV,@R*h/`*M^I);$M6SR*@F4D3qQY^,3YfiBXhnEIa5$Ra-92+Mk2E6.Ak7NMVJ-LMR[=W%=ILS*khuGM"
"f01pMk[=W%L1;_AWn@`A(sTR*/<V^N)QKvL)PEmLpc`=-5@mXM3[uG-86OJ-&$<uM6AbrMIq,-P&31pMYhqD.EPOV-&xf20j)+:_.)`3_xfk-$3@pjiIJ=jL=I7aj]GmlfiNOgLL<4bj"
";Ul##Q/IL('&[6#tqVe$FUScjC;2/((WLm/C'+&#b]m6#:WTs$*6,F%d(et1jK;b7kvN^2k2/uI-#Hg)Rhhl8sQYF%-N2K(k^5(JW#V*O7AKq/A4'q/vh;;Zxv%NFZ=lKP.et8.1XM8v"
"n/c5/[JlY#xUw;8x?x%u2CuS*h5'##<C1^#5fqw-eY5]O+]4$Oshc*v)t`=-v;xU.)G)8vt)+$%`EJG;uk-B5fsp6#e?wa.`PZ6#F&$t$xs+F%TxO&`FK6R*0,Sh(E)b;&Wd#,2bmpj_"
"UAtR8:Jh]nX5Z6#Oo@7&J6gZ7.5BX@9]O&#FfG<-IZM&.]>gkLX6Z6#`=DX-'@lb6E''MD%0?&6'^Bv-'BqB#+Z8f3t<7f3P;2]-moT%6oMg@#*t8c$*;2+%g(aK;11t#-8s.Ac9Uh$'"
"^:9o0,,]Ac3VSY*j%B+*@<@m/S'n<-,]&COnlu8.9JpCON6Bm/Soap#=LX&#^&g7.w?BsL;q@u-g%to79fF597q)<%ZC^CsN0wdM)[%,vpqb`'DXoPBEvq>REt@u-XLL]ND@*YlP7?5/"
"gl[_A*TnR*o(0S*AA](N=_YgLTd<s#5fqw-F8D2MrVx_s$),##S+tR*BXk%4bX`<-BE7?8pZkxuLC4R-g9;qPtqeBMg31pM,2g)v*J;QM%&<*MYu@u-W*nYOs]`xN1hV+vx==*vA3Oxu"
"kem_#*Y9o/>t=$GXaIx2oW6m%JQf;-j>Q)/V?kc2,M7R3,^mER;KWl83/vERBL]PB>gjp9%#KfhuRc#HaKBV?=Ers^Zv^(M8GVPM%lV+vs4o-#1w'3$o>S9$d:kv0mKnq$4$_3_VWj-$"
"t%_-4qi_:%2Sjr?_bg2b$r^#$Q3TLMe0B$.81LcDiE?/;vB'a+t8r40V$nO(<64,;V],E%17OMKKikr-+i?8%g`5J*6FtsU$ArB#h)='%TZg8.f4F`W-ld;-$(mS(Aa.S:bl$&6=9kMG"
"Mw=?e'ie_#b%jE-?cjTC*^GF@/'YEda`Ti^t1%&Y[>.E</5LMU[Cr29<Lu>@.S448vr4R*NKi,#Bg1*Mt`du#;6=&#QIXS%70<r7;TF&#j/p;-%M#<-B@00%:vcG*-6,t-2eHiL6Y$2&"
"H8YY#d2Av$Qdt;-'M#<->PeE&+jZw'cTN^2L%^=%7?t-$rXe8.e.Y)4`]`/1LCr8.;Dgua8gbb.SM8i$Ijo0##ALM9H;eB#i`Ov@VtARP>s:T.j5KC1r+TV-CL)qKL'F4#5(V$#n>$(#"
"Zr,U;#?C<-mlUU+@G`W-5ch?eU$iS'I8<ki[1$##@DCh2K*OP&jB*20a0%##'[CJ;QAV:U#cl-T^*=_$_6Z$(%jghuYoFi#N?v+&)PT`3+RlFr=3vFrh=pxO/VH/1T'mrZLIt.q[C:s-"
"l1k`N#W+T.:lfYYCpDa*Y?^Snk;kAF]X+aWV.]g(1h>sd]i-B=sPFm]7>YT.`?+<HCHQsdO$TH2Rk(<ZQt&h1GS5HihmgH2l1gaN0W1<mfb?t-%I5t?<OqaNh9x#lTjS0:=*H<d*.Mn8"
"sDO6]WIDU.SW$OKRBWBtO3l6J[;Etdp;)+3$]GbN*F@hhRtdhCdvHnoDaBV.uD=V7a?e=Z=RXV%/1pCbq0(VntbLD+[-l=mtHfV.Y-5>HJ`+j(lPo1q/@TPTOd`2(>&r]GoGTDOIj7,W"
"t]Ip&N)-W.)Kf>6YmH&>49,dEeZeJM?'H2UpH+p]JkdVe%7G>mXoW&>6?5dEganJMojJ^,wd)E4Q0c,<,REjC]t(QK7@b8ShbDvZB.(^cubA&lQ4%ds,J'3(qv*?6L*AdN4)W,afN131"
"W#o>dh**##$&###G.Me$1)C##av5A4P5@D*.FRe$W=`@k[YhA52RRe$']DlS#a'^>mUQe$f>%Vd%s^>?-CRe$m;`:d*.$29p_Qe$hH_+MrviG<#%Re$O:;i^%s^>?:kRe$0nAxk(53>5"
"C0Se$CY>l]#a'^>oVKe$hK7;-.VmZ?e_lr-s4'S[B-kEI]nRe$YU[=lV,5d3A*Se$idP4oZVq]5oH6j:>DH_&t*eCj#a'^>^(Qe$TOV._%s^>?B-Se$,[X_&3WU7nqeHq;qH/20Ic1pJ"
"+8Le$kAPe$*ZmOo#a'^>.EOe$&Zs@O,JZZ?-O*/13DK=u9&vG*qbQe$8`3'oR)V`3*^G_&=E]%FYG1a4sgNe$HGQrH[YhA5D1Me$lU$REERAk=i`H_&LSC3ku]HY>'TG_&pB,MK&&$Z?"
"'0Oe$ptNoIKM'F.n^bf1s*8)X[gI9iTURe$hQ$GMWPN(v(`Q(#8:fnL>E-'vlSGs-/@P)M@Q?'v*TGs-A@onLDjd'vxAg;-%))t-9ADmLP8E(voSGs-MR4oL$;DPM=AMPM2GVPMW]s(v"
"#W5lLF.F'M>E-'v^Cg;-G(-l.SXc'v_$)t-o,-&ML>E(vs_>lLjw3'M?K6'vPTGs-*VA(MJ23(vPeGlLhPvwLK>N(v'UGs-__w#M$5voL2C+)#,,k9#4dq@-oTGs-U'O$M?K6'v)TGs-"
"^?t$MDjd'vuBg;-%))t-768vLP8E(v5UGs-jQ9%Mx(doLM>+)#h9k$MTPj(vCh(T.6(c6#lS]F-kTGs-t8?&MLDW(vQ6@m/OpN9#*]ewu*h(T.]mG3#4_Xv-d&AnL0R1:#lAg;-HUGs-"
"tI/(M?K6'voBg;-6BrP-7_Xv-a8]nL`_e&MAQ6'vBDonLE.r%Mk/BnL$PpnLp]c&#'5)=--h(T.[g>3#$gG<-.h(T.i``4#%gG<-[h(T.'.l6#@Rx>-k[,%.LoXoL]b6lLAQ6'vZuboL"
"R%1kLAQ6'vW%loLQw3'MAQ6'vP+uoL_nHlLAQ6'v]C_PMJN`PM0D$##ca''#S?*1#8b?iL1+)t-ZFFgL:=`T.-V(?#k&:W.B$4A#UrG<-BKx>-u@`T.;C7@#$sG<-8X`=-:A`T.7oL?#"
"?sG<-/fG<-NA`T.mIjD#YA`T.K-O]#MYPH2Jw<]#T7>##1fu##^+E`#vB`T.;hNX$U@`T.8O*X$h@`T.b@A]$rrG<-BKx>-VA`T.Q)HZ$esG<-8X`=-7B`T.F*tX$EtG<-/fG<-_9D9."
"Y6Wd$EODT&xixDE@#`s-jCoX%lSG,3+YR@-qD0H-Og.>B-dLq1jJWnD7.<hFT:7/M'*DVC)2ddGD=rF=oqqE-$WA>BdH`9C@w*_I2MCnDK.x?07bCG-n)A>BElwr1b2<rCN6:?-$@Z<B"
";-OG-5#hoDQ2[DIfCunM7&223BFwgF#--p8,)DEH3rI+HJM[rLV/5bH@g0H-[6iTC?D5_Aq_0Q4_tF3Noe*LDJD7r1e.6+HKN]'.W@riL#ulUCo62=BH2w%Jl,%[T:P]:CG^Y?^lmw9)"
"-arTC%hitBGnZbn<:)*HMfa4C;pwF-?Wae3tYx0Fu+ZlEeh#SCAMN6asUv#JX.2W-wW.l+,o`w0bnx['u:1eGgD90/)-7L2NJG[9jr3TBHhl)Ni$b>-g$fFH>eo*HtisjE7-giF*vIqC"
";aeYH7.FcH,I8eG>pSGMQpRSD7/C8D3CRSDv.5UC11(@'RP.#H>]DiFljMn3AACs-Q)+8M:p.N1/ZXVCvm0:B#HX7Dqt1C8^8x+FCe4bHH>k?H*F%F-*:`E-xBrE-,G2eG_rRsADcq?-"
">Hs6*IkD5B9oVMFIQYMC'--h-kPNq2tE'mB%,&gD(<dH.=VmLFK7onE$^HmB&dCEHLj`DFP&ReHAY`M:=K>jFJ?,jF4=rE-><RRM7gO-8Zl9H4M>Lx&E3FGH,C%12b=)CO,$>L'Iu%5#"
"vP?(#L)B;-Z(MT.,n@-#FN#<-oM#<-AmL5/DH4.#9'BkLU/w+#9gG<-H;.6/Nmk.#3#krLbU5.#Jpe]GPBlMCRuDR3HSTD=:=S5B-I)F.Pww?0Suo3+iC#F7>aQX(U1Pk+*:6F7mcOcD"
"uD(@'=%g+M6xXrLWx.qLG-lrLa[`=-^MuG-%T]F-J5u(.AKNjL/LM/#28w2M+;oQMc:(sL%(crLW#=J-j.%I-qW_@-`N(XNmMKC-q%kB-5+?D-%n(P-WFII-H5g,MuN#.#r&@A-kR<b."
"T;L/#3*B;-^5u(.kg$qL,(8qLjbeFNeFOJMkopF-OCsM-csQ:O4Kr'#PGuG-=bDE-SaDE-dM#<-142uO(qY.#W?'F[pG0eQg-_9MxRwWUd5Qq2U1Pk+*3o,4C3urL0#/qLOa%,87sX_/"
"`UFwKJ(h--rxOcD7P,j196r`Fm&tV$j/>=-K$jE-M*LX'$%#gDuDOW/-uR(#u*JnLUrY.#XFII-MOYO-h9bJ-%#)t--)trL4fipLup3_$-eW<-t.T`Q5.@78XUaM,*]'@BQj]3Ff*LXL"
"Na>G2k^TLPGIoDcq`9@P`OG&#VEJqD-=auGnm/<-e@qS-ge<+.:xjrLV2oiLYFp-#qdBK-+qdT-%wgK->N#<-Z?:@-&XjU-lwpd%d)q92-1Z>H%>8R*Xk)F.6hf'&ucVJDRH:/D9-MDF"
";Ke]G7RdYHUE.#H[PA&G%d_w'tLb-6SJe>-0r@r%'[Rw94rk`FCBe-?ZW.#Hg/J>H6+3T.'n@-#B;Mt-<EA[8,UAFeYHW58W=eM1A.0gDM/+LPPA6eH[)2R3m+-AFgK/F%MDmEIBKj&?"
"F='[8UGI21nTKQ(qip98/$2)FQrSDFB69L,q*r?9&@Q,MvW@$8@twlB4=H>H8PgYHaai]G,_UDFXiJk=sWPrigqhWhm''mBh-^MCv@Ze6aJNDF)4D;I?L2j1tWqM:g?Ne$*5$FR2du7I"
";wlcEb@tcESkTGE(nnp7Lhjp'#PrO3)/Vu--#krLd3v.#e,KkLMqXe/sQ,Woc:$##T(+GMm=IlJooYb%=8'/`0]iOB5SgAb8h7-#Inq7#?-85#WKf5#<vO6#8j=6#^Vx5#[B0:#bPo5#"
"-RD4#-G31#H(02#9fa1#4x&2#Da^:#t8J5#`+78#4uI-#n*]-#T1f-#b=R8#kIe8#kUw8#^sN9#qh<9#:=x-#60k9##+b9#k_W1#LH4.#QaX.#0?)4#Y#(/#[)1/#b;L/#dAU/#iMh/#"
"mSq/#rl?0#AE24#wrH0#%/e0#igg:#50?;#JZ)<#i45##Jgb.#:G]^H4t.GMP_?##+/5##58Yuu&u:T.$),##29u%4(a*`s3oZS%*85uu5:no%+(<5&`)Ke$[RNP&.nIp&b/Ke$fk/2'"
".ISM'd5Ke$d-gi'2<b2(f;Ke$nEGJ(1kkf(hAKe$l^(,)6a#K)jGKe$5KKe$_3-)*3;t[tP/%)*:/;d*mPKe$'H[`*7WDA+oVKe$%a<A+>SR&,q]Ke$/#tx+:#]Y,scKe$-;TY,Bxj>-"
"uiKe$5S5;-=Dtr-G=g;.'PD_&6olr-Mt(t.=A]Csn0MS.Ib(T/%&Le$>I.5/QB@60@DA(svael/M0@m0)2Le$F$FM0UgWN1CG&cr(<'/1OHE/2T^sJ23uD_&UW^f1It&)3/DLe$Sp>G2"
"V,5d31JLe$^2v(3L?>A43PLe$[JV`3ZPL&55VLe$fc7A4OaUY57]Le$d%ox4_ud>69cLe$l=OY5R,nr6d:a;7AIE_&mX0;6jqxs7PPI.qNqgr6f_xS8?uLe$u3HS7n?:69SS.ipVK)58"
"j-:m9C+Me$'e`l8rdQN:VViLp_&AM9nQQ/;G7Me$/?x.:nWvf;m2cpofM=J:`e;D<J@Me$6gt+;$W+*=[Y2lon(Uc;vD+a=NLMe$>A6D<(&CB>_]mOovXm%=$jB#?RXMe$FrM]=,JZZ?"
"b`Q4o(4/>>(8Z;@VeMe$NLfu>(>)s@idJWn/[+;?l>DPAYnMe$Utbr?4=46BgcqRn76CS@0+4mB^$Ne$^N$5A8bKNCjfU7n?gZlA4OK/Db0Ne$f)<MB<0dgDmi:rmGAs.C8tcGEf<Ne$"
"9_ae$u]SfCwe1AFhBNe$su4GD=K@&GjHNe$'8l(E$1IYGlNNe$%PL`EApW>HnTNe$/i-AF'RarHpZNe$-+exFE>pVIraNe$7CEYG*tx4JtgNe$5[&;HGVuoJLlL5K&NG_&@w]rH.HUiK"
"xsNe$>9>SIN:eML$$Oe$HQu4J1jm+M&*Oe$FjUlJR_&gM(0Oe$P,7MK45/DN*6Oe$NDn.LV->)O,<Oe$V]NfL7VF]O[H:&P4#H_&Wx/GMb)R^P0)G%k9:g(N^mQ>Q2NOe$`RG`NfMjvQ"
"3,,`jAk(AOb;jVR6ZOe$h-`xOjr+9S6/gCjIE@YPf`+pS:gOe$p^w:QffOPTY1^giPm<VQD9k.U=pOe$w/t7RreZjU;20ciXGToRnRZJVA&Pe$)a5PSv3s,W>5kFiaxl1TrwrcWE2Pe$"
"1;MiT$X4EXA8O+iiR.JUvE4&YI>Pe$9le+VvKX]YUcENhp$+GVPis:ZLGPe$@=b(W,KdvZF;oIhxTB`W(9dV[PSPe$Hn#AX0p%9]I>S.h*0ZxX,^%p]T`Pe$PH;YY4>=Q^LA8ig2ar:Z"
"0,=2_XlPe$1Gde$`&SrZ[9a+`ZrPe$^>4S[5Ypf`]xPe$hVk4]_ZxCa_(Qe$foKl]9(2)ba.Qe$p1-M^b&:]bc4Qe$nId._=LIAce:Qe$xbDf_eGQucg@Qe$@ude$w'&G`NZ^vdYJ[4f"
"X?](aD6^VekLQe$)X=`aLmu8f]M@oeapt@bHZuofoXQe$13UxbP;7Qg`P%SeiJ6YcL)72hseQe$9dm:dT`NjhcS`7eq%NrdNA<JiSVjfi'RJ_&HA/SetBrCj#xQe$FYf4fU%,)k%(Re$"
"PrFlfwd3]k'.Re$N4(MgYICAl)4Re$XL_.h$0Kul+:Re$Ve?fh^nZYm-@Re$_'wFi'Qc7nc3WVn5'K_&`BW(jijo8op]-YcAZ8`jeWooo3RRe$hso@km81Qps`h=cI5Pxki&12q7_Re$"
"pM1Ylq]HjqvcLxbQfh:mmJHJr;kRe$x(IrmsuM,s#g1]bOWfe$'D*Snx4&)t?wRe$+]a4o$Y=at&jl@bctAlovF=AuC-Se$;7#Mp:qDuu)mP%bc&7k;jS&##";

static const char* GetMenuCompressedFontDataTTFBase85()
{
	return MenuFOnt_compressed_data_base85;
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
	ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
	if (!font_cfg_template)
	{
		font_cfg.OversampleH = font_cfg.OversampleV = 1;
		font_cfg.PixelSnapH = true;
	}
	if (font_cfg.Name[0] == '\0') strcpy(font_cfg.Name, "<default>");

	const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
	const char* menu_compressed_base85 = GetMenuCompressedFontDataTTFBase85();

	ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, 15.0f, &font_cfg, GetGlyphRangesCyrillic());

	Vars.font = AddFontFromMemoryCompressedBase85TTF(menu_compressed_base85, 25.0f, &font_cfg, GetGlyphRangesCyrillic());

	return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
	int data_size = 0;
	void* data = ImLoadFileToMemory(filename, "rb", &data_size, 0);
	if (!data)
	{
		IM_ASSERT(0); // Could not load file.
		return NULL;
	}
	ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
	if (font_cfg.Name[0] == '\0')
	{
		// Store a short copy of filename into into the font name for convenience
		const char* p;
		for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
		snprintf(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s", p);
	}
	return AddFontFromMemoryTTF(data, data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NBM Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
	ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
	IM_ASSERT(font_cfg.FontData == NULL);
	font_cfg.FontData = ttf_data;
	font_cfg.FontDataSize = ttf_size;
	font_cfg.SizePixels = size_pixels;
	if (glyph_ranges)
		font_cfg.GlyphRanges = glyph_ranges;
	return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
	const unsigned int buf_decompressed_size = stb_decompress_length((unsigned char*)compressed_ttf_data);
	unsigned char* buf_decompressed_data = (unsigned char *)ImGui::MemAlloc(buf_decompressed_size);
	stb_decompress(buf_decompressed_data, (unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

	ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
	IM_ASSERT(font_cfg.FontData == NULL);
	font_cfg.FontDataOwnedByAtlas = true;
	return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, font_cfg_template, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
	int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
	void* compressed_ttf = ImGui::MemAlloc((size_t)compressed_ttf_size);
	Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
	ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
	ImGui::MemFree(compressed_ttf);
	return font;
}

bool    ImFontAtlas::Build()
{
	IM_ASSERT(ConfigData.Size > 0);

	TexID = NULL;
	TexWidth = TexHeight = 0;
	TexUvWhitePixel = ImVec2(0, 0);
	ClearTexData();

	struct ImFontTempBuildData
	{
		stbtt_fontinfo      FontInfo;
		stbrp_rect*         Rects;
		stbtt_pack_range*   Ranges;
		int                 RangesCount;
	};
	ImFontTempBuildData* tmp_array = (ImFontTempBuildData*)ImGui::MemAlloc((size_t)ConfigData.Size * sizeof(ImFontTempBuildData));

	// Initialize font information early (so we can error without any cleanup) + count glyphs
	int total_glyph_count = 0;
	int total_glyph_range_count = 0;
	for (int input_i = 0; input_i < ConfigData.Size; input_i++)
	{
		ImFontConfig& cfg = ConfigData[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];

		IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == this));
		const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
		IM_ASSERT(font_offset >= 0);
		if (!stbtt_InitFont(&tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
			return false;

		// Count glyphs
		if (!cfg.GlyphRanges)
			cfg.GlyphRanges = GetGlyphRangesCyrillic();
		for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
		{
			total_glyph_count += (in_range[1] - in_range[0]) + 1;
			total_glyph_range_count++;
		}
	}

	// Start packing. We need a known width for the skyline algorithm. Using a cheap heuristic here to decide of width. User can override TexDesiredWidth if they wish.
	// After packing is done, width shouldn't matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
	TexWidth = (TexDesiredWidth > 0) ? TexDesiredWidth : (total_glyph_count > 4000) ? 4096 : (total_glyph_count > 2000) ? 2048 : (total_glyph_count > 1000) ? 1024 : 512;
	TexHeight = 0;
	const int max_tex_height = 1024 * 32;
	stbtt_pack_context spc;
	stbtt_PackBegin(&spc, NULL, TexWidth, max_tex_height, 0, 1, NULL);

	// Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
	ImVector<stbrp_rect> extra_rects;
	RenderCustomTexData(0, &extra_rects);
	stbtt_PackSetOversampling(&spc, 1, 1);
	stbrp_pack_rects((stbrp_context*)spc.pack_info, &extra_rects[0], extra_rects.Size);
	for (int i = 0; i < extra_rects.Size; i++)
		if (extra_rects[i].was_packed)
			TexHeight = ImMax(TexHeight, extra_rects[i].y + extra_rects[i].h);

	// Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
	int buf_packedchars_n = 0, buf_rects_n = 0, buf_ranges_n = 0;
	stbtt_packedchar* buf_packedchars = (stbtt_packedchar*)ImGui::MemAlloc(total_glyph_count * sizeof(stbtt_packedchar));
	stbrp_rect* buf_rects = (stbrp_rect*)ImGui::MemAlloc(total_glyph_count * sizeof(stbrp_rect));
	stbtt_pack_range* buf_ranges = (stbtt_pack_range*)ImGui::MemAlloc(total_glyph_range_count * sizeof(stbtt_pack_range));
	memset(buf_packedchars, 0, total_glyph_count * sizeof(stbtt_packedchar));
	memset(buf_rects, 0, total_glyph_count * sizeof(stbrp_rect));              // Unnecessary but let's clear this for the sake of sanity.
	memset(buf_ranges, 0, total_glyph_range_count * sizeof(stbtt_pack_range));

	// First font pass: pack all glyphs (no rendering at this point, we are working with rectangles in an infinitely tall texture at this point)
	for (int input_i = 0; input_i < ConfigData.Size; input_i++)
	{
		ImFontConfig& cfg = ConfigData[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];

		// Setup ranges
		int glyph_count = 0;
		int glyph_ranges_count = 0;
		for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
		{
			glyph_count += (in_range[1] - in_range[0]) + 1;
			glyph_ranges_count++;
		}
		tmp.Ranges = buf_ranges + buf_ranges_n;
		tmp.RangesCount = glyph_ranges_count;
		buf_ranges_n += glyph_ranges_count;
		for (int i = 0; i < glyph_ranges_count; i++)
		{
			const ImWchar* in_range = &cfg.GlyphRanges[i * 2];
			stbtt_pack_range& range = tmp.Ranges[i];
			range.font_size = cfg.SizePixels;
			range.first_unicode_codepoint_in_range = in_range[0];
			range.num_chars = (in_range[1] - in_range[0]) + 1;
			range.chardata_for_range = buf_packedchars + buf_packedchars_n;
			buf_packedchars_n += range.num_chars;
		}

		// Pack
		tmp.Rects = buf_rects + buf_rects_n;
		buf_rects_n += glyph_count;
		stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
		int n = stbtt_PackFontRangesGatherRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
		stbrp_pack_rects((stbrp_context*)spc.pack_info, tmp.Rects, n);

		// Extend texture height
		for (int i = 0; i < n; i++)
			if (tmp.Rects[i].was_packed)
				TexHeight = ImMax(TexHeight, tmp.Rects[i].y + tmp.Rects[i].h);
	}
	IM_ASSERT(buf_rects_n == total_glyph_count);
	IM_ASSERT(buf_packedchars_n == total_glyph_count);
	IM_ASSERT(buf_ranges_n == total_glyph_range_count);

	// Create texture
	TexHeight = ImUpperPowerOfTwo(TexHeight);
	TexPixelsAlpha8 = (unsigned char*)ImGui::MemAlloc(TexWidth * TexHeight);
	memset(TexPixelsAlpha8, 0, TexWidth * TexHeight);
	spc.pixels = TexPixelsAlpha8;
	spc.height = TexHeight;

	// Second pass: render characters
	for (int input_i = 0; input_i < ConfigData.Size; input_i++)
	{
		ImFontConfig& cfg = ConfigData[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];
		stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
		stbtt_PackFontRangesRenderIntoRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
		tmp.Rects = NULL;
	}

	// End packing
	stbtt_PackEnd(&spc);
	ImGui::MemFree(buf_rects);
	buf_rects = NULL;

	// Third pass: setup ImFont and glyphs for runtime
	for (int input_i = 0; input_i < ConfigData.Size; input_i++)
	{
		ImFontConfig& cfg = ConfigData[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];
		ImFont* dst_font = cfg.DstFont;

		float font_scale = stbtt_ScaleForPixelHeight(&tmp.FontInfo, cfg.SizePixels);
		int unscaled_ascent, unscaled_descent, unscaled_line_gap;
		stbtt_GetFontVMetrics(&tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

		float ascent = unscaled_ascent * font_scale;
		float descent = unscaled_descent * font_scale;
		if (!cfg.MergeMode)
		{
			dst_font->ContainerAtlas = this;
			dst_font->ConfigData = &cfg;
			dst_font->ConfigDataCount = 0;
			dst_font->FontSize = cfg.SizePixels;
			dst_font->Ascent = ascent;
			dst_font->Descent = descent;
			dst_font->Glyphs.resize(0);
		}
		dst_font->ConfigDataCount++;
		float off_y = (cfg.MergeMode && cfg.MergeGlyphCenterV) ? (ascent - dst_font->Ascent) * 0.5f : 0.0f;

		dst_font->FallbackGlyph = NULL; // Always clear fallback so FindGlyph can return NULL. It will be set again in BuildLookupTable()
		for (int i = 0; i < tmp.RangesCount; i++)
		{
			stbtt_pack_range& range = tmp.Ranges[i];
			for (int char_idx = 0; char_idx < range.num_chars; char_idx += 1)
			{
				const stbtt_packedchar& pc = range.chardata_for_range[char_idx];
				if (!pc.x0 && !pc.x1 && !pc.y0 && !pc.y1)
					continue;

				const int codepoint = range.first_unicode_codepoint_in_range + char_idx;
				if (cfg.MergeMode && dst_font->FindGlyph((unsigned short)codepoint))
					continue;

				stbtt_aligned_quad q;
				float dummy_x = 0.0f, dummy_y = 0.0f;
				stbtt_GetPackedQuad(range.chardata_for_range, TexWidth, TexHeight, char_idx, &dummy_x, &dummy_y, &q, 0);

				dst_font->Glyphs.resize(dst_font->Glyphs.Size + 1);
				ImFont::Glyph& glyph = dst_font->Glyphs.back();
				glyph.Codepoint = (ImWchar)codepoint;
				glyph.X0 = q.x0; glyph.Y0 = q.y0; glyph.X1 = q.x1; glyph.Y1 = q.y1;
				glyph.U0 = q.s0; glyph.V0 = q.t0; glyph.U1 = q.s1; glyph.V1 = q.t1;
				glyph.Y0 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
				glyph.Y1 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
				glyph.XAdvance = (pc.xadvance + cfg.GlyphExtraSpacing.x);  // Bake spacing into XAdvance
				if (cfg.PixelSnapH)
					glyph.XAdvance = (float)(int)(glyph.XAdvance + 0.5f);
			}
		}
		cfg.DstFont->BuildLookupTable();
	}

	// Cleanup temporaries
	ImGui::MemFree(buf_packedchars);
	ImGui::MemFree(buf_ranges);
	ImGui::MemFree(tmp_array);

	// Render into our custom data block
	RenderCustomTexData(1, &extra_rects);

	return true;
}

void ImFontAtlas::RenderCustomTexData(int pass, void* p_rects)
{
	// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
	// The white texels on the top left are the ones we'll use everywhere in ImGui to render filled shapes.
	const int TEX_DATA_W = 90;
	const int TEX_DATA_H = 27;
	const char texture_data[TEX_DATA_W*TEX_DATA_H + 1] =
	{
		"..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX"
		"..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X"
		"---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X"
		"X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X"
		"XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X"
		"X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X"
		"X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX"
		"X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      "
		"X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       "
		"X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        "
		"X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         "
		"X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          "
		"X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           "
		"X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            "
		"X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           "
		"X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          "
		"X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          "
		"X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       ------------------------------------"
		"X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           "
		"XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           "
		"      X..X          -  X...X  -         X...X         -  X..X           X..X  -           "
		"       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           "
		"------------        -    X    -           X           -X.....................X-           "
		"                    ----------------------------------- X...XXXXXXXXXXXXX...X -           "
		"                                                      -  X..X           X..X  -           "
		"                                                      -   X.X           X.X   -           "
		"                                                      -    XX           XX    -           "
	};

	ImVector<stbrp_rect>& rects = *(ImVector<stbrp_rect>*)p_rects;
	if (pass == 0)
	{
		// Request rectangles
		stbrp_rect r;
		memset(&r, 0, sizeof(r));
		r.w = (TEX_DATA_W * 2) + 1;
		r.h = TEX_DATA_H + 1;
		rects.push_back(r);
	}
	else if (pass == 1)
	{
		// Render/copy pixels
		const stbrp_rect& r = rects[0];
		for (int y = 0, n = 0; y < TEX_DATA_H; y++)
			for (int x = 0; x < TEX_DATA_W; x++, n++)
			{
				const int offset0 = (int)(r.x + x) + (int)(r.y + y) * TexWidth;
				const int offset1 = offset0 + 1 + TEX_DATA_W;
				TexPixelsAlpha8[offset0] = texture_data[n] == '.' ? 0xFF : 0x00;
				TexPixelsAlpha8[offset1] = texture_data[n] == 'X' ? 0xFF : 0x00;
			}
		const ImVec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
		TexUvWhitePixel = ImVec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);

		// Setup mouse cursors
		const ImVec2 cursor_datas[ImGuiMouseCursor_Count_][3] =
		{
			// Pos ........ Size ......... Offset ......
			{ ImVec2(0,3),  ImVec2(12,19), ImVec2(0, 0) }, // ImGuiMouseCursor_Arrow
			{ ImVec2(13,0), ImVec2(7,16),  ImVec2(4, 8) }, // ImGuiMouseCursor_TextInput
			{ ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_Move
			{ ImVec2(21,0), ImVec2(9,23), ImVec2(5,11) }, // ImGuiMouseCursor_ResizeNS
			{ ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 5) }, // ImGuiMouseCursor_ResizeEW
			{ ImVec2(73,0), ImVec2(17,17), ImVec2(9, 9) }, // ImGuiMouseCursor_ResizeNESW
			{ ImVec2(55,0), ImVec2(17,17), ImVec2(9, 9) }, // ImGuiMouseCursor_ResizeNWSE
		};

		for (int type = 0; type < ImGuiMouseCursor_Count_; type++)
		{
			ImGuiMouseCursorData& cursor_data = GImGui->MouseCursorData[type];
			ImVec2 pos = cursor_datas[type][0] + ImVec2((float)r.x, (float)r.y);
			const ImVec2 size = cursor_datas[type][1];
			cursor_data.Type = type;
			cursor_data.Size = size;
			cursor_data.HotOffset = cursor_datas[type][2];
			cursor_data.TexUvMin[0] = (pos)* tex_uv_scale;
			cursor_data.TexUvMax[0] = (pos + size) * tex_uv_scale;
			pos.x += TEX_DATA_W + 1;
			cursor_data.TexUvMin[1] = (pos)* tex_uv_scale;
			cursor_data.TexUvMax[1] = (pos + size) * tex_uv_scale;
		}
	}
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar*   ImFontAtlas::GetGlyphRangesDefault()
{
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0,
	};
	return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesKorean()
{
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x3131, 0x3163, // Korean alphabets
		0xAC00, 0xD79D, // Korean characters
		0,
	};
	return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesChinese()
{
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0x4e00, 0x9FAF, // CJK Ideograms
		0,
	};
	return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesJapanese()
{
	// Store the 1946 ideograms code points as successive offsets from the initial unicode codepoint 0x4E00. Each offset has an implicit +1.
	// This encoding helps us reduce the source code size.
	static const short offsets_from_0x4E00[] =
	{
		-1,0,1,3,0,0,0,0,1,0,5,1,1,0,7,4,6,10,0,1,9,9,7,1,3,19,1,10,7,1,0,1,0,5,1,0,6,4,2,6,0,0,12,6,8,0,3,5,0,1,0,9,0,0,8,1,1,3,4,5,13,0,0,8,2,17,
		4,3,1,1,9,6,0,0,0,2,1,3,2,22,1,9,11,1,13,1,3,12,0,5,9,2,0,6,12,5,3,12,4,1,2,16,1,1,4,6,5,3,0,6,13,15,5,12,8,14,0,0,6,15,3,6,0,18,8,1,6,14,1,
		5,4,12,24,3,13,12,10,24,0,0,0,1,0,1,1,2,9,10,2,2,0,0,3,3,1,0,3,8,0,3,2,4,4,1,6,11,10,14,6,15,3,4,15,1,0,0,5,2,2,0,0,1,6,5,5,6,0,3,6,5,0,0,1,0,
		11,2,2,8,4,7,0,10,0,1,2,17,19,3,0,2,5,0,6,2,4,4,6,1,1,11,2,0,3,1,2,1,2,10,7,6,3,16,0,8,24,0,0,3,1,1,3,0,1,6,0,0,0,2,0,1,5,15,0,1,0,0,2,11,19,
		1,4,19,7,6,5,1,0,0,0,0,5,1,0,1,9,0,0,5,0,2,0,1,0,3,0,11,3,0,2,0,0,0,0,0,9,3,6,4,12,0,14,0,0,29,10,8,0,14,37,13,0,31,16,19,0,8,30,1,20,8,3,48,
		21,1,0,12,0,10,44,34,42,54,11,18,82,0,2,1,2,12,1,0,6,2,17,2,12,7,0,7,17,4,2,6,24,23,8,23,39,2,16,23,1,0,5,1,2,15,14,5,6,2,11,0,8,6,2,2,2,14,
		20,4,15,3,4,11,10,10,2,5,2,1,30,2,1,0,0,22,5,5,0,3,1,5,4,1,0,0,2,2,21,1,5,1,2,16,2,1,3,4,0,8,4,0,0,5,14,11,2,16,1,13,1,7,0,22,15,3,1,22,7,14,
		22,19,11,24,18,46,10,20,64,45,3,2,0,4,5,0,1,4,25,1,0,0,2,10,0,0,0,1,0,1,2,0,0,9,1,2,0,0,0,2,5,2,1,1,5,5,8,1,1,1,5,1,4,9,1,3,0,1,0,1,1,2,0,0,
		2,0,1,8,22,8,1,0,0,0,0,4,2,1,0,9,8,5,0,9,1,30,24,2,6,4,39,0,14,5,16,6,26,179,0,2,1,1,0,0,0,5,2,9,6,0,2,5,16,7,5,1,1,0,2,4,4,7,15,13,14,0,0,
		3,0,1,0,0,0,2,1,6,4,5,1,4,9,0,3,1,8,0,0,10,5,0,43,0,2,6,8,4,0,2,0,0,9,6,0,9,3,1,6,20,14,6,1,4,0,7,2,3,0,2,0,5,0,3,1,0,3,9,7,0,3,4,0,4,9,1,6,0,
		9,0,0,2,3,10,9,28,3,6,2,4,1,2,32,4,1,18,2,0,3,1,5,30,10,0,2,2,2,0,7,9,8,11,10,11,7,2,13,7,5,10,0,3,40,2,0,1,6,12,0,4,5,1,5,11,11,21,4,8,3,7,
		8,8,33,5,23,0,0,19,8,8,2,3,0,6,1,1,1,5,1,27,4,2,5,0,3,5,6,3,1,0,3,1,12,5,3,3,2,0,7,7,2,1,0,4,0,1,1,2,0,10,10,6,2,5,9,7,5,15,15,21,6,11,5,20,
		4,3,5,5,2,5,0,2,1,0,1,7,28,0,9,0,5,12,5,5,18,30,0,12,3,3,21,16,25,32,9,3,14,11,24,5,66,9,1,2,0,5,9,1,5,1,8,0,8,3,3,0,1,15,1,4,8,1,2,7,0,7,2,
		8,3,7,5,3,7,10,2,1,0,0,2,25,0,6,4,0,10,0,4,2,4,1,12,5,38,4,0,4,1,10,5,9,4,0,14,4,2,5,18,20,21,1,3,0,5,0,7,0,3,7,1,3,1,1,8,1,0,0,0,3,2,5,2,11,
		6,0,13,1,3,9,1,12,0,16,6,2,1,0,2,1,12,6,13,11,2,0,28,1,7,8,14,13,8,13,0,2,0,5,4,8,10,2,37,42,19,6,6,7,4,14,11,18,14,80,7,6,0,4,72,12,36,27,
		7,7,0,14,17,19,164,27,0,5,10,7,3,13,6,14,0,2,2,5,3,0,6,13,0,0,10,29,0,4,0,3,13,0,3,1,6,51,1,5,28,2,0,8,0,20,2,4,0,25,2,10,13,10,0,16,4,0,1,0,
		2,1,7,0,1,8,11,0,0,1,2,7,2,23,11,6,6,4,16,2,2,2,0,22,9,3,3,5,2,0,15,16,21,2,9,20,15,15,5,3,9,1,0,0,1,7,7,5,4,2,2,2,38,24,14,0,0,15,5,6,24,14,
		5,5,11,0,21,12,0,3,8,4,11,1,8,0,11,27,7,2,4,9,21,59,0,1,39,3,60,62,3,0,12,11,0,3,30,11,0,13,88,4,15,5,28,13,1,4,48,17,17,4,28,32,46,0,16,0,
		18,11,1,8,6,38,11,2,6,11,38,2,0,45,3,11,2,7,8,4,30,14,17,2,1,1,65,18,12,16,4,2,45,123,12,56,33,1,4,3,4,7,0,0,0,3,2,0,16,4,2,4,2,0,7,4,5,2,26,
		2,25,6,11,6,1,16,2,6,17,77,15,3,35,0,1,0,5,1,0,38,16,6,3,12,3,3,3,0,9,3,1,3,5,2,9,0,18,0,25,1,3,32,1,72,46,6,2,7,1,3,14,17,0,28,1,40,13,0,20,
		15,40,6,38,24,12,43,1,1,9,0,12,6,0,6,2,4,19,3,7,1,48,0,9,5,0,5,6,9,6,10,15,2,11,19,3,9,2,0,1,10,1,27,8,1,3,6,1,14,0,26,0,27,16,3,4,9,6,2,23,
		9,10,5,25,2,1,6,1,1,48,15,9,15,14,3,4,26,60,29,13,37,21,1,6,4,0,2,11,22,23,16,16,2,2,1,3,0,5,1,6,4,0,0,4,0,0,8,3,0,2,5,0,7,1,7,3,13,2,4,10,
		3,0,2,31,0,18,3,0,12,10,4,1,0,7,5,7,0,5,4,12,2,22,10,4,2,15,2,8,9,0,23,2,197,51,3,1,1,4,13,4,3,21,4,19,3,10,5,40,0,4,1,1,10,4,1,27,34,7,21,
		2,17,2,9,6,4,2,3,0,4,2,7,8,2,5,1,15,21,3,4,4,2,2,17,22,1,5,22,4,26,7,0,32,1,11,42,15,4,1,2,5,0,19,3,1,8,6,0,10,1,9,2,13,30,8,2,24,17,19,1,4,
		4,25,13,0,10,16,11,39,18,8,5,30,82,1,6,8,18,77,11,13,20,75,11,112,78,33,3,0,0,60,17,84,9,1,1,12,30,10,49,5,32,158,178,5,5,6,3,3,1,3,1,4,7,6,
		19,31,21,0,2,9,5,6,27,4,9,8,1,76,18,12,1,4,0,3,3,6,3,12,2,8,30,16,2,25,1,5,5,4,3,0,6,10,2,3,1,0,5,1,19,3,0,8,1,5,2,6,0,0,0,19,1,2,0,5,1,2,5,
		1,3,7,0,4,12,7,3,10,22,0,9,5,1,0,2,20,1,1,3,23,30,3,9,9,1,4,191,14,3,15,6,8,50,0,1,0,0,4,0,0,1,0,2,4,2,0,2,3,0,2,0,2,2,8,7,0,1,1,1,3,3,17,11,
		91,1,9,3,2,13,4,24,15,41,3,13,3,1,20,4,125,29,30,1,0,4,12,2,21,4,5,5,19,11,0,13,11,86,2,18,0,7,1,8,8,2,2,22,1,2,6,5,2,0,1,2,8,0,2,0,5,2,1,0,
		2,10,2,0,5,9,2,1,2,0,1,0,4,0,0,10,2,5,3,0,6,1,0,1,4,4,33,3,13,17,3,18,6,4,7,1,5,78,0,4,1,13,7,1,8,1,0,35,27,15,3,0,0,0,1,11,5,41,38,15,22,6,
		14,14,2,1,11,6,20,63,5,8,27,7,11,2,2,40,58,23,50,54,56,293,8,8,1,5,1,14,0,1,12,37,89,8,8,8,2,10,6,0,0,0,4,5,2,1,0,1,1,2,7,0,3,3,0,4,6,0,3,2,
		19,3,8,0,0,0,4,4,16,0,4,1,5,1,3,0,3,4,6,2,17,10,10,31,6,4,3,6,10,126,7,3,2,2,0,9,0,0,5,20,13,0,15,0,6,0,2,5,8,64,50,3,2,12,2,9,0,0,11,8,20,
		109,2,18,23,0,0,9,61,3,0,28,41,77,27,19,17,81,5,2,14,5,83,57,252,14,154,263,14,20,8,13,6,57,39,38,
	};
	static ImWchar base_ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
	};
	static bool full_ranges_unpacked = false;
	static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(offsets_from_0x4E00) * 2 + 1];
	if (!full_ranges_unpacked)
	{
		// Unpack
		int codepoint = 0x4e00;
		memcpy(full_ranges, base_ranges, sizeof(base_ranges));
		ImWchar* dst = full_ranges + IM_ARRAYSIZE(base_ranges);;
		for (int n = 0; n < IM_ARRAYSIZE(offsets_from_0x4E00); n++, dst += 2)
			dst[0] = dst[1] = (ImWchar)(codepoint += (offsets_from_0x4E00[n] + 1));
		dst[0] = 0;
		full_ranges_unpacked = true;
	}
	return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesCyrillic()
{
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0,
	};
	return &ranges[0];
}

//-----------------------------------------------------------------------------
// ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
	Scale = 1.0f;
	FallbackChar = (ImWchar)'?';
	Clear();
}

ImFont::~ImFont()
{
	// Invalidate active font so that the user gets a clear crash instead of a dangling pointer.
	// If you want to delete fonts you need to do it between Render() and NewFrame().
	// FIXME-CLEANUP
	/*
	ImGuiContext& g = *GImGui;
	if (g.Font == this)
	g.Font = NULL;
	*/
	Clear();
}

void    ImFont::Clear()
{
	FontSize = 0.0f;
	DisplayOffset = ImVec2(0.0f, 1.0f);
	Glyphs.clear();
	IndexXAdvance.clear();
	IndexLookup.clear();
	FallbackGlyph = NULL;
	FallbackXAdvance = 0.0f;
	ConfigDataCount = 0;
	ConfigData = NULL;
	ContainerAtlas = NULL;
	Ascent = Descent = 0.0f;
	//MetricsTotalSurface = 0;
}

void ImFont::BuildLookupTable()
{
	int max_codepoint = 0;
	for (int i = 0; i != Glyphs.Size; i++)
		max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

	IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
	IndexXAdvance.clear();
	IndexLookup.clear();
	GrowIndex(max_codepoint + 1);
	for (int i = 0; i < Glyphs.Size; i++)
	{
		int codepoint = (int)Glyphs[i].Codepoint;
		IndexXAdvance[codepoint] = Glyphs[i].XAdvance;
		IndexLookup[codepoint] = (unsigned short)i;
	}

	// Create a glyph to handle TAB
	// FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
	if (FindGlyph((unsigned short)' '))
	{
		if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
			Glyphs.resize(Glyphs.Size + 1);
		ImFont::Glyph& tab_glyph = Glyphs.back();
		tab_glyph = *FindGlyph((unsigned short)' ');
		tab_glyph.Codepoint = '\t';
		tab_glyph.XAdvance *= 4;
		IndexXAdvance[(int)tab_glyph.Codepoint] = (float)tab_glyph.XAdvance;
		IndexLookup[(int)tab_glyph.Codepoint] = (unsigned short)(Glyphs.Size - 1);
	}

	FallbackGlyph = NULL;
	FallbackGlyph = FindGlyph(FallbackChar);
	FallbackXAdvance = FallbackGlyph ? FallbackGlyph->XAdvance : 0.0f;
	for (int i = 0; i < max_codepoint + 1; i++)
		if (IndexXAdvance[i] < 0.0f)
			IndexXAdvance[i] = FallbackXAdvance;
}

void ImFont::SetFallbackChar(ImWchar c)
{
	FallbackChar = c;
	BuildLookupTable();
}

void ImFont::GrowIndex(int new_size)
{
	IM_ASSERT(IndexXAdvance.Size == IndexLookup.Size);
	int old_size = IndexLookup.Size;
	if (new_size <= old_size)
		return;
	IndexXAdvance.resize(new_size);
	IndexLookup.resize(new_size);
	for (int i = old_size; i < new_size; i++)
	{
		IndexXAdvance[i] = -1.0f;
		IndexLookup[i] = (unsigned short)-1;
	}
}

void ImFont::AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst)
{
	IM_ASSERT(IndexLookup.Size > 0);    // Currently this can only be called AFTER the font has been built, aka after calling ImFontAtlas::GetTexDataAs*() function.
	int index_size = IndexLookup.Size;

	if (dst < index_size && IndexLookup.Data[dst] == (unsigned short)-1 && !overwrite_dst) // 'dst' already exists
		return;
	if (src >= index_size && dst >= index_size) // both 'dst' and 'src' don't exist -> no-op
		return;

	GrowIndex(dst + 1);
	IndexLookup[dst] = (src < index_size) ? IndexLookup.Data[src] : (unsigned short)-1;
	IndexXAdvance[dst] = (src < index_size) ? IndexXAdvance.Data[src] : 1.0f;
}

const ImFont::Glyph* ImFont::FindGlyph(unsigned short c) const
{
	if (c < IndexLookup.Size)
	{
		const unsigned short i = IndexLookup[c];
		if (i != (unsigned short)-1)
			return &Glyphs.Data[i];
	}
	return FallbackGlyph;
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
	// Simple word-wrapping for English, not full-featured. Please submit failing cases!
	// FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

	// For references, possible wrap point marked with ^
	//  "aaa bbb, ccc,ddd. eee   fff. ggg!"
	//      ^    ^    ^   ^   ^__    ^    ^

	// List of hardcoded separators: .,;!?'"

	// Skip extra blanks after a line returns (that includes not counting them in width computation)
	// e.g. "Hello    world" --> "Hello" "World"

	// Cut words that cannot possibly fit within one line.
	// e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

	float line_width = 0.0f;
	float word_width = 0.0f;
	float blank_width = 0.0f;

	const char* word_end = text;
	const char* prev_word_end = NULL;
	bool inside_word = true;

	const char* s = text;
	while (s < text_end)
	{
		unsigned int c = (unsigned int)*s;
		const char* next_s;
		if (c < 0x80)
			next_s = s + 1;
		else
			next_s = s + ImTextCharFromUtf8(&c, s, text_end);
		if (c == 0)
			break;

		if (c < 32)
		{
			if (c == '\n')
			{
				line_width = word_width = blank_width = 0.0f;
				inside_word = true;
				s = next_s;
				continue;
			}
			if (c == '\r')
			{
				s = next_s;
				continue;
			}
		}

		const float char_width = ((int)c < IndexXAdvance.Size ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
		if (ImCharIsSpace(c))
		{
			if (inside_word)
			{
				line_width += blank_width;
				blank_width = 0.0f;
				word_end = s;
			}
			blank_width += char_width;
			inside_word = false;
		}
		else
		{
			word_width += char_width;
			if (inside_word)
			{
				word_end = next_s;
			}
			else
			{
				prev_word_end = word_end;
				line_width += word_width + blank_width;
				word_width = blank_width = 0.0f;
			}

			// Allow wrapping after punctuation.
			inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
		}

		// We ignore blank width at the end of the line (they can be skipped)
		if (line_width + word_width >= wrap_width)
		{
			// Words that cannot possibly fit within an entire line will be cut anywhere.
			if (word_width < wrap_width)
				s = prev_word_end ? prev_word_end : word_end;
			break;
		}

		s = next_s;
	}

	return s;
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
	if (!text_end)
		text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

	const float line_height = size;
	const float scale = size / FontSize;

	ImVec2 text_size = ImVec2(0, 0);
	float line_width = 0.0f;

	const bool word_wrap_enabled = (wrap_width > 0.0f);
	const char* word_wrap_eol = NULL;

	const char* s = text_begin;
	while (s < text_end)
	{
		if (word_wrap_enabled)
		{
			// Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
			if (!word_wrap_eol)
			{
				word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
				if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
					word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
			}

			if (s >= word_wrap_eol)
			{
				if (text_size.x < line_width)
					text_size.x = line_width;
				text_size.y += line_height;
				line_width = 0.0f;
				word_wrap_eol = NULL;

				// Wrapping skips upcoming blanks
				while (s < text_end)
				{
					const char c = *s;
					if (ImCharIsSpace(c)) { s++; }
					else if (c == '\n') { s++; break; }
					else { break; }
				}
				continue;
			}
		}

		// Decode and advance source
		const char* prev_s = s;
		unsigned int c = (unsigned int)*s;
		if (c < 0x80)
		{
			s += 1;
		}
		else
		{
			s += ImTextCharFromUtf8(&c, s, text_end);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32)
		{
			if (c == '\n')
			{
				text_size.x = ImMax(text_size.x, line_width);
				text_size.y += line_height;
				line_width = 0.0f;
				continue;
			}
			if (c == '\r')
				continue;
		}

	//	const float char_width = ((int)c < IndexXAdvance.Size ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
		const float char_width = ((int)c < IndexXAdvance.Size) ? IndexXAdvance[(int)c] * scale : FallbackXAdvance * scale;
		if (line_width + char_width >= max_width)
		{
			s = prev_s;
			break;
		}

		line_width += char_width;
	}

	if (text_size.x < line_width)
		text_size.x = line_width;

	if (line_width > 0 || text_size.y == 0.0f)
		text_size.y += line_height;

	if (remaining)
		*remaining = s;

	return text_size;
}

void ImFont::RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, unsigned short c) const
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // Match behavior of RenderText(), those 4 codepoints are hard-coded.
		return;
	if (const Glyph* glyph = FindGlyph(c))
	{
		float scale = (size >= 0.0f) ? (size / FontSize) : 1.0f;
		pos.x = (float)(int)pos.x + DisplayOffset.x;
		pos.y = (float)(int)pos.y + DisplayOffset.y;
		ImVec2 pos_tl(pos.x + glyph->X0 * scale, pos.y + glyph->Y0 * scale);
		ImVec2 pos_br(pos.x + glyph->X1 * scale, pos.y + glyph->Y1 * scale);
		draw_list->PrimReserve(6, 4);
		draw_list->PrimRectUV(pos_tl, pos_br, ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V1), col);
	}
}

void ImFont::RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width, bool cpu_fine_clip, bool scaled) const
{
	if (!text_end)
		text_end = text_begin + strlen(text_begin); // ImGui functions generally already provides a valid text_end, so this is merely to handle direct calls.

	// Align to be pixel perfect
	pos.x = (float)(int)pos.x + (scaled ? 2.f : DisplayOffset.x);
	pos.y = (float)(int)pos.y + (scaled ? 2.f : DisplayOffset.y);
	float x = pos.x;
	float y = pos.y;
	if (y > clip_rect.w)
		return;

	const float scale = size / FontSize;
	const float line_height = FontSize * scale;
	const bool word_wrap_enabled = (wrap_width > 0.0f);
	const char* word_wrap_eol = NULL;

	// Skip non-visible lines
	const char* s = text_begin;
	if (!word_wrap_enabled && y + line_height < clip_rect.y)
		while (s < text_end && *s != '\n')  // Fast-forward to next line
			s++;

	// Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
	const int vtx_count_max = (int)(text_end - s) * 4;
	const int idx_count_max = (int)(text_end - s) * 6;
	const int idx_expected_size = draw_list->IdxBuffer.Size + idx_count_max;
	draw_list->PrimReserve(idx_count_max, vtx_count_max);

	ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
	ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
	unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

	while (s < text_end)
	{
		if (word_wrap_enabled)
		{
			// Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
			if (!word_wrap_eol)
			{
				word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
				if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
					word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
			}

			if (s >= word_wrap_eol)
			{
				x = pos.x;
				y += line_height;
				word_wrap_eol = NULL;

				// Wrapping skips upcoming blanks
				while (s < text_end)
				{
					const char c = *s;
					if (ImCharIsSpace(c)) { s++; }
					else if (c == '\n') { s++; break; }
					else { break; }
				}
				continue;
			}
		}

		// Decode and advance source
		unsigned int c = (unsigned int)*s;
		if (c < 0x80)
		{
			s += 1;
		}
		else
		{
			s += ImTextCharFromUtf8(&c, s, text_end);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32)
		{
			if (c == '\n')
			{
				x = pos.x;
				y += line_height;

				if (y > clip_rect.w)
					break;
				if (!word_wrap_enabled && y + line_height < clip_rect.y)
					while (s < text_end && *s != '\n')  // Fast-forward to next line
						s++;
				continue;
			}
			if (c == '\r')
				continue;
		}

		float char_width = 0.0f;
		if (const Glyph* glyph = FindGlyph((unsigned short)c))
		{
			char_width = glyph->XAdvance * scale;

			// Arbitrarily assume that both space and tabs are empty glyphs as an optimization
			if (c != ' ' && c != '\t')
			{
				// We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
				float x1 = x + glyph->X0 * scale;
				float x2 = x + glyph->X1 * scale;
				float y1 = y + glyph->Y0 * scale;
				float y2 = y + glyph->Y1 * scale;
				if (x1 <= clip_rect.z && x2 >= clip_rect.x)
				{
					// Render a character
					float u1 = glyph->U0;
					float v1 = glyph->V0;
					float u2 = glyph->U1;
					float v2 = glyph->V1;

					// CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
					if (cpu_fine_clip)
					{
						if (x1 < clip_rect.x)
						{
							u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
							x1 = clip_rect.x;
						}
						if (y1 < clip_rect.y)
						{
							v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
							y1 = clip_rect.y;
						}
						if (x2 > clip_rect.z)
						{
							u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
							x2 = clip_rect.z;
						}
						if (y2 > clip_rect.w)
						{
							v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
							y2 = clip_rect.w;
						}
						if (y1 >= y2)
						{
							x += char_width;
							continue;
						}
					}

					// We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
					// Inlined here:
					{
						idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx + 1); idx_write[2] = (ImDrawIdx)(vtx_current_idx + 2);
						idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx + 2); idx_write[5] = (ImDrawIdx)(vtx_current_idx + 3);
						vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
						vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
						vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
						vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
						vtx_write += 4;
						vtx_current_idx += 4;
						idx_write += 6;
					}
				}
			}
		}

		x += char_width;
	}

	// Give back unused vertices
	draw_list->VtxBuffer.resize((int)(vtx_write - draw_list->VtxBuffer.Data));
	draw_list->IdxBuffer.resize((int)(idx_write - draw_list->IdxBuffer.Data));
	draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.Size);
	draw_list->_VtxWritePtr = vtx_write;
	draw_list->_IdxWritePtr = idx_write;
	draw_list->_VtxCurrentIdx = (unsigned int)draw_list->VtxBuffer.Size;
}

//-----------------------------------------------------------------------------
// DEFAULT FONT DATA
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array.
// Use the program in extra_fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(unsigned char *input)
{
	return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier, *stb__barrier2, *stb__barrier3, *stb__barrier4;
static unsigned char *stb__dout;
static void stb__match(unsigned char *data, unsigned int length)
{
	// INVERSE of memmove... write each byte before copying the next...
	IM_ASSERT(stb__dout + length <= stb__barrier);
	if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
	if (data < stb__barrier4) { stb__dout = stb__barrier + 1; return; }
	while (length--) *stb__dout++ = *data++;
}

static void stb__lit(unsigned char *data, unsigned int length)
{
	IM_ASSERT(stb__dout + length <= stb__barrier);
	if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
	if (data < stb__barrier2) { stb__dout = stb__barrier + 1; return; }
	memcpy(stb__dout, data, length);
	stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static unsigned char *stb_decompress_token(unsigned char *i)
{
	if (*i >= 0x20) { // use fewer if's for cases that expand small
		if (*i >= 0x80)       stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
		else if (*i >= 0x40)  stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
		else /* *i >= 0x20 */ stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
	}
	else { // more ifs for cases that expand large, since overhead is amortized
		if (*i >= 0x18)       stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
		else if (*i >= 0x10)  stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
		else if (*i >= 0x08)  stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
		else if (*i == 0x07)  stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
		else if (*i == 0x06)  stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
		else if (*i == 0x04)  stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
	}
	return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
	const unsigned long ADLER_MOD = 65521;
	unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
	unsigned long blocklen, i;

	blocklen = buflen % 5552;
	while (buflen) {
		for (i = 0; i + 7 < blocklen; i += 8) {
			s1 += buffer[0], s2 += s1;
			s1 += buffer[1], s2 += s1;
			s1 += buffer[2], s2 += s1;
			s1 += buffer[3], s2 += s1;
			s1 += buffer[4], s2 += s1;
			s1 += buffer[5], s2 += s1;
			s1 += buffer[6], s2 += s1;
			s1 += buffer[7], s2 += s1;

			buffer += 8;
		}

		for (; i < blocklen; ++i)
			s1 += *buffer++, s2 += s1;

		s1 %= ADLER_MOD, s2 %= ADLER_MOD;
		buflen -= blocklen;
		blocklen = 5552;
	}
	return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length)
{
	unsigned int olen;
	if (stb__in4(0) != 0x57bC0000) return 0;
	if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
	olen = stb_decompress_length(i);
	stb__barrier2 = i;
	stb__barrier3 = i + length;
	stb__barrier = output + olen;
	stb__barrier4 = output;
	i += 16;

	stb__dout = output;
	for (;;) {
		unsigned char *old_i = i;
		i = stb_decompress_token(i);
		if (i == old_i) {
			if (*i == 0x05 && i[1] == 0xfa) {
				IM_ASSERT(stb__dout == output + olen);
				if (stb__dout != output + olen) return 0;
				if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
					return 0;
				return olen;
			}
			else {
				IM_ASSERT(0); /* NOTREACHED */
				return 0;
			}
		}
		IM_ASSERT(stb__dout <= output + olen);
		if (stb__dout > output + olen)
			return 0;
	}
}


static const char MyFont_compressed_data_base85[39000 + 1] =
"7])#######n-J$l'/###I),##c'ChLYqH##7HJf=w4Xf]3->>#VLE/1b]n42JT97?v6YY#MW.?6b%lj;taY)F<@uu#qPUV$6PV=B(9YpNCvmo%sci21B2XGHavq7#v2NP&OX+41$Jk7D"
"m^e#]v;jl&0d3r0^`(*HUYI%#9C/2'2GF'#+-0%J4;7;$=Cc97QXxI_L>K0FxIUVaex.YcMxrx+Z,d<B_`7Zb6v(Yl;M]&#TT$=(W0(3$ETG-MJ]%KDBDA/cHer:##%*&#4i0_EBaR7M"
"9&F5#U8BkFLtlvEj=@Q`vX++#2ko(M>e#3FqMu+$1#=$Mpb`pL$BNDFLoXQrZ3A%MUv-tL@M,:GnrC;:.k[/#2di,#-9_(G.(Zl*`S.7#Nr8vL>7K$.fQYfLG^*V7B>hA#D*]+9tOqe8"
"q3n0#JCw(<eXZV$7)1G`A]`w'`5LF%ev$8#1&ffLaeVV$d,p*%w3[0#?ae--keFp8K]IoLO]:v-uFj%v73-_#^MHP-'XVR8UE31#G/>>#)4h'#0D$o%+Vl##)#5>#-YGs-c@=gL^'3$#"
"W`R+MBXPgLV*E$#1*+Q'gcC_&ahjl&6T03(-5T=uiJ^e$f-KM';j^/)e8Ke$jE,/(;,dg)089xtnY^e$oacf(@A;d*iDKe$s#DG):5`D+%]4EtS2`c):m%#,lMKe$$K@D*>Yw],I=0$-"
"5;X@t]fw%+C+0Z-pYKe$D)_e$.,X]+R'dW.8>=%tfC9>,HXc8/tfKe$I8_e$7`pu,WT@60;Ax_sowPV-K$.m0P9[21(SD_&@=28.Tdaj1>D]Cs1P_e$EXio.Y#9g2&)Le$IqIP/Sm]G3"
"v.Wgr**fl/N?#&4)2Le$PBFM0YG1a4+8Le$X`Le$&_:>5DJ&cr5jBJ1^lH#6.ALe$_x_e$c/$,2U5nr60GLe$aGZc2a7jV7fLAs78.E_&lc;D3Y`JP84SLe$g:`e$k(s%4o6uP96YLe$"
"u@S]4^4(/:8`Le$lI`e$t[4>5tdQ/;:fLe$&uku5b_Zc;smM,<BLE_&':LV6s)/d<>rLe$rUMe$:18A=RVI.qbaHS7wMF&>A%Me$xn`e$9'*58j^ku>C+Me$7?al8$pgY?)/?v?KhE_&"
"BZAM9n2HS@G7Me$@sx.:)GD8A.]rSAOtE_&K8Yf:r]%2BKCMe$/@ae$JS:G;7FO2CMIMe$Rlq(<v1XfC6OK/DU0F_&S1R`<6b,gDQUMe$5LNe$NY5DEacmOo8XN]=:0D)FT_Me$_q/>>"
":6i`Fjmfsn?*KY>):.>GWhMe$fB,;?>Z*#HI>9?Hff6onH^cr?C,9vH[tMe$D*be$p#DS@R(msIiiqRnQ;%5AFMYSJKc1pJfaF_&#W[lAQCIQKllU7nZo<MBK%72LP:eMLjmF_&,5t.C"
"Tej/Moo:rm1Qbe$1PTfCY$B,NhBNe$5i5GDSnfcNe?3?mlwPcD=c+AOkKNe$<:2DEW<(&Pcv6BPtrY:muTi%F]d6#QoWNe$EnI]F]jZYQhMjvQwu>ul(3+>Gb;jVRsdNe$c/ce$ONbuG"
"q7GTS$#$Yl1gBVHe]45TjrbPT'QG_&X,$8InFh2U'&_=lJGce$^GZoIs[?/V%'Oe$b`;PJstDgV*)CxkOVce$g%s1Kx3scW)3Oe$k=SiK$X4EX,)(]kKLo.LvE4&Y,<Oe$reOfLvKX]Y"
"+0h#Z/,c@kT*1GM%tgYZ0HOe$&&de$&Fh(N4pDW[2/G%k^^H`N(?28]-T`S]:5H_&/$*AO35x5^52,`jg;axO-mel^2,=2_>AH_&8WAYP6VBj_85gCjiLde$=sx:Q;lpf`<mOe$A5YrQ"
"5`>GaY1^gixCu7Rj2Y%b?vOe$H]UoR9.V`bDhe&c=80ci+x6PS>Ue]cC,Pe$Q:n1T>[3>dCqaYdKiH_&]UNiTr1j7eG8Pe$D+ee$[q/JULZ>8fI>Pe$d3g+Vv[FlfKd:5gQ%I_&eNGcV"
"KvqlgMJPe$J7Re$N.$JhGD4fhIvC`WOD3/iPSPe$p8%AXOJWfiTY*3hPG@]X)erCjS]Pe$w`w=YSoo(k_R(EkLGS.hY%XuYX@(&lWiPe$Ykee$+A9VZh<[#mOJ8igcXp7[[bHYmawvum"
"bUI_&4uPo[gW8WnRMsLgl62P]a9&8ofNSSofbI_&=Ri1^j#Y5pUPW1gF<fe$BnIi^o812qd7Qe$F0+J_i,UiqO,MTf'?Ff_=7pFrg@Qe$MW'G`mPm+sx4&HsZSwOf0s^(arx%)tkLQe$"
"safe$W8?`a+uX&u^V[4f9Pv@bwOX]uoXQe$xpfe$J5YY#Rre+MYW/%#.=.6/>r###[Fb(N6M#<-HKi.M7I(aN8,JH#r2IcrnL-W$Aeap%QWG3'bJ.L(r=ke),1Q(+<$8A,LmtY-]`Zs."
"mRA60'F(O179eh2G,K+4Wu1D5C2<)#p6b>7**HW8:s.q9Jfk3;ZXQL<kK8f=%?u(?W*uoL#ib6^e0K9bROH/fiE8dg'KCBi7>*[jG1^XkZ?I4n/Xl;#47&##hN(gA=G0B[%@e%M)&]o@"
"0]j-$)l>igH^+pL:<l`3Pa;)^Jb)woRU'oD[dDon&?H>o5xN(4QFgj]^7.__vEkw`oPt@b+?G8#9Qo3+D%o5#oI:;$ihI_&Htf(#G;G##]Kb&#/Oi,#pf60#^[*9#GsM<#glo=##'2G#"
"g<eo#mZn<$/<FE$_u@=-V5:?-%l&#.cle+MOKG&#wo-A-bP&;//VL7#VT8(Mq>b<#NmkOMfERR$<_w<$)bSJ-Hj[IM9:NiMb.[=lWS#]kqx8;-U>T(/gG+##@qr-$Gk#ipY'>U):u6c;"
"g9I'l3fGxkC:$7l`t65#2V5lLi$[lL+1x(#XAg;-=6T;-W6T;-da/,M'vb1#]ZPs-i[B%M1YjfL1L</1r>/=#e;&=#c<M,#J6_S-:reU.2Ur,#rbYs-_ILpL%JG8.wW>PA,]%pA.VWh#"
",.<I$$'QV[+f[PB0cjh#%CqH#,]U7#Tih7#Xu$8#31d3#_1@8#c=R8#:@2O#fKDO#iN;4#U/j<#)MFI#w->>#2q%P#cvSE#)EfP#nfw#M7af#MO^pP#XaCxLjo2R#pdiO#$-AP#52uQ#"
"86)=-4NE/12d=Q#1Nf5#LN=.#2t?a.PZO.#u$T,Ml?Z.#Bi8*#x9VG#A7YY#VeOU%T,GuuoN]._E(W0;5/Ka</JM>#p;xQ#bd^6#>-d6#9;WPAhWZ6#;5Y>-r3GZ[imb$'Jv/kLJY$[-"
"Bj8&$gcVZ[9uXH=ns`D-LZ$Y:#UgP9r/;9C,f&KMCZ^40F2Q52G&Q]=mINe$m_t`-3I.g+>RtMC='^lLh=5,MLskkLVB-f?ri7,Eu8.F%`vN/2%&IW%m7W0;sKN5/v2cC?_6rd+]P[^?"
"Kpb>-*=)u<1&8+9hpo`?F6@v$4vC']DEqk*/Ba;%+?Nv$e3S>-lwWB-j.[mLl.OJO]=BoL+U@5/@ISUO+O3uLPht?/i+<Z-d9b(Ne8Z6#G8'F.b&u59n(@g<%40W7Zm@iLUpP?.aAxiL"
"j^3v8/ils%FmtC?B$kF.v?k'#*IZ$$-MYY#4]>>#&D(v#C<t87=Y`O#3mb.#r&U'#Mam^7d=Xw$8Umc*2&)'#r%B+*k?,3'q0L7'`^As$ne^>,%u#?,9s]I*BWZJ(p$P:&C9I$$M&-?$"
"Z`'nf7.II$&`X&#lbFX(Ul'nfAWo6*4a?T.e>``3*6qB#9:k=._HF:.uiWI)*AWD#N.OF3)vsI3hK]P/6WhV%@KhR/'K8f3Uu_f:iCXi(s8)f+b@9t%Ok:=6UM?V7im':%gc+.)dB'/+"
",*#A#i;mW.M)D:/n&8k(F8hbEq,FL)Tf3'4Jbk33[PUV$C.$##2p`V6MGQEnr`3>50[qu,WiSX%.#E[$bP7&4@cb[5s8'w#SDNfL,Hj>7pmMhL6FUv-Q6FA#d+O$TweD=Q)4M8.iK@lL"
"w&<M-nj4wMiYv2$&@v_+`L.W$.C-G*nnouc>QO2(I%x],Ch4qAGR+/`?<^V.D7[xM5P'Jh64PM(5EJE+24&*+-Qpr.#W#r.]XO',5E&JhgvFk=`@DvL]Pq'#E<bxu8SXe#+@O&#o>cX7"
"d4$##JX*=$]r`w$Uwd>#C,Na*EJ%q/HUDv#7EW/WNe,87,c'nf5C*$uj`I&49,B+4C]R_#xH:a#A<(C&omqc)jJ@d)F^'a45J%HDfftL3Ik7l'WcsE*sst.+CB>01@q8r.1G^k(URRs$"
"4#Vh1xp/b2+o=m',(ci(obKK2;&&12bZ4,4cY$8/@$g30cr###EBbA#`t9f#J6>##ra3v#-=#12fg4:.(^4o;)YioINn7j1eUkFr<^DA+.N[s&mh0-MwMAs$jT/j0TZqK2$JH1M8xjBO"
"28V-MK)3KrZsEH*nC=M(K6NA5^lw.2h41<3+dKZ-9[Mt-HWSI*J#-Au`J<x-p;w6NEt[.Mpq/%#CSY##2AYY#K[;q1C=S_#BsMF35Hrc)1hqdM=n9a#4c'kLT#iU8mA67&D8uB,o;]>6"
"E87_#48Yr/G6E@,rW]:H,B(u$hO#H3%/5##6/vN#Jd39#.**/MFg4X$bfjA#&:n;-,(-&%2cQP/ZE>V/k/5J*XjOoLSWG>-cqZ(+oG#^,%DxI2J&I?.c]Ls7$.UE#jtl3+L_l3+tQCS0"
"UH+Z-`Hr0,$T&1,4p^P1e6f],)]gb*OOn<$HB0>$34Qc*L7@<$(U;78n'MjV]BFg$D.,Q'lbI#1PHF:.9EsI3)`OF384#$-xo<<B)Z^iPr1xTCLRK#8,=[2LgL)/1*>I30s:bF3f#@g1"
"Ls]e-9wV<#72wiLm<MT7,Duu#-.pK29Fk=.=x,@q,Zbe-jQXrH]uFe-F@,B$-oQS%:0?(N:o%g>M^c)#r&niL+e][#MI></2f1?#jp3mL@C`#$DB$.2W9B;6)*IP8wQ`SAu)mJ*oXtx+"
"<[+&#f[DM0eq$9.7lc>#.N0j0d$Hp00BmD-/1eq$2F/[#&4`[,>J))3IuPK)`5qB#dO]g15Hrc)Q^ma*NLcj+DU:HMA;Lr.Jm[%%%@`w,bQOt$7pv5/6_/F*mpf],FHK[u69_41iZ;e3"
"7<X#-8WK;.SjUk';ite)PH%eMl/>`Wh^WV$$h(?%2G5##Z$/j0*k/I3J.OF36%jt1;CjBucJQaM:R.j#1e0'#csj[,Q9s$#OMP##SvPV%b9t=$SO[^M8b8H9EcmDcK@#)1Q6FA#Lov[-"
"8H8f3[l]B#Qu8r.-k2#)Mbak(Qlm,*ER.@#Bx]<5&xXKlc&tq*+lOJ26=cm'5U_J)#%:?Q9BDXI[bV*4ZU,#vAbimL[*rP0G%Qj),uu,*I-=p%XX6p.M@H##K1M;$3`'nfC'2874j,d."
"/?EE4Wt%?&k0.rVowAw6BTD61AwAr.1[[&9`=cI<Um7l'U`sE*Sj0V+XY?QS.6f$?3bP$-]2Z8/3^KZ-W&jhL%Yb6/Q8CC,3.[Y#cmX<U*G)_Sub/2'K1Qj)A1.s$5M5##BXNT%::R8%"
"BXA2'>dS/2M<h-%Xddh2]02N(<#NF3s^,V/'B(8ISDM>i43>JQbu+DW?w=gEdF/s7:(.W-+=tE@8PPV-9=A%#L1RS%M.B9rr'fs$4]G##w(Od=xL$e1*6qB#7(4]-cOr_,^bt<qkAw/1"
";cjU.x#*ru@)7F*M<Pj'Nu,eo38*S&DA$P1?[4_uMfgV.:;mW.lEU,342.^M^1pg#7wK'#`9`F*G[2$#Qcqr$nOk&+^4P;-313p%sEs^fOf1$u&Xsd3BsMF326MG)#w8391bgDlk4*D#"
"+9x'A'P$3)Jr:v#9v]$P7]jH*s>lO0q`2Q0+BEXCjB?jDgq<Y-nUTN(I/%I2S4n[-uk6I$5dc'&*c3T%:kC;65R/2'j5###7@*T%iArK26V5g)8jdC#$L77le]C;6ng:*M-Y5;-Jrl&6"
"37,H3HfZc2e(+)#NR$29`<x+2qds20o?@o'5D<,)+/###3^0j0^Ks58bK9v-BUjp.%0,x6lmAaYkI?x6/=DX-BSAQ_Tu0l%O,m]#P7f31/<Sa+m%<H*<h330/,wR&Lj8b*fAIB$anu58"
"LQm%-Lk>x>D6ci*lgJA,RC?;-:$R,*:Ilm'9vcG*(u0^ueY9?-GjniLH>1?-=s5s.&YCp7>_`=.`sJ%#J<,/(ZNfs$M<K2'HDoY,rd/n8S@bB,uwWD#0-+Z6]Z`<h1XTK1j>.E*qP^F."
"hvpA=eaK)c=8vJ.D=lGMWH1g23=@X-*Z%hE,p%i;*(uM(X4lDc@R8(#E<bxuq_bI#)Nc##3@dU7A@@<$?FET%9rHH+`s?L28@b=.LSEF3IjPF%SH,9/Jpt-$#BxF4%3'd4fW8c$]gJ`E"
"(C%v#AE(,)w47s$5(.<$T%o+MuBh0MntHg)0t`b71sG'61No0#c47M#NJ#G4DG4;--j's$-`qr$o'Ka#Www+2/;9u.Av4c4gu$s$>#]S?9'tvPkKQfLB3a+VCFu+DAXA']@Kgs$VI;'6"
"VNqK2XMi=.*?k-$FqPmo43GA4dkI^9YnC(AX;aEG/'SP/l&L:%&lwrm+m720.^,&'x=-GMURF&#kgC;6l6.5/MOi,+(GKj(Ik%W$R:U8.xKI5/]Yn`*T7Z5'sF5m0Brh29(DHg)rbI79"
"UeB#$f[es@068E+_P3HbtZV-*);a4'nMw9002*S&*>h(WiIi$-Wm,s./Bb#-:j>8/N;fQKJ4]_ujM[OMA>le<KYb+i18r%4#2s80R:UB+iG:m&a'^Q&cQ=x#X`uY#H3=F+/egM'GX`;$"
"UkH##pHiGbNq=n0/;49#1R;bP[vdh2liCv-4Fb<U&tFA#KDDHOZH8-;klC*4m>:Z-x5MG)[&LnLpH@KW0GT`+8aAn/iwYc*br&5'V87Z$3tt8n[1hXuM5.&::8+h<ew%s$f$XFjCbb^,"
"&4v_+Au]T&D9Upne[ixnx;3k1%l9j%t2YY#79<)#=@%%#2mlp%34[8%8oLZ#&hj>7`_k[$*mUD3wSqR/p,]+4)WkL)eHf@#]w'dH/iY7*LS[D#)>cY#K34cK<&US771Is$2r+DNr=T;I"
"ri-/(wj*20JBc&#d_@%#Pvv%+Kd:K(g3kP&/`_;$,8JK<7u+8732A+Mn@k(&%c*o9Xg<HX&ml]#Gi@n0/?]a+(h,t/$Dm?GR@iM%d3oR(=vgPS.6x?\?jFV_,/eOM1T'b6/8;U%k=#6X$"
"RN/.*iF?>#ka;3tW`7qV13`c))MV$#65,##4xCZ#i2U'#<n*J-T[re$xAYt-GWo>:6D&sRSCGD%i(*4:k]aJ2lsap7w6^VIPmt7'Zrsx+2u%%#GQ$)*Kv6H)3]c>#w1:9p:l(l/kSna*"
"PE`BskWJW-`lk<LmK2gLo/v1%l<QF:bfkA#kFU`.7qVV76d^W$TqR3kUSlY#a6/j0jLiKl,-eh2'wZ)4f3>)30[+,2.1G;_rdf0iWeoBu1e:J:lxWw9t2W]+%)d##/;5##mtmW_QcCv#"
"gwWB-:@,w.6$vM(1<=[9%<$_u[/^DIH<vE7)X'H290B?.=h;AF$.>$-DrPj)L_3=$@T^O03xQS%N?'>$]bTDNkju</r<Hg)0;wJ;TB1B#M%80GgUJg1iaZr.j`ja*e+H>#Rqg08MF,Ra"
"lb/_J*5LS.&v>##DeaT%fL+gLi;S8%4l1$#@>Cs-r:8K8X/0Z-2^FL#bZtO(WIx=]1p[m#[m4Y#As?S#QMIFMuRG&#s30sLL`?##'*bv#0Duu#;4Ar%%8I,?A?]58koTN()-`v%gXbgL"
"<qB)M115c29v?*nfmpm/9=LG7c%xL(@]eB20v,N#.,M%O3R_J)'eV*4fPp;-`]-&/.@dU7.N14b#h]M26ED,)P`i=.6%p-)r,eta'x*)?E:$C#R,%+Mb5WZuf`jj2/Vj)#&.HU7.&###"
"7f:Z#i*3t_EK4mLx9xiLie1N(C4^1?du:?ueL:@-rqif54X2V7Otap%>=IW$6rq;$5@R<$SN#,2dK4mLG1x9.*o`*IWuJL(m+pdGQS=KEpBD;6$Ce+`58p5T&3TiTU&Od4q1%5Jx=cs%"
"2aw8't>E#$s.AU;2iFGMgA%5Jl10?8d2B$.RW9%MsJUiu.nd.%TrGL31cx`&(MuXZN_%<-$jM^%<8;QCX</H;R6=^,<P`I;:9lA#IX.74EeDV7a/@D*6735&?_ET%1PG##^TD#nl?0h;"
"O7Im0WF.r8<sOg11e'8I/?]a+J$kr(TI7W$VS6F<N(<H*)Xho*c;QAugho^+?;a0(j@4J2M3#s8gB.&cs<RkFY=QV-C'P&#n*]f1W8Vg(j`h^#7GcY#OId&%Ga,u-/o269D0NaGQ4r_,"
"@Br7[068E+Qq%;%K8Xx5)bOa,QnT%8*HYb%b$4U.7`jl0c^'5)^ZXT.^p+X7RCf;%,C+9%Y^gi'Ntiv#L>SY,QQFP9_TF&uC'287U>Tj2j=xj1l(9f35CI>#5r8r.'<`V9Y$ph;cE+X$"
"6X*)+l3lC41cN=ct$b)uBVZo/ip2UMik&o87Y9/E9pBJ1DrPj).c#k(>O@s$oT3m/P)n`*JYbV-^]R'=e@O^,*@_Y,k[gW&(h-**T>R#89g%?5DgPr`0)e7&0_Qg)m5.;%WKqEWO;jd*"
")OlC4KL;@,2=e9.OM?@-f37c*T2M>$ep0R/Zr;=.cL)-4bO#e#jMc##9TKw#%n-'#3o1?#,S/Q_rs*$u<Olh2:H7g)V$nO(DZ'`aPg%j't/neqZ-UdkUN0l##rn%#:&vT%Q&v/(r':hL"
"_15)%@.N,;DKtM(r_R'&Y8b%%kZ`0CcZk9%gt8r.?YZr.0v,N#fCd>8^]C_&A'aDN&l=h#o(V$#*).j'2k3D<Okv5/hCke)+W8f38Vg/).5*h>*<P>#ik/)Ij&#5&8uh;$9Yl>#ucc.8"
"*c0^#tR(905`gT7ain)*[&8_89v^NkB7gfL`Ir8.n?7_8)cB)NB+=F3ul)`##V^C-u=58.X#tc##]kA#xE)a-qv?b@m:@W$=esp%(u`l&Q<#n&2AV=c$XwOS2,oA#Q<8F-=cO_.CR3T%"
"K^t042nh/)R.6J*)$_oI(3u_?o>13<%%Xw0o@Yrupd$#kJv;+Nn:SqL?(n##+N#9%9u_v#w2's$AEN/M2J+F3O/H_@1QB&IHM,Am470quTQZ*NJB^nL1ov##jZS-HTYc>#t3i/:@aV]-"
"o4t'4:]E%$S,:[eFlM^icH7]k=^2G)NeYl$DvFu#.@q@%vV&eZsVTI*X3*_tZu#Q#[#>4MnH9YcnT;29-(qWhSiCv#MG?9/,3=1)LO5gLR[/#2t+EP0?NX(L9%`q^A*Nf:uN4PJMok?["
"Wgdh2SdWb.DJ,G;'xQ.%ev$8#7f/KMn_;v#OJK/2)Sc<%>e/k#;Gmb:kWB,t8LoqLIAD'#g#t[,MQT2'i<*#%5.r;$a&-?$7I[<$d4,87*+#J(W/eh2hmii%-)]L(3V^[%m^D.3Z^0W$"
"Ipa[,p58DE7A?w-[Kt/)nLKq%SrfB+dB=9%iu%m*pN96&u&p@#-8*x-[7.[-=op[,.=R=-rKFG2W/0.*T5KE+=@@XI*7:>,B`p2)WQ,V%o^$iLY,1f)SOH*RZu$h*]GI^$&`BbG')m]#"
"dsUO'cDQJ(hND.32nBSI^M@Q/^F2+*D*cJ(hBac>vME-*w+Y/-#;oI2-:Cj(vIsd*:i(?ua]%b47YiF4Zk^]+3dw8'@1r?#:0Yj'Gg?)*ONf;-A^/c$QV2Z67DoIkk>GA#SGqE44<JP'"
"J/***Ft;>M3gf)*TbPU&wLhq[,fx1(nwl)4@hC)4L59:Dke%kk-/pu,*A)$#r5a9DZr(Z#n6LhL,ok4%Z'$cMWTR2.MLTfLDHb`>i5(4F[NZV9PC&;8ARN-*p+HA3.53DuUj5:9>>ie*"
"8k=;R<oH98WkE>H2<'22EuPj)%Km?$ZBU,)A_IW$OB0m&LlB8.2'Ap7t91@I$(,Q'Nx^GM8ZR_#;nR,3V$1T&,UYg(&4iTN$_/,TBhF@4IZ]k9aNtP&SfSBHvS?7&QX/.29Od6snlII2"
"+b)H*rciD+6=pp&MG..2)Y2U8R/(<-4tG;6X@18.+Av##1;5##R3BU%MKYJ(H*fw#sn^3(HN[Q%g2=1)4Jp>Ikk5g)K:hNgQHWE*npP$.mJ;Z>FK3.*>Z_d2IiH[->Qm(NE&;B75@&5J"
"jA020EuPj)nv,?$N[oi'9.2?#GhnS%G;S>,<>_W-0TfoUV_Hf:c^G8JvPZU6t>lO0l>#o(N7%W$xT1>?3ZFB,q>)Q0scHC#t^Ow4FjJu.9=:b*Ws&/--4aZuRCCj(=/:7/PMu;-643U-"
"1`Z*2HH_,)CRes$Bw8Q&iUOgLl:MhL/18n$q#DE#[p6l115%&4_GPD%5jZlJ7A?w-=*_]IZ_`Z#CffA4_t?S#bRgr.Z`<9#E9F&#H_Hg#TYYM9+tVw$2lCv#4Sc##5+R8%:-7^4D-eh2"
"Fo0N(NTIg)O^LD?$4#;6VQ1R:]^)#vdSd&MNqW$#=eDV7Ya1,)9@35&<F%%#tRffLnB'nf:H3m0rteG*bK(VK5-;hL&,)6?v)+^,j?k50(Rfc54hXf(3)[7&c49g2RYrE@bOe?TBMb5;"
"AV2T/]O5gLm)B+4Eow`ZIIQ-?s(#4fR`tAur=IA;?v@T.kI529T4fc)Re.H+ALj5&FQgU%G_ET%=C@w#?+rv#2Juu#[lGd$Rg`)lRib4%FCuM(:Y`S%@,m]#;(6298JZ<.P#R`QTw2+*"
"rrNS7SquM(IxYoRBWZ&HUIFR-`;C`.<kMV7CYfF-S:fnMM%$l$=HI<8Cde/&JOpBA]QT9?jjr,MAk3g&SZ;uL_kJ%#Tm@#/rV4Z,'WC0c$nK2'JbJ)+NkIW-(T8w%7wL.&dB:a#Jl/k$"
"PbLgLN#I]OFa?Q/+;3Q0_2*E*+cVd.lGDv>j$Zc*=A=M(O53**YK)+*irNlX^a[g)exj&4]?[[dwlDd*/W^V.aQNX-xI#C+jCYQs:JsD#o'.8#M3&W7f>[`*w6Vs%i#qTVDfpx7%YR_#"
"*1*<-9;3j$L[=j0D@lO0D@[`?+<ut$L,#^>8KZh-d6/UMvUHg#,^af2]X2=-:'NL)&w9FIb+nEIXhsEIou>>#Y]]B+C.2?#D_nS%DvVA+LVc;-'SJZ$>vBn8@BHO(ZgD6.s*+9%LhJ&G"
"1:6gL97_fb^tGga:U9,PML,m'b]5EYHYkW#Qn:$#,3bZ#9@@s$DHD2%S?qK2k.j=.IA*F3SH:a#l?UYRXSC[,a0uW2kLUR#b9NH*h[<^-kJaE[WUel/E'>&#ZXjl&E0k=$]RR<$L(MZ#"
";AYY#-O3r$H*VfFav6l1t3vr-c?>89Y_,g)@_Ap.m]WI)q8k,4%78iDiLt.)G_eJV-6hJ)gkU[,06-k'e9=Q^G@x1(>d1I2OQ-q9elIm0NFFR0,ucC+lbHF+6.i-QEF)s#r:r$#Kc[n&"
"-A###7%2?#>*YJ(@oh;$VoGp7qh@3kw)p<A3=P)4E[)P(+gIA%oMOv6uvbt$b;.=IPw]21xlHC#td@&,n^,F@o8ZlJ+duO(92t5(.lQS%7YC;$?CR<$k4'p7v`hKa>DUv-DX)P(v(rJ3"
"rE.>JY_`Z#l6P40dxr=cg(p$,6hFG25B6g2#[T9V<'niLY%buLZ^)w$xZc#%9x_;$gi*87Xjeh2S-N?#8DeC#vxSZAs+DK#VetI#x>%/N:[@DEf.Hq&,<%<&G_ET%cR')NKl6MMU^5A#"
"rvKF?kteqAh'#AJfuG@9j,wA#q(69KZb^iB$VUT&'wCZ%BmD:Mso#?%6W_:%-xbF3Xt@->0h@cG(d8q9a)<YcutkukP@v_Me?EY5KV]'8[X/2'XqO3bVtH-b^MY[0mCh._hFcZBAwqnL"
"b>9P#'5d%ddN1I$?6h34l?(,)scY##(&###Z-@0u5O/g:wgUq8ws?%k7QOC#?p7_u1Vq-6R-;,#/Rg2#uepT7.;>##vl2T%59eh2*o0N(FB.2$wZh7#EJ4;-jm]rH4h%v#Cpv%+vlc>#"
"2JY>#77w8%Q]'h3[Kcn0fg4:.Fax9.8:&g>#oKB#I/4e##.h+MAQq'#8H)s#`f[%#tU[Z7bq8Q&Y]%T&KeDv#B+H>#nJ[`*c[>c*v)vf(ZP)W%s9-L,/l^3(POWfL%3#,2<NMx$6(*<-"
"9&>4%m_j=.w8?E&e+Nx-gc``3E;cl5)Wv5/D-s@,M6Y/(Hg'?-?uc,iEfu7.NI6_+C]&Q*PJ*b35_`PK?mRfL6]?$v,WD;64XZi9Ie&%#UWuN'kr8'+V3t=$Y&?V%cmf%#E5,##b-r@,"
"ns0U%7-b)<mS5UV0^4dl*A'nfL8pJ2c6ea$+&V]=A1T;.2ld8.N.OF3</dK)b&p%SbQOt$5HPS.sJ-cBJ#9;-D[Eb$UCekC-#b*b+Wro]n7kvL3@]qL^o7e**AWs/v==i(_u[h(=$#N'"
"Hg:O':+)Z#@7d>#NW^m&n%F**YDRD*?R)r7+pEwTK,KkL7M:x$vY^2(uXY8.4wm)4e.@p.2n3Q/vqVbR#w@+4$_j>7.Qj8:nJ3sLC11a>JLg^-u23?m#FOh8w#w/;r>uu#W_-eQffDR#"
"nA%%#02<)#,IuF3i?<$6`,qi'_rM@$j]H_#2?D8&=b*x#mqR>oJe=n0SqUmM;0RK)MVvJ;Vn1I3'uI8%o]d8/wHeF4@3c'&:hq>,,]WF3=2xA5`4+k:-R`A5`1oj:jW8E+.fw8/kwZ(+"
"-`RW.G5)M:5>dc*ejEN,f`8]c?US%-?M4U0?O8`,Q$d<.9vLv-<i@W.B&#jL_l8X)&H(v'?<4^,[NtfM>Q1U#T7CPMY,l)#Dl<k'KN1&$`6``N8C5qAoZp:`Mc7C#BsMF3X[A795S[A,"
">wBT%It_D4Vcrm9xq5)4Wcim9`0Ed*&>.W.cUhF*&A%<.P[uX?V6dnAK1hj%En2)+nTB6&+`vm9r_gc3Wl%n9qU^c3O)-T8&7dD4.NF/M3t&V8,8=>d8mLN(*7#70NwR%#EJ4;-Nv18@"
"[]V-dm,t5(OLn<$Op#?$Q4@<$=qE=$v,l%=g7r)HUq8['p?>g+qG,#-g-*D+;%Ld2mx>b+XeEu-0UvG*w^<u-1X2d*WfOv,QrXI)iMY>-Q%ue)[6iJ)E+C+*@o4.)O2-F%qL$##-lv7<"
"fWCB#'jxi'i&F_]Q>>8.n985&O,%)*(DD$#C0cJ(^%#b[->,b[lvcn0e>``3Nn?g)^^D.3cN=:.s^v)4)g^I*KW+T8bV-B6xe$O2=HlK1kt7[#FRO'Qoc<%.V.9q/e8=qMTO6##C*DG#"
"h6B2#l^''#px_7/k[^>,@[[8%RF6p.g++a*QWT2'hG]6EG*bi0c1C%/o&l]#wCeP)gF?x6i.2.3x?7f3W5mk0VE/[#_v@+4Y#PS(kCbt?^J0j(fxEh1ZQiK,eQ.AOok%##s&niL`l2[$"
"w@ac)s&^f1B_S%#Or=Z,OQK>$0`_;$,lQS%U[w<$6(`v#.o^3(cZCP8.Uge=_[$6%FU)t0DMCs$PUFb3Qh%i)ps^6=B]T(N8j_:%8GQ<.Cn/F4BKwL21o'_2R.e[-Bj6g2P8ie5VIQ$?"
"`>uu#p6.B4lwSE#>l9'#J]&*#TgWa4q=W'#TBfl/nnlr-IORs$40<s-tT<Q/0PkY5p(aG)m1958U@(kLtp+87c]c8.5uV.1Ypk/:/Imv$/9ki0+9/?&(xI`,^E6T:Y_'QA-J5b>?&'i)"
"n**iMk,Xs88/U?-uAfT&$WXaE7&@r%7,U?-$KoT&[]@L(ntBR4>U7K)HclY#'$XU.b=pP4gSYZM0H(u$==<p%1&hZ-nCn^45%7W$2K,w7*I@_+=J3t(fs=h:8ucH=g<0<-9Vbe#rTl##"
"K_R%#KxNk'`d^Q&a,@D*=R*9%['/j0rJ1)&,'0<-p+8#%4m#<-qdw6%kLfQ0Q@-W0F$?*=NmtA#2441#Y),##)1?;#v<i$#sFBT7-MY##XZv*=;*#tnlM&90,Fk:mg)5#-kNpJ179ZV#"
"/uC$#(d1m&4&q<1To%9.$cm5/M,%E3/rXF3psC%$ML]i1pZ/W$E>%C#Tia&+$da&+7SX^#Jl0f#@lT0'fFoU/d/&%%eE[WZ=L70t:(^fL&gC:#mMvtL<fd##Zgd,MSmVV$1D>##Z#],M"
"tF5gL7Ups;6waV;5uuCj,DP5#WT2^#q%@A-*ksu..G:;$H-$M5VZrhLr7bF37fq@--?pV-VG7L5mG4;-$9%;ZWGTw^w(ffLT>eh26ctM(DA1VHoH;ku]#ffLmIH>#HQrMMoe3$#@5Dq%"
"2cl>#5uZS%7`l##T.i>7Gl9hL+2Dh$lXj=.6dk-$pKZG#Y@8b>FHViLF1$##w/d^P%v(6,9>)2qF%a'-Nq5)*XXrX'8^Q6WX]g*sw68(#]66P.FKt]#;6*^4ToA*#w<M,#b]eZ7CKal8"
"i<I9:H2O`+jl@SBZ?$<6G$7Q8rwT)$<_'j0-i4C-9aM($v;a]$HSV'#Fo/$p<eGKjX$*$uoP2?--aY8(]pTb?o?VcDf5ftBu#5uDsnX]uruObH*w-oDJkaSAh&&/2enLxI%6YY#nXa.L"
"6i-/1-7&>-8hP]'#/ZiZS$bY?XV,eQua08I)4:>,Ac5N)V3t=$WbxM0?%Vv#ERI[#BMU69c-TK2F5WO'cDQJ(^(.W-fA.`ZgfHC#wi8b*^t]muKN]r.thqW-#JYL<bQOt$iX9?-Hdo_2"
"/eOM1)nw#6NE/.*jOZY#l]_#S:E$##<1B;6G^6x.[9j;$6oCv#U2U'#3^p_%(<$_u%P)m&n&[o#T(fQ-8sib4DL7%#]tcI).f1$#:0(,)))###M'bP&nUiJ)QBAm8#K)B4_Xj=.2.,Q'"
"oZw9.UH:a#>uv9.'Zn#2r,Wd*8>U*%x%m?G]bm_#Zm`*4qs8M#@Cb#-fW7TAMQOa?SaPVHIk&P(:/Xp'R-'U%ERns$SgCk'd?/j0,86W-/sJ2jEG_R/:<UD36$nO(o_`la;mY0?YM/+%"
"Ap_20Z6cIq]4wi'E0&.$w,PsK%l>>PhBV5AbVQZ$4f(Z#&^Yg$A6fG3>KkH3ldfw#IaWFAuS#'%MbSZ$+oZg#%[u##<FmU7<`l##EFiv#/qs`<:GE/2G'K#6T2o?9)9dJ(3p<q0LtNDF"
"j7^/XhEQ*3#N0C,CYs[$H4sl0$h7]FG:'0D%qJr.@nDr7>EIaH9h'#vIeHg#q4i$#AAVq%><lJ(KW:K(A_wS%]*/j0x.i>7G1<:8v)'tq(+,Q')A*F3nj8b*I7f1s02*S&+=D%($Fs?u"
"uU)a,FHK[uS^]aA?V2$/s*Eb$S@%=MO9e##9N'@#47[8%:+.<$-DG##,uVT.2ctM(]INJ1DPM(WliF;6ID-m&b:qv@(Hfu@Ki4p@)GQ&#&w;'%kl5N)N)wd)m?<q&h>ux#`<B:%Y^ml$"
"YN#,2EC*$uV4GI)*AWD#Q&H#>RB7+460x9.^7^V6qI<s%q/`.V^DlpU.<@GG[t7w#BrA/+C2Q$G]A,++K&HA#i-RX+1voWZpdaQ_Ejs?6H'DP8x5(Z-PG[I2X?ZL2c#>p%6PP##$QDW%"
"Xa#j'XDZv$<4M8.T_ri0Ke4/%TIm@..D'nfjr7e%OHF:.kmqc)duT3BSs?_#3tJ7/'G#)H/vQ7&?hu/1n._b*J*,N'?c,'?Kj%P'T`/b*rsT+,jR;?#bjXG4wqWX.rKR(+9=uJ2VP<f/"
"J/ga+$`:`j@Z'q7I]Sg4IE-m#iYu##@&Zt$%/5##<eNT%6rCv#X?nguo=X&u<Olh2gOR4CT62=.Uo`u1_R[P2$3ibraF+GMm_FD<)CEV&Ci5N)5r_Z#>nap%;7@<$2rq;$a%]fL;MZHN"
"gFl(NuQx9.Q^FD<R1):TXWZ_#[hsNMS+?D%=#-R<(M-5/1];$#/9$D-<WT4MkCQ`-Yqkh>[;:bOwS.(#uHqA#-?iM3IqVV7Q.ZY#DRw<$R2ZV%Yo&^+_(D^H=ZnO(N.]TgW851:[Dp;."
"Trx384mC5K&YcL%AXnh#d,n38.5;_$K7$##@_R-H*8pu,mS;f*VNTY$KPA#,iXcv,haT>$S*X=$=/0?.e'Rq$f)r_G(xB2sLLbF3%s(8I/Eo&,Jj4#?]fP#?nDZG8logHQqEnHQ+@O'J"
"G4H/CNRYQsnv$)*a>_,*EKl_J_:h)J6a3jG?NmT%qUi,+a?0>$DFe<$VR3T%dN9>$2VUV$:o5##3`'nfjJ9nBYx50)k&Cu$>tG@H-N3j1*i><-U(;e$Mwr@,i`%N'boWeF]&_CG[L]r."
">`*DWw/t[,_a7=(nF4J2HRY_$TTibrW$TM1_KGjTx&$5J*+.20NLMg*s?Ds@mO-C#`IU8.Xa$d)G=Z>#4@B0%$>M)dsLJL(tB:a#=HWm$kZD.3q..9.(V'v5De&T.+HHm0o1$A)S3XH."
"lF)?#,Vjp@Ao^?-.C&@#DECj(IQm3+[/5##D,>>#Uw$8#t%*)#-xJi1^MC'#QhK_,@B:K(UN:o0Gb@s$sA4fGOSDV?^p?#6k_`*//]NT/f^st&'1,Q')0rhLS.E.3P`50&l#fF4nve>5"
"NCI8%qti:.m.BF*mVed)`2^F.<PW2Kp?n0#7F5e)S1Mv#nC<6JUJF9'D17s$Mm8E+XBj[,bKlK1Ia'$-E49g2Q7JC#w<H,*+Lqf)4f7m0[dB:%q2LU%B5P@MQP6ipA/OZ$.H@W&1Fq>$"
"X/dEP:P4W3<Olh2J02N('JOjf@]4W2IxYoR)13'B0i)_#,'.8#C?8W7j)Vx$Trh;$Z[Vb5e(>o$$]e[-V1x;%@:ut?e=)h)Oa,]#`]o0?Psql/W&bmMs(#4f%7pr-7l#&M2@_l876_M:"
"_ncx-K7m/*^ETu$MakA#8RET%a,[H)r%q'F^6X2(u)X(&8mE.3hJ83%>29f3]*U&$Y@/T%*[D.3(_Aj0]puT)C7IdfR%,M(M'F9%(eDd*DCUB,LMEf/ajck:ih5Y%+ZtY-<Jd<.)?Q?."
"Ye9F@B[E^#qqJfLt=D8.Soou,<A9Q(Qqw;-LEE>&Ihj>7(?(p(=B-:9R]Yw&,/fn:ew*v'`PmJ;5;aE,a=0H#Ib($#@>`6&@Ies$Dhm`E?jv.4s<j>7@=.<-a8]b'3G]u5DiAjV`a>W%"
"dFNP&=.RS%?1%w#5V1v#(P`W-/-fX^I/]3%>%PA>iXKf)BsOP.#oM?#H.]q/T]kI#mFX$6R()235#%8#8qVV7Rtap%>=IW$B[QDE(?N;7rlsWU`Y.n/8W%oNG@gg>@-Yk/IMw:6$`k@O"
"[VdDcJ:?>#*aFT&NLMg*5E5<-`@P?F8J>b@Pad'A_kY)4KRJ(3=m?PJ,U+)?P;(c^Jtj(l#p@32C$%DUikANaHeL]$PR1XBQYP)d<gQ$vE,E`#E2<)#UQed3,c;'#[?45&Cu3V/,io)*"
"?:@s$Qd0#$hh)6/&J.$$PJuu#[?/?&9'1-YAPH1'xCV<-KXnq$fH:a##;oI%4n[W6$l+E*<]%g>RUaH*r8cO0%ZJ0)M77s$Q1A21.Q_ZJm6tg;+NPW-b7Vw%:Wo=-neXg2G%9xXi:%R8"
",olg+;]$##?tx=cV(5;--P;$#a^Nh#o$X5&?h/m&=uY>#UMcGDxbp/)i&l]#g(j/1UD.&4rue)*;`>;-Z@c$%KSZK4'cIC#aE+t9dtvi&92PV-$m5##S?TU%5S:;$==I<$L<xw#1gL-%"
"I8Jp0SO;8.v4b_$m3q6_Y*PF3F'kuAb-Aw#hRE:..^RG#Xo,m&>J:gGA[1j(KsaV$g]Ps-`cpLC(BSX%LFR<$=7.w#?u_v#uQ6IM6:V'#LVco$,Cw-NiF(I;m+NedH))B#tKsi>_EZ7&"
"u`D*eTquM(M<]>P=rpuGVFFD*W*f),%>Bp7Kq7I*CkW5&GBG/(?h/m&=kEiG/i9%OBTNJ%j8?*nXK&=-9LN*IA]V$QP8@OCD69T9Gj.n$,7oEIBJpu,?h4&#FN(,)R.CW-J)3&+a/)%."
")ZvM;UKK/)jIM<-9u;J'cC+j1Eq<:%u:r8%Cs'M:ISNJM[VME+PO:+*5)2M:KHsrR-I*W-=<c?KGi18.SnId+lP7p&X:Y;-V:$9.lt:s-(oCp.O$As$*A'kjO(C0M5`Y)%M?ti0Q:YoN"
"6ImO(O6<b$3RE:.$^Bc.tZr0?fQFf-fF=qMA5c;-tjGD/]Tw$,]XbgL2/SS%B^ScNh*^c$AV]nTQsCER^IrM%tQYxbpZEM0@[]%#`0gi'R<+9%i:B#,[SI)*:@<T%m=Ds-]3.kL'/=,%"
"3`^F*mK9=8q:U%62aA(%@<iJ)hLxw>UAik':AS-)e[C@,i0Y%*[=2?#3@7O(l5PhD=?D9/8PA-)4NlO0T`Zxb)p1'#j`;]$4kHYYAE(,)nsx+2N2]h(piN**V1k=?(1]ibwWxb%=fkpI"
"I]lS/IW$WH`MlDkuhS60*[o0##05Gm5#GSR<,h;-wLjN4o'L'#4pm(#3v_Y7F=Rx6Q%``5&jSDE3<n>6POnGEdtK3(*=49#Yd$J(47K2`,n<j&2/ZaE`sGUV.08l*;;Fu/Omwv?`VS50"
"I$l>-w[l:>$Vc]ojmE:eCTbV11Eu@7j6G?HWM2&&V:8J.f$M`EFQ9H+rVHQ#J*CB#Twc+Do1'`sF$OP&;j&+*^9x+2rVTI*Xc7C#thj=.jUKF*&E>wJbO:V*eO`S%p,7H)[#6R&O;-6)"
"#e9Z$6IaP&1D,?,9r6s$e79f)@/5##kc6K>(M#F#6Xcb#.rn%#EM&`7B3Y]#*ct.)tu7L(cnLw,KXRW$7l:v#kVou,DsV]+kkP?,o]w`*t>)o&o7B+*FnwS%Nc:v#ts/j0f&&+'ol'`("
"U93B2XTsJ22sO^]/tHg)VDl-$hUa8.)vsI3QB.U/ZUfC#E)8q9_W?.<.lf*%2Knh2lW/F%b]p0#qmVG<OdfG3%BXb$xQXrHgNwu#E,W]+xul>#7(.s$6a:i'c/6c2kmx9.7/BqDrCZc>"
"eMRfUdG+I7K(Tv.-WGrm+wdPEB?4:.'L@lL-]6%J*e0^#OQEx$W,f-#Bx+`M,XeU7fC=GM(kn`*]>:T.89eh2[_48MI:+$8/hM`N$jDxX1X[e$S/5##iVw8#Cc]=#9(V$#h`=w$;llY#"
"EkJM'WNUG&Jm<p@>/;9fuhfQ0QooQ0:%x1AdfXvIKg[lDE&`@>E?[VUPN,;HI;$@THn_#.CB+;H)>%2X3Eh.=_)9]8qJ>vIw%>BI]D(d-XCQ8`L2$##wV*R'(kMHI<h2K,CW/HuQI`&K"
"%tWvH/k28.0FYLLlJ4;-Pe%j0aM`p.$pm(#7VIs.iWe],*'u(3F=vu#8ol##uPd]4r]q11E@d>#MX4-t0+I0DR+&L=&95c2Z6nO(_%g^$hB:a#w06l1Fv*'%H0x9.^dx9(oogS8n$Br."
")jPr.dRtX?;<q#?'eZ0%tUKb*@G3k0C*j]Fd(vB,d]pI*ud5s%o(%p%>v$]>ICkM(o<BV%#&830M'jV7d;[`*-VO?gslRfL$,6/(VXw<$-=W7A,RK/)0-hIDi)@[^fnNs-i'U9M;&,mb"
"02*S&0<Sa+P'Pb*fH*L#:`Z*IkF/21>j>D/VdgV.fC1DEgQbA#/F*(.hnA;Il3f8;.90/&.d8>,*D2$#=RET%_mC,)UH+u'$>V'#8>kTJ^u]G3Nxi<-&3ap&jWH;6.[M0(BOac;-dBcJ"
"0cmA#j;xlD.w>wCY+eVU[HK;0nS><Fh[h%.s&niL3ITIUxIgLI=-Xv0o-bV$*[w]HX=Z&#l<)E.FXg`Mwm:WKWU#-4@HqN>cg1rKc4b&#6Z(^o_-55k;MaMJW4I#-GDOVG._dI#UU6##"
"E5YY##w$8#-AMp.D0>8/]S?.O-JM>$NORw#g5s]+sH0>$dM1#$,o^3(.Nx+%`OnB&WRXFPLx1g*U/?9.No@Z$2Ygk(jN&)?v#dW%A9cO0+;3Q0DX%)?`c8oAr8i&lErf:/QtYd3M#DE6"
"+95Q^`CKK4CG?t$sR4edko5N)K))(=B5dg)Uf32(s1%iNTX?\?&(W`+=lFS49QVg;.d=;nNCx]m*3M8K1.XFF*$5P-lu@6g)0)gLCN#9K2,4:j(l_1H-^Gj>KJLeA#WB,x9X,8o80wS&#"
"O-^hX)u/<Tft9s$22;%GZp;^#lPfLC#HUkXCM5;-9CS%#L=no%HAou,GRMZ#s#ZJ(TEBm/1C*$ux_t'4[u[<]xxC?'Z28&%+BIS:`2RC[#`F_$)@XG2=Cb#-Qs7JPBmNL)9)$_F]1,p'"
"IR6W%+LJbI*GofLGS()N%InY%kiiA,fJckM8[kD#BQ*fDcZHZZ4<JP'`Mmq/gjDE'B]od)pk`8(WpEJCN,>>#?Q;4#l&U'#;W,l&$G*c<e?B%>-gD(=re46'9Ub,25wU:@iQD(=#r_s-"
"eZEa*#da&+EJ4;-lXh._Nt78%G?gi'rr_;$;hW9%DP^e*Qhas-3E+f*VK3b*(7&<-(mH=0=$AP#'5d%dI[0(A^TZ`*uh[d*uWhA-K)Z*6`aI%#1f1$#?qVV7Yvhg(K^uN'<7Dv#1P:v#"
"+&+r&Xax9.sYdAQw*v81Gia0?vbJ60@jIl$K4#;6RrA+*(9k-$R7]^-ovCoe/6W,A/[]s-eU.n*$acp.$hO=cH5ANjW2_dm%o$##^8s<#,FmU72>uu#8rU;$3uqV$xt_/2sor)P8dF:."
"i@'kpJjRG#^/Ve*s&cp.S'+,2Kx*L5UE@W&2r$<$GkB15*-eh2V@lJM]K,,27:$d#>BEkLmfcgL[L6;-t^blJK2UP&A]p2);RWT%:@7<$3u?8%Ec`C=`&^<7$06c2)'A+4e)kJM?YMR^"
"F(B][nRu02T;=`EIMw:6iA$6`QvEV7<p_*P@J/oP&V:w2pne2?SsE(=-UN:I^jk,2^uv=c3ufr6O<#&#;t8R/$'X>-sKrS.V:$9.kkuV-?@iS.Lbrr$^1^>,Uk.j0g4d;-g%Yr*.Sv;-"
"?GJF-:HEFlj:1-4-8_Tps975/^lLY*OZI2?0$#S8%KfP^,c'g*T*>T%_&E;6$Z:D3D_A%#VFr$#N%RS%hC0B+wB)`+`B'>$maxw#GAYY#'fmh-$nL7JZ^`C=(m#<.Fl*F3Spf,3;g%/L"
"MrIj'W^+)?d]+8/WYD(=WvjUIFW5W.1R@M9_/(<-]rv=c'9'/1V,`'#-rho.DX*=$X.RS%o/$$$nT0#$h?96C6t_3(Ztr$-#F/[#&Sia/(V'v5<7$jq<JaC*-?2N1P&x%%xY'+E1sdk0"
"s9k]-kqsf(Q>pa+/a&%vJhHg#5'U'#rT`J)]G2g(N_Q##-0$A1uiVW%aE0>$.8r#$'gav%p@Iv7TsM7Wpj@+4tB:a##(]s8o`R[+i>?B=b%xL(x6b*8*W6`,FHK[u4CM%($Fs?us?(E4"
"b/5##G6h`05b%-#;xL$#W$Mb*0FWs-]SNa*5_&9.3A),2RE2f;Xug/)7;pb*'ok,2Eh#4fmH]u5Uisi9D,:%#D$<vH4Dr-?[jw8'DX<T%DL2rIc$C#$CB/wHOHipUu^1L$PT/rIgq0^#"
"ICOV0=Zr,#1Yu##e&wgLl]xS%7L<T%1]:v#+cf0Y^nxr%Nba3=f6Wp>D@^oR9;aD#:Bh@9U@%Ak,ekd*sV5##^4ofLeu(t/[:SC#if*F3(3em1D2weq]S.k#xBO=cqPb?MEaf/&[YBZC"
"g[Zp::0BZJ2a@(hAa0)IF,>>#A&ml<?rc0:(<nX-iK$iLUfZFNu$[CRYq@g#$d7D=V';kB^/Qh./aD;6'[gj>B&<VEq[)]&-$c(Em7i&F8&..Fn6$##XPSSN-Z^DE=0E6i,TSHYsmIc$"
"JJL`$IUgYBj5(iF?EEIP1dZe6N5=b-,_?_+^YC/<&`G[faKbd=EhwM?dBAw##[da$)^5A#:X+_&KIke>0F>=pdZqb?Yt*V(6GND9RqIcG7c[+dAD;[u%qb%F5g6B#C.$##9m4h#($EZN"
"%JiD,o_+/(:G?`a7SP##1`U;$,DlY#,DcY#,PuY#;L>-MuwSfL-IccM.S5W-owbn*u?n0#7eQv$-g9[0PcL;$8X_^#.GcY#h7s9)OY(Z#whg>$M/1[-iRo6*XcCv#0lqV$1MlY#-+1^#"
"T`h@-W+n=.2fLv#D$`>$(+-$$3rh;$bOF,M4O%:.2u$W$LwMU)Rx6s$Tc$1,ZV^-Q=nM^#.]_V$ZhY#$CV*wpV2O0PUqeHZ2=a9`%A=,Mh.j;$6.Is$LN`iL]5pfL;6S>-)`Ys-^FXGM"
"ew%W$PBb3OW]5kXwxRfL@PmY#*7?v$Uw5X1(xK#$((-xL?XhS-p/-&N?.2#QR&@T-&_<3Mk*^fL;5)=-v4T;-a_N^-qO8U)ZI<9%/S1Z#dpdA#$pa>$Yj.@'1Fq>$i7s9).<e,=JEMiL"
"ab=SMB,*q-)rkJaSUmY#nwo&$$4pfL-G5gL(JdY#*76Z$`vf*%V%r;$PYl6<TlLv#w(<p^OCXn-]Ew0#''^e$sf8Z$0^X$0fOGgLC9+;.74Is$qcJv$fpEs78G@36MB;qM%k/I$NV1v#"
"sS9_A>#w0GT^MqM/tQwBVxh;$;/4kOr_.[g0,*=(H0%Z$_+.L,ru+=(Z]#IQ$5x+Mt-gfL5F5gL4FL58kMvv$4uq;$f:KiYB5h<Cq54F@A5(W-)[f2D$N6aM@db0M%.gfLnH)qLRO5l$"
">Cd<%,>>##;]Y##?X#I6+I4thO`U;$#R]J.6x$W$4xIe-&u^>$#o,Z$$P5W-sqOn*+:Z;%DwL#$hhZiMNE>,M34AW$=I]6EV:e8%NbnJaY*a;$7lCv#cZ-[B&>xfLK(8s$hXqlL>7+KM"
"8MYGMn@hLM9O_lLX*JY-VoCn<)YkGMsXlGM:.gfLO-Yc-r'r26XNgB]Yf]#$=mYEn'MXGM&-.e-8QDqMJ(lJaI18rL4MYGM#;+KMp-8s$fO4gLhICp7;vg>$4kcl$^4d;-4b]w7OK'L5"
"T+7W$EidiYWSPn*D,Fv$?rYCo5T]_o+8@[9wK*1#MS1v#:f0Z$5*a>McuV;$:_jT%IOi;-CTkr..]L;$M6pTVwHw0#MYL;$=Vf'&&D=,M$(^fL'@,gL6=eM0::IW$1SuY#`@=,M)+sV$"
"2iqV$cmdp-fc$@'Z:Is$i1Wt(L&q<1Qr$W$FNtJa]KOg$CtDe-X+@s$8+%W$.mIb%KPk;%K0Y[$?[d--Dh6l+]Fn8%bOXGM?X1)N(GjlL?*J8%=AtanOfhV$'fmBf%;xfLP32X-(xiEI"
"?+$<%fRXGMh>dtM.Eo8%LvkZ-/d]m(PeDv#b6`v$m:B58;>vV%,QWh>U1@W$xn#Z$o0-4=2]#W%1dch,Phq63l@c;%GF][905F#$1J'?$_4ffLd'/W$5f#W%8XC'#@ic##7x-W$XI7'M"
"P_vY#_CFGMcaGhLlfXkLeK$T-UtBo-3I-db(L>gLbeDv#]pr>$.%lJa*UY,MxXE^-fGl'&Mt*T.5iU;$6di38sem;%KW7_8Sf.R3?x^;%Y,=s%u:(:)k<pTVY;9U)V=wS%w+>W%%VKp7"
"upJe$Hj+%$3xH8%3u$W$-S1v#5(.W$5]Kv$*S#<-OCArLPN>gL3F5gLR+A>-7K2381>d;%<e%Y-AcXvI'vv*MG4Js$kHKC-(]5<-5if(NsM>gL'p,D-/n]f.:7Is$s<*3M=N];Pa6pfL"
"B:#gLEQ#<-fP<rM*KG,M`(/W$;7ms%/GG##7@#l$J(lJabBSs$:7Rs$pl]GM,C>,M`6Js$ii$@'5XQv$],.kXRr_v#X3Le$QiqV$v91O+&;ofLTQOp%c[`*7]Cn8%6rqV$'0<Z$hrZw'"
"^=n8%2fU;$c@+gL3:AW$f,,F%)M4gL.8x#/@UNp%esJe$-4_>$+4$Z$iLik+^[<T%Rc$1,'JOGM>1(Z])u6E.4uhv#f2N^#/V:v#+fJq;(*g*%'AxfL+MtJ-Dx.7MQM>gL[F@FNUHoY-"
"u$GR**S=gLW3/ZM-9Js$1+lJav*&f.@=Is$*lls-dL+gLf^hS-r/-&NEawr7rP%Co0=m]O$VM6M1hF?-Xu6A-(ho/.e_kGMoe_p0>.%W$@-g6&6ohv#B4:_#74Rs$/fhV$aC4gL/@,gL"
"HOmY#B*@v$4esJaU9f8%0OZ;%>*n;%)cQ-HT4nS%IOi;-Z4e]-.(lJaaTxs$EtY*7ZCn8%f9P'A]Rns$O0b5/9u_;$bUFgL:spg-u`8UD/#Jb%aLn8%FhWp%t)tV$*]Y2V(L>gL_=%'M"
">RcGMk'/W$UZ.Z$HE/2Mq`)Z#8Z8c$N]Cv#75Nq;'eLb%T2n*%F1sEI^I[s$#2uNFY-DgL'$vl$*`tGMlWXp%PBb3O'JOGM^G5gL;wDv#87Rs$5`JcMTOFp%0PafL?'WuMxkL)N*a?T-"
"D1SRM=#L)M[0I&O#M2qL,O<3M^c)Z#,xkJaDlVqLn&-/(>.RS%5il*8iCH?$3rUv#eL4gLLH5gL7(xfM-_4,/97n8%w-l3+W;9U)-Y4gLA;oS%5X%u-a:ofL'1As$%_6L,-g-@'_X3T%"
"OF0k.>O*9%A=uc-74lJaX9JW$oqLs%K8hW.@h&Q&)V>W-dp/I$^Cn8%Dhs5&9f&gL3;]8%&FLk+;tv;%h_=gLaZk5&>wHv$UZeI-?@2@^3x%W$ST#N01S(Z#8@w8%dRO,M`$`1.j[4gL"
"u<jM-F=YG-@+uC.;@n8%u:$be.FZv$iqk,MoaXp%hUbGMwEPGM6&NZ#V?n0#YFw8%3DNfLscPW-n$R<_l1c;%T4:bnA,pCol.,Z$n_c;%Shh^-0Hh)0<R<T%:Fw8%G6fn22PP##H%v##"
"<:n8%6u-W$1oK<hTx_v#@*[;%tL'^#3L$Z$`7cw'BqDe-GukJa)L>gL?UY,MB98W$bL4,MoF5gLSqV;$dUOgLj3AW$99JT&;%Vv#5[rX-4.lJaV6AW$agMM/=1.W$d[OgLxiTK-4nUI&"
"chs5&H6^Q&u-RmLhXlGMgYXp%urSv$(@Lk+xIo*%V4Is$Ti@36G7(?$`@ffLr9(p7dUGq;v+Vn<ouB*eG4sEI7JWt([;4kO?O-L,a3%W%`2d/1E*96&EX<T%DOn8%@9xu-bIF,M)L>gL"
"9w`;$/(lJa.lls-hX4gL0)iIOWv5`-J2)bn>m]CoNJ^_oGI$=(-TPtCVfU;$<R<T%/(lJa_B8W$JH0q%^CofLh&mH08oU;$7.%<$nxZw'v[B#$6_?Z$&cTp7jCQv$tK/q`?33W%i/Ke$"
"9A0p7mRm;%6>$=1^lG*RcnN9%HwY*7__Wp%)cmHHdnE9%juZw'bUET%7+@s$jbtGMW#DHMiU49%UIOn-;:lJa1q:HMTcl,Mus'q%<.,dMl0Js$/Uv;%AaaP]1hfv097@<$8x_v#2f_;$"
"B+Cv$XdT0PQe_-?_[*t$0@#l$jRV'Seb<T%B_Wp%PVFo$`'TQ&Uqq;%2K]'/GMqJa>[_p7mk7p&#+d;%uq`I.CkaT%sNM+nd4c;%jcX8&M*uSp(KmD-qvK)MAwZp-+nd--a#q?9XCn8%"
"IOi;-5O5l$E'Sq&4YY##M1)$#@L<T%G5+Z$Y-DgL,N>gLNb;v#Wi$A-BG<+0?L3T%=I<T%/(2t-j[=gLIN>gL3BAW$=LwS%J9%Z$A-IZ$AhjT%wn,Z$?WcH2FX39%6=*T%C_39%>es5&"
"6I>1%^Uns$p'%T&IZNW%F0g2'_`OkLaUus/EqaT%N<9q%87)=-t'ImLmj'6&(Ag;-S7bN.MNG3'D6[;%Wt$W%M)Y@$<RjP&:Cn8%2lhV$>U*9%3q0[KRl0W%@lb_.DhWT%^#kF.G$BQ&"
"957qi?+,7*,V4gLkePW-#.GR*g6^Q&%]Kp7mL%C8c7>Z$c>c'&kW5n&M2BnW`naT%60BiYX&+W%QU4SM<f5AP>N+9%,/m<CaX)dk_^4T%5x-W$jhb,MsH[uLg@Ss$)=mq7hF)W%QqP-0"
"5fLv#>O<p%/FZxLv-gfL4@,gLP*MP-4:2=-Z9hS-54Dt-jX=gL_stT%OjtJa]*jv#K$vhL:M+T%-rTp7otrW%B=.<$H?#n&Ho1e$iE/W-NBrB8^_<9%KbIs-*i^p7%E;s%S.?p&OtI<-"
"k)EQ.K?gm&qOGv$5Y<36Y4Is$#l#q7e(h#$<RET%Sxg8&A(GG-3[uh%be<T%84)=-8[uh%C<tJa.q:HMpvKQ&4lh-6X@e8%.[Vs%[7xfLao^K-*5pr-gL+gLbvTm&2PafL)F?ANcauGM"
"Eawr7ZLiBoNJ^_oGI$=(-TPtCX';gL[UFT%LTO7'6]Y##Q:2$#C[ap%:1Rs$4.lJa^dbp%fjj-$X*M,M&4J+/@Xap%%4l3+L`O?gfHGN')=Zv$eOxfLqmFT%=:rv#@nJ2'B?Es%2:V9."
"Jb<9%S(_W%GqWT%bx]E[X77W$&VC9&>17W$jX+gL,t9Q&nk=gL_m06&1w%X%I-0q%RN^6&4@J'J`[Np%oqX,Mm;(q%I3p2'L*'6&t6ugL.Hfs$9X`=-KEjlLw(Wv#@:[s$d.^#$eNL$B"
"-fb,M,ibp%o*ugLk_OT%9PF$/K*tp%FA4kO9jxQE@.,7*anap%/X;W%p31-MP0]Y-bW*1#YL*9%IWW8&c7>Z$RJ,U;)_n0#.ib,Mb#_m&*&^+n/otGMdg(HM&K+Z-W+]CorX1<%Jnjp%"
"nc4W%aQ3L#tSg;-.T)E-<r0@/EIw8%4b?q7gF;s%Nww8.@XW5&P-49.Bk&6&KbIs-f=ffLcaYgLr<;X-Sl*wp`77W$Eejp%)l^p7phMs%'U-L,[7iv#G(Yp7kXMs%0(lJa1klgLpZ49%"
"r*(hL;ekp%+`uW%J39q%TWg6&q,*U-gX3B-QE`.MEmkp%%oSq)gB,3'2YPs-hXO,M^sH80;=e8%=I[<$2+lJae#C6&;#t;%l$b-?b$'q%N0&6'L?,3'vS9R*+#CR*Mqn8.KHG3'W-Rs%"
"7T]'/ghap%AL*T%Dkjp%R<XT.WE0U%vb)i-t0R<_^Z<&OaTGgL7@qS-rm9)M[3S>-JUkkNR&@T-PBreM?d[$80`&UDbLw8%U/Hn&bRxfLv$8s$qiA$$;RWp%Cnap%g]^,.`+ffL$q06&"
"B)MP-Ve_q7le%T&GaKS/N-kp%CnAm&P9'Q/?C7<$C$gM'Q^[v$iXFgLWhqp7p-]T&BFe8%3@6q7wT=U&A:@W$Gtj5&I0^m&Xb9O.IqWT%@;Ks%:wqe&04]20Kwsp%RdYN']Xg9rk$OwT"
"h7hBJ]hJ2'/I>1%%rS*@ctsp%5Mo^f-`FgLP32X-2.jEIJUr;-wfSR.M306&FW&T&7bWj0DUns$Z);k'NE53'NmWW%EhNT%f2p*%[%V;$81Is$p>9m'%=Ds%+x#xLcfXT%?\?ZM878/Fe"
"%;`3=^_Np%O4g'&f-06&leDFO&i.nM,dVqL_ui;$4ruS&7rGs%@@HX0HR*9%W8Qn&bRxfL<IS8%?:rv#<UWp%uE*nLcu96&IOi;-Da1*3G'96&BnWT%M-96&Jtjp%u3(hLU3AW$bD=W%"
"kR+gLtZOp%n$PgLTWXp%HS64(DI7<$@nsp%GRRW$Pj1k'P$S<-Wa%jLSKv/(TGU[-k&0I$j$06&Uauj'VH^6&Ptds%MZlj'A?Ns%-OLk+5@_HMfJOV.D[<T%&,TV-'F%36e@e8%?X*X$"
"?=7<$7(7W$9u,W%]?usL8Y20((>58.PWPN'^N<T&s-ugLp_OT%b9hS-LgXZ-oMYb%c3p2'@?&T&+IQa$dUns$Fq&6&PDMBo(J=,Mq(2hLWm06&2k`s%QE>N'2B]8&fO_8&4sLe-:w%?P"
"oVE^OeN>gL]l1HMHhkp%`J#D/94Rs$q-:HM,YPgLt9u9%28###UTs-$]bJ9tKaus'o?)&+M5DX(JqQZV+HqF%.;j`#%_@+MWF?##Zik0Mb9:P1[#MY50l7GVdO5;-tVFX:I_9.$[NLR#"
",I5HMqRA8#9P=.#aYfvLPJntL/pNuL:dn*N>3urL&>pV-IU9R*XUr-$l+aw'Q>l-$PmG_&<o2F%%gp-$.+n-$amo-$h,p-$f&p-$V>2`&RW;uL);n]-5Rt9)J)l-$Kf:5K;4V`bkIdfV"
"Nq*F7jcH_&rU;.$[v0F%G[Kk4C*Xk+$&R;7mQEVnd`OPTJTZ-H]2HSfOPUk+Wj0F%J*o-$L=MJV9Y;/MtomcN:*2)F98%a+4]irenMQfrTlScjn&dERaS]vL##ZoLX%buL^C9vL)JSBN"
"6OYO-7$JY-Q>dw't^$:ML6d9Mq:aw'uSp-$tpCF@1K,F%RBo-$nFg9rB$pEI];H_&qP@Ji1.=2CA1urRswG,MM,/b-GL9R*ew7>dHSYSJC=kS8Nr:5K&<7s._a.R3&;(_]7Ps9)kSKd4"
",-w=P,=?v$D$KVeM,@Mh#4n;-a3S>-ahG<-3p-A-Ik*s71PSSS'@n;-qb>W-J0wJWk629N@RKvL$fu)88@-/V=-I,MaBqS-)8=g$r.Ik=kfH_&HP8_A8In-$orH_&=r2F%-/,.-aE@_8"
"OfQR<2-4m'jAWVRJhA5B:hpfDLYl;-Mn3_$dfxvLGE`hLi(k?%$Zk]>Z/[-HPU3F%(lkA#i4S>-(,Uh$4`4R*]8/vL5#5$Mr3EiMp^R`ERKn21tH<D<=+i;-87r8mE;1pp4%co77^kA#"
"2M)Y$^=2mL@(i%M8eov7trlcEZ,J,Mg&7tLo1/BNIQ+[&@RWw0YfKK3vY.j8&Qm92wlBE8$q9B#2M'^#w=V2%>r@:2^b3?7oLJ'Nx9/,;/dTW-MhE_&_[23)#i1E8+]'^#gRv;-EqJ0%"
"54f>[%r1*8Rs(3;.:%b8ReuoJj[)W-7CkwKTwt-$A<4W-NQLW/8+JV%b@m;-:I]v$8KmY8ZLv92vCm2Mmx+`8ZL'NC5^mIqurZ-Q2[6gL'1#Giu&0W-E>1F%f3A.$+k]w0Q/#3)qRZL-"
"6aCH-Z6T;-j6T;-gWjfLH+p+MbQKvL-Gmt73_bA#9A%RE+U^t8BdxQE-1Y&Za<QD-oObw($oZ&Hs(sb&w[b&ZlR.+8P['^#tR`HNChT@8m^bA#)4eq7*Im92>M:dk;BU1pn=j>R/:5L%"
":%r;-ec%_'mi#<-oF4%('D$<-^Y:I4$),##Y4x8#SY7p&2:ns$I0`$#U7j'M.:F%vam8g1ilr4#'.#'v+<r$#w,X$Mu4i#vOUGs-c+W'M?E$'v[/LhL$:X'M-4=%vVUGs-f=s'MCQ$'v"
"3H.%#7c)'MbmfwuSCg;-,IKC-XpX?-A$)t-[uD'MY$wLMh03MMC?$'v4mw6/KjE9#'FqhLr^8%#KX`=-`>:@-XH`t-Zo;'MWh?lLae8%#c1a'M37F%vn7r]5sHU2:[kAX(ZvF'SZd,>>"
"Cf(^>/<CX(agGxb#gK>?[2_,*YuGq;5F9e?t4+)*G-?JiV,5d3gCQe$JgrOfYG1a4p_Qe$iGhQaAj2>5lwI_&cRW_&+7=fU%T4&>ni_,*n5tKGd#du>]_FD*BIf34ssCG)V7I_&q;X1^"
"L'%Q0rdNe$0Ee:Zb@/s7];$H*;?Bk=JrBP8ptwG*&p`-6QIv.:l83L,`x7qVY<O`<6-K_&0ZIR*+mpW_`/);?JY)d*=rofiL,:-mTURe$h0PX(632]X+O5R*RORe$3[T(W^iw%+paCZ-"
"`cxO]]DYD4[&IZ-bc]4]blhA5TcPe$AS>l]qmM,<xrKe$l1pLgxVbA>(4Re$m(9lfZQ%Q0/IRe$(/Av-6d@Gj>c:_A`uWEe.^48.S-GPgOBwM13URe$^WoW_MDNY5='m;.OHPX(an(eZ"
"Y>`KcuE2L,kmBk=j/hx=LoH_&>?6`WwSk]>-R(<.Gb3SIU3aEeiiu?0w6no.bAJ+i3KK/)8a#K)AMH_&/f@k=-m[`*t%=s.kBCJiOUT;.MU5s.id2p/KhL508s7s.r?W]cf#Am04XRe$"
"mV9F.F#O.hY)^G3keKs.PRXEeM8nx4o4`A5f<K$.[Pd&MxB3mLORFjLNs*'M=qB&v?Cg;-))>G-X=K$.u<mtL#B#OM#ncOMb2<(vnG`t-@axvLK>N(vDCg;-BRx>-.0&F-]/RI/r`''#"
"rd*$MlSlxu7UGs-E+'sL4_'&v1UGs-_8?&M7qB&v;Cg;--YU).g[imL8XOjLo,jxLLQ?'vKCg;-oGg;-h%a..tZXOMtlcOMg,3(vn;xu-`wE$MJ8E(v6UGs-0O2uLKAW(va-/?\?]iKP/"
";fm92.j@D*EQrS/2q05gfl1a4,@Re$>r3'oR;+XCpuH_&'A4_ANfD_&e$/20wGxD=xsEM0T;oP0s[?/V5?oW_L=Re$dU)`ji*C2:L.Cm0xFe?KsP->>6QCX(q:pLg7J+XCCxQe$KoqKG"
"^+$RE>7J_&$G+wp9;Pw9PnJ_&]j3YPX>lD4NPPe$l4%MT[YhA52QOe$_X%]kcIJ888HN21qdZVR$EJ)=0LRe$>EQ%XvJOA>d0.31OTc.LqAPwTNALe$v&(,2I?v+j[YhA52RRe$;<MxX"
"fVdf(wtQe$%t5qVN7(eZ?:J_&3k:oeL'%Q0TbMe$`c+XCXYnx4Vl#K27R(eZZlNY5>'/K2;0$GMdE$'vxK>LMJD_kLkKt'MEE$'v(@gkLU9X'M,iG<-$5kj.4YH(#>aDE-&;kj.5am$v"
"X6)=-+:7I-R<dD-UhG<-Rn,D-ag>5M`u6]-[KU_&#La@k;S(^>#%Re$d,Duc#gK>?/]$d3Mr@5g2C(eZoMPe$%AiCaL'%Q04WOe$/<IuYdLAs7H>Pe$s1d4Sf_xS8XoPe$/nP%Xk6U2:"
"ViPe$7<2]XmNZJ;rd2g;^LI_&Gf>l]rviG<[xPe$)wPX(2uaKc$ZL]=g6BX(p,Uk=XVs-6.Sh7R08C#?S`Pe$6(*PSW6Q?gH1Re$/ItFVrpE'SJ7Re$#(>rdC+0Z-f@Qe$`l?iT`.N;7"
"k'/E4=1hFisTh2:_%W]4(Sv`4OAUYd^$(@0D]Pw9liH_&'Sv`4)Y)a4EsX?-#_Xv-iw(MMenHlLPhN4#:2RA-ICg;-*i`e.(Ckxu6$)t-[ZDuL`B3mLx>+)#V^K%MiTNmLR;+)#iZ0@0"
"V#wRewZU2:XoPe$f%5igrviG<,@Re$grS1gr&8)==S(^>0qJ_&kC8qVr6sKGRTQY5-/&/ieU]881ORe$%VDlSt2J)=/IRe$)X&]k$]AD*oBh2:l-LS7?@V`bqZ63:dqgo7XKqOf6a#K)"
"QYPe$vOExbj`g-6m/pW_)fGP8lbp(k3KK/)PphS8`eGM_o<C2:4w#T86EN-Q7EJ9i`del8Z1$)bb:a;7'1Re$?/'S[X-7REg4Me$asEM9aPUDXG7Me$>s=J:>UBMh6a#K)nXQe$':Y7e"
"dLAs72RRe$Q^?rdf_xS8B-Se$*.xIUrpDg;Y4_f:OxgD=@0]q;r$F_&DB=G;+T`S]Ya>X(D%Re$B]Y1^rpDg;50o(<OWgremBh2:k/9D<GGtc<cd0aX1l*,)'Bsc<'7DGa?8vG*,?Oe$"
"^1-cimBh2:*&q%=L_?)=`3@GaAJV)+%+Re$3*m@XwMF&>4XRe$%_AxkwSk]>o2x)=V?*`j?XQw94JQe$$u]:dmBh2:(,Q]=1nr`=fSI8]l/tKGJ7Re$['DrZ4TgJ)*e7&>g34/Mf@N;7"
"S6<&>KlA5^tq(F.QLRe$g5pI_9&vG*9gOe$q'X_&f^jo.[=4_ASRRe$06N.hmBh2:x1iu>cN<Db5W^/)IAPe$1YxFMhmfwu>gG<-(Lx>-s*d%.S7LPM91moL_NJ#MhmfwuUnA,Mli^5#"
"RSL7#7go_M4NR7v8Kf5#Q'N'MWpp_M,0@7vhZwm/IQv;#Bf$4v4Cg;-1k)M-Weq@-T$)t-fVl)MSWK_M*$.7v1$)t-jj](M2HR7vEZ4Q#iBg;-s],%.9EI#M8<Y$M-9R7v.:LB#]c;<#"
"iuWZMt0l5v_Zwm/K2s<#3N+2v%UGs-O<lwL+af#M5t56#'^,%.rOK[Mbor3vqZ`=-JU]F-MmUH-4hG<-Xs@u-M2ZtLKRR&M_u56#_1r1..hD^M5fn&M`[]#M)-kuLe-b^MY3k^MZ9t^M"
"t?'_M#OC6v5)A>-j/A>-_<)=-YnG<-##Y?-UIg;-X[Gs-'7cwL'w#`Mr&-`Mg,6`M03?`M]8H`M^>Q`MqDZ`M.Kd`MgPm`M@OZDMK5/GVaqsjtZGKl]g#3L,mws@bYeh1^n^?GjAqaYd"
"7bRe$Jo=lf[[$#munQe$vL>L,<2)0v8^n#MeX[&M*$.7v:Cg;-b6@m/6N+2vevO6#9[Gs-RDP)Mk+84v-UGs-F_v&MIqE^M]wN^MM3k^MN9t^M&b_6vg)A>-vfXv-)CuwL1*77vBCg;-"
"s/A>-B_3B-%Tx>-j<)=-e*`5/G3H;#lcw#McAZ6#0-?;#TmUH-WCg;-39aM-swgK-@hG<-CKA/.3Ud&MYIv6#L[Gs-b25)M17u5vdCg;-Y<)=--<-5.;6&_M/OC6v&[`=-QIg;-fa`=-"
"ZnG<-^*)t-8HR#Mqv#`MR'-`M;-6`M[2?`Mo8H`Md>Q`M1Nn7verX?-#Cq[/`8I+r+[4GVfvmA#x0&F-=,Yg.rPC7#D>Y0.+cK%M?Il$M_5O'M6$.7vrZW/1DQv;#-:20vn]U7#A6)=-"
"SCrP-<hG<-Wm@u-ENJ_M*$.7v66)=-8=n]-B:v92BqJulf3BX(J2Iucxb0XCQ&v@b/sEo['6mr[/g@5^#IJ_&:IExk<C.&c/IRe$VFe1TGQYSfgCQe$gF?F.mJNuYP)VPgIAPe$doFL,"
"&l?i^SDRMhOSPe$mX?F.$t0:2u?8@0dJNR*''1:2Y9^_&T[ee$$O8@0]B^_&p=GL,Xhee$f,VX(sFGL,h2VX(h-DXCdW^_&eZ^_&(7@F.fXK_Am>MX(j7l<Mm1i&M@Hf(WI']ab'XiCj"
"rQ3]kB-Se$v[K+rD`Oo[,@Re$3FAul/#xl^p_Qe$:7I%k=LIAc-CRe$GX]_&E.ee$?SX1gXM72hIBSe$,61:2fV;oniKEvm,8VxbW[JX(&bxPpDY4S[:kRe$9@*]k9(2)b@'Se$289ig"
"E?#se.FRe$SKUX(Oq]_&J=ee$vg0:2axjIqdo@Mhgx[ih]f<Ji^oWfiRSs+jf=9GjA-Vcj[+p(ku'6Dk`Ic`kM>e-6KHbq;f,VX(sFGL,F2e:dm9.>m7FMYmmEwum.9CX((7@F.mAVX("
"#V>L,g%P<MK5/GVoOdfV5:lA#xGwM0Yr64vc%.8#c7i*MjIf4vWUGs-Y&#)MG_e&M55/8#[l`a-(#3XCLEGrmiM6Yc>tlfiNpl34L=Re$HNuRn?eNYdG?#seIgK_&I:ee$Rj)GMt2?`M"
"3Tn7vKug%M[qL:#MmX2veUGs--LZ&Mcu%4v0UGs-LdS(Mh7A4v:%q%MVv<<#`Mx>-XUGs-89j'Mcu%4v5UGs-Q,,)Mh7A4vF1-&MOKR;#1F`&0.Vw8#*_^:#JhG<-dh(T.DQv;#Q6)=-"
"RUGs-g^J(MWjK(Mw-59#I<S5#HUd&M8Z[&MkKt'MT3O'M4#59#ej](M0$.7vjCg;-e<)=-b,#O-%*LS-p6)=-x)A>-+F3#.n[m&M(.F'M)w-7vX=1Alg)DmgW11ciAc4L,9Ic&#l1`*M"
"x$Y5vTCg;-JIg;-%sN+.P$a^MM3k^M#:t^Mb?'_M]E0_M$UL6v9A:@-XnG<-Tc%^.)Xg6vtH`t-UXm&MVjg_Mppp_M3w#`M=oK4._#5`Mb2?`M19H`M>?Q`M_DZ`MrJd`MGQm`M8m38v"
"1(kT#2G=;.fV*crtk),W]S%2ht&Rgi-04]k(=$JhrS-/r,=Re$lT[+irK(/ie=RVeLacofvhF/is_9igNsCPg2?)/ixTixlQ8@Mh.5//i3d:1.ZHA_MC__6v(:UcjZg6/iYN8@0Weee$"
"9Px-6ilrOo46jxl:Y//i#_xf.69d7vg6)=-5)S/MvGu9#X'a..iDYDM%=aM-NT]J.;@9U#M=BJi(7[Vn/FRe$`BW(jgjOPp1LRe$NCRe$**u92XWk=lqlNGjidZ3OeG<R*fa8`j-#g]l"
";?^_&X'sCjJ/p@kbtHYmYCu`k8T4]k5)t%lE?#sePVPe$bO(`sEEGSf^D;5gANK_&HUiCjKvqlg/IRe$b+0fqM8w.icx<JiT`/,jMADX(^2Z7nRfScjptP`keXl%l5*<AlvAdD-ma`=-"
"[Ig;-mc#pL_:b<#vxo$MNn4ona'c9MXbRe$:JtLgEEGSfp8JAl;2NR*EB)GM01l5vwZ`=-MIg;-ca`=-*[kR-,Ik3.xgS(M]Dg;-O6)=-Qp,D-hH`t-l>G)MU^9(Mx9.;#hK/(Mc2$)M"
"Rw^(MOp3<#6xQx-xsf(M>p*<#Y=?&Mf.q(MtWZ)MOkw&Mm1<q-0qJ_&bQ^_&mUnQa+_&@0RdGrm/A)vdCQK_&)J*Sn?eNYd^h6#m1&Donk^&Wns;No[hFQe$+ShCj-mel^<1MDbHdK_&"
"Aea=lAqaYd<qRe$,wKxb=+*XCU+P`kkkl%llt1Ale'eumHEojUvlM.q3Lo=c?$Se$RT^:mU+P`kfb1Al[Ohxlk=-/V#X`]ljP+F.^qRe$FE)GM0$.7vphG<-^hG<-M@0,.x0PwL:/q(M"
"WjK(Mr=ZwL96@V#%H`t-0=cwL[^9(M+'.7v?mj1#(#K/#=.<D#?lHK#gP2s#9P&a#(t[d#[$@o#x`B#$0?D0$#sg>$n<LN$P5?Z$rmn`$&:#f$8efx$GU-0%;Ad;%3;6C%PT5N%taET%"
"JhUZ%)wCc%&a#r%Z9WH&au%9&c->B&b);K&,*9Q&0+lU&*agY&gCO.'>Z<t&FpCG';N39'.vEA'^a(G'ekbQ'ma]a':#)k'n>[w'd;.)(CGi0(cGp6(s&@<().xL(oOn%)8Qhr(+<u#)"
"3D,3)Haoa)uB6K)rflN)/p&U)?ahZ)NeD`)C5%d)k(tx)rDs-*hn79*hA1_*7&*P*O(0Y*IRBb*B9$k*.=;<+)*j)+,qY;+ORTB+BQZK+'H4u+v>Hg+L4w<,&&,/,''Z?,q0rK,Mrds,"
"Q:[`,O0Oi,Fwcv,hU:,-EE5V-&-ZC-CFYN-41e[-?Llj-aXc7.Q'>/.WD=:.MnWE.MAQk.s%J].5(Pf./X(4/(9Dw.i3@-/f,46/hp$H/5X:k/(Q%X/b>9f/]Ais/24AI0b%L;0c,@h0"
"V0<X02iid08=&m050pu0,'IH1MUZ81+EUc1b,%P1)F$[1p0/i1%L6w1[,BF2We4/2R%-42:*dG2e4WpL4Yepac0pBu8Avt75FcqS8n]C,3*1.EtTgq]w8p[6c$%rJEtjqfW2)]-UxS@H"
"m6RUomtjLDmSx7']TkINA#(opS>a:S<QoUoLWa%?Q53fjp@);AP&[.jEo7;8$wESKoO^(l]Vuu@)egu[5wD22hMp%d-WG&6^u-)YaR_22[g>8g>xAp9[Awiib6Lp9wQY;]o'%326W5pp"
"M&#03$-p5Ln6@ga*qP,tbtOW8&:rAmqYr)PHEa&m1@-t/G@^aPXRiK3w-BBZU+&_6IUa^dXZGh*]]/hEFXujiGWf32=/;nL_t5*u&#Ce=;XoHX>LZk;a6?bP[/gBmns%4;e9xEc*e$1E"
"mHZ*ubIO_ZDIsR(bQ%%RD>tF#iJ.rBa4:7_-qAl2h8K:TDVG`$wx&cPXd8+lvsMr9f*fIbra;V983#]neVs4D6S+Vg+[j%IG'$j*>B.A[>tR;0n(u1j]m8v/^LtlM'n;JtG8tV9n9$jN"
"^BRPi4fI)?xQmisBw7gFfjP2W$&###F<v.CX.f+MF=6##,e79&%/5##Y3+C&<Ul&#ca''#>t?['qaFoL/+)t-ZFFgL:=`T.+GY##N':W.a+3)#<A`T.7.`$#wA`T.94i$#1B`T.8(V$#"
"BB`T.'hc+#QB`T.W)hB#M@`T.Hu0J#x@`T.BI@@#B#XN0L*=A#G7>##_RFgL%59P#lB`T.8LR[#iA`T.3(rZ#%B`T.GF$g#1B`T.JTB#$O@`T.MaT#$p@`T.JH0#$<A`T.'rI1$d>^gL"
"OE7p.8$&9&*w+pI9-AT&Pr;v>>Yo/1mLS]$o,U.G5E+7DHX5p/xFTD=kW_J;f<ZWB?XhM1);X7Dc5:?-a(mhFO7Fr.S5xmD7*8R*L0=gG%.C,38'rk1^DxkE@2uD59rCVC:1k(Iv@Zw?"
"&kitBs$<gGlRv1Fss_0>JxgM1>2S8/xL+7<@r6U;'chcNqMkfM57<MMQ-$l0pkn+H5-xF-DA(%6cB'UD+X,447jn@-,YD5B6J/>BgTV=B;kl`F1CRSD9X`S-wfh30h/EVCnm`9CVnU/P"
"?GIKDb3Mf5q_[DIw1:%8hv#kC>=+,H<rY1FwZk5Cx[Wu.VWur1g01=1F0=gG*2kaG@.<,N$@iVCP3^-N,vHI5+/G]FcgXoIT1PcDH]4GDcM8;-YqTiB+D-F%N3v1BV_k>-NX.F%;Rn-$"
"KC)F.B58R*K,l-$o7-AF#j+j1JXP8/mZ;,<JNQGE&>ocEBHHp8r%j+Mb_4rLtGG&#YH#mB[h?j1TO7X1Rm>G2OZ8g24xD_&HdJ`E<`W>-lv.#HQD4^GQe`uG=Ww]GwpNe$S*x?0AoGe-"
"Dh-XCFf0L,V[%:;.b<kF%Vh%F?7#dE5W'PPE-:kLQcvh-wjc:VDWjM1iGdKuXfNY5K.aYCwT_-6[1/_JbY2F.3Od$13Y%RaB8#_]#V-AFuk6wp]xju5#YI(8sIlM1p-=bA?e9e?1HKq;"
"*um-$1C9qVa&s.CQwG)FhgF_&D#w9)qcSfCI3UJDPFX>-xLCp75p,j1[EiS8@d_MCURL>H7BC2CAZuPBk^FdO4,;'#-#krLd3v.#e,KkLhT`/#M?<j10K1W]eLp-#.T3rL5F;/#7g$qL"
">X+rL-&H98$&p5F.>DJ1uqsE@-IAVH=VS2rmJG&#95F3.8Eh]PKO4o$iu*L5(Yk&hx)'9pM@?uu^G.%#1c9B#7.>>#]BM^5-Mc##4Yu##8f1$#<rC$#@(V$#D4i$#v-LhLRE9%#PXI%#"
"Te[%#Xqn%#]'+&#a3=&#dRqt3iKb&#mWt&#qd0'#upB'##'U'#Dj/V-+2ik&[pYc232;D37Jr%4;cR]4?%4>5C=ku5GUKV6N-do7SHDP8Wa%29[#]i9`;=J:dSt+;hlTc;l.6D<pFm%="
"t_M]=xw.>>&:fu>*RFV?.k'8@2-_o@&Z$DWf`>c`E'D]O1fj+MOcHfUwD3S[;+`+`+[;ci<lGi^8Sg1^VqTuY,VC;$0o$s$41[S%8I<5&<bsl&@$TM'D<5/(HTlf(LmLG)P/.)*TGe`*"
"X`EA+]x&#,a:^Y,eR>;-ikur-m-VS.qE75/u^nl/#wNM0'90/1+Qgf1/jGG23,))37D``3;]@A4?uwx4C7XY5GO9;6Khpr6O*QS7SB258;Sl@kEH1VdDMSrZ>vViBn_t.CTp;JCbMMrd"
"ku.Sek+f4f^w>igqOb1g:-r+D6j<fh#+$Jhk4b%OLoOcDQ@h%FSL-AF3HJ]FZndxF_6auGcH&;Hggx7Il2u4Jn>:PJsSUlJ(UFcVwl6MK%53JLGb[(a@Yk4]G6p=cYs5Yc7BS%k+bwr$"
",]UV$PDSP8Yg.29^)fi9bAFJ:fY',;jr^c;n4?D<rLv%=veV]=$(8>>(@ou>,XOV?0q08@43ho@8KHPA<d)2B@&aiBD>AJCHVx+DLoXcDP1:DETIq%FXbQ]F]$3>Ga<juGeTJVHim+8I"
"m/coIqGCPJu`$2K##[iK';<JL+Ss+M/lScM3.5DN7Fl%O;_L]O?w->PC9euPGQEVQKj&8RO,^oRSD>PSW]u1T[uUiT`77JUdOn+VhhNcVl*0DWpBg%XtZG]Xxs(>Y&6`uY*N@VZ.gw7["
"2)Xo[6A9P]:Yp1^>rPi^B42J_FLi+`JeIc`N'+DaR?b%bVWB]bZp#>c_2ZuccJ;Vdgcr7ek%Soeo=4PfsUk1gwnKig%1-Jh)Id+i-bDci1$&Dj5<]%k9T=]k=mt=lA/UulEG6VmJlDon"
"UF&PoQ4a4oTUxLpKl2SnO.j4oSFJloW_+Mp[wb.q`9CfqdQ$GrhjZ(sl,<`spDs@tt]Sxtxu4YujB>ip:i(cHId,N9ZTEI3QkJF%gIIF%/R3.NYqvhMYmdL2Wg/g2kcYs-KH9NM1M#<-"
"`4m<-h7pGM#7VIMYM#<-le`=-]Y#<-^Y#<-n7pGMtB=KMDN#<-#f`=-jY#<-'SlDN14pjMt1pj2YA8FI0N:FI0N:FI0N:FI0N:FI([akF([akF([akF([akF([akFnuW_A?tRhMp8#k2"
"I[/:Dwk1:Dwk1:Dwk1:Dwk1:DDSHkk)_akF)_akF)_akF)_akF)_akFDSHkkDSHkk9DjwKA']hM2=50N2=50N2=50N2=50N2=50N2=50N2=50N2=50N2=50N2=50N2=50N2=50N$-9N2"
"#4`N;E[nLM8$->%pVbe3/*?_J/F[hM'4N.3pq(eMs'3i2]42d3VZ;a-bt]k=*C]d+[/N.3;iC,3SXvk4me2d3[KpkMf(3i2]42d3Yn]j-TTYwT/F[hM^(3i2/X3d3LvAr-/Q4.-?f5K3"
"Z/N.36ZF,3.BF,33PC,3F5G,3;iC,3&*F,3EX:FItm9FI6Fr-Q$Eg8.Ba3i2IHfG3hIdG35c$d3*=eG3/KbG31Z0d3:;h-NU%<.N5%<.Nuu2.NgL&c$2gjI3_F<:D`1dG3x$eG3.pPF7"
"3_*d3w9=:DiR)d3MU%d3sY`kFUQ(.?Q+^kX^0&d3LYl;--)%>N_bm-Nb,E.NVbm-Ng+E.NVbm-N),E.NWddh2KZF)4VWaJ2gQA)4VWaJ2A8C)4Kb8:DV^jJ2E9OJ-C`<^-cRNR<<afJ2"
"[/>)4u_hJ2[/>)4G-jJ2[/>)4G-jJ2[/>)4mem;-V`>W-u&v-?\?a#0N/(^jM0giq8LApJ2/@S>-3'Q11vdx$&i2bnD;<eh2bl5<-cl5<-drPW-m:E(&[=E(&]@E(&_FE(&`IE(&aLE(&"
"WbD,3WbD,3Xk`G3Xk`G3Xk`G3YqiG3C(XB-c5jE-c5jE-c5jE-c5jE-c5jE-c5jE-d5jE-d5jE-d5jE-d5jE-d5jE-d5jE-d5jE-d5jE-d5jE-d5jE-3^L@-3^L@-3^L@-3^L@-3^L@-"
"Cd0K-9'+J-9'+J-9'+J-]NdD-]NdD-]NdD-]NdD-]NdD-^NdD-I-eT-h5jE-h5jE-h5jE-h5jE-h5jE-h5jE-h5jE-h5jE-i5jE-i5jE-i5jE-i5jE-i5jE-i5jE-i5jE-i5jE-('cZ-"
"QSG@9.RNeNZ=&fNM+aeNa?sINnaQL28hfL,drxwB.RNeN#>&fNpC/fNb.s*O$H_S-iO;=-p;MP-ZDDU%<%nt7/:YD4fYte?sM]L,.RNeN;6aINTbQL2&^c*4gX)##)AYY#$V<n0<<Im/"
"2-_'#U@%%#8e3n/Z]Q(#j'+&#Dk3n/,Z%-#LdZ(#L4;t-?OVmL>8v&#n-Sj07=G##Y'Jj0uJawt$031#";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
	return MyFont_compressed_data_base85;
}
