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

#ifndef TOOLS_H
#define TOOLS_H

#include <QtCore/QObject>

#include <KUrl>

#include <kmflistmodel.h>
#include <ui_tools.h>

class ToolItem
{
    public:
        ToolItem() : mediaMenu(false), runInTerminal(false){};
        QString name;
        QString description;
        QString command;
        QString icon;
        QString desktopFile;
        QString workPath;
        bool mediaMenu;
        bool runInTerminal;

        bool operator<(const ToolItem &t) const
        {
            return (name < t.name);
        }
};

Q_DECLARE_METATYPE(ToolItem)

class ToolListModel : public KMFListModel<ToolItem>
{
    virtual int columnCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int column, Qt::Orientation, int role) const;
};

class Tools : public QWidget, public Ui::Tools
{
    Q_OBJECT

    public:
        Tools(QWidget *parent = 0);
        virtual ~Tools();

    public slots:
        virtual void save();
        virtual void load();

    protected slots:
        virtual void addClicked();
        virtual void propertiesClicked();
        virtual void removeClicked();
        virtual void enableButtons();

    protected:
        bool writableItem(const ToolItem& item);

    private:
        KUrl::List m_remove;
        ToolListModel m_model;
};

#endif
