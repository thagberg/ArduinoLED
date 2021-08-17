#pragma once

#include <Windows.h>
#include <fileapi.h>
#include <exception>

#include <DeviceControl.h>

namespace hvk
{
	namespace control
	{
		class ArduinoException : public std::exception
		{
		public: 
			ArduinoException(std::string message);
			virtual const char* what() const throw() override;
		private:
			std::string mMessage;
		};

		class ArduinoDevice : public Device
		{
		public:
			ArduinoDevice();
			~ArduinoDevice();

			virtual size_t write(Framebuffer& framebuffer) override;
		private:
			HANDLE mCommHandle;
			DCB mDcb;
			bool mDeviceReady;
			char mReadBuffer[256];
		};
	}
}
