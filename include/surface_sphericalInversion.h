#ifndef _SURFACE_SPHERICALINVERSION_PLUGIN_H_
#define _SURFACE_SPHERICALINVERSION_PLUGIN_H_

#include "plugin_processing.h"

#include "camera.h"

#include "dialog_surface_sphericalInversion.h"
#include "Utils/chrono.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_SphericalInversion_Plugin : public Plugin
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
	Surface_SphericalInversion_Plugin()
	{}

	~Surface_SphericalInversion_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

private slots:
	void openSphericalInversionDialog();
	void closeSphericalInversionDialog();

	void inverseFromDialog();

	void initializeConvexHull(PFP2::MAP& map, VertexAttribute<PFP2::VEC3, PFP2::MAP>& attribute);

public slots:

	void inverse(const QString& mapName, const QString& attributeName);

	void computeConvexHull(const QString& mapName, const QString& attributeName);

private:
	Dialog_Surface_SphericalInversion* m_sphericalInversionDialog;

	QAction* m_sphericalInversionAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
