
#include "PAL_pch.h"
#include "PAL/PAL_font.h"

PAL_Font* PAL_LoadFont(const char* filepath, u32 size)
{
    CHECK_CONTEXT(return nullptr)
    PAL_Font* font = new PAL_Font();
    CHECK_ERR(font, "failed to create font", return nullptr)
   
    FT_Face font_face;
    i32 error = FT_New_Face(s_Library, filepath, 0, &font_face);
    if (error == FT_Err_Unknown_File_Format) {
        _SetError("Unknown file format"); 
    }
    else if (error != 0) {
        _SetError("filepath is null"); 
    }

    FT_Set_Pixel_Sizes(font_face, 0, size);
    u32 padding = 2;
    u32 row = 0;
    u32 col = padding;
    static u32 texture_size = 512;
    char buffer[texture_size  * texture_size];
    FT_GlyphSlot slot =  font_face->glyph;

    for (u8 c = 32; c < 127; c++) {
        if (FT_Load_Char(font_face, c, FT_LOAD_RENDER)) {
            _SetError("Failed to load Glyph");
            continue;
        }

        FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

        if (col + slot->bitmap.width + padding >= texture_size) {
            col = padding;
            row += size;
        }

        for (u32 y = 0; y < slot->bitmap.rows; ++y) {
            for (u32 x = 0; x < slot->bitmap.width; ++x) {
            buffer[(row + y) * texture_size + col + x] =
                slot->bitmap.buffer[y * slot->bitmap.width + x];
            }
        }

        PAL_Glyph glyph;
        glyph.index_x = (f32)col;
        glyph.index_y = (f32)row;

        glyph.offset_x = (f32)slot->bitmap_left;
        glyph.offset_y = (f32)slot->bitmap_top;

        glyph.width = (f32)slot->bitmap.width;
        glyph.height = (f32)slot->bitmap.rows;

        glyph.advance_x = (u32)slot->advance.x >> 6;
        glyph.advance_y = (u32)slot->advance.y >> 6;

        font->glyphs.insert(std::pair<u8, PAL_Glyph>(c, glyph));
        col += slot->bitmap.width + padding;
    }

    PAL_TextureDesc desc;
    desc.flag = PAL_TextureFlags_Storage;
    desc.storage_data = (char*)buffer;
    desc.format = PAL_TextureFormats_R8;
    desc.width = texture_size;
    desc.height = texture_size;
    font->texture = PAL_CreateTexture(desc);
    CHECK_ERR(font->texture, "failed to create font texture atlas", delete font; return nullptr)

    font->path = filepath;
    font->size = size;
    font->baseline = font->glyphs.at('H').offset_y;
    FT_Done_Face(font_face);

    s_ActiveContext->data.fonts.push_back(font);
    return font;
}

void PAL_DestroyFont(PAL_Font* font)
{
    CHECK_CONTEXT(return)
    CHECK_ERR(font, "font is null", return)
    PAL_Context* context = s_ActiveContext;

    auto it = std::find(context->data.fonts.begin(), context->data.fonts.end(), font);
    if (it != context->data.fonts.end()) {
        context->data.fonts.erase(it); 
    }

    font->glyphs.clear();
    delete font;
    font = nullptr;
}

u32 PAL_GetFontSize(PAL_Font* font)
{
    CHECK_ERR(font, "font is null", return 0)
    return font->size;
}

const char* PAL_GetFontPath(PAL_Font* font)
{
    CHECK_ERR(font, "font is null", return nullptr)
    return font->path;
}

f32 PAL_GetFontBaseLine(PAL_Font* font)
{
    CHECK_ERR(font, "font is null", return 0.0f)
    return font->baseline;
}

PAL_Texture* PAL_GetFontTexture(PAL_Font* font)
{
    CHECK_ERR(font, "font is null", return nullptr)
    return font->texture;
}

PAL_Glyph* PAL_GetFontGlyph(PAL_Font* font, u8 c)
{
    CHECK_ERR(font, "font is null", return nullptr)
    return &font->glyphs.at(c);
}