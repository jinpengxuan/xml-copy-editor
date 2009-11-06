#ifndef PLAYLIST_RENAMER_H
#define PLAYLIST_RENAMER_H

#include <string>
#include <vector>

class PlayListRenamer
{
public:
    PlayListRenamer() {}
    ~PlayListRenamer() {}
    bool run ( const std::string& folder );
private:
    bool readFile ( const std::string& path, std::string& buffer );
    bool splitBuffer ( const std::string& buffer, std::vector<std::string> lineVector );
    void renameFile ( const std::string& from, const std::string& to, const std::string& folder );
    void editFiles ( const std::string& from, const std::string& to, const std::string& folder );
    std::string numToString ( int i );
};

#endif
