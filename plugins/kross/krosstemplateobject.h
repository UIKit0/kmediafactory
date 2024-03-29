// **************************************************************************
//   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
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

#ifndef KROSSTEMPLATEOBJECT_H
#define KROSSTEMPLATEOBJECT_H

#include <kdeversion.h>

#if KDE_IS_VERSION(4, 4, 0)
#include <Kross/Object>
#else
#include <kross/core/object.h>
#endif

#include <kmediafactory/plugininterface.h>

class KrossTemplateObject : public KMF::TemplateObject
{
    Q_OBJECT

    public:
        KrossTemplateObject(QObject *parent, Kross::Object::Ptr templateObject);
        ~KrossTemplateObject();

        virtual QVariant call(const QString &func, QVariantList args = QVariantList());
        virtual void toXML(QDomElement *) const;
        virtual bool fromXML(const QDomElement &);
        virtual QPixmap pixmap() const;
        virtual void actions(QList<QAction *> *) const;
        virtual bool prepare(const QString &);
        virtual void finished();
        virtual QMap<QString, QString> subTypes() const;
        virtual QString title() const;

        virtual QImage preview(const QString &page=QString(), int title=0, int chapter=0);
        virtual QStringList menus();
        virtual bool clicked();

    public slots:
        virtual void clean();

    private:
        mutable Kross::Object::Ptr m_object;
};

#endif // KROSSTEMPLATEOBJECT_H
