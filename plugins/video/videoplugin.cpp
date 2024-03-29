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

#include <config.h>
#include "videoplugin.h"

#include <QtCore/QRegExp>
#include <QtGui/QCheckBox>
#include <QtGui/QPixmap>

#include <KAboutData>
#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KFileDialog>
#include <KGenericFactory>
#include <KGlobal>
#include <KIcon>
#include <KIconLoader>
#include <KMessageBox>
#include <KStandardDirs>
#include <kdeversion.h>

#include <kmfmediafile.h>
#include <ui_videoconfig.h>
#include <videopluginsettings.h>
#include "videoobject.h"

K_EXPORT_KMEDIAFACTORY_PLUGIN(video, VideoPlugin)

class VideoConfig : public QWidget, public Ui::VideoConfig
{
    public:
        VideoConfig(QWidget *parent = 0) : QWidget(parent)
        {
            setupUi(this);
        };
};

VideoPlugin::VideoPlugin(QObject *parent, const QVariantList &)
    : KMF::Plugin(parent)
{
    KGlobal::locale()->insertCatalog("kmediafactory_video");
    setObjectName("KMFImportVideo");
    setupActions();
}

VideoPlugin::~VideoPlugin()
{
}

const KMF::ConfigPage *VideoPlugin::configPage() const
{
    KMF::ConfigPage *configPage = new KMF::ConfigPage;

    configPage->page = new VideoConfig;
    configPage->config = VideoPluginSettings::self();
    configPage->itemName = i18n("Video");
    configPage->itemDescription = i18n("Video Settings");
    configPage->pixmapName = "video-mpeg";
    return configPage;
}

QAction *VideoPlugin::setupActions()
{
    // Add action for menu item
    QAction *addVideoAction = new KAction(KIcon("video-mpeg"), i18n("Add Video"),
            parent());

    // addVideoAction->setShortcut(Qt::CTRL + Qt::Key_V);
    actionCollection()->addAction("video", addVideoAction);
    connect(addVideoAction, SIGNAL(triggered()), SLOT(slotAddVideo()));

    setXMLFile("kmediafactory_videoui.rc");

    interface()->addMediaAction(addVideoAction);

    return addVideoAction;
}

void VideoPlugin::init(const QString &type)
{
    kDebug() << type;
    deleteChildren();

    QAction *action = actionCollection()->action("video");

    if (!action) {
        return;
    }

    if (type.left(3) == "DVD") {
        action->setEnabled(true);
    } else   {
        action->setEnabled(false);
    }
}

void VideoPlugin::slotAddVideo()
{
    QCheckBox *multipleFiles = new QCheckBox(0);
    QPointer<KFileDialog> dlg = new KFileDialog(KUrl("kfiledialog:///<AddVideo>"),
                    "video/mpeg",
                    kapp->activeWindow(), multipleFiles);

    multipleFiles->setText(i18n("Multiple files make multiple titles."));
    multipleFiles->setChecked(true);
    dlg->setOperationMode(KFileDialog::Opening);
    dlg->setCaption(i18n("Select Video Files"));
    dlg->setMode(KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);
    dlg->exec();

    QStringList filenames = dlg->selectedFiles();

    KMF::PluginInterface *m = interface();

    kDebug() << m << filenames;

    if (m && (filenames.count() > 0)) {
        VideoObject *vob = 0;

        foreach (const QString& filename, filenames) {
            QFileInfo fi(filename);

            if (fi.isDir()) {
                KMessageBox::error(dlg->parentWidget(),
                        i18n("Cannot add directory."));
                continue;
            }

            if (multipleFiles->isChecked() || (filename == filenames.first())) {
                vob = new VideoObject(this);
            }

            switch (vob->addFile(filename)) {
                case VideoObject::StatusOk:
                    break;

                case VideoObject::StatusInvalidResolution:
                {
                    QSize res(KMFMediaFile::mediaFile(filename).resolution());
                    KMessageBox::error(dlg->parentWidget(),
                            i18n("Cannot use %1.\n%2x%3 is an invalid resolution",
                                 filename, res.width(), res.height()));
                    delete vob;
                    vob = 0L;
                    break;
                }

                case VideoObject::StatusNonCompataible:
                    KMessageBox::error(dlg->parentWidget(),
                        i18n("Cannot use %1.\nIt is not a DVD compatible file.",
                             filename));
                    delete vob;
                    vob = 0L;
                    break;
            }

            if (!vob) {
                break;
            }

            vob->setTitleFromFileName();

            if (multipleFiles->isChecked() || (filename == filenames.last())) {
                if (!m->addMediaObject(vob)) {
                    KMessageBox::error(dlg->parentWidget(),
                            i18n("A DVD can only have a maximum of 99 titles.\n"),
                            i18n("Too Many Titles"));
                    delete vob;
                    break;
                }
            }
        }
        if (vob && (!multipleFiles->isChecked() || filenames.count() == 1)) {
            kapp->setActiveWindow(dlg->parentWidget());  // Otherwise activeWindow is not set and dialog has no parent!
            vob->slotProperties();
        }
    }
    delete dlg;
}

KMF::MediaObject *VideoPlugin::createMediaObject(const QDomElement &element)
{
    KMF::MediaObject *mob = new VideoObject(this);

    if (mob) {
        if (!mob->fromXML(element)) {
            delete mob;
            return 0;
        }
    }

    return mob;
}

QStringList VideoPlugin::supportedProjectTypes() const
{
    QStringList result;

    result << "DVD-PAL" << "DVD-NTSC";
    return result;
}

#include "videoplugin.moc"
