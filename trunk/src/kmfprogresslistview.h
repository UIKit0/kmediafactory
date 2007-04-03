//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#ifndef KMFPROGRESSLISTVIEW_H
#define KMFPROGRESSLISTVIEW_H

#include <QProgressBar>
#include <QItemDelegate>
#include <QAbstractListModel>

class KMFProgressItem
{
  public:
    KMFProgressItem() : max(0), value(0) {};
    QString pixmap;
    QString text;
    int max;
    int value;
};

class KMFProgressItemModel : public QAbstractListModel
{
  public:
    KMFProgressItemModel(QObject* parent = 0) : QAbstractListModel(parent) {};
    QList<KMFProgressItem>* data() { return &m_data; };
    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    void update() { reset(); };

  private:
    QList<KMFProgressItem> m_data;
};

class KMFProgressItemDelegate : public QItemDelegate
{
    Q_OBJECT
  public:
    KMFProgressItemDelegate(KMFProgressItemModel* model, QObject* parent = 0) :
        QItemDelegate(parent), m_model(model) {};
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;
  private:
    KMFProgressItemModel* m_model;
};

#endif
