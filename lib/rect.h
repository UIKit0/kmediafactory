// **************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
//   petri.damsten@iki.fi
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#ifndef KMFRECT_H
#define KMFRECT_H

#include <QtCore/QRect>

#include <kdemacros.h>

namespace KMF
{

class KDE_EXPORT Rect : public QRect
{
    public:
        Rect();
        Rect(int left, int top, int width, int height);
        Rect(const QRect &rc);
        Rect(const QPoint &topLeft, const QSize &size);
        ~Rect();

        enum VAlign {
            Top, Middle, Bottom, VDefault
        };
        enum HAlign {
            Left, Center, Right, HDefault
        };

        void set(const QRect &maxRect);

        void set(const QRect &maxRect, double aspectRatio);

        void align(const QRect &parentRect, HAlign halign, VAlign valign);
};

}

#endif
