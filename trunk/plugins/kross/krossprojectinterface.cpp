//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "krossprojectinterface.h"
#include <KDebug>

KrossProjectInterface::KrossProjectInterface(QObject *parent, KMF::ProjectInterface* projectIf)
 : QObject(parent), m_projectIf(projectIf)
{
}

KrossProjectInterface::~KrossProjectInterface()
{
}

QList<MediaObject*> KrossProjectInterface::mediaObjects()
{
  return m_projectIf->mediaObjects();
}

QString KrossProjectInterface::title()
{
  return m_projectIf->title();
}

void KrossProjectInterface::setTitle(QString title)
{
  m_projectIf->setTitle(title);
}

QString KrossProjectInterface::projectDir(const QString& subDir)
{
  return m_projectIf->projectDir(subDir);
}

void KrossProjectInterface::cleanFiles(const QString& subDir, const QStringList& files)
{
  return m_projectIf->cleanFiles(subDir, files);
}

void KrossProjectInterface::setDirty(KMF::ProjectInterface::DirtyType type)
{
  return m_projectIf->setDirty(type);
}

QString KrossProjectInterface::type()
{
  return m_projectIf->type();
}

QString KrossProjectInterface::lastSubType()
{
  return m_projectIf->lastSubType();
}

QDateTime KrossProjectInterface::lastModified(KMF::ProjectInterface::DirtyType type)
{
  return m_projectIf->lastModified(type);
}

int KrossProjectInterface::serial()
{
  return m_projectIf->serial();
}

bool KrossUiInterface::addMediaAction(const QString& icon, const QString& text,
                                      const QString& shortcut, const QString& name,
                                      Kross::Object::Ptr obj, const QString& method)
{
  KrossPlugin* plugin = qobject_cast<KrossPlugin*>(parent());
  QAction* act = new KAction(KIcon(icon), text, plugin->parent());
  act->setShortcut(QKeySequence::fromString(shortcut));
  //kDebug() << "addAction" << name << act;
  plugin->actionCollection()->addAction(name, act);
  connect(act, SIGNAL(triggered()), plugin, SLOT(actionTriggered()));
  QVariant arg;
  arg.setValue(obj);
  plugin->actionMap()->insert(act, QVariantList() << arg << method);
  return m_uiIf->addMediaAction(act);
}

void KrossUiInterface::setActionEnabled(const QString& name, bool enabled)
{
  KrossPlugin* plugin = qobject_cast<KrossPlugin*>(parent());
  QAction* action = plugin->actionCollection()->action(name);
  if(!action)
    return;
  action->setEnabled(enabled);
}

bool KrossUiInterface::addMediaObject(Kross::Object::Ptr media) const
{
  Q_UNUSED(media)
  return false;
}

bool KrossUiInterface::addTemplateObject(Kross::Object::Ptr tob)
{
  Q_UNUSED(tob)
  return false;
}

bool KrossUiInterface::addOutputObject(Kross::Object::Ptr oob)
{
  Q_UNUSED(oob)
  return false;
}

bool KrossUiInterface::removeMediaObject(Kross::Object::Ptr media) const
{
  Q_UNUSED(media)
  return false;
}

bool KrossUiInterface::removeTemplateObject(Kross::Object::Ptr tob)
{
  Q_UNUSED(tob)
  return false;
}

bool KrossUiInterface::removeOutputObject(Kross::Object::Ptr oob)
{
  Q_UNUSED(oob)
  return false;
}

bool KrossUiInterface::message(KMF::MsgType type, const QString& msg)
{
  return m_uiIf->message(type, msg);
}

bool KrossUiInterface::progress(int advance)
{
  return m_uiIf->progress(advance);
}

bool KrossUiInterface::setItemTotalSteps(int totalSteps)
{
  return m_uiIf->setItemTotalSteps(totalSteps);
}

bool KrossUiInterface::setItemProgress(int progress)
{
  return m_uiIf->setItemProgress(progress);
}

QObject* KrossUiInterface::logger()
{
  return m_uiIf->logger();
}

void KrossUiInterface::addMediaObjectFromXML(const QString& xml)
{
  kDebug() << xml;
  m_uiIf->addMediaObject(xml);
}

void KrossUiInterface::setTemplateFromXML(const QString& xml)
{
  m_uiIf->selectTemplate(xml);
}

void KrossUiInterface::setOutputFromXML(const QString& xml)
{
  m_uiIf->selectOutput(xml);
}

QStringList KrossUiInterface::getOpenFileNames(const QString &startDir, const QString &filter, 
                                               const QString &caption)
{
  return m_uiIf->getOpenFileNames(startDir, filter, caption);
}

void KrossUiInterface::debug(const QString &txt)
{
  return m_uiIf->debug(txt);
}

int  KrossUiInterface::messageBox(const QString &caption, const QString &txt, int type)
{
  return m_uiIf->messageBox(caption, txt, type);
}

QObject* KrossUiInterface::progressDialog(const QString &caption, const QString &label, 
                                          int maximum)
{
  return m_uiIf->progressDialog(caption, label, maximum);
}

QObject* KrossUiInterface::progressDialog()
{
  return m_uiIf->progressDialog();
}

#include "krossprojectinterface.moc"