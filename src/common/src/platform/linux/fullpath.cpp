/**
 * @file fullpath.cpp
 * @author badbouille
 * @date 22/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "platform/fullpath.h"
#include <cstdlib>

int fullpath_getpath(const char *src, char *dst, size_t dst_len) {

    if (src == nullptr || dst == nullptr) {
        return -1;
    }

    char * p = realpath(src, dst);

    if (p == dst) {
        return 0;
    }

    return 1;
}
