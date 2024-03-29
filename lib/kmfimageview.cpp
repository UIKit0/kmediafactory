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

#include "kmfimageview.h"

#include <QtGui/QContextMenuEvent>

#include <KDebug>

KMFImageView::KMFImageView(QWidget *parent)
    : QGraphicsView(parent)
    , m_imageItem(0)
    , m_scaled(false)
{
    setScene(&m_scene);
}

KMFImageView::~KMFImageView()
{
}

void KMFImageView::setImage(const QImage &image)
{
    m_image = image;
    newImage();
}

void KMFImageView::clear()
{
    m_image = QImage();
    newImage();
}

void KMFImageView::setScaled(bool scaled)
{
    m_scaled = scaled;
    scale();
}

void KMFImageView::scale()
{
    if (m_scaled) {
        if (m_imageItem) {
            fitInView(m_imageItem);
        }
    } else   {
        fitInView(0, 0, viewport()->width(), viewport()->height());
    }
}

void KMFImageView::newImage()
{
    if (m_imageItem) {
        m_scene.removeItem(m_imageItem);
        delete m_imageItem;
        m_imageItem = 0;
    }

    if (!m_image.isNull()) {
        m_scene.setSceneRect(0, 0, m_image.width(), m_image.height());
        m_imageItem = m_scene.addPixmap(QPixmap::fromImage(m_image));
        m_imageItem->setTransformationMode(Qt::SmoothTransformation);
        scale();
    }
}

void KMFImageView::resizeEvent(QResizeEvent *)
{
    scale();
}

// From QTable
void KMFImageView::contextMenuEvent(QContextMenuEvent *e)
{
    if (!receivers(SIGNAL(contextMenuRequested(const QPoint &)))) {
        e->ignore();
        return;
    }

    if (e->reason() == QContextMenuEvent::Keyboard) {
        emit contextMenuRequested(rect().center());
    } else   {
        emit contextMenuRequested(e->globalPos());
    }
}

#include "kmfimageview.moc"
