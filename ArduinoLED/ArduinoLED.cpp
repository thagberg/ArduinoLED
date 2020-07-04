// ArduinoLED.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <thread>
#include <assert.h>

#include <Windows.h>
#include <fileapi.h>

#include <ArduinoControl.h>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

const size_t kStreamBufferSize = 64;

bool gRunning = false;

const std::array<hvk::Color, 8*5> kColors =
{ {
    {255, 1, 1}, {255, 1, 1}, {255, 1, 1}, {255, 1, 1}, {255, 1, 1}, {255, 1, 1}, {255, 1, 1}, {255, 1, 1},
    {190, 1, 60}, {190, 1, 60}, {190, 1, 60}, {190, 1, 60}, {190, 1, 60}, {190, 1, 60}, {190, 1, 60}, {190, 1, 60},
    {130, 1, 120}, {130, 1, 120}, {130, 1, 120}, {130, 1, 120}, {130, 1, 120}, {130, 1, 120}, {130, 1, 120}, {130, 1, 120},
    {70, 1, 180}, {70, 1, 180}, {70, 1, 180}, {70, 1, 180}, {70, 1, 180}, {70, 1, 180}, {70, 1, 180}, {70, 1, 180},
    {1, 1, 255}, {1, 1, 255}, {1, 1, 255}, {1, 1, 255}, {1, 1, 255}, {1, 1, 255}, {1, 1, 255}, {1, 1, 255}
} };

const std::array<hvk::Color, 8*5> kWhites =
{ {
    {255, 255, 255},{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255},{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255},{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255},{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255},
    {255, 255, 255},{255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}, {255, 255, 255}
} };

const std::array<hvk::Color, 8 * 5> kOrange =
{ {
    {255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},
    {255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},
    {255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},
    {255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},
    {255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0},{255, 50, 0}
} };

const std::array<hvk::Color, 8 * 5> kGrey =
{ {
    {50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},
    {50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},
    {50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},
    {50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},
    {50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50},{50, 50, 50}
} };

const std::array<hvk::Color, 8 * 5> kDarkGrey =
{ {
    {25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},
    {25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},
    {25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},
    {25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},
    {25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25},{25, 25, 25}
} };

const std::array<hvk::Color, 8 * 5> kTestGrey =
{ {
    {8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},
    {8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},
    {8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},
    {8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},
    {8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25},{8, 6, 25}
} };

void getInput(HANDLE commHandle)
{
    char readBuffer[256];
    memset(readBuffer, '\0', 256);

    DWORD bytesRead;
    while (gRunning)
    {
		//OVERLAPPED comRead = { 0 };
  //      comRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  //      bool readSuccess = ReadFile(commHandle, readBuffer, 256, &bytesRead, &comRead);
  //      //bool readSuccess = ReadFileEx(commHandle, readBuffer, 256, &comRead, [](DWORD dwErrorCode, DWORD dwNumberOfBytes, LPOVERLAPPED lpOverlapped) {
  //      //});
  //      assert(readSuccess);

        DWORD errors;
        COMSTAT stat;
        ClearCommError(commHandle, &errors, &stat);
        if (stat.cbInQue > 0)
        {
			OVERLAPPED comRead = { 0 };
			comRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			bool readSuccess = ReadFile(commHandle, readBuffer, stat.cbInQue, &bytesRead, &comRead);
            if (readSuccess)
            {
                std::cout << readBuffer << std::endl;
                memset(readBuffer, '\0', stat.cbInQue);
            }
        }
        Sleep(16);
    }
}

int main()
{
    // create stream buffer
    char streamBuffer[kStreamBufferSize];
    memset(streamBuffer, 0, kStreamBufferSize);

    hvk::control::ArduinoController<8, 5> controller;
    controller.Init();

    //std::thread readThread(getInput, commFile);

    Sleep(1000);

    controller.WritePixels(kColors);

    Sleep(1000);

    std::array<hvk::Color, 8*5> blankColors;
    for (int i = 0; i < blankColors.size(); ++i)
    {
        blankColors[i] = { 0, 0, 0 };
    }

    controller.WritePixels(blankColors);

    Sleep(1000);

    controller.WritePixels(kWhites);

    Sleep(1000);

    controller.WritePixels(blankColors);

    Sleep(1000);

    controller.WritePixels(kOrange);

    Sleep(1000);

    controller.WritePixels(kGrey);

    Sleep(1000);

    controller.WritePixels(kDarkGrey);

    Sleep(1000);

    controller.WritePixels(kTestGrey);

    Sleep(1000);

    HANDLE commFile;
    DCB dcb;

    //commFile = CreateFileA(
    //    "COM4",
    //    GENERIC_READ | GENERIC_WRITE,
    //    NULL,
    //    NULL,
    //    OPEN_EXISTING,
    //    NULL,
    //    NULL);
    //if (commFile == INVALID_HANDLE_VALUE)
    //{
    //    std::cerr << "Failed to open COMM port" << std::endl;
    //    return -1;
    //}

    //memset(&dcb, 0, sizeof(DCB));
    //bool success = GetCommState(commFile, &dcb);
    //assert(success);
    //if (!success)
    //{
    //    std::cerr << "Failed to get initial COMM State" << std::endl;
    //    return -2;
    //}

    //dcb.BaudRate = CBR_9600;
    //dcb.ByteSize = 8;
    //dcb.Parity = NOPARITY;
    //dcb.StopBits = ONESTOPBIT;

    //success = SetCommState(commFile, &dcb);
    //assert(success);
    //if (!success)
    //{
    //    std::cerr << "Failed to set COMM State" << std::endl;
    //    return -3;
    //}

    //success = GetCommState(commFile, &dcb);
    //assert(success);
    //if (!success)
    //{
    //    std::cerr << "Failed to get updated COMM State" << std::endl;
    //    return -4;
    //}

    //std::cout << "Successfully configured device on COMM port" << std::endl;

    //gRunning = true;

    //// begin serial input thread
    //std::thread readThread(getInput, commFile);

    //while (gRunning)
    //{
    //    int userChoice = 0;
    //    std::cout << "Choose an action\n\t1: Set LED color\n\t2: Exit\n..." << std::endl;
    //    std::cin >> userChoice;
    //    switch (userChoice)
    //    {
    //    case 1:
    //    {
    //        char colorChoice;
    //        std::cout << "Choose color:\n\tR\n\tG\n\tB" << std::endl;
    //        std::cin >> colorChoice;
    //        Color newColor = { 1, 1, 1 };
    //        switch (colorChoice)
    //        {
    //        case 'R':
    //            newColor.r = 255;
    //            break;
    //        case 'G':
    //            newColor.g = 255;
    //            break;
    //        case'B':
    //            newColor.b = 255;
    //            break;
    //        default:
    //            newColor.r = 255;
    //            break;
    //        }
    //        OVERLAPPED oWrite;
    //        oWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    //        DWORD bytesWritten;
    //        //char output[7] = { 'R', newColor.r, 'G', newColor.g, 'B', newColor.b, '>' };
    //        char output[121];
    //        memset(output, 0, 120);
    //        for (size_t i = 0; i < 120; i += 3)
    //        {
    //            output[i] = newColor.r;
    //            output[i + 1] = newColor.g;
    //            output[i + 2] = newColor.b;
    //        }
    //        output[120] = '>';
    //        //char output[] = { 100, 1, 1, 100, 1, 1, '>' };
    //        bool success = WriteFile(commFile, output, sizeof(output), &bytesWritten, NULL);
    //        break;
    //    }
    //    case 2:
    //        gRunning = false;
    //        break;
    //    default:
    //        std::cout << "Invalid option\n" << std::endl;
    //    }
    //}

    //// rejoin input thread
    //readThread.join();

    //bool closeSuccess = CloseHandle(commFile);
    //assert(closeSuccess);
    //if (!closeSuccess)
    //{
    //    std::cerr << "Error when closing COMM port" << std::endl;
    //}

 //   DWORD bytesWritten;
	//char output[7] = { 'R', 0, 'G', 255, 'B', 0, '>' };
 //   bool success = WriteFile(commFile, output, sizeof(output), &bytesWritten, NULL);
 //   if (!success)
 //   {
 //       return -2;
 //   }

 //   DWORD bytesRead;
 //   char readBuffer[64];
 //   memset(readBuffer, 0, 64);
 //   success = ReadFile(commFile, readBuffer, 64, &bytesRead, NULL);
 //   if (!success)
 //   {
 //       return -3;
 //   }
}
