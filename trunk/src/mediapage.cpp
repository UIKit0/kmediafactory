//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "mediapage.h"
#include "kmediafactory.h"
#include "kmfapplication.h"
#include "sizewidget.h"
#include <kmftools.h>
#include <KXMLGUIFactory>
#include <KActionCollection>
#include <QPoint>
#include <QMenu>
#include <QTimer>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QCursor>

#define SHOW_ALL_ACTIONS
#define SHOW_ACTIONS_ON_LEFT

class MediaItemDelegate : public QStyledItemDelegate
{
    public:
        
    MediaItemDelegate(QObject *p, QWidget *widget) : QStyledItemDelegate(p), m_widget(widget) { }
    virtual ~MediaItemDelegate() { }
    
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize sz(QStyledItemDelegate::sizeHint(option, index));
        int   textHeight=m_widget? m_widget->fontMetrics().height() : QApplication::fontMetrics().height();
        return QSize(qMax(sz.width(), KMF::MediaObject::constIconSize)+(constBorder*2), qMax((textHeight+constBorder)*3, sz.height()+(constBorder*2)));
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (!index.isValid())
            return;

        QVariant dec=index.data(Qt::DecorationRole);
        QVariant disp=index.data(Qt::DisplayRole);
        QVariant user=index.data(Qt::UserRole);

        if(QVariant::Pixmap==dec.type() && QVariant::String==disp.type() && QVariant::String==user.type())
        {
            m_widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0L);

            QRect                r(option.rect);
            QFont                fnt(m_widget->font());
            QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                                            ? QPalette::Normal : QPalette::Disabled;
            QPixmap              pix(dec.value<QPixmap>());
            int                  textHeight=m_widget->fontMetrics().height(),
                                 iconPosModX=constBorder+((KMF::MediaObject::constIconSize-pix.width())/2),
                                 iconPosModY=(option.rect.height()-pix.height())/2;
            
            painter->drawPixmap(r.adjusted(iconPosModX, iconPosModY, iconPosModX, iconPosModY).topLeft(), pix);
            
            fnt.setBold(true);
            
            painter->setFont(fnt);
            
            if (QPalette::Normal==cg && !(option.state & QStyle::State_Active))
                cg = QPalette::Inactive;

            r.moveLeft(KMF::MediaObject::constIconSize+(constBorder*2));
            r.moveTop(r.y()+(constBorder+textHeight));
            painter->setPen(option.palette.color(cg, option.state&QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
            painter->drawText(r.topLeft(), disp.toString());
            r.moveTop(r.y()+(constBorder+textHeight));
            painter->setFont(m_widget->font());
            painter->drawText(r.topLeft(), user.toString());
            
            if (option.state & QStyle::State_HasFocus)
            {
                QStyleOptionFocusRect o;
                o.QStyleOption::operator=(option);
                o.rect = option.rect; // m_widget->style()->subElementRect(QStyle::SE_ItemViewItemFocusRect, &option, m_widget);
                o.state |= QStyle::State_KeyboardFocusChange;
                o.state |= QStyle::State_Item;
                cg = option.state&QStyle::State_Enabled  ? QPalette::Normal : QPalette::Disabled;
                o.backgroundColor = option.palette.color(cg, option.state&QStyle::State_Selected ? QPalette::Highlight : QPalette::Window);
                m_widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, m_widget);
            }

            if (option.state & QStyle::State_MouseOver && kmfApp->project()->mediaObjects()->isValid(index))
            {
                KMF::MediaObject* ob = kmfApp->project()->mediaObjects()->at(index);
                QList<QAction*> actions;
                
                ob->actions(&actions);

                r=option.rect;

                QSize                          size(constActionIconSize, constActionIconSize);
#ifdef SHOW_ALL_ACTIONS
                QList<QAction*>::ConstIterator it(actions.begin()),
                                               end(actions.end());
                int                            yOffset=constBorder,
#ifdef SHOW_ACTIONS_ON_LEFT
                                               xPos=r.x()+(constBorder/2);
#else
                                               xPos=r.x()+r.width()-(1+(constBorder/2)+constActionIconSize);
#endif
                for(; it!=end && (yOffset+constActionIconSize+constBorder)<r.height(); ++it)
                {
                    painter->drawPixmap(QRect(xPos, r.y()+yOffset,
                                              constActionIconSize, constActionIconSize),
                                              (*it)->icon().pixmap(size, QIcon::Normal, QIcon::Off));
                    yOffset+=constActionIconSize;
                }

                if((yOffset+constActionIconSize+constBorder)<r.height())
                        painter->drawPixmap(QRect(xPos, r.y()+yOffset,
                                                  constActionIconSize, constActionIconSize),
                                                  KIcon("edit-delete").pixmap(size, QIcon::Normal, QIcon::Off));
#else
                if(actions.count())
                    painter->drawPixmap(QRect(option.rect.x()+constBorder, option.rect.y()+constBorder,
                                              constActionIconSize, constActionIconSize),
                                        KIcon("arrow-down").pixmap(size, QIcon::Normal, QIcon::Off));
#endif
            }
        }
        else
            QStyledItemDelegate::paint(painter, option, index);
    }

    static const int constActionIconSize=24;
    static const int constBorder=6;
    
    QWidget *m_widget;
};

MediaPage::MediaPage(QWidget *parent) :
  QWidget(parent)
{
  setupUi(this);

//   mediaFiles->setSpacing(5);
  mediaFiles->setItemDelegate(new MediaItemDelegate(this, mediaFiles));
//   mediaFiles->setIconSize(QSize(128, 128));
  connect(mediaFiles, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenuRequested(const QPoint&)));
  connect(mediaFiles, SIGNAL(clicked(const QModelIndex&)),
          this, SLOT(itemClicked(const QModelIndex&)));
}

MediaPage::~MediaPage()
{
}

void MediaPage::projectInit()
{
  mediaModified();
  mediaFiles->setModel(kmfApp->project()->mediaObjects());
}

void MediaPage::mediaModified()
{
  calculateSizes();

  QList<KMF::MediaObject*> mobs = kmfApp->project()->mediaObjects()->list();
  KMF::MediaObject         *mob;
  QTime                    duration;

  foreach(mob, mobs)
    duration = duration.addMSecs((mob->duration().hour()*60*60*1000)+
                                 (mob->duration().minute()*60*1000)+
                                 (mob->duration().second()*1000)+
                                  mob->duration().msec());

  emit details(i18n("Titles: %1 Duration: %2",
                    mobs.count(),
                    KGlobal::locale()->formatTime(duration, true, true)));
}

void MediaPage::contextMenuRequested(const QPoint &pos)
{
  QModelIndex i = mediaFiles->indexAt(pos);

  if(!kmfApp->project()->mediaObjects()->isValid(i))
    return;

  showMenu(i, mediaFiles->viewport()->mapToGlobal(pos));
}

void MediaPage::itemClicked(const QModelIndex& index)
{
  QRect  r(mediaFiles->visualRect(index));
  QPoint pos(mediaFiles->viewport()->mapToGlobal(QPoint(0, 0)));

#ifdef SHOW_ALL_ACTIONS
  KMF::MediaObject* ob = kmfApp->project()->mediaObjects()->at(index);
  QList<QAction*> actions;
  ob->actions(&actions);

  QList<QAction*>::ConstIterator it(actions.begin()),
                                 end(actions.end());
  int                            yOffset=MediaItemDelegate::constBorder,
                                 actWidth=MediaItemDelegate::constActionIconSize+8,
#ifdef SHOW_ACTIONS_ON_LEFT
                                 xPos=r.x()+pos.x()+(MediaItemDelegate::constBorder/2);
#else
                                 xPos=r.x()+pos.x()+r.width()-(1+(MediaItemDelegate::constBorder/2)+
                                                               MediaItemDelegate::constActionIconSize);
#endif

  for(; it!=end && (yOffset+MediaItemDelegate::constActionIconSize+MediaItemDelegate::constBorder)<r.height(); ++it)
  {
    if(QRect(xPos, r.y()+pos.y()+yOffset,
             actWidth, MediaItemDelegate::constActionIconSize).contains(QCursor::pos()))
    {
      (*it)->trigger();
      return;
    }
    yOffset+=MediaItemDelegate::constActionIconSize;
  }

  if((yOffset+MediaItemDelegate::constActionIconSize+MediaItemDelegate::constBorder)<r.height() &&
      QRect(xPos, r.y()+pos.y()+yOffset,
            actWidth, MediaItemDelegate::constActionIconSize).contains(QCursor::pos()))
    kmfApp->mainWindow()->actionCollection()->action("delete")->trigger();
#else
  r=QRect(r.x()+pos.x()+MediaItemDelegate::constBorder,
          r.y()+pos.y()+MediaItemDelegate::constBorder,
          MediaItemDelegate::constActionIconSize, MediaItemDelegate::constActionIconSize);

  if(r.contains(QCursor::pos()))
    showMenu(index, QCursor::pos());
#endif
}

void MediaPage::showMenu(const QModelIndex& index, const QPoint &pos)
{
  KMF::MediaObject* ob = kmfApp->project()->mediaObjects()->at(index);
  KMediaFactory* mainWindow = kmfApp->mainWindow();
  KXMLGUIFactory* factory = mainWindow->factory();

  QList<QAction*> actions;
  ob->actions(&actions);
  factory->plugActionList(mainWindow,
      QString::fromLatin1("media_file_actionlist"), actions);
  QWidget *w = factory->container("media_file_popup", mainWindow);
  if(w)
  {
    QMenu* popup = static_cast<QMenu*>(w);
    popup->exec(pos);
  }
  factory->unplugActionList(mainWindow, "media_file_actionlist");
}

void MediaPage::calculateSizes()
{
  quint64 max = 4700372992LL;
  quint64 size = 0;

  if(kmfApp->project())
  {
    QList<KMF::MediaObject*> mobs = kmfApp->project()->mediaObjects()->list();
    KMF::MediaObject *mob;

    foreach(mob, mobs)
      size += mob->size();
    size += mobs.size() * 200 * 1024; // Not very good estimate...
  }

  emit sizes(max, size);
}

#include "mediapage.moc"
