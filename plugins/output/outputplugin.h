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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************
#ifndef OUTPUTPLUGIN_H
#define OUTPUTPLUGIN_H



#include <kmf_stddef.h>
#include <kmediafactory/plugin.h>

class QAction;

class OutputPlugin : public KMF::Plugin
{
    Q_OBJECT
  public:
    OutputPlugin(QObject *parent, const QVariantList&);

    void play(const QString& player = QString::null);

  public slots:
    virtual void init(const QString &type);

  public:
    virtual QStringList supportedProjectTypes() const;

  public slots:
    void slotPreviewDVDXine();
    void slotPreviewDVDKaffeine();
    void slotDVDInfo();

  private:
    QAction* dvdInfo;
    QAction* addPreviewDVDXine;
    QAction* addPreviewDVDKaffeine;
    QString m_xine;
    QString m_kaffeine;
};

#endif // OUTPUTPLUGIN_H


