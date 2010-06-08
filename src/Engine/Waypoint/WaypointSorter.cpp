#include "WaypointSorter.hpp"
#include <algorithm>
#include "Navigation/Geometry/GeoVector.hpp"
#include "UtilsText.hpp"
#include "Compatibility/string.h" /* _tcsnicmp -> _strnicmp -> strncasecmp */

#include <string>

void
WaypointSelectInfoVector::push_back(const Waypoint &way_point,
                                    const GEOPOINT &Location,
                                    const fixed distance_factor)
{
  WayPointSelectInfo info;

  info.way_point = &way_point;

  const GeoVector vec(Location, way_point.Location);

  info.Distance = vec.Distance*distance_factor;
  info.Direction = vec.Bearing;

  info.FourChars =
    ((way_point.Name.c_str()[0] & 0xff) << 24)
    +((way_point.Name.c_str()[1] & 0xff) << 16)
    +((way_point.Name.c_str()[2] & 0xff) << 8)
    +((way_point.Name.c_str()[3] & 0xff));

  std::vector<WayPointSelectInfo>::push_back(info);
}

WaypointSorter::WaypointSorter(const Waypoints &way_points,
                               const GEOPOINT &Location,
                               const fixed distance_factor)
{
  m_waypoints_all.reserve(way_points.size());

  for (Waypoints::WaypointTree::const_iterator it = way_points.begin();
       it != way_points.end(); ++it) {
    m_waypoints_all.push_back(it->get_waypoint(), Location, distance_factor);
  }
  sort_name(m_waypoints_all);
}

const WaypointSelectInfoVector& 
WaypointSorter::get_list()
{
  return m_waypoints_all;
}

static bool 
WaypointAirportFilter(const WayPointSelectInfo& elem1) 
{
  return !elem1.way_point->Flags.Airport;
}

void
WaypointSorter::filter_airport(WaypointSelectInfoVector& vec)
{
  vec.erase(std::remove_if(vec.begin(), vec.end(), WaypointAirportFilter), vec.end());
}

static bool 
WaypointLandableFilter(const WayPointSelectInfo& elem1) 
{
  return !elem1.way_point->is_landable();
}

void
WaypointSorter::filter_landable(WaypointSelectInfoVector& vec)
{
  vec.erase(std::remove_if(vec.begin(), vec.end(), WaypointLandableFilter), vec.end());
}

static bool 
WaypointTurnPointFilter(const WayPointSelectInfo& elem1) 
{
  return !elem1.way_point->Flags.TurnPoint;
}

void
WaypointSorter::filter_turnpoint(WaypointSelectInfoVector& vec)
{
  vec.erase(std::remove_if(vec.begin(), vec.end(), WaypointTurnPointFilter), vec.end());
}

class WaypointFileIdxFilter {
  int file_num;

public:
  WaypointFileIdxFilter(int _file_num):file_num(_file_num) {}

  bool operator()(const WayPointSelectInfo &i) const {
    return i.way_point->FileNum != file_num;
  }
};

void
WaypointSorter::filter_file(WaypointSelectInfoVector& vec,
                            const int file_idx)
{
  const WaypointFileIdxFilter filter(file_idx);
  vec.erase(std::remove_if(vec.begin(), vec.end(), filter), vec.end());
}

static unsigned char MatchChar = 0;
static TCHAR MatchString[NAMEFILTERLEN+1];

static bool
WaypointNameFilter(const WayPointSelectInfo& elem1)
{
  if (((elem1.FourChars & 0xff000000) >> 24) != MatchChar) {
    return true;
  }
  else if (_tcsnicmp( elem1.way_point->Name.c_str(),MatchString,_tcslen(MatchString)) != 0) {
    return true;
  }

  return false;

}

void
WaypointSorter::filter_name(WaypointSelectInfoVector& vec,
                            const TCHAR * s)
{
  MatchChar = s[0] &0xff;
  _tcscpy(MatchString, s);

  vec.erase(std::remove_if(vec.begin(), vec.end(), WaypointNameFilter), vec.end());
}

static const fixed fixed_18(18);

class WaypointDirectionFilter {
  Angle direction;

public:
  WaypointDirectionFilter(Angle _direction):direction(_direction) {}

  bool operator()(const WayPointSelectInfo& i) const {
    fixed DirectionErr = (i.Direction - direction).as_delta().magnitude_degrees();
    return DirectionErr > fixed_18;
  }
};

void 
WaypointSorter::filter_direction(WaypointSelectInfoVector& vec,
                                 const Angle direction)
{
  const WaypointDirectionFilter filter(direction);
  vec.erase(std::remove_if(vec.begin(), vec.end(), filter), vec.end());
}

class WaypointDistanceFilter {
  fixed max_distance;

public:
  WaypointDistanceFilter(fixed _max_distance):max_distance(_max_distance) {}

  bool operator()(const WayPointSelectInfo& i) const {
    return i.Distance > max_distance;
  }
};

void 
WaypointSorter::filter_distance(WaypointSelectInfoVector& vec,
                                const fixed distance)
{
  const WaypointDistanceFilter filter(distance);
  vec.erase(std::remove_if(vec.begin(), vec.end(), filter), vec.end());
}


static bool
WaypointDistanceCompare(const WayPointSelectInfo& elem1, 
                        const WayPointSelectInfo& elem2 ) 
{
  return (elem1.Distance < elem2.Distance);
}

void 
WaypointSorter::sort_distance(WaypointSelectInfoVector& vec)
{
  std::sort(vec.begin(),
            vec.end(),
            WaypointDistanceCompare);
}


static bool
WaypointNameCompare(const WayPointSelectInfo& elem1, 
                    const WayPointSelectInfo& elem2 ) 
{
  if (elem1.FourChars != elem2.FourChars) {
    return (elem1.FourChars < elem2.FourChars);
  }
  else {
    // if they're equal to 4 chars, then do full name comparison
    return elem1.way_point->Name.compare(elem2.way_point->Name) < 0;
  }
}


void 
WaypointSorter::sort_name(WaypointSelectInfoVector& vec)
{
  std::sort(vec.begin(),
            vec.end(),
            WaypointNameCompare);
}
