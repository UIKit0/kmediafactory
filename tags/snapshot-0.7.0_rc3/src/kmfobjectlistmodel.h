//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef KMFTEMPLATEICONVIEW_H
#define KMFTEMPLATEICONVIEW_H

#include <kmediafactory/object.h>
#include "kmflistmodel.h"
#include <KLocale>

template <class T>
class KMFObjectListModel : public KMFListModel<T>
{
  public:
    QVariant data(const QModelIndex &index, int role) const
    {
      if(!KMFListModel<T>::isValid(index))
        return QVariant();

      if (role == Qt::DisplayRole)
        return KMFListModel<T>::at(index)->title();
      else if (role == Qt::DecorationRole)
        return KMFListModel<T>::at(index)->pixmap();
      return QVariant();
    }

    QVariant headerData(int, Qt::Orientation, int role) const
    {
      if (role != Qt::DisplayRole)
        return QVariant();

      return ::i18n("Title");
    }
};

#endif // KMFTEMPLATEICONVIEW_H