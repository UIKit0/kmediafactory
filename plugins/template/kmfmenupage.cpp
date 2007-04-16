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
#include "kmfmenupage.h"
#include "kmfbutton.h"
#include "kmfgeometry.h"
#include "kmfmenu.h"
#include "templateobject.h"
#include "templatepluginsettings.h"
#include <kmftime.h>
#include <kmftools.h>
#include <run.h>
#include <kmediafactory/projectinterface.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <QVariant>
#include <QObject>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <QPainter>
#include <sys/types.h>
#include <sys/stat.h>

KMFMenuPage::KMFMenuPage(QObject *parent) :
  KMFWidget(parent), m_language("en"), m_modifiedLayers(0), m_titles(0),
  m_chapters(0), m_titleStart(0), m_chapterStart(0), m_index(0), m_titleset(0),
  m_titlesetCount(0), m_count(0), m_vmgm(false),
  m_directPlay(false), m_directChapterPlay(false), m_continueToNextTitle(true)
{
  m_buttons = new QList<KMFButton*>;
  setResolution(KMF::Tools::maxResolution(m_prjIf->type()));
}

KMFMenuPage::~KMFMenuPage()
{
  qDeleteAll(*m_buttons);
  delete m_buttons;
}

void KMFMenuPage::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_titles = element.attribute("titles", "0").toInt();
  m_chapters = element.attribute("chapters", "0").toInt();
  QString s;
  if(m_titles > 0)
    s = QString("%1").arg((m_titleStart / m_titles) +1);
  else if(m_chapters > 0)
    s = QString("%1_%2").arg(m_titleStart+1)
                        .arg((m_chapterStart / m_chapters) +1);
  else if(m_chapters > 0)
    s = QString("%1_%2").arg(m_titleStart+1)
                        .arg(m_chapterStart+1);
  QString n = QString("%1_%2").arg(objectName()).arg(s);
  setObjectName(n);
}

void KMFMenuPage::setResolution(QSize resolution)
{
  KMFUnit::setMaxRes(resolution);
  m_resolution = resolution;
  m_background = QImage(m_resolution, QImage::Format_ARGB32);
  m_background.fill(KMF::Tools::toColor("#444444FF").rgba());
  m_background.setDotsPerMeterX(DPM);
  m_background.setDotsPerMeterY(DPM);
  m_sub = QImage(m_resolution, QImage::Format_ARGB32);
  m_sub.fill(KMF::Tools::toColor("#000000FF").rgba());
  m_sub.setDotsPerMeterX(DPM);
  m_sub.setDotsPerMeterY(DPM);
  m_subHighlight = QImage(m_resolution, QImage::Format_ARGB32);
  m_subHighlight.fill(KMF::Tools::toColor("#000000FF").rgba());
  m_subHighlight.setDotsPerMeterX(DPM);
  m_subHighlight.setDotsPerMeterY(DPM);
  m_subSelect = QImage(m_resolution, QImage::Format_ARGB32);
  m_subSelect.fill(KMF::Tools::toColor("#000000FF").rgba());
  m_subSelect.setDotsPerMeterX(DPM);
  m_subSelect.setDotsPerMeterY(DPM);
  m_temp = QImage(m_resolution, QImage::Format_ARGB32);
  m_temp.setDotsPerMeterX(DPM);
  m_temp.setDotsPerMeterY(DPM);
  geometry().left().set(0, KMFUnit::Absolute);
  geometry().top().set(0, KMFUnit::Absolute);
  geometry().width().set(resolution.width(), KMFUnit::Absolute);
  geometry().height().set(resolution.height(), KMFUnit::Absolute);
}

bool KMFMenuPage::writeSpumuxXml(QDomDocument& doc)
{
  QDomElement root = doc.createElement("subpictures");
  QDomElement stream = doc.createElement("stream");
  QDomElement spu = doc.createElement("spu");

  spu.setAttribute("end", "00:00:00.00");
  if(m_modifiedLayers & Sub)
    spu.setAttribute("image", QString("%1_sub.png").arg(objectName()));
  if(m_modifiedLayers & Highlight)
    spu.setAttribute("highlight",
                     QString("%1_highlight.png").arg(objectName()));
  if(m_modifiedLayers & Select)
    spu.setAttribute("select",
                     QString("%1_select.png").arg(objectName()));
  spu.setAttribute("force", "yes");

  // We can't search and loop in the same list concurrently
  QList<KMFButton*> temp = *m_buttons;
  foreach(KMFButton *btn, temp)
  {
    if(btn->isHidden())
      continue;
    /*QDomComment comment =
        doc.createComment(QString("Button: %1").arg(btn->objectName()));
    spu.appendChild(comment);*/
    QDomElement button = doc.createElement("button");
    button.setAttribute("name", btn->objectName());
    QRect rc = btn->paintRect();
    //kdDebug() << rc << endl;
    button.setAttribute("x0", rc.left());
    button.setAttribute("y0", rc.top());
    button.setAttribute("x1", rc.right());
    button.setAttribute("y1", rc.bottom());
    if(btn->down())
      button.setAttribute("down", btn->down()->objectName());
    if(btn->up())
      button.setAttribute("up", btn->up()->objectName());
    if(btn->right())
      button.setAttribute("right", btn->right()->objectName());
    if(btn->left())
      button.setAttribute("left", btn->left()->objectName());
    spu.appendChild(button);
  }
  stream.appendChild(spu);
  root.appendChild(stream);
  doc.appendChild(root);
  return true;
}

bool KMFMenuPage::parseButtons(bool addPages)
{
  //kdDebug() << k_funcinfo << endl;
  foreach(KMFButton* btn, *m_buttons)
    if(btn->parseJump(addPages) == false)
      return false;
  foreach(KMFButton* btn, *m_buttons)
    btn->parseDirections();
  return true;
}

void KMFMenuPage::writeDvdAuthorXml(QDomElement& element, QString type)
{
  QDomDocument doc = element.ownerDocument();
  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");

  checkDummyVideo();
  if(isVmgm())
  {
    if(isFirst())
      pgc.setAttribute("entry", "title");

    QString s = "\n    {\n";
    // Direct play
    if(isFirst())
    {
      s += "      if (g3 gt 0)\n"
           "      {\n"
           "        g7 = g3;\n"
           "        g3 = 0;\n"
           "        g1 = 1;\n";
      for(int i = 1; i <= m_titlesetCount; ++i)
      {
        s += QString("        if(g7 eq %1)\n").arg(i);
        s += "        {\n";
        s += QString("          jump titleset %1 menu; \n").arg(i);
        s += "        }\n";
      }
      s += "      }\n";
    }

    // Jump to saved page if necessary
    if(isFirst())
      for(int i = 2; i <= m_count; ++i)
        s += QString("      if (g5 eq %1)\n"
                     "        jump menu %2;\n").arg(i).arg(i);

    // Highlight saved button
    s += "      if (g2 lt 1024)\n"
         "        button = 1024;\n"
         "      else\n"
         "        button = g2;\n";
    // Reset titleset page index
    s += QString("      g6 = 0;\n");
    // Reset titleset page button
    s += QString("      g4 = 0;\n");
    // Reset played chapter
    s += QString("      g0 = 0;\n");
    // Close code
    s += "    }\n    ";
    text.setData(s);
  }
  else
  {
    if(isFirst())
      pgc.setAttribute("entry", "root");

    QString s = "\n    {\n";
    // If chapter specified jump there
    if(isFirst())
      s += "      if (g1 gt 0)\n"
           "      {\n"
           "        g0 = g1;\n"
           "        g1 = 0;\n"
           "        jump title 1 chapter 1;\n"
           "      }\n";

    // Jump to saved page if necessary
    if(isFirst())
      for(int i = 2; i <= m_count; ++i)
        s += QString("      if (g6 eq %1)\n"
                     "        jump menu %1;\n").arg(i).arg(i);

    // Highlight saved button
    s += "      if (g4 lt 1024)\n"
         "        button = 1024;\n"
         "      else\n"
         "        button = g4;\n";
    // Close code
    s += "    }\n    ";
    text.setData(s);
  }
  pre.appendChild(text);
  pgc.appendChild(pre);

  pgc.setAttribute("pause", "0");
  QDomElement vob = doc.createElement("vob");
  QString file = QString("./menus/%1.mpg").arg(objectName());
  vob.setAttribute("file", file);
  vob.setAttribute("pause", "inf");
  pgc.appendChild(vob);

  foreach(KMFButton* btn, *m_buttons)
  {
    if(btn->isHidden())
      continue;
    QDomElement button = doc.createElement("button");
    button.setAttribute("name", btn->objectName());
    QDomText text = doc.createTextNode("");
    btn->writeDvdAuthorXml(text, type);
    button.appendChild(text);
    pgc.appendChild(button);
  }
  element.appendChild(pgc);
}

void KMFMenuPage::checkDummyVideo()
{
  QFileInfo fi(m_prjIf->projectDir("media") + "dummy.mpg");

  if(!fi.exists())
  {
    QImage temp;
    QString size;

    if(m_prjIf->type() == "DVD-PAL")
      temp = QImage(720, 576, QImage::Format_RGB32);
    else
      temp = QImage(720, 480, QImage::Format_RGB32);
    temp.fill(QColor("black").rgba());
    // PNM P6 256
    temp.save(m_prjIf->projectDir("media") + "dummy.pnm", "PPM");
    m_sound = "";
    runScript("dummy", "media");
  }
}

void KMFMenuPage::writeDvdAuthorXmlNoMenu(QDomElement& element)
{
  QDomDocument doc = element.ownerDocument();
  QDomElement pgc = doc.createElement("pgc");
  QDomElement pre = doc.createElement("pre");
  QDomText text = doc.createTextNode("");

  checkDummyVideo();
  pgc.setAttribute("entry", "root");

  QString s = "\n    {\n";
  // If chapter specified jump there
  s += "      if (g1 gt 0)\n"
      "      {\n"
      "        g1 = 0;\n"
      "        jump title 1 chapter 1;\n"
      "      }\n"
      "      else\n"
      "      {\n"
      "        jump vmgm menu ;\n"
      "      }\n";
  // Close code
  s += "    }\n    ";

  text.setData(s);
  pre.appendChild(text);
  pgc.appendChild(pre);

  pgc.setAttribute("pause", "0");
  QDomElement vob = doc.createElement("vob");
  QString file = "./media/dummy.mpg";
  vob.setAttribute("file", file);
  vob.setAttribute("pause", "inf");
  pgc.appendChild(vob);
  element.appendChild(pgc);
}

bool KMFMenuPage::saveImages()
{
  QString file;
  QString menus = m_prjIf->projectDir("menus");

  // Save subpicture files
  if(m_modifiedLayers & Sub)
  {
    file = menus + QString("%1_sub.png").arg(objectName());
    m_sub.save(file);
  }

  if(m_modifiedLayers & Highlight)
  {
    file = menus + QString("%1_highlight.png").arg(objectName());
    m_subHighlight.save(file);
  }

  if(m_modifiedLayers & Select)
  {
    file = menus + QString("%1_select.png").arg(objectName());
    m_subSelect.save(file);
  }

  file = menus + QString("%1.pnm").arg(objectName());
  // PNM P6 256
  return m_background.save(file, "PPM");
}

bool KMFMenuPage::writeSpumuxXml()
{
  // Write spumux xml
  QString s;
  QDomDocument doc("");
  if(writeSpumuxXml(doc) == false)
    return false;

  QFile file(m_prjIf->projectDir("menus") + objectName() + ".xml");
  if (!file.open(QIODevice::WriteOnly))
    return false;
  QTextStream stream(&file);
  stream << doc.toString();
  file.close();
  return true;
}

bool KMFMenuPage::runScript(QString scriptName, QString place)
{
  QString menuSound = "silence.mp2";
  KMF::Time seconds = 1.0;

  if(m_sound.isEmpty())
  {
    menuSound = KStandardDirs::locate("data",
                                      "kmediafactory/media/silence.mp2");
  }
  else
  {
    menuSound = m_sound;
  }

  uint frames;
  if(m_prjIf->type() == "DVD-PAL")
  {
    frames = (uint)(seconds.toSeconds() * 25.0);
  }
  else // NTSC
  {
    frames = (uint)(seconds.toSeconds() * 30000.0 / 1001.0);
  }

  Run run(QString("make_mpeg %1 %2 %3 %4").arg(m_prjIf->type()).arg(frames)
          .arg(scriptName).arg(menuSound),
          m_prjIf->projectDir(place));

  m_uiIf->logger()->message(run.output());
  if(run.result() != 0)
  {
    m_uiIf->message(KMF::Error, i18n("   Conversion error."));
    return false;
  }
  return true;
}

bool KMFMenuPage::makeMpeg()
{
  QString file;

  QDir dir(m_prjIf->projectDir("menus"));

  if(paint() == false)
    return false;
  if(writeSpumuxXml() == false)
    return false;
  if(saveImages() == false)
    return false;
  if(runScript(objectName()) == false)
    return false;
  return true;
}

QImage& KMFMenuPage::layer(Layer layer)
{
  switch(layer)
  {
    case Temp:
      return m_temp;
    case Sub:
      return m_sub;
    case Highlight:
      return m_subHighlight;
    case Select:
      return m_subSelect;
    case None:
    case Background:
    default:
      return m_background;
  }
  return m_background;
}

bool KMFMenuPage::paint()
{
  m_modifiedLayers = 0;
  return paintChildWidgets(this);
}

bool KMFMenuPage::paintChildWidgets(QObject* parent)
{
  foreach(QObject *obj, parent->children())
  {
    if(obj->inherits("KMFWidget"))
    {
      KMFWidget* widget = static_cast<KMFWidget*>(obj);
      if(widget->isHidden())
        continue;
      if(TemplatePluginSettings::widgetDebug())
      {
        QRect paintRC = widget->paintRect();
        QRect rc = widget->rect();
        QString type = widget->metaObject()->className();
        QColor color;

        if(TemplatePluginSettings::widgetDebugInfo())
          kDebug() << type << " (" << widget->objectName() << "): " << rc
              << " - " << paintRC << endl;

        if(type == "KMFImage" &&
            TemplatePluginSettings::widgetDebugImage())
        {
          color.setNamedColor("blue");
        }
        else if(type == "KMFLabel" &&
                TemplatePluginSettings::widgetDebugLabel())
        {
          color.setNamedColor("red");
        }
        else if(type == "KMFFrame" &&
                TemplatePluginSettings::widgetDebugFrame())
        {
          color.setNamedColor("green");
        }
        else if(type == "KMFButton" &&
                TemplatePluginSettings::widgetDebugButton())
        {
          color.setNamedColor("yellow");
        }
        else if((type == "KMFVBox" || type == "KMFHBox")
                  && TemplatePluginSettings::widgetDebugBox())
        {
          color.setNamedColor("white");
        }
        else if(type == "KMWidget" &&
                TemplatePluginSettings::widgetDebugWidget())
        {
          color.setNamedColor("gray");
        }
        if(color.isValid())
        {
          QPainter p(&m_background);

          p.setPen(QPen(QBrush(color), 1));
          if(TemplatePluginSettings::withMargin())
          {
            color.setAlphaF(0.1);
            p.setBrush(QBrush(color));
            p.drawRect(rc);
          }
          color.setAlphaF(0.3);
          p.setBrush(QBrush(color));
          p.drawRect(rc);
        }
      }
      widget->paint(this);
      m_modifiedLayers |= widget->layer();
      if(paintChildWidgets(obj) == false)
        return false;
      // Just to make ui more responsible
      if(m_uiIf->progress(0))
        return false;
    }
  }
  return true;
}

KMFButton* KMFMenuPage::button(const QString& name)
{
  // We can't search and loop in the same list concurrently
  QList<KMFButton*> temp = *m_buttons;
  foreach(KMFButton *btn, temp)
  {
    if(!btn->isHidden() && btn->objectName() == name)
      return btn;
  }
  return 0;
}

void KMFMenuPage::setProperty(const QString& name, QVariant value)
{
  KMFWidget::setProperty(name, value);
  if(name == "language")
  {
    //kdDebug() << k_funcinfo << "Setting language" << endl;
    m_language = value.toString();
    menu()->setLanguage("template", m_language);
  }
  else if(name == "direct_play")
    m_directPlay = value.toBool();
  else if(name == "direct_chapter_play")
    m_directChapterPlay = value.toBool();
  else if(name == "sound")
    m_sound = value.toString();
  else if(name == "continue_to_next_title")
    m_continueToNextTitle = value.toInt();
}

KMFWidget::Layer KMFMenuPage::layerType(const QImage& img)
{
  if(&img == &m_background)
    return Background;
  else if(&img == &m_sub)
    return Background;
  else if(&img == &m_subHighlight)
    return Highlight;
  else if(&img == &m_subSelect)
    return Select;
  else if(&img == &m_temp)
    return Temp;
  return None;
}

#include "kmfmenupage.moc"