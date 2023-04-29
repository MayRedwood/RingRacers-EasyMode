// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by James Robert Roman
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------

#ifndef __V_DRAW_HPP__
#define __V_DRAW_HPP__

#include <string>
#include <optional>
#include <utility>

#include <fmt/core.h>

#include "doomdef.h" // skincolornum_t
#include "doomtype.h"
#include "screen.h" // BASEVIDWIDTH
#include "typedef.h"
#include "v_video.h"

namespace srb2
{

class Draw
{
public:
	enum class Font
	{
		kThin,
		kGamemode,
		kConsole,
		kFreeplay,
	};

	enum class Align
	{
		kLeft,
		kCenter,
		kRight,
	};

	enum class Stretch
	{
		kNone,
		kWidth,
		kHeight,
		kBoth,
	};

	class TextElement
	{
	public:
		explicit TextElement(std::string string) : string_(string) {}

		template <class... Args>
		explicit TextElement(fmt::format_string<Args...> format, Args&&... args) :
			TextElement(fmt::format(format, args...))
		{
		}

		const std::string& string() const { return string_; }
		std::optional<Font> font() const { return font_; }
		std::optional<INT32> flags() const { return flags_; }

		int width() const;

		TextElement& string(std::string string)
		{
			string_ = string;
			return *this;
		}

		TextElement& font(Font font)
		{
			font_ = font;
			return *this;
		}

		TextElement& flags(INT32 flags)
		{
			flags_ = flags;
			return *this;
		}

	private:
		std::string string_;
		std::optional<Font> font_;
		std::optional<INT32> flags_;
	};

	class Chain
	{
	public:
		float x() const { return x_; }
		float y() const { return y_; }

		// Methods add relative to the current state
		Chain& x(float x);
		Chain& y(float y);
		Chain& xy(float x, float y);
		Chain& flags(INT32 flags);

		// Methods overwrite the current state
		Chain& width(float width);
		Chain& height(float height);
		Chain& size(float width, float height);
		Chain& scale(float scale);
		Chain& font(Font font);
		Chain& align(Align align);
		Chain& stretch(Stretch stretch);

		// Absolute screen coordinates
		Chain& clipx(float left, float right); // 0 to BASEVIDWIDTH
		Chain& clipy(float top, float bottom); // 0 to BASEVIDHEIGHT

		Chain& clipx() { return clipx(x_, x_ + width_); }
		Chain& clipy() { return clipy(y_, y_ + height_); }

		Chain& colormap(const UINT8* colormap);
		Chain& colormap(skincolornum_t color);
		Chain& colormap(INT32 skin, skincolornum_t color);
		Chain& colorize(skincolornum_t color);

		void text(const char* str) const { string(str, flags_, font_); }
		void text(const std::string& str) const { text(str.c_str()); }
		void text(const TextElement& elm) const
		{
			string(elm.string().c_str(), elm.flags().value_or(flags_), elm.font().value_or(font_));
		}

		template <class... Args>
		void text(fmt::format_string<Args...> format, Args&&... args) const { text(fmt::format(format, args...)); }

		void patch(patch_t* patch) const;
		void patch(const char* name) const;

		void thumbnail(UINT16 mapnum) const;

		void fill(UINT8 color) const;

	private:
		constexpr Chain() {}
		explicit Chain(float x, float y) : x_(x), y_(y) {}
		Chain(const Chain&) = default;

		struct Clipper
		{
			explicit Clipper(const Chain& chain);
			~Clipper();
		};

		float x_ = 0.f;
		float y_ = 0.f;
		float width_ = 0.f;
		float height_ = 0.f;
		float scale_ = 1.f;

		float clipx1_ = 0.f;
		float clipx2_ = BASEVIDWIDTH;
		float clipy1_ = 0.f;
		float clipy2_ = BASEVIDHEIGHT;

		INT32 flags_ = 0;

		Font font_ = Font::kThin;
		Align align_ = Align::kLeft;
		Stretch stretch_ = Stretch::kNone;

		const UINT8* colormap_ = nullptr;

		void string(const char* str, INT32 flags, Font font) const;

		friend Draw;
	};

	constexpr Draw() {}
	explicit Draw(float x, float y) : chain_(x, y) {}
	Draw(const Chain& chain) : chain_(chain) {}

	// See class Chain for documentation

	float x() const { return chain_.x(); }
	float y() const { return chain_.y(); }

#define METHOD(Name) \
	template <typename... Args>\
	Chain Name (Args&&... args) const { return Chain(chain_).Name(std::forward<Args>(args)...); }

	METHOD(x);
	METHOD(y);
	METHOD(xy);
	METHOD(flags);
	METHOD(width);
	METHOD(height);
	METHOD(size);
	METHOD(scale);
	METHOD(font);
	METHOD(align);
	METHOD(stretch);
	METHOD(clipx);
	METHOD(clipy);
	METHOD(colormap);
	METHOD(colorize);

#undef METHOD

#define VOID_METHOD(Name) \
	template <typename... Args>\
	void Name (Args&&... args) const { return chain_.Name(std::forward<Args>(args)...); }

	VOID_METHOD(text);
	VOID_METHOD(patch);
	VOID_METHOD(thumbnail);
	VOID_METHOD(fill);

#undef VOID_METHOD

private:
	Chain chain_;

	static int font_to_fontno(Font font);
	static INT32 default_font_flags(Font font);
	static fixed_t font_width(Font font, INT32 flags, const char* string);
};

#include "v_draw_setter.hpp"

}; // namespace srb2

#endif // __V_DRAW_HPP__