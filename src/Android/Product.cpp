/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Product.hpp"
#include "Main.hpp"
#include "NativeView.hpp"

#include <assert.h>
#include <string.h>

bool
IsGalaxyTab22()
{
  assert(native_view);

  return native_view->GetAPILevel() == 8 &&
    (strcmp(native_view->GetProduct(), "GT-P1000") == 0 ||
     strcmp(native_view->GetProduct(), "GT-P1010") == 0);
}

bool
IsNookSimpleTouch()
{
  if (native_view == nullptr)
    return false;

  static int cached_type = -1;
  if (cached_type == -1) {
    if (strcmp(native_view->GetProduct(), "NOOK") == 0)
      cached_type = 1;
    else
      cached_type = 0;
  }
  return cached_type == 1;
}
