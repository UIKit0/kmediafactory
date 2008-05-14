//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�n
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
#ifndef SLIDESHOWPROPERTIES_H
#define SLIDESHOWPROPERTIES_H

#include <slideshowlayout.h>
#include <slideshowobject.h>

class KFileItem;
class SlideshowObject;

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/

class SlideshowProperties : public SlideshowPropertiesLayout
{
    Q_OBJECT
  public:
    SlideshowProperties(QWidget *parent = 0, const char *name = 0);
    ~SlideshowProperties();

    void getData(SlideshowObject& obj) const;
    void setData(const SlideshowObject& obj);

  protected:
    void addSlides(const SlideList& slides);
    void select(QListViewItem* item);

  public slots:
    void gotPreview(const KFileItem* item, const QPixmap& pixmap);

  protected slots:
    virtual void moveUp();
    virtual void moveDown();
    virtual void add();
    virtual void remove();
    virtual void audioClicked();
    virtual void okClicked();
    void updateInfo();

  private:
    const SlideshowObject* m_sob;
    QStringList m_audioFiles;
};

#endif