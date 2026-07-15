#include <PIMM/Resource/Resource.h>

pimm::Resource::Resource(const ResourceDescriptor& descriptor) :
	Base(descriptor.base),
	m_path(descriptor.path),
	m_manager(descriptor.manager)
{
}
