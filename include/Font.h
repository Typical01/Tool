#pragma once

#ifndef _FONT_H
#define _FONT_H

#include "pch.h"
//#include "Typical_Tool.h"


namespace Typical_Tool {

	namespace WindowsSystem {

		class  WindowFont {
		private:
			static HFONT Font;

		public:
			WindowFont()
			{
				Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
				/*
				Font = CreateFont(
					-16, -7, 0, 0,
					400, //粗度 一般这个值设为400
					FALSE, //斜体
					FALSE, //下划线
					FALSE, //删除线
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
					FF_DONTCARE,
					"微软雅黑"
				);
				*/
			}
			WindowFont(HFONT hFont) {
				Font = hFont;
			}

			void SetFont(HFONT hFont);

		public:
			void SetWindowFont(HWND hwnd);
		};
#define 窗口字体 WindowFont
	}
}


#endif