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

#ifndef CHAPTERS_H
#define CHAPTERS_H

#include <Phonon/VideoPlayer>

#include <kmf_stddef.h>
#include <kmftime.h>
// How to use namespaces / Phonon in ui files?
using namespace Phonon;
#include <ui_chapters.h>
#include "videooptions.h"

class CellListModel;

class Chapters : public QWidget, public Ui::Chapters
{
    Q_OBJECT

    public:
        Chapters(QWidget *parent = 0);
        ~Chapters();

        void setData(const QDVD::CellList &cells, const VideoObject *obj, VideoOptions *vidOpt);

        const QDVD::CellList& cells() const
        {
            return m_cells;
        }

        bool ok();

    protected slots:
        void slotForwardShort();
        void slotRewindShort();
        void slotForward();
        void slotRewind();
        void slotStart();
        void slotEnd();
        void slotRemove();
        void slotAdd();
        void slotSelectionChanged();
        void slotSliderMoved(int value);
        void slotContextMenu(const QPoint &p);
        void renameAll();
        void deleteAll();
        void autoChapters();
        void import();
        void saveCustomPreview();
        void slotPlay();
        void slotTick(qint64 time);
        void slotTotalTime(qint64 totalTime);
        void setThumbnail();
        void chapterThumbnailButtonClicked();
        void unsetThumbnail();
        void chapterDoubleClicked(const QModelIndex &index);

    private:
        void setThumbnail(int index);

    protected:
        virtual void showEvent(QShowEvent *event);

    private:
        const VideoObject *m_obj;
        VideoOptions *m_vidOpt;
        KMF::Time m_pos;
        QString m_duration;
        QDVD::CellList m_cells;
        CellListModel *m_model;
        QString m_lastFile;
        KMF::Time m_difference;
        qreal m_msecPhononRatio;

        void seekVideo(int msec);
        void setSliderAndTime(int msec);
        void moveMSecs(int direction);
        void updateVideo();
        void checkLengths();
        QModelIndex popupIndex;
};

#endif
