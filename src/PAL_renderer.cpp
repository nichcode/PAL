
#include "PAL_pch.h"
#include "PAL/PAL_renderer.h"
#include "PAL/PAL_buffer.h"
#include "PAL/PAL_shader.h"
#include "PAL/PAL_texture.h"
#include "PAL/PAL_context.h"
#include "PAL_shader_sources.h"

#define MAX_SPRITES 10000
#define MAX_VERTICES MAX_SPRITES * 4
#define MAX_INDICES MAX_SPRITES * 6
#define TEXTURE_ID 0.0f
#define FONT_ID 1.0f

void _ResetBatch(PAL_Renderer* renderer)
{
    renderer->sprites.clear();
    renderer->textures.clear();
    renderer->indexCount = 0;
    renderer->texIndex = 1;

    renderer->textures.push_back(renderer->whiteTexture);
}

void _CheckBatch(PAL_Renderer* renderer)
{
    if (renderer->indexCount >= MAX_INDICES || renderer->texIndex >= PAL_MAX_TEXTURE_SLOTS) {
        PAL_RendererFlush(renderer);
    }
}

f32 _GetTextureIndex(PAL_Renderer* renderer, PAL_Texture* texture)
{
    u32 tex_index = 0.0f;
    for (u32 i = 1; i < renderer->texIndex; i++) {
        if (renderer->textures[i] == texture) {
            tex_index = (f32)i;
            break;
        }
    }

    if (tex_index == 0.0f) {
        tex_index = renderer->texIndex;
        renderer->textures.push_back(texture);
        renderer->texIndex++;
    }

    return tex_index;
}

PAL_Renderer* PAL_CreateRenderer()
{
    CHECK_CONTEXT(return nullptr)
    PAL_Renderer* renderer = new PAL_Renderer();
    CHECK_ERR(renderer, "failed to create renderer", return nullptr)

    renderer->vertices[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderer->vertices[1] = { 1.0f, 0.0f, 0.0f, 1.0f };
    renderer->vertices[2] = { 1.0f, 1.0f, 0.0f, 1.0f };
    renderer->vertices[3] = { 0.0f, 1.0f, 0.0f, 1.0f };

    renderer->texCoords[0] = { 0.0f, 0.0f };
    renderer->texCoords[1] = { 1.0f, 0.0f };
    renderer->texCoords[2] = { 1.0f, 1.0f };
    renderer->texCoords[3] = { 0.0f, 1.0f };

    u32* indices = new u32[MAX_INDICES];
    u32 offset = 0;
    for (u32 i = 0; i < MAX_INDICES; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;

        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;

        offset += 4;
    }

    PAL_ShaderLayout layout;
    PAL_ShaderAttrib position_attrib;
    PAL_ShaderAttrib color_attrib;
    PAL_ShaderAttrib texture_attrib;
    position_attrib.type = PAL_ShaderDataTypes_Float3;
    color_attrib.type = PAL_ShaderDataTypes_Float4;
    texture_attrib.type = PAL_ShaderDataTypes_Float4;
    layout.attribs[0] = position_attrib;
    layout.attribs[1] = color_attrib;
    layout.attribs[2] = texture_attrib;
    layout.count = 3;

    PAL_BufferDesc buffer_desc;
    buffer_desc.type = PAL_BufferTypes_Vertex;
    buffer_desc.data = nullptr;
    buffer_desc.size = sizeof(PAL_Vertex) * MAX_VERTICES;
    buffer_desc.usage = PAL_BufferUsages_Dynamic;
    renderer->vbo = PAL_CreateBuffer(buffer_desc);

    buffer_desc.type = PAL_BufferTypes_Index;
    buffer_desc.data = indices;
    buffer_desc.size = sizeof(u32) * MAX_INDICES;
    buffer_desc.usage = PAL_BufferUsages_Static;
    renderer->ibo = PAL_CreateBuffer(buffer_desc);

    PAL_ShaderDesc shader_desc;
    shader_desc.load = false;
    shader_desc.type = PAL_ShaderSourceTypes_GLSL;
    shader_desc.vertex_src = s_VertexSource;
    shader_desc.pixel_src = s_PixelSource;
    shader_desc.layout = layout;
    renderer->shader = PAL_CreateShader(shader_desc);

    PAL_TextureDesc desc;
    desc.format = PAL_TextureFormats_RGBA16;
    desc.flag = PAL_TextureFlags_None;
    desc.width = 1;
    desc.height = 1;
    renderer->whiteTexture = PAL_CreateTexture(desc);
    renderer->textures.push_back(renderer->whiteTexture);

    PAL_BindBuffer(renderer->vbo);
    PAL_BindBuffer(renderer->ibo);
    PAL_BindShader(renderer->shader);
    PAL_SetBlendMode(PAL_BlendModes_Alpha);

    i32 samplers[PAL_MAX_TEXTURE_SLOTS];
    for (u32 i = 0; i < PAL_MAX_TEXTURE_SLOTS; i++) { samplers[i] = i; }
    PAL_SetIntArray("u_Textures", samplers, PAL_MAX_TEXTURE_SLOTS);

    delete[] indices;
    return renderer;
}

void PAL_DestroyRenderer(PAL_Renderer* renderer)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)

    PAL_DestroyTexture(renderer->whiteTexture);
    PAL_DestroyBuffer(renderer->vbo);
    PAL_DestroyBuffer(renderer->ibo);
    PAL_DestroyShader(renderer->shader);

    renderer->sprites.clear();
    renderer->textures.clear();

    delete renderer;
    renderer = nullptr;
}

void PAL_RendererDrawRect(PAL_Renderer* renderer, const PAL_Rect rect, const PAL_Color color)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    _CheckBatch(renderer);
    
    PAL_Mat4 transform = _Translate({ rect.x, rect.y, 0.0f }) 
        * _Scale({ rect.w, rect.h, 1.0f });

    for (size_t i = 0; i < 4; i++) {
        PAL_Vec4 position = transform * renderer->vertices[i];
        PAL_Vertex sPAL_ite;
        sPAL_ite.position.x = position.x;
        sPAL_ite.position.y = position.y;
        sPAL_ite.position.z = 0.0f;

        sPAL_ite.texture.x = renderer->texCoords[i].x;
        sPAL_ite.texture.y = renderer->texCoords[i].y;
        sPAL_ite.texture.z = 0.0f; // texture index
        sPAL_ite.texture.w = TEXTURE_ID;

        sPAL_ite.color = color;
        renderer->sprites.push_back(sPAL_ite);
    }
    renderer->indexCount += 6;
}

void PAL_RendererDrawRectEx(PAL_Renderer* renderer, const PAL_Rect rect, f32 rotation, u32 anchor, const PAL_Color color)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    _CheckBatch(renderer);

    if (!rotation) { PAL_RendererDrawRect(renderer, rect, color); return; }
    PAL_Mat4 transform;

    switch (anchor) {
        case PAL_Anchors_TopLeft: {
            transform = _Translate({ rect.x, rect.y, 0.0f }) 
                    * _RotateZ(rotation)
                    * _Scale({ rect.w, rect.h, 1.0f });       
            break;
        }

        case PAL_Anchors_Center: {
            PAL_Vec2 origin;
            origin.x = rect.w / 2.0f;
            origin.y = rect.h / 2.0f;

            transform = _Translate({ rect.x + origin.x, rect.y + origin.y, 0.0f })
                        * _RotateZ(rotation)
                        * _Translate({ -origin.x, -origin.y, 0.0f })
                        * _Scale({ rect.w, rect.h, 1.0f });         
            break;
        }
    }

    for (size_t i = 0; i < 4; i++) {
        PAL_Vec4 position = transform * renderer->vertices[i];
        PAL_Vertex sPAL_ite;
        sPAL_ite.position.x = position.x;
        sPAL_ite.position.y = position.y;
        sPAL_ite.position.z = 0.0f;

        sPAL_ite.texture.x = renderer->texCoords[i].x;
        sPAL_ite.texture.y = renderer->texCoords[i].y;
        sPAL_ite.texture.z = 0.0f; // texture index
        sPAL_ite.texture.w = TEXTURE_ID;

        sPAL_ite.color = color;
        renderer->sprites.push_back(sPAL_ite);
    }
    renderer->indexCount += 6;
}

void PAL_RendererDrawTexture(PAL_Renderer* renderer, const PAL_Rect rect, PAL_Texture* texture)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    if (!texture) { // texture is null
        PAL_RendererDrawRect(renderer, rect, { 1.0f, 1.0f, 1.0f, 1.0f });
        return;
    }

    _CheckBatch(renderer);
    PAL_Mat4 transform = _Translate({ rect.x, rect.y, 0.0f }) 
        * _Scale({ rect.w, rect.h, 1.0f });

    f32 index = _GetTextureIndex(renderer, texture);

    for (size_t i = 0; i < 4; i++) {
        PAL_Vec4 position = transform * renderer->vertices[i];
        PAL_Vertex sPAL_ite;
        sPAL_ite.position.x = position.x;
        sPAL_ite.position.y = position.y;
        sPAL_ite.position.z = 0.0f;

        sPAL_ite.texture.x = renderer->texCoords[i].x;
        sPAL_ite.texture.y = renderer->texCoords[i].y;
        sPAL_ite.texture.z = index;
        sPAL_ite.texture.w = TEXTURE_ID;

        sPAL_ite.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        renderer->sprites.push_back(sPAL_ite);
    }
    renderer->indexCount += 6;
}

void PAL_RendererDrawTextureEx(PAL_Renderer* renderer, const PAL_Rect rect, f32 rotation, 
                            u32 anchor, PAL_Texture* texture, const PAL_Color tint_color, u32 flip)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    if (!texture) {
        PAL_RendererDrawRectEx(renderer, rect, rotation, anchor, { 1.0f, 1.0f, 1.0f, 1.0f });
        return;
    }
    _CheckBatch(renderer);

    PAL_Mat4 transform;
    switch (anchor) {
        case PAL_Anchors_TopLeft: {
            transform = _Translate({ rect.x, rect.y, 0.0f }) 
                    * _RotateZ(rotation)
                    * _Scale({ rect.w, rect.h, 1.0f });       
            break;
        }

        case PAL_Anchors_Center: {
            PAL_Vec2 origin;
            origin.x = rect.w / 2.0f;
            origin.y = rect.h / 2.0f;

            transform = _Translate({ rect.x + origin.x, rect.y + origin.y, 0.0f })
                        * _RotateZ(rotation)
                        * _Translate({ -origin.x, -origin.y, 0.0f })
                        * _Scale({ rect.w, rect.h, 1.0f });         
            break;
        }
    }

    f32 index = _GetTextureIndex(renderer, texture);

    f32 left = 0.0f;
    f32 top = 0.0f;
    f32 right = 1.0f;
    f32 bottom = 1.0f;

    if (flip & PAL_Flips_Horizontal) {
        f32 temp = right;
        right = left;
        left = temp;
    }
    if (flip & PAL_Flips_Vertical) {
        f32 temp = top;
        top = bottom;
        bottom = temp;
    }

    PAL_Vec2 coords[4];
    coords[0] = { left, top };
    coords[1] = { right, top };
    coords[2] = { right, bottom };
    coords[3] = { left, bottom };

    for (size_t i = 0; i < 4; i++) {
        PAL_Vec4 position = transform * renderer->vertices[i];
        PAL_Vertex sPAL_ite;
        sPAL_ite.position.x = position.x;
        sPAL_ite.position.y = position.y;
        sPAL_ite.position.z = 0.0f;

        sPAL_ite.texture.x = coords[i].x;
        sPAL_ite.texture.y = coords[i].y;
        sPAL_ite.texture.z = index;
        sPAL_ite.texture.w = TEXTURE_ID;

        sPAL_ite.color = tint_color;
        renderer->sprites.push_back(sPAL_ite);
    }
    renderer->indexCount += 6;
}

void PAL_RendererDrawText(PAL_Renderer* renderer, f32 x, f32 y, f32 scale, const char* text, PAL_Font* font, const PAL_Color color)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    CHECK_ERR(font, "font is null", return)
    CHECK_ERR(text, "text is empty", return)

    PAL_Texture* texture = font->texture;
    if (!texture) { return; }

    f32 index = _GetTextureIndex(renderer, texture);
    f32 size = texture->width;
    f32 base = font->baseline;
    PAL_Vec2 origin = { x, y };

    while(char c = *(text++)) {
        auto glyph = font->glyphs.at(c);
        f32 xpos = origin.x + glyph.offset_x;
        f32 ypos = origin.y + (base - glyph.offset_y) * scale;

        f32 width = glyph.width * scale;
        f32 height = glyph.height * scale;

        f32 left = glyph.index_x / size;
        f32 top = glyph.index_y / size;
        f32 right = (glyph.index_x + glyph.width) / size;
        f32 bottom = (glyph.index_y + glyph.height) / size;

        PAL_Vec2 coords[4];
        coords[0] = { left, top };
        coords[1] = { right, top };
        coords[2] = { right, bottom };
        coords[3] = { left, bottom };

        PAL_Vec2 position[4];
        position[0] = { xpos, ypos };
        position[1] = { xpos + width, ypos };
        position[2] = { xpos + width, ypos + height };
        position[3] = { xpos, ypos + height };

        for (size_t i = 0; i < 4; i++) {
            PAL_Vertex sPAL_ite;
            sPAL_ite.position.x = position[i].x;
            sPAL_ite.position.y = position[i].y;
            sPAL_ite.position.z = 0.0f;

            sPAL_ite.texture.x = coords[i].x;
            sPAL_ite.texture.y = coords[i].y;
            sPAL_ite.texture.z = index;
            sPAL_ite.texture.w = FONT_ID;

            sPAL_ite.color = color;
            renderer->sprites.push_back(sPAL_ite);
        }
        renderer->indexCount += 6;
        origin.x += glyph.advance_x * scale; 
    }
}

void PAL_RendererFlush(PAL_Renderer* renderer)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)

    if (renderer->indexCount) {
        PAL_BindBuffer(renderer->vbo);
        PAL_BindBuffer(renderer->ibo);
        PAL_BindShader(renderer->shader);

        u32 size = sizeof(PAL_Vertex) * renderer->indexCount;
        PAL_SetBufferData(PAL_BufferTypes_Vertex, renderer->sprites.data(), size);

        for (u32 i = 0; i < renderer->texIndex; i++) {
            PAL_BindTexture(renderer->textures[i], i);
        }
        
        PAL_DrawElements(PAL_DrawModes_Triangles, renderer->indexCount);
        _ResetBatch(renderer);
    }
}

void PAL_SetRendererCamera(PAL_Renderer* renderer, PAL_Camera camera)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(renderer, "renderer is null", return)
    
    PAL_Mat4 transform;
    if (camera.rotation) {
        transform = _Translate({ camera.view.x, camera.view.y, 0.0f }) 
            * _RotateZ(camera.rotation);
    }
    else {
        transform = _Translate({ camera.view.x, camera.view.y, 0.0f });
    }

    PAL_Mat4 projection = _Ortho(
        0.0f, 
        camera.view.width,
        camera.view.height, 
        0.0f,
        -1.0f, 1.0f);
        
    PAL_SetMat4("u_ViewProjection", projection.data);
}