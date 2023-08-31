#pragma once
#include "def.h"
#include "cube.h"

struct Coord
{
    /* Phase 1 coordinate space: N_TWIST * N_FLIP * N_SLICE */

    int twist, flip, slice;

    /* Phase 2 coordinate space: N_CORNER * N_EDGE8 * N_EDGE4 */

    int corner, edge4, edge8;

    static const Coord id;

    /* auxiliary conversion functions */

    static CornerOri    twist2co(int);
    static int          co2twist(const CornerOri &);

    static EdgeOri      flip2eo(int);
    static int          eo2flip(const EdgeOri &);

    static CornerPerm   corner2cp(int);
    static int          cp2corner(const CornerPerm &);

    static int          ep2slice(const EdgePerm &); 
    static int          ep2edge4(const EdgePerm &);
    static int          ep2edge8(const EdgePerm &);
    static EdgePerm     slice2ep(int i);        // incomplete EdgePerm
    static EdgePerm     edge42ep(int i);        // incomplete EdgePerm 
    static EdgePerm     edge82ep(int i);        // incomplete EdgePerm
    static EdgePerm     see2ep(int, int, int);  // complete EdgePerm
    
    /* conversion between Coord and CubieCube */

    static Coord        CubieCube2Coord(const CubieCube &);
    static CubieCube    Coord2CubieCube(const Coord &);
};

inline bool operator==(const Coord &c1, const Coord &c2)
{
    return c1.twist == c2.twist && c1.flip == c2.flip && c1.slice == c2.slice 
        && c1.corner == c2.corner && c1.edge4 == c2.edge4 && c1.edge8 == c2.edge8;
}