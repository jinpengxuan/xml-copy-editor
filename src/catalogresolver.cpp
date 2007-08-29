#include <memory>
#include "wraplibxml.h"
#include "catalogresolver.h"

std::string CatalogResolver::lookupPublicId(
  const std::string& publicId,
  const std::string& catalogPath)
{
  std::auto_ptr<WrapLibxml> libxml(new WrapLibxml(false, catalogPath));
  return libxml->lookupPublicId(publicId);
}
