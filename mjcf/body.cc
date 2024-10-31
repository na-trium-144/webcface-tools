#include "main.h"

void parseBody(const pugi::xml_node &body) {
    // attribute
    // todo

    // children
    // ignore: inertial, camera, light,
    // todo: joint, freejoint, attach, site, plugin, composite, flexcomp
    for (pugi::xml_node geom = body.child("geom"); geom;
         geom = geom.next_sibling("geom")) {
        parseGeom(geom);
    }
}
