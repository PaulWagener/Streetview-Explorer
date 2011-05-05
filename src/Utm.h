/************************************************************************
 *
 * File:			utm.h
 * RCS:			$Header: /cvsroot/stelvio/stelvio/NavStar/Utm.h,v 1.2 2002/04/23 05:02:00 steve_l Exp $
 * Author:		Steve Loughran
 * Created:		2001
 * Language:		C++
 * Package:
 * Status:		Experimental
 * @doc
 *
 ************************************************************************/

#ifndef UTM_H
#define UTM_H

struct utmPosition {
    int zoneX;
    char zoneY;

    double easting;
    double northing;
};

void LatLonToUtmWGS84(struct utmPosition &utm, double lat, double lon);

void UtmToLatLonWGS84(struct utmPosition utm, double& lat, double& lon);

#endif
