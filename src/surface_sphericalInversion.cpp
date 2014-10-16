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

            Camera* camera = m_schnapps->getSelectedView()->getCurrentCamera();

            if(camera)
            {
                GLdouble gl_mvm[16];
                camera->getModelViewMatrix(gl_mvm);

                PFP2::MATRIX44 camera_matrix, camera_matrix_inverse;

                for(int i = 0; i < camera_matrix.m(); ++i)
                {
                    for(int j = 0; j < camera_matrix.n(); ++j)
                    {
                        camera_matrix(i, j) = gl_mvm[i*4+j];
                    }
                }

                camera_matrix.invert(camera_matrix_inverse);

                //Get furthest point from the camera
                float R = 0.f;
                for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
                {
                    PFP2::VEC4 position = camera_matrix*PFP2::VEC4(positionMap[d][0], positionMap[d][1], positionMap[d][2], 1.f);

                    positionMap[d] = PFP2::VEC3(position[0]/position[3], position[1]/position[3], position[2]/position[3]);
                    float norm2_point = positionMap[d].norm2();
                    if (norm2_point>R)
                    {
                        R = norm2_point;
                    }
                }

                for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
                {
                    PFP2::VEC4 position = camera_matrix*PFP2::VEC4(positionMap[d][0], positionMap[d][1], positionMap[d][2], 1.f);
                    positionMap[d] = PFP2::VEC3(position[0]/position[3], position[1]/position[3], position[2]/position[3]);

                    float norm_point = positionMap[d].norm();

                    positionMap[d] = positionMap[d]+2*(R-norm_point)*(positionMap[d]/norm_point);
                    position = PFP2::VEC4(positionMap[d][0], positionMap[d][1], positionMap[d][2], 1.f);
                    PFP2::VEC4 final_position = position*camera_matrix_inverse;
                    positionMap[d] = PFP2::VEC3(final_position[0]/final_position[3], final_position[1]/final_position[3], final_position[2]/final_position[3]);
                }

                mh_map->updateBB(positionMap);
                mh_map->updateVBO(positionMap);
                m_schnapps->getSelectedView()->updateGL();
            }
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
