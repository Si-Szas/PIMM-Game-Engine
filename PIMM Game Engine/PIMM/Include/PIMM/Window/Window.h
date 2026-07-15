#pragma once
#include <PIMM/Core/Base.h>
#include <PIMM/Core/Common.h>

namespace pimm
{
	//Follows RAII. Resources acquired during initialization and released during destruction.
	class Window : public Base
	{
		public:
			//CONSTRUCTOR
			explicit Window(const WindowDescriptor& descriptor);

			//FUNCTIONS
			pimm::Rect GetClientAreaInScreenSpace();

			//DESTRUCTOR
			virtual ~Window() override;

		protected:
			void* m_handle{}; //Void pointer to not expose public header files
			Rect m_size{};
	};
}