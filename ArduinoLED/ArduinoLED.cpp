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


const hvk::Color Grey = { 25, 25, 25 };
const hvk::Color GreyRed = { 50, 0, 0 };
const hvk::Color Red = { 255, 0, 0 };
const hvk::Color Orange = { 255, 128, 0 };
const hvk::Color Yellow = { 255, 255, 0 };
const hvk::Color Lime = { 128, 255, 0 };
const hvk::Color Green = { 0, 255, 0 };
const hvk::Color Sea = { 0, 255, 128 };
const hvk::Color Cyan = { 0, 255, 255 };
const hvk::Color Teal = { 0, 128, 255 };
const hvk::Color Blue = { 0, 0, 255 };
const hvk::Color Violet = { 128, 0, 255 };
const hvk::Color Purple = { 255, 0, 255 };
const hvk::Color Pink = { 255, 0, 128 };

std::array<hvk::Color, 8 * 5> makeSingleLedTest()
{
    std::array<hvk::Color, 8 * 5> filled;
    filled.fill({ 0, 0, 0 });
    filled[0] = { 50, 0, 0 };
    return filled;
}

std::array<hvk::Color, 8 * 5> makeColor(const hvk::Color& c)
{
    std::array<hvk::Color, 8 * 5> filled;
    filled.fill(c);
    return filled;
}

int main()
{
    const DWORD sleepTime = 1000;
    // create stream buffer
    char streamBuffer[kStreamBufferSize];
    memset(streamBuffer, 0, kStreamBufferSize);

    hvk::control::ArduinoController<8, 5> controller;
    controller.Init();

    Sleep(1000);
    controller.WritePixels(makeColor(Grey));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(GreyRed));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Red));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Orange));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Yellow));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Lime));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Green));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Sea));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Cyan));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Teal));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Blue));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Violet));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Purple));
    Sleep(sleepTime);
    controller.WritePixels(makeColor(Pink));
}
