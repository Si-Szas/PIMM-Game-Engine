#include <PIMM/InputSystem/InputCommand.h>

pimm::InputCommand::InputCommand(const InputSystemDescriptor& descriptor) :
	Identifier({ descriptor.base }),
	m_base(descriptor.base),
	m_world(descriptor.world)
{
}

pimm::InputCommand::~InputCommand()
{
}
