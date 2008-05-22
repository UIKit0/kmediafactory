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
#include "kmfproject.h"
#include "kmfapplication.h"
#include "kmediafactorysettings.h"
#include <kmftools.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <klocale.h>
#include <ksavefile.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <ksavefile.h>
#include <ktemporaryfile.h>
#include <QDir>
#include <QDomElement>

template <class S, class T>
void addMap(QMap<S, T>* map, const QMap<S, T>& add)
{
  QList<S> keys = add.keys();

  for(int i = 0; i < keys.size(); ++i)
    map->insert(keys[i], add[keys[i]]);
}

KMFProject::KMFProject(QObject *parent) :
  QObject(parent), m_template(0), m_output(0), m_modified(false),
  m_loading(false), m_initializing(false), m_serial(0)
{
  m_lastModified[ModMedia].setTime_t(0);
  m_lastModified[ModTemplate].setTime_t(0);
  m_lastModified[ModOutput].setTime_t(0);

  m_type = KMediaFactorySettings::defaultProjectType();
  m_title = i18n("Untitled");

  QDir dir(KMediaFactorySettings::defaultProjectDirectory().path());
  int i = 1;

  while(1)
  {
    QString file = i18n("Project%1", i);

    QDir projectDir(dir.filePath(file));
    if(!projectDir.exists())
    {
      m_directory = projectDir.path();
      break;
    }
    ++i;
  }
  //kDebug() << m_directory;
}

KMFProject::~KMFProject()
{
}

bool KMFProject::validProject() const
{
  setError();
  if(m_list.count() == 0)
  {
    setError(i18n("No media files."));
    return false;
  }
  if(!m_template)
  {
    setError(i18n("No template selected."));
    return false;
  }
  if(!m_output)
  {
    setError(i18n("No output type selected."));
    return false;
  }
  return true;
}

void KMFProject::addItem(KMF::MediaObject *mob)
{
  m_list.append(mob);
  setDirty(KMF::DirtyMedia);
}

void KMFProject::removeItem(KMF::MediaObject *mob)
{
  m_list.removeAll(mob);
  setDirty(KMF::DirtyMedia);
}

void KMFProject::setType(const QString& type)
{
  if(type != m_type)
  {
    m_type = type;
    emit preinit(m_type);
    m_list.clear();
    emit init(type);
  }
}

void KMFProject::setDirectory(const QString& directory)
{
  m_directory = KMF::Tools::addSlash(directory);
  if(m_directory.startsWith("file://")) m_directory = m_directory.mid(7);
  setDirty(KMF::DirtyAny);
}

void KMFProject::setTitle(const QString& title)
{
  m_title = title;
  setDirty(KMF::DirtyTemplate);
}

void KMFProject::setTemplateObj(KMF::TemplateObject* tmplate)
{
  m_template = tmplate;
  setDirty(KMF::DirtyTemplate);
}

void KMFProject::setOutput(KMF::OutputObject* output)
{
  m_output = output;
  setDirty(KMF::DirtyOutput);
}

void KMFProject::init()
{
  m_initializing = true;
  kDebug() << "preinit";
  emit preinit(m_type);
  kDebug() << "init";
  emit init(m_type);
  m_initializing = false;
  setDirty(KMF::DirtyAny, false);
}

void KMFProject::mediaObjFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));

  if(plugin)
  {
    KMF::MediaObject *mob = plugin->createMediaObject(e);
    if(mob)
      addItem(mob);
  }
}

void KMFProject::templateFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));
  if(plugin)
  {
    m_template =
        plugin->findChild<KMF::TemplateObject*>(e.attribute("object"));
    if(m_template)
      m_template->fromXML(e);
  }
}

void KMFProject::outputFromXML(const QDomElement& e)
{
  KMF::Plugin *plugin =
      kmfApp->pluginInterface()->findChild<KMF::Plugin*>(e.attribute("plugin"));
  if(plugin)
  {
    m_output =
        plugin->findChild<KMF::OutputObject*>(e.attribute("object"));
    if(m_output)
      m_output->fromXML(e);
  }
}

void KMFProject::fromXML(QString xml)
{
  emit preinit(m_type);
  QDomDocument doc("kmf_project");
  doc.setContent(xml);
  QDomElement element = doc.documentElement();
  m_type = element.attribute("type");
  m_directory = element.attribute("dir");
  if(m_directory.startsWith("file://")) m_directory = m_directory.mid(7);
  m_title = element.attribute("title");
  m_serial = element.attribute("serial").toInt();

  if(element.hasAttribute("last_mod_media"))
    m_lastModified[ModMedia] =
    QDateTime::fromString(element.attribute("last_mod_media"), Qt::ISODate);
  if(element.hasAttribute("last_mod_template"))
    m_lastModified[ModTemplate] =
    QDateTime::fromString(element.attribute("last_mod_template"), Qt::ISODate);
  if(element.hasAttribute("last_mod_output"))
    m_lastModified[ModOutput] =
    QDateTime::fromString(element.attribute("last_mod_output"), Qt::ISODate);
  if(element.hasAttribute("last_subtype"))
    m_subType = element.attribute("last_subtype");

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "media")
      {
        mediaObjFromXML(e);
      }
      if(e.tagName() == "template")
      {
        templateFromXML(e);
      }
      if(e.tagName() == "output")
      {
        outputFromXML(e);
      }
    }
    n = n.nextSibling();
  }
  emit init(m_type);
}

void KMFProject::saveObj(QDomDocument& doc, QDomElement& root,
                         const char* name, KMF::Object* ob)
{
  if(ob)
  {
    QDomElement e = doc.createElement(name);
    if(ob->parent())
      e.setAttribute("plugin", ob->parent()->objectName());
    e.setAttribute("object", ob->objectName());
    ob->toXML(&e);
    root.appendChild(e);
  }
}

QString KMFProject::toXML()
{
  KMF::MediaObject *obj;
  QDomDocument doc("kmf_project");
  doc.appendChild(doc.createProcessingInstruction("xml",
                  "version=\"1.0\" encoding=\"UTF-8\""));

  QDomElement root=doc.createElement("project");
  root.setAttribute("type", m_type);
  root.setAttribute("dir", m_directory);
  root.setAttribute("title", m_title);
  root.setAttribute("serial", m_serial);
  root.setAttribute("last_mod_media",
                    m_lastModified[ModMedia].toString(Qt::ISODate));
  root.setAttribute("last_mod_template",
                    m_lastModified[ModTemplate].toString(Qt::ISODate));
  root.setAttribute("last_mod_output",
                    m_lastModified[ModOutput].toString(Qt::ISODate));
  root.setAttribute("last_subtype", m_subType);

  doc.appendChild(root);

  foreach(obj, m_list.list())
    saveObj(doc, root, "media", obj);
  saveObj(doc, root, "template", m_template);
  saveObj(doc, root, "output", m_output);
  return doc.toString();
}

QString KMFProject::directory(const QString& subDir, bool create) const
{
  QString result = m_directory;
  if(!subDir.isEmpty())
    result = KMF::Tools::joinPaths(result, subDir);

  QDir dir(result);
  if(create && !QDir(result).exists())
    dir.mkpath(result);
  return KMF::Tools::addSlash(result);
}

bool KMFProject::prepare(const QString& type)
{
  if(!validProject())
    return false;

  foreach(KMF::MediaObject *obj, m_list.list())
  {
    if(!obj->prepare(type))
    {
      return false;
      break;
    }
    CHECK_IF_STOPPED(false);
  }
  if (!m_template->prepare(type))
    return false;

  CHECK_IF_STOPPED(false);

  if (!m_output->prepare(type))
    return false;

  CHECK_IF_STOPPED(false);

  m_subType = type;
  return true;
}

void KMFProject::finished()
{
  foreach(KMF::MediaObject* obj, m_list.list())
    obj->finished();
  m_template->finished();
  m_output->finished();
}

bool KMFProject::open(const KUrl &url)
{
  bool result = false;
  QString tmp;

  m_loading = true;
  if (KMF::Tools::loadStringFromFile(url, &tmp))
  {
    m_url = url;
    setDirty(KMF::DirtyAny, false);
    result = true;
  }
  m_loading = false;
  return result;
}

bool KMFProject::save(KUrl url)
{
  bool saveas = !url.isEmpty();

  if(url.isEmpty())
  {
    if(m_url.isEmpty())
      return false;
    else
      url = m_url;
  }

  if(KIO::NetAccess::exists(url, KIO::NetAccess::DestinationSide,
                            kmfApp->widget()) && saveas)
  {
    if(KMessageBox::questionYesNo(kmfApp->activeWindow(),
        i18n("Do you want to overwrite %1", url.prettyUrl()))
      == KMessageBox::No)
    {
      return true;
    }
  }

  KMF::Tools::saveString2File(url, toXML());

  m_url = url;
  setDirty(KMF::DirtyAny, false);
  return true;
}

void KMFProject::setDirty(KMF::DirtyType type, bool dirty)
{
  m_modified = dirty;
  if(dirty && !m_loading && !m_initializing)
  {
    if(type & KMF::DirtyMedia)
      m_lastModified[ModMedia] = QDateTime::currentDateTime();
    if(type & KMF::DirtyTemplate)
      m_lastModified[ModTemplate] = QDateTime::currentDateTime();
    if(type & KMF::DirtyOutput)
      m_lastModified[ModOutput] = QDateTime::currentDateTime();
  }
  if(!m_initializing)
    emit modified(m_title, dirty);
  if(dirty && !m_loading && !m_initializing)
  {
    if(type & KMF::DirtyMedia)
    {
      kDebug() << "media modified";
      emit mediaModified();
    }
    if(type & KMF::DirtyTemplate)
      emit templatesModified();
    if(type & KMF::DirtyOutput)
      emit outputsModified();
  }
}

QDateTime KMFProject::lastModified(KMF::DirtyType type) const
{
  QDateTime d;

  if((type & KMF::DirtyMedia) &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModMedia];
  if((type & KMF::DirtyTemplate) &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModTemplate];
  if((type & KMF::DirtyOutput) &&
     d < m_lastModified[ModTemplate])
    d = m_lastModified[ModOutput];
  return d;
}

QMap<QString, QString> KMFProject::subTypes() const
{
  QMap<QString, QString> result;
  KMF::MediaObject* obj;

  foreach(obj, m_list.list())
  {
    if(obj)
      addMap(&result, obj->subTypes());
  }
  if(m_template)
    addMap(&result, m_template->subTypes());
  if(m_output)
    addMap(&result, m_output->subTypes());
  return result;
}

#include "kmfproject.moc"
