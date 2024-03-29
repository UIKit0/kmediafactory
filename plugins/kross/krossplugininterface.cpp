// **************************************************************************
//   Copyright (C) 2008 by Petri Damsten <damu@iki.fi>
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

#include "krossplugininterface.h"

#include <KAction>
#include <KActionCollection>
#include <KDebug>

#include "krossjob.h"
#include "krossmediaobject.h"
#include "krossoutputobject.h"
#include "krossplugin.h"
#include "krosstemplateobject.h"

KrossPluginInterface::KrossPluginInterface(QObject *parent, KMF::PluginInterface *interface)
    : QObject(parent)
    , m_interface(interface)
{
}

KrossPluginInterface::~KrossPluginInterface()
{
}

QList<KMF::MediaObject *> KrossPluginInterface::mediaObjects()
{
    return m_interface->mediaObjects();
}

KMF::TemplateObject *KrossPluginInterface::templateObject()
{
    return m_interface->templateObject();
}

KMF::OutputObject *KrossPluginInterface::outputObject()
{
    return m_interface->outputObject();
}

QString KrossPluginInterface::title()
{
    return m_interface->title();
}

void KrossPluginInterface::setTitle(QString title)
{
    m_interface->setTitle(title);
}

QString KrossPluginInterface::projectDir(const QString &subDir)
{
    return m_interface->projectDir(subDir);
}

void KrossPluginInterface::setDirty(KMF::DirtyType type)
{
    return m_interface->setDirty(type);
}

QString KrossPluginInterface::projectType()
{
    return m_interface->projectType();
}

QString KrossPluginInterface::lastSubType()
{
    return m_interface->lastSubType();
}

QDateTime KrossPluginInterface::lastModified(KMF::DirtyType type)
{
    return m_interface->lastModified(type);
}

int KrossPluginInterface::serial()
{
    return m_interface->serial();
}

QString  KrossPluginInterface::newAction(const QString &name, const QString &icon,
        const QString &text, const QString &shortcut, Kross::Object::Ptr obj,
        const QString &method)
{
    KrossPlugin *plugin = qobject_cast<KrossPlugin *>(QObject::parent());
    QAction *act = new KAction(KIcon(icon), text, plugin->parent());

    act->setShortcut(QKeySequence::fromString(shortcut));
    // kDebug() << "addAction" << name << act;
    plugin->actionCollection()->addAction(name, act);
    connect(act, SIGNAL(triggered()), plugin, SLOT(actionTriggered()));
    QVariant arg;
    arg.setValue(obj);
    plugin->actionMap()->insert(act, QVariantList() << arg << method);
    return name;
}

bool KrossPluginInterface::addMediaAction(const QString &name)
{
    KrossPlugin *plugin = qobject_cast<KrossPlugin *>(QObject::parent());
    QAction *act = plugin->actionCollection()->action(name);

    return m_interface->addMediaAction(act);
}

void KrossPluginInterface::setActionEnabled(const QString &name, bool enabled)
{
    KrossPlugin *plugin = qobject_cast<KrossPlugin *>(QObject::parent());
    QAction *action = plugin->actionCollection()->action(name);

    if (!action) {
        return;
    }

    action->setEnabled(enabled);
}

uintptr_t KrossPluginInterface::addMediaObject(Kross::Object::Ptr media)
{
    KrossMediaObject *mob = new KrossMediaObject(this, media);

    if (m_interface->addMediaObject(mob)) {
        return (uintptr_t)mob;
    }

    return 0;
}

uintptr_t KrossPluginInterface::addTemplateObject(Kross::Object::Ptr tob)
{
    KrossTemplateObject *mob = new KrossTemplateObject(this, tob);

    if (m_interface->addTemplateObject(mob)) {
        return (uintptr_t)mob;
    }

    return 0;
}

uintptr_t KrossPluginInterface::addOutputObject(Kross::Object::Ptr oob)
{
    KrossOutputObject *mob = new KrossOutputObject(this, oob);

    if (m_interface->addOutputObject(mob)) {
        return (uintptr_t)mob;
    }

    return 0;
}

bool KrossPluginInterface::removeMediaObject(uintptr_t mediaId)
{
    return m_interface->removeMediaObject((KMF::MediaObject *)mediaId);
}

bool KrossPluginInterface::removeTemplateObject(uintptr_t templateId)
{
    return m_interface->removeTemplateObject((KMF::TemplateObject *)templateId);
}

bool KrossPluginInterface::removeOutputObject(uintptr_t outputId)
{
    return m_interface->removeOutputObject((KMF::OutputObject *)outputId);
}

uintptr_t KrossPluginInterface::addJob(Kross::Object::Ptr job, KMF::JobDependency dependency)
{
    KrossJob *j = new KrossJob(this, job);

    m_interface->addJob(j, dependency);
    return (uintptr_t)j;
}

uintptr_t KrossPluginInterface::addJob(Kross::Object::Ptr job, uintptr_t dependency)
{
    KrossJob *j = new KrossJob(this, job);

    m_interface->addJob(j, (KMF::Job *)dependency);
    return (uintptr_t)j;
}

uint KrossPluginInterface::messageId()
{
    return m_interface->messageId();
}

void KrossPluginInterface::message(uint id, KMF::MsgType type, const QString &msg)
{
    m_interface->message(id, type, msg);
}

void KrossPluginInterface::setMaximum(uint id, int maximum)
{
    m_interface->setMaximum(id, maximum);
}

void KrossPluginInterface::setValue(uint id, int value)
{
    m_interface->setValue(id, value);
}

void KrossPluginInterface::log(uint id, const QString &msg)
{
    m_interface->log(id, msg);
}

uint KrossPluginInterface::subId(uint id)
{
    return m_interface->subId(id);
}

uint KrossPluginInterface::parent(uint id)
{
    return m_interface->parent(id);
}

void KrossPluginInterface::addMediaObjectFromXML(const QString &xml)
{
    kDebug() << xml;
    m_interface->addMediaObject(xml);
}

void KrossPluginInterface::setTemplateFromXML(const QString &xml)
{
    m_interface->selectTemplate(xml);
}

void KrossPluginInterface::setOutputFromXML(const QString &xml)
{
    m_interface->selectOutput(xml);
}

QStringList KrossPluginInterface::getOpenFileNames(const QString &startDir, const QString &filter,
        const QString &caption)
{
    return m_interface->getOpenFileNames(startDir, filter, caption);
}

void KrossPluginInterface::debug(const QString &txt)
{
    return m_interface->debug(txt);
}

int  KrossPluginInterface::messageBox(const QString &caption, const QString &txt, int type)
{
    return m_interface->messageBox(caption, txt, type);
}

QObject *KrossPluginInterface::progressDialog(const QString &caption, const QString &label,
                                              int maximum)
{
    return m_interface->progressDialog(caption, label, maximum);
}

QObject *KrossPluginInterface::progressDialog()
{
    return m_interface->progressDialog();
}

#include "krossplugininterface.moc"
