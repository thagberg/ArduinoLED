// ArduinoControl.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <assert.h>

#include "ArduinoControl.h"

namespace hvk
{
	namespace control
	{
		const char kTerminatorChar = '>';

		template <uint8_t Width, uint8_t Height>
		ArduinoController<Width, Height>::ArduinoController()
			: mCommHandle(INVALID_HANDLE_VALUE)
			, mDcb()
			, mDeviceReady(false)
			, mWriteBuffer()
			, mWaitBuffer()
		{
		}

		template <uint8_t Width, uint8_t Height>
		ArduinoController::~ArduinoController()
		{
			CloseHandle(mCommHandle);
		}

		int8_t ArduinoController::Init()
		{
			int8_t ret = 0;

			mCommHandle = CreateFileA(
				"COM4",
				GENERIC_READ | GENERIC_WRITE,
				NULL,
				NULL,
				OPEN_EXISTING,
				NULL,
				NULL);
			assert(mCommHandle != INVALID_HANDLE_VALUE);
			if (mCommHandle == INVALID_HANDLE_VALUE)
			{
				return -1;
			}

			memset(&mDcb, 0, sizeof(DCB));
			bool success = GetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				return -2;
			}

			mDcb.BaudRate = CBR_9600;
			mDcb.ByteSize = 8;
			mDcb.Parity = NOPARITY;
			mDcb.StopBits = ONESTOPBIT;
			success = SetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				return -3;
			}

			success = GetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				return -4;
			}

			mDeviceReady = true;

			return ret;
		}

		DWORD ArduinoController::WritePixels(const std::vector<Color>& pixels)
		{
			//std::vector<uint8_t> writeData;
			//writeData.resize(pixels.size() * 3 + 1);
			//memcpy(writeData.data(), pixels.data(), pixels.size() * 3);
			//writeData[writeData.size() - 1] = kTerminatorChar;

			DWORD bytesWritten = -1;
			if (mDeviceReady)
			{
				//bool success = WriteFile(mCommHandle, writeData.data(), writeData.size(), &bytesWritten, NULL);
				bool success = WriteFile(mCommHandle, pixels.data(), pixels.size() * sizeof(Color), &bytesWritten, NULL);
				assert(success);

				mDeviceReady = false;
			}

			return bytesWritten;
		}
	}
}
