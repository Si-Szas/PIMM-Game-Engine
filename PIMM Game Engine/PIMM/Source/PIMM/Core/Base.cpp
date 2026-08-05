#include <PIMM/Core/Base.h>
using namespace pimm;

Base::Base(const BaseDescriptor& descriptor): m_logger(descriptor.logger)
{

}

Logger& Base::GetLogger() const noexcept
{
	return m_logger;
}

Base::~Base()
{

}