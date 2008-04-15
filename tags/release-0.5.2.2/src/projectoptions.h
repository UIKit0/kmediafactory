//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#ifndef PROJECTOPTIONS_H
#define PROJECTOPTIONS_H

#include <projectoptionslayout.h>
#include "kmfproject.h"

/**
*/
class ProjectOptions : public ProjectOptionsLayout
{
    Q_OBJECT
  public:
    ProjectOptions(QWidget* parent = 0, const char* name = 0,
                   bool modal = FALSE, WFlags fl = 0);
    ~ProjectOptions();
    void getData(KMFProject& project) const;
    void setData(const KMFProject& project);

  protected slots:
    void titleChanged(const QString&);
    void okClicked();

  private:
    bool titleChangesPath;
    QString m_type;
    QString m_saved;
    int m_count;
};

#endif