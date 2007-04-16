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
#include "kmfmenu.h"
#include "kmfwidgetfactory.h"
#include "kmftime.h"
#include "kaboutdata.h"
#include <kstore/KoStore.h>
#include <kdebug.h>
#include <klocale.h>
#include <kcomponentdata.h>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

KMFMenu::KMFMenu(const QString& tmplate, QObject *parent)
  : KMFTemplateBase(parent),
    m_template(tmplate), m_templateXML("kmf_project")
{
  m_templateXML.setContent(m_template.readFile("template.xml"));
  QDomElement element = m_templateXML.documentElement();
  m_id = element.attribute("id");
  m_title = element.attribute("name");

  QDomNodeList list = m_templateXML.elementsByTagName("page");
  for(int i=0; i < list.count(); ++i)
    m_menus.append(list.item(i).toElement().attribute("name"));
}

KMFMenu::~KMFMenu()
{
  foreach(QList<KMFMenuPage*> pages, m_pages)
    qDeleteAll(pages);
}

bool KMFMenu::writeDvdAuthorXml(QDomDocument& doc, QString type)
{
  QDomElement root = doc.createElement("dvdauthor");
  int i, j;
  QString header = i18n("\n"
    " *********************************************************************\n"
    " %1\n"
    " *********************************************************************\n "
    );
  QString comment = i18n("\n"
    "**********************************************************************\n"
    "\n"
    "This file was made with %1 - %2\n"
    "http://www.iki.fi/damu/software/kmediafactory/\n"
    "\n"
    "**********************************************************************\n"
    "\n"
    "Register usage:\n"
    "================\n"
    "g0: Playing chapter saved here.\n"
    "g1: Play this chapter when entering titleset menu. 0 == show menu.\n"
    "    Also used by Back button.\n"
    "g2: Used vmgm button is saved here\n"
    "g3: Jump to this titleset. Also used for direct play.\n"
    "g4: Used titleset button is saved here\n"
    "g5: vmgm page is saved here\n"
    "g6: titleset page is saved here\n"
    "g7: temporary variable\n"
    "\n"
    "**********************************************************************\n"
    , KGlobal::mainComponent().aboutData()->programName()
    , KGlobal::mainComponent().aboutData()->version());

  doc.appendChild(doc.createComment(comment));
  doc.appendChild(doc.createTextNode("\n"));

  root.setAttribute("dest", "./DVD");
  doc.appendChild(root);

  root.appendChild(doc.createTextNode("\n "));
  root.appendChild(doc.createComment(
      header.arg(i18n("Main menu for %1").arg(m_prjIf->title()))));
  root.appendChild(doc.createTextNode("\n "));

  QDomElement elem = doc.createElement("vmgm");

  QDomElement fpc = doc.createElement("fpc");

  int titleset = 0;
  if(templateObject()->property("%KMFMenuPage%", "direct_play").toBool())
    titleset = 1;
  fpc.appendChild(doc.createTextNode(QString(
      " { g0 = 0; g1 = 0; g2 = 0; g3 = %1; g4 = 0; g5 = 0; g6 = 0;"
      " jump menu 1; }").arg(titleset)));
  elem.appendChild(fpc);

  QDomElement menus = doc.createElement("menus");

  if(pages() > 0)
  {
    KMF::MediaObject *mob;
    QList<KMF::MediaObject*>* mobs = m_prjIf->mediaObjects();

    j=1;
    foreach(KMFMenuPage* ob, m_pages[0])
    {
      ob->setIndex(0, mobs->count(), j, m_pages[0].count());
      ob->writeDvdAuthorXml(menus, type);
      j++;
    }
    if(menus.hasChildNodes())
      elem.appendChild(menus);
    root.appendChild(elem);

    i=1;
    foreach(mob, *mobs)
    {
      root.appendChild(doc.createTextNode("\n "));
      root.appendChild(doc.createComment(header.arg(mob->text())));
      root.appendChild(doc.createTextNode("\n "));

      QDomElement elem = doc.createElement("titleset");
      QDomElement menus = doc.createElement("menus");
      if(i < m_pages.count() && m_pages[i].count() != 0)
      {
        j=1;
        foreach(KMFMenuPage* ob, m_pages[i])
        {
          ob->setIndex(i, mobs->count(), j, m_pages[i].count());
          ob->writeDvdAuthorXml(menus, type);
        }
      }
      if(!menus.hasChildNodes())
      {
        KMFMenuPage page(this);
        page.writeDvdAuthorXmlNoMenu(menus);
      }
      elem.appendChild(menus);

      QString post;

      if(i < mobs->count() && templateObject()->property("%KMFMenuPage%",
                                 "continue_to_next_title").toInt() == 1)
      {
        post = QString(" g3 = %1 ; ").arg(i+1);
      }
      post += " call vmgm menu 1 ; ";
      mob->writeDvdAuthorXml(elem, m_template.language(), post, type);
      root.appendChild(elem);
    }
    return true;
    //kdDebug() << doc.toString() << endl;
  }
  else
  {
    m_uiIf->message(KMF::Error, i18n("No pages."));
    return false;
  }
}

bool KMFMenu::writeDvdAuthorXml(const QString& fileName, QString type)
{
  QDomDocument doc("");
  doc.appendChild(doc.createProcessingInstruction("xml",
                  "version=\"1.0\" encoding=\"UTF-8\""));
  if(writeDvdAuthorXml(doc, type))
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
      return false;
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << doc.toString();
    file.close();
    return true;
  }
  return false;
}

bool KMFMenu::makeMenuMpegs()
{
  for(int i=0; i < m_pages.count(); ++i)
  {
    if(m_pages[i].count() > 0)
    {
      foreach(KMFMenuPage* ob,  m_pages[i])
      {
        progress(m_pagePoints);
        if(m_uiIf->message(KMF::Info,
           i18n("   Menu: %1", uiText(ob->objectName()))))
          return false;

        if(!ob->makeMpeg())
          return false;
      }
    }
  }
  return true;
}

QList<KMFMenuPage*>* KMFMenu::titlePages(int title)
{
  while(m_pages.count() <= title)
    m_pages.append(QList<KMFMenuPage*>());
  return &m_pages[title];
}

bool KMFMenu::addPage(const QDomElement& element, int pageSet,
                      int title, int chapter)
{
  KMFMenuPage* menuPage = static_cast<KMFMenuPage*>
      (KMFWidgetFactory::createPage(element, this, title, chapter));
  progress(m_pagePoints);
  if(menuPage)
  {
    if(m_uiIf->message(KMF::Info,
       i18n("   Menu: %1", uiText(menuPage->objectName()))))
      return false;
    if(pageSet == 0)
      menuPage->setVmgm(true);
    titlePages(pageSet)->append(menuPage);
    return menuPage->parseButtons();
  }
  return false;
}

bool KMFMenu::addPage(const QString& name, int title, int chapter)
{
  QDomElement element = m_templateXML.documentElement();
  QDomElement pageElement = getPage(element.firstChild(), name);
  QList<KMF::MediaObject*>* mobs = m_prjIf->mediaObjects();
  KMFMenuPage temp(this);

  temp.fromXML(pageElement);

  if(temp.titles() > 0)
  {
    for(int i = 0; i < ((mobs->count() - 1) / temp.titles()) + 1; ++i)
    {
      if(addPage(pageElement, title, i * temp.titles(), chapter) == false)
        return false;
    }
  }
  else if(temp.chapters() > 0)
  {
    if(title <= mobs->count())
    {
      for(int i = 0;
          i < ((mediaObjChapterCount(title-1) - 1) / temp.chapters()) + 1;
          ++i)
      {
        if(addPage(pageElement, title, title - 1, i * temp.chapters()) == false)
          return false;
      }
    }
  }
  else
  {
    return addPage(pageElement, title, title, chapter);
  }
  return true;
}

QDomElement KMFMenu::getPage(const QDomNode& node, const QString& name)
{
  QDomNode n = node;
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "page" && e.attribute("name") == name)
      {
        return e;
      }
    }
    n = n.nextSibling();
  }
  m_uiIf->message(KMF::Error,
                  i18n("Cannot find page %1 from template.", name));
  return QDomElement();
}

QImage KMFMenu::templateImage(const QString& image) const
{
  QImage img;
  img.loadFromData(m_template.readFile(image));
  return img;
}

QImage KMFMenu::makeMenuPreview(QString page)
{
  clear();
  if(page.isEmpty())
  {
    return templateImage("preview.jpg");
  }
  else
  {
    QDomElement element = m_templateXML.documentElement();
    QDomElement e = getPage(element.firstChild(), page);
    KMFMenuPage* p =
        static_cast<KMFMenuPage*>(KMFWidgetFactory::createPage(e, this));
    if(p)
    {
      p->parseButtons(false);
      p->paint();
      return p->layer(KMFMenuPage::Background);
    }
  }
  return QImage();
}

int KMFMenu::pages()
{
  int result = 0;

  for(int i=0; i < m_pages.count(); ++i)
  {
    for(int j=0; j < m_pages[i].count(); ++j)
    {
      ++result;
    }
  }
  return result;
}

void KMFMenu::progress(int points)
{
  if(points > m_points)
    points = m_points;
  m_points -= points;
  m_uiIf->progress(points);
}

bool KMFMenu::makeMenu(QString type)
{
  clear();
  m_points = TotalPoints / 4;
  m_pagePoints = m_points / ((m_prjIf->mediaObjects()->count() * 2) + 1);
  QDomElement element = m_templateXML.documentElement();
  QString page = element.attribute("first_page");

  if(m_uiIf->message(KMF::Info, i18n("Generating menus")))
    return false;
  if(addPage(page, 0, 0))
  {
    int p = pages();
    progress(TotalPoints);
    m_points = TotalPoints - (TotalPoints / 4);
    if(p > 0)
      m_pagePoints = m_points / p;
    // Generate jpgs
    m_uiIf->message(KMF::Info, i18n("Making menu mpegs"));
    if(makeMenuMpegs() == false)
      return false;

    // Generate xml
    if(m_uiIf->message(KMF::Info, i18n("Generating DVDAuthor xml")))
      return false;
    bool result = writeDvdAuthorXml(m_prjIf->projectDir() + "/dvdauthor.xml",
                                   type);
    progress(TotalPoints);
    clear();
    return result;
  }
  return false;
}

int KMFMenu::mediaObjCount()
{
  return m_prjIf->mediaObjects()->count();
}

int KMFMenu::mediaObjChapterCount(int title)
{
  QList<KMF::MediaObject*>* mobs = m_prjIf->mediaObjects();
  KMF::MediaObject* mob = mobs->at(title);
  int result = mob->chapters();
  KMF::Time chapter = mob->chapterTime(result);

  // Don't put chapter selection for chapter really close to end
  //kdDebug() << k_funcinfo << mob->duration() << ", " << chapter << endl;
  if(chapter + 3.0 > mob->duration() && result > 1)
    --result;
  return result;
}

#include "kmfmenu.moc"
