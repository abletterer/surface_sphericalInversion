#include "surface_sphericalInversion.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_SphericalInversion_Plugin::enable()
{
	m_sphericalInversionDialog = new Dialog_Surface_SphericalInversion(m_schnapps);

	m_sphericalInversionAction = new QAction("Spherical Inversion", this);

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

		inverse(mapName, positionName);
	}
	m_sphericalInversionDialog->close();
}

void Surface_SphericalInversion_Plugin::inverse(const QString& mapName, const QString& attributeName)
{
	MapHandler<PFP2>* mh_map = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));

	if(mh_map)
	{
		PFP2::MAP* map = mh_map->getMap();
		VertexAttribute<PFP2::VEC3, PFP2::MAP> positionMap = mh_map->getAttribute<PFP2::VEC3, VERTEX>(attributeName);
		if(!positionMap.isValid())
		{
			CGoGNerr << attributeName.toStdString() << " attribute is not valid" << CGoGNendl;
			return;
		}

		Camera* camera = m_schnapps->getSelectedView()->getCurrentCamera();
		if(camera)
		{
			CGoGNout << camera->position().x << " " << camera->position().y << " " << camera->position().z << CGoGNendl;

			CGoGNout << "Spherical inversion on " << map->getNbCells(VERTEX) << " points .." << CGoGNflush;
			Utils::Chrono chrono;
			chrono.start();

			float R = 0.f;

			//Get furthest point from the camera, it is the radius of the sphere
			TraversorV<PFP2::MAP> trav_vert_map(*map);
			for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
			{
				qglviewer::Vec world_position(positionMap[d][0], positionMap[d][1], positionMap[d][2]), camera_position;
				camera_position = camera->cameraCoordinatesOf(world_position);

				float norm2_point = camera_position.x*camera_position.x+camera_position.y*camera_position.y+camera_position.z*camera_position.z;

				if (norm2_point>R)
				{
					R = norm2_point;
				}
			}

			R = sqrt(R);

			for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
			{
				qglviewer::Vec world_position(positionMap[d][0], positionMap[d][1], positionMap[d][2]), camera_position;
				camera_position = camera->cameraCoordinatesOf(world_position);

				PFP2::VEC3 position(camera_position[0], camera_position[1], camera_position[2]);
				float norm_point = position.norm();

				camera_position = camera_position+2*(R-norm_point)*(camera_position/norm_point);
				world_position = camera->worldCoordinatesOf(camera_position);
				positionMap[d] = PFP2::VEC3(world_position[0], world_position[1], world_position[2]);
			}

			CGoGNout << ".. performed in " << chrono.elapsed() << " ms" << CGoGNendl;

			mh_map->updateBB(positionMap);
			mh_map->notifyAttributeModification(positionMap);
			mh_map->updateVBO(positionMap);
			m_schnapps->getSelectedView()->updateGL();
		}
	}
}

void Surface_SphericalInversion_Plugin::computeConvexHull(const QString& mapName, const QString& attributeName)
{
	MapHandler<PFP2>* mh_map = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));

	if(mh_map)
	{
		PFP2::MAP* map = mh_map->getMap();
	}
}

void Surface_SphericalInversion_Plugin::initializeConvexHull(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& attribute)
{
	TraversorV<PFP2::MAP> trav_vert_map(map);


	for(Dart d = trav_vert_map.begin(); d != trav_vert_map.end(); d = trav_vert_map.next())
	{

	}
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_SphericalInversion_Plugin, Surface_SphericalInversion_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_SphericalInversion_PluginD, Surface_SphericalInversion_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
