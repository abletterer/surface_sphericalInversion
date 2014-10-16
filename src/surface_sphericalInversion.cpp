#include "surface_sphericalInversion.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_SphericalInversion_Plugin::enable()
{
    m_sphericalInversionDialog = new Dialog_Surface_SphericalInversion(m_schnapps);

    m_sphericalInversionAction = new QAction("Subdivide surface", this);

    m_schnapps->addMenuAction(this, "Surface;Spherical Inversion", m_sphericalInversionAction);

    connect(m_sphericalInversionAction, SIGNAL(triggered()), this, SLOT(openSphericalInversionDialog()));

    connect(m_sphericalInversionDialog, SIGNAL(accepted()), this, SLOT(inverseFromDialog()));
    connect(m_sphericalInversionDialog->button_cancel, SIGNAL(clicked()), this, SLOT(closeSphericalInversionDialog()));
    connect(m_sphericalInversionDialog->button_ok, SIGNAL(clicked()), this, SLOT(inverseFromDialog()));

	return true;
}

void Surface_SphericalInversion_Plugin::disable()
{
    disconnect(m_sphericalInversionAction, SIGNAL(triggered()), this, SLOT(openSphericalInversionialog()));

    disconnect(m_sphericalInversionDialog, SIGNAL(accepted()), this, SLOT(inverseFromDialog()));
    disconnect(m_sphericalInversionDialog->button_cancel, SIGNAL(clicked()), this, SLOT(closeSphericalInversionDialog()));
}

void Surface_SphericalInversion_Plugin::openSphericalInversionDialog()
{
    m_sphericalInversionDialog->show();
}

void Surface_SphericalInversion_Plugin::closeSphericalInversionDialog()
{
    m_sphericalInversionDialog->close();
}

void Surface_SphericalInversion_Plugin::inverseFromDialog()
{
    QList<QListWidgetItem*> currentItems = m_sphericalInversionDialog->list_maps->selectedItems();
	if(!currentItems.empty())
	{
		const QString& mapName = currentItems[0]->text();
        const QString& positionName = m_sphericalInversionDialog->combo_positionAttribute->currentText();

        MapHandler<PFP2>* mh_map = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));

        if(mh_map)
        {
            PFP2::MAP* map = mh_map->getMap();
            VertexAttribute<PFP2::VEC3, PFP2::MAP> positionMap = mh_map->getAttribute<PFP2::VEC3, VERTEX>(positionName);

            TraversorV<PFP2::MAP> trav_vert_map(*map);

            //Get furthest point from the camera
            float R = 0.f;
            for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
            {
                float norm2Point = positionMap[d].norm2();
                if (norm2Point>R)
                {
                    CGoGNout << "--Point plus éloigné trouvé!--" << CGoGNendl;
                    CGoGNout << "Ancienne valeur : " << R << CGoGNendl;
                    CGoGNout << "Nouvelle valeur : " << norm2Point << CGoGNendl;
                    R = norm2Point;
                }
            }

            for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
            {
                float normPoint = positionMap[d].norm();
                positionMap[d] = positionMap[d]+2*(R-normPoint)*(positionMap[d]/normPoint);
            }
            mh_map->updateBB(positionMap);
            mh_map->updateVBO(positionMap);
        }
	}
    m_sphericalInversionDialog->close();
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_SphericalInversion_Plugin, Surface_SphericalInversion_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_SphericalInversion_PluginD, Surface_SphericalInversion_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
