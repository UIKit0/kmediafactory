// **************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#ifndef KMF_KMEDIAFACTORY_H
#define KMF_KMEDIAFACTORY_H

namespace KMF
{
enum DirtyType { Media = 1, Template = 2, Output = 4, MediaOrTemplate = 3, Any = 7};
enum MsgType { Root = 0, Start, Done, Info, Warning, Error, OK};
enum JobDependency { None, All, Last};
}

#endif // KMF_DEFS
