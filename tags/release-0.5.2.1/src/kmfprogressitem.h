//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�n
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
#ifndef KMFPROGRESSITEM_H
#define KMFPROGRESSITEM_H

#include <klistview.h>

class KProgress;

/**
@author Petri Damsten
*/
class KMFProgressItem : public KListViewItem
{
  public:
    enum { ProgressWidth = 150, Margin = 5 };
    
    KMFProgressItem(QListView *parent, QListViewItem *after);
    ~KMFProgressItem();

    void paintCell(QPainter* p, const QColorGroup& cg,
                   int column, int width, int align);
    
  public slots:
    void showProgressBar(bool showProgressBar);
    void setTotalSteps(int totalSteps);
    void setProgress(int progress);
       
  private:
    KProgress* m_bar;

    void checkBar();
};

#endif
