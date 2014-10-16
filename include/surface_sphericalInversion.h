#ifndef _SURFACE_SPHERICALINVERSION_PLUGIN_H_
#define _SURFACE_SPHERICALINVERSION_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_surface_sphericalInversion.h"

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

public slots:

private:
    Dialog_Surface_SphericalInversion* m_sphericalInversionDialog;

    QAction* m_sphericalInversionAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
