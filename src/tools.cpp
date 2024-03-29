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

#include "tools.h"

#include <QtCore/QFileInfo>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include <KApplication>
#include <KDebug>
#include <KDesktopFile>
#include <KGlobal>
#include <KIconLoader>
#include <KPushButton>
#include <KStandardDirs>
#include <KIO/DeleteJob>
#include <KIO/Job>

#include <kmftools.h>
#include "toolproperties.h"

// From kmenuedit - treeview.cpp
static QPixmap appIcon(const QString &iconName)
{
    QPixmap normal = KIconLoader::global()->loadIcon(iconName, KIconLoader::Small,
            0, KIconLoader::DefaultState, QStringList(), 0L, true);

    // make sure they are not larger than 20x20
    if ((normal.width() > 20) || (normal.height() > 20)) {
        QImage tmp = normal.toImage();
        tmp = tmp.scaled(20, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        normal.fromImage(tmp);
    }

    return normal;
}

int ToolListModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant ToolListModel::data(const QModelIndex &index, int role) const
{
    int i = index.row();

    if (!isValid(i)) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
            return at(i).name;

            case 1:
            return at(i).description;
        }
    }

    if (role == Qt::DecorationRole) {
        switch (index.column()) {
            case 0:
            return appIcon(at(i).icon);
        }
    }

    return QVariant();
}

QVariant ToolListModel::headerData(int column, Qt::Orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (column) {
        case 0:
        return i18n("Name");

        case 1:
        return i18n("Description");
    }

    return "";
}

Tools::Tools(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    connect(parent, SIGNAL(okClicked()), this, SLOT(save()));
    toolsListView->setModel(&m_model);
    load();
    connect(toolsListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(enableButtons()));
}

Tools::~Tools()
{
}

void Tools::addClicked()
{
    QPointer<ToolProperties> dlg = new ToolProperties(kapp->activeWindow());

    if (dlg->exec()) {
        ToolItem item;
        dlg->getData(&item);

        int i = 0;
        QFileInfo file;
        QString name = KMF::Tools::simpleName(item.name);

        file = KStandardDirs::locateLocal("appdata",
                "tools/" + name + ".desktop");

        while (file.exists()) {
            file = KStandardDirs::locateLocal("appdata", "tools/" +
                    QString("%1_%2").arg(name).arg(++i) + ".desktop");
        }
        item.desktopFile = file.absoluteFilePath();

        m_model.append(item);
    }
    delete dlg;
    enableButtons();
}

void Tools::propertiesClicked()
{
    QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();

    if (list.size() > 0) {
        ToolItem item = m_model.at(list.first());
        QPointer<ToolProperties> dlg = new ToolProperties(kapp->activeWindow());
        dlg->setData(item);
        dlg->setReadOnly(!writableItem(item));

        if (dlg->exec()) {
            dlg->getData(&item);
            m_model.replace(list.first(), item);
        }
        delete dlg;
    }
}

void Tools::removeClicked()
{
    QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();

    if (list.size() > 0) {
        ToolItem item = m_model.at(list.first());

        if (!item.desktopFile.isEmpty()) {
            m_remove.append(item.desktopFile);
        }

        m_model.removeAt(list.first());
        enableButtons();
    }
}

void Tools::save()
{
    kDebug() << m_remove;

    if (!m_remove.isEmpty()) {
        KIO::del(m_remove);
    }

    QList<ToolItem> items = m_model.list();

    foreach(const ToolItem& item, items) {
        if (!writableItem(item)) {
            continue;
        }

        kDebug() << item.desktopFile;
        KDesktopFile df(item.desktopFile);
        KConfigGroup group = df.group("Desktop Entry");
        group.writeEntry("Name", item.name);
        group.writeEntry("Type", "Application");
        group.writeEntry("Icon", item.icon);
        group.writeEntry("GenericName", item.description);
        group.writeEntry("Exec", item.command);
        group.writeEntry("Path", item.workPath);
        group.writeEntry("Terminal", item.runInTerminal);
        group.writeEntry("X-KMF-MediaMenu", item.mediaMenu);
    }
}

void Tools::load()
{
    QStringList files =
        KGlobal::dirs()->findAllResources("appdata", "tools/*.desktop");

    for (QStringList::ConstIterator it = files.begin();
         it != files.end(); ++it)
    {
        kDebug() << *it;
        KDesktopFile df(*it);

        if (df.readType() != "Application") {
            continue;
        }

        ToolItem item;
        item.name = df.readName();
        item.icon = df.readIcon();
        item.description = df.readGenericName();
        item.workPath = df.readPath();
        KConfigGroup group = df.group("Desktop Entry");
        item.command = group.readEntry("Exec");
        item.mediaMenu = (group.readEntry("X-KMF-MediaMenu") == "true");
        item.runInTerminal = (group.readEntry("Terminal") == "true");
        item.desktopFile = *it;
        m_model.append(item);
    }

    enableButtons();
}

bool Tools::writableItem(const ToolItem& item)
{
    QFileInfo fi(item.desktopFile);

    if (fi.exists()) {
        return fi.isWritable();
    } else {
        return true;
    }
}

void Tools::enableButtons()
{
    QModelIndexList list = toolsListView->selectionModel()->selectedIndexes();
    bool writable = false;

    if (list.size() > 0) {
        ToolItem item = m_model.at(list.first());
        writable = writableItem(item);
    }

    propertiesButton->setEnabled(list.size() > 0);
    removeButton->setEnabled(list.size() > 0 && writable);
}

#include "tools.moc"
