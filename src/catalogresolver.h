#ifndef CATALOG_RESOLVER_H
#define CATALOG_RESOLVER_H

#include <string>

class CatalogResolver
{
  public:
    static std::string lookupPublicId(
      const std::string& publicId,
      const std::string& catalogPath);
};

#endif
