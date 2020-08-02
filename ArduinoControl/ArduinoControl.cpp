// ArduinoControl.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"


#include "ArduinoControl.h"

namespace hvk
{
	namespace control
	{
		ArduinoController::ArduinoController(size_t size)
			: mCommHandle(INVALID_HANDLE_VALUE)
			, mDcb()
			, mDeviceReady(false)
			, mFramebuffers()
			, mBackbuffer(0)
			, mDeviceConnected(false)
			, mIOThread()
			, mBufferMutex()
			, mDisplayReady(false)
			, mSize(size)
		{
			ZeroMemory(mReadBuffer, 256);

			// initialize framebuffers
			for (auto& fb : mFramebuffers)
			{
				fb.buffer.resize(mSize);
				std::fill(fb.buffer.begin(), fb.buffer.end(), kWhite);
			}
		}

		ArduinoController::~ArduinoController()
		{
			if (mDeviceReady)
			{
				Stop();
			}
		}

		bool ArduinoController::Stop()
		{
			mDeviceReady = false;
			mDeviceConnected = false;

			auto success = CancelIo(mCommHandle);
			assert(success);
			mIOThread.join();
			success = CloseHandle(mCommHandle);
			assert(success);

			return success;
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
				FILE_ATTRIBUTE_NORMAL,
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
				return -3;
			}

			success = GetCommState(mCommHandle, &mDcb);
			assert(success);
			if (!success)
			{
				return -4;
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
				return -5;
			}

			mDeviceConnected = true;
			mDeviceReady = true;

			mIOThread = std::thread(&ArduinoController::UpdateDevice, this);

			return ret;
		}

		void ArduinoController::UpdateDevice()
		{
			while (mDeviceConnected)
			{
				if (mDisplayReady)
				{
					mDisplayReady = false;
					int frontbufferIndex;
					{
						std::unique_lock swapLock(mBufferMutex);
						mBackbuffer++;
						if (mBackbuffer > 1)
						{
							mBackbuffer = 0;
						}

						frontbufferIndex = ~mBackbuffer & 1;
					}

					const auto& frontbuffer = mFramebuffers[frontbufferIndex];

					const uint8_t numPixelsToWrite = frontbuffer.buffer.size();
					DWORD numBytesWritten;
					bool writeSuccess = WriteFile(mCommHandle, frontbuffer.buffer.data(), sizeof(Color) * numPixelsToWrite, &numBytesWritten, nullptr);
					assert(writeSuccess);

					DWORD numBytesRead;
					bool readSuccess = ReadFile(mCommHandle, mReadBuffer, 1, &numBytesRead, nullptr);
					assert(readSuccess);
				}

				Sleep(16);
			}
		}

		DWORD ArduinoController::WritePixels(const std::vector<Color>& colors)
		{
			assert(colors.size() == mSize);
			{
				std::unique_lock bufferLock(mBufferMutex);
				auto& backbuffer = mFramebuffers[mBackbuffer];
				auto copyStatus = memcpy_s(backbuffer.buffer.data(), sizeof(Color) * colors.size(), colors.data(), sizeof(Color) * colors.size());
				assert(copyStatus == S_OK);
				mDisplayReady = true;
			}

			return 0;
		}
	}
}
