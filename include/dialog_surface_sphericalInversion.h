#ifndef _DIALOG_SURFACE_SPHERICALINVERSION_H_
#define _DIALOG_SURFACE_SPHERICALINVERSION_H_

#include "ui_dialog_surface_sphericalInversion.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;

class Dialog_Surface_SphericalInversion : public QDialog, public Ui::Dialog_Surface_SphericalInversion
{
	Q_OBJECT

public:
    Dialog_Surface_SphericalInversion(SCHNApps* s);

private:
	SCHNApps* m_schnapps;
	MapHandlerGen* m_selectedMap;

public slots:
	void selectedMapChanged();
	void addMapToList(MapHandlerGen* map);
	void removeMapFromList(MapHandlerGen* map);
	void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
