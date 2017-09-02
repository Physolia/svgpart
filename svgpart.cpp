/*
Copyright 2007 Aurélien Gâteau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "svgpart.h"

// KF
#include <KAboutData>
#include <KLocalizedString>
#include <KActionCollection>
#include <KStandardAction>
#include <KPluginFactory>

// Qt
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QGraphicsView>
#include <QSvgRenderer>


static KAboutData createAboutData()
{
    KAboutData aboutData(QStringLiteral("svgpart"), i18n("SVG Part"),
                         QStringLiteral("1.0"),
                         i18n("A KPart to display SVG images"),
                         KAboutLicense::KAboutLicense::GPL,
                         i18n("Copyright 2007, Aurélien Gâteau <aurelien.gateau@free.fr>"));
    return aboutData;
}

//Factory Code
K_PLUGIN_FACTORY(SvgPartFactory, registerPlugin<SvgPart>();)


SvgPart::SvgPart(QWidget* parentWidget, QObject* parent, const QVariantList&)
    : KParts::ReadOnlyPart(parent)
    , mItem(nullptr)
{
    setComponentData(createAboutData());

    mRenderer = new QSvgRenderer(this);
    mScene = new QGraphicsScene(this);
    mView = new QGraphicsView(mScene, parentWidget);
    mView->setFrameStyle(QFrame::NoFrame);
    mView->setDragMode(QGraphicsView::ScrollHandDrag);
    setWidget(mView);

    KStandardAction::actualSize(this, SLOT(zoomActualSize()), actionCollection());
    KStandardAction::zoomIn(this, SLOT(zoomIn()), actionCollection());
    KStandardAction::zoomOut(this, SLOT(zoomOut()), actionCollection());
    setXMLFile(QStringLiteral("svgpart.rc"));
}


bool SvgPart::openFile()
{
    if (!mRenderer->load(localFilePath())) {
        return false;
    }
    mItem = new QGraphicsSvgItem();
    mItem->setSharedRenderer(mRenderer);
    mScene->addItem(mItem);
    // we reuse the scene, whose scenerect though is not properly resetable, so ensure up-to-date one
    mScene->setSceneRect(mItem->boundingRect());
    return true;
}


bool SvgPart::closeUrl()
{
    mView->resetMatrix();
    mView->resetTransform();
    // cannot reset the rect completely, as a null QRectF is ignored
    // so at least just a 1 pixel square one
    mScene->setSceneRect(QRectF(0,0,1,1));

    delete mItem;
    mItem = nullptr;

    return KParts::ReadOnlyPart::closeUrl();
}


void SvgPart::zoomIn()
{
    setZoom(zoom() * 2);
}


void SvgPart::zoomOut()
{
    setZoom(zoom() / 2);
}


void SvgPart::zoomActualSize()
{
    setZoom(1.0);
}


qreal SvgPart::zoom() const
{
    return mView->matrix().m11();
}


void SvgPart::setZoom(qreal value)
{
    QMatrix matrix;
    matrix.scale(value, value);
    mView->setMatrix(matrix);
}

#include "svgpart.moc"
