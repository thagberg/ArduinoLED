#include "ArduinoDevice.h"

#include <assert.h>

namespace hvk
{
	namespace control
	{
		ArduinoException::ArduinoException(std::string message)
			: mMessage(message)
		{
		}

		const char* ArduinoException::what() const
		{
			return mMessage.c_str();
		}

		ArduinoDevice::ArduinoDevice()
			: mCommHandle(INVALID_HANDLE_VALUE)
			, mDcb()
			, mDeviceReady(false)
		{
			ZeroMemory(mReadBuffer, _countof(mReadBuffer));

			mCommHandle = CreateFileA(
				"COM4",
				GENERIC_READ | GENERIC_WRITE,
				NULL,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			assert(mCommHandle != INVALID_HANDLE_VALUE);
			if (mCommHandle == INVALID_HANDLE_VALUE)
			{
				throw ArduinoException("Failed to open COM4 to Arduino Device");
			}

			memset(&mDcb, 0, sizeof(DCB));
			bool success = GetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				throw ArduinoException("Failed to determine CommState with Arduino Device");
			}

			mDcb.BaudRate = CBR_38400;
			mDcb.ByteSize = 8;
			mDcb.Parity = NOPARITY;
			mDcb.StopBits = ONESTOPBIT;
			mDcb.fRtsControl = RTS_CONTROL_ENABLE;
			mDcb.fDtrControl = DTR_CONTROL_ENABLE;
			success = SetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				throw ArduinoException("Failed to set CommState of Arduino Device");
			}

			success = GetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				throw ArduinoException("Failed to validate updated CommState of Arduino Device");
			}

			COMMTIMEOUTS timeouts = {};
			timeouts.ReadTotalTimeoutConstant = 1000;
			timeouts.ReadIntervalTimeout = 0;
			timeouts.ReadTotalTimeoutMultiplier = 1;
			timeouts.WriteTotalTimeoutConstant = 1000;
			timeouts.WriteTotalTimeoutMultiplier = 1;
			success = SetCommTimeouts(mCommHandle, &timeouts);
			assert(success);
			if (!success)
			{
				throw ArduinoException("Failed to set CommTimeouts with Arduino Device");
			}

			mDeviceReady = true;
		}

		ArduinoDevice::~ArduinoDevice()
		{
			if (mDeviceReady)
			{
				auto success = CancelIo(mCommHandle);
				// not sure what the risk is of leaving the handle open if closing IO fails
				if (success)
				{
					CloseHandle(mCommHandle);
				}
			}
		}

		size_t ArduinoDevice::write(Framebuffer& framebuffer)
		{
			DWORD numBytesWritten;
			bool writeSuccess = WriteFile(mCommHandle, framebuffer.data(), sizeof(Color) * framebuffer.size(), &numBytesWritten, nullptr);
			assert(writeSuccess);
			DWORD numBytesRead;
			bool readSuccess = ReadFile(mCommHandle, mReadBuffer, 1, &numBytesRead, nullptr);
			assert(readSuccess && numBytesRead == 1);

			return static_cast<size_t>(numBytesWritten);
		}
	}
}