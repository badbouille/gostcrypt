/**
 * @file fullpath.cpp
 * @author badbouille
 * @date 22/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "platform/fullpath.h"
#include <fileapi.h>

int fullpath_getpath(const char *src, char *dst, size_t dst_len) {

    DWORD ret = 0;

    if (src == nullptr || dst == nullptr) {
        return -1;
    }

    ret = GetFullPathNameA(src, dst_len, dst, nullptr);

    // returns the length of the copied string or 0
    if (ret > 0) {
        return 0;
    }

    return 1;
}
