/**
 * @file FuseFileSystemNone.h
 * @author badbouille
 * @date 08/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _FUSEFILESYSTEMNONE_H
#define _FUSEFILESYSTEMNONE_H

#include <Volume.h>
#include "FuseFileSystem.h"

namespace GostCrypt
{

    class FuseFileSystemNone : public FuseFileSystem
    {
    public:

        FuseFileSystemNone() : FuseFileSystem(), userID(geteuid()), groupID(getegid()) { };

        // infos
        std::string getName() override { return "No FileSystem"; };
        std::string getID() override { return "none"; };
        std::string getDescription() override { return "Not a filesystem, just a FUSE mountpoint of the RAW content "
                                                      "of the Volume with no interpretation."; };

        // create
        void create(Volume *target) override {}; // none filesystem does not need to be built

        // Fuse entrypoint
        void start_fuse(const char * mountpoint, Volume *target);

        // tool
        uid_t getUID() const { return userID; };
        uid_t getGID() const { return groupID; };

    private:
        uid_t userID;
        gid_t groupID;

    };

}


#endif //_FUSEFILESYSTEMNONE_H
