﻿#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "setting.h"
#include "Random.h"
void Block(wchar_t*& pBuffer, WORD*& pColor, int nTetromino, int nPosX, int nPosY)
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (tetromino.at(nTetromino).at(0).at(j * 8 + i) != L'.')
            {
                pBuffer[(nPosY + j) * nScreenWidth + (nPosX + i)] = L'▓';
            }
            else
            {
                pBuffer[(nPosY + j) * nScreenWidth + (nPosX + i)] = L' ';
            }
            pColor[(nPosY + j) * nScreenWidth + (nPosX + i)] = 8 * 16 + nTetromino;
        }
    }
}

void Text(wchar_t*& pBuffer, WORD*& pColor, wstring wsContent, WORD wColor, int nPosX, int nPosY)
{
    for (int i = 0; i < wsContent.length(); i++, nPosX++)
    {
        pBuffer[nPosY * nScreenWidth + nPosX] = wsContent.at(i);
        pColor[nPosY * nScreenWidth + nPosX] = wColor;
    }
}
bool CheckPiece(int*& pMatrix, int nTetromino, int nRotation, int nPosX, int nPosY)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (nPosX + i >= 0 && nPosX + i < nBoardWidth)
            {
                if (nPosY + j >= 0 && nPosY + j < nBoardHeight)
                {
                    if (tetromino.at(nTetromino).at(nRotation).at(j * 8 + i) != L'.' && pMatrix[(nPosY + j) * nBoardWidth + (nPosX + i)] != 0)
                    {
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

int main()
{
    configure();

    // Buffer data
    WORD* pColor = new WORD[nScreenWidth * nScreenHeight];
    wchar_t* pBuffer = new wchar_t[nScreenWidth * nScreenHeight];

    // Create screen buffer
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    DWORD dwBytesWritten = 0;

    while (1)
    {
        SetConsoleActiveScreenBuffer(hConsole);

        // GET STARTED

        const vector<wstring> wsThree = {
            L"──▄",
            L" ─█",
            L"──▀"
        };

        const vector<wstring> wsTwo = {
            L"──▄",
            L"▄─▀",
            L"▀──"
        };

        const vector<wstring> wsOne = {
            L"─▄ ",
            L" █ ",
            L" ▀ "
        };

        const vector<wstring> wsReady = {
            L"▄──┐ ▄── ┌──▄ ▄──┐ ▄ ┬",
            L"█─┬┘ █─  ├──█ █ ┌┘ ▀▄┘",
            L"▀ └─ ▀── ┴  ▀ ▀─┘   ▀ "
        };

        const vector<vector<wstring>> wsCountDown = { wsThree, wsTwo, wsOne, wsReady };

        // Clear screen
        for (int i = 0; i < nScreenWidth; i++)
        {
            for (int j = 0; j < nScreenHeight; j++)
            {
                pBuffer[j * nScreenWidth + i] = L' ';
                pColor[j * nScreenWidth + i] = 8 * 16 + 9;
            }
        }

        // Count down
        for (int i = 0; i < wsCountDown.size(); i++)
        {
            for (int j = 0; j < wsCountDown.at(i).size(); j++)
            {
                if (i == 3)
                {
                    Text(pBuffer, pColor, wsCountDown.at(i).at(j), 8 * 16 + 4, 9, 9 + j);
                }
                else
                {
                    Text(pBuffer, pColor, wsCountDown.at(i).at(j), 8 * 16 + 4, 18, 9 + j);
                }

            }

            for (int j = 0; j < nScreenHeight; j++)
            {
                for (int i = 0; i < nScreenWidth; i++)
                {
                    COORD cPos;
                    cPos.X = i;
                    cPos.Y = j;
                    WriteConsoleOutputAttribute(hConsole, &pColor[j * nScreenWidth + i], 1, cPos, &dwBytesWritten);
                }
            }
            WriteConsoleOutputCharacter(hConsole, pBuffer, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
            Sleep(1000);
        }

        // Create game screen
        for (int i = 0; i < nScreenWidth; i++)
        {
            for (int j = 0; j < nScreenHeight; j++)
            {
                pBuffer[j * nScreenWidth + i] = L' ';
                if ((i / 2 + i % 2 + j) % 2 == 0)
                    pColor[j * nScreenWidth + i] = 8 * 16 + 9;
                else pColor[j * nScreenWidth + i] = 7 * 16 + 9;
            }
        }
        // Game data
        int* pMatrix = new int[nBoardWidth * nBoardHeight];
        for (int i = 0; i < nBoardWidth; i++)
        {
            for (int j = 0; j < nBoardHeight; j++)
            {
                if (j == nBoardHeight - 1)
                {
                    if (i == 0)
                    {
                        pMatrix[j * nBoardWidth + i] = 4;
                    }
                    else if (i == nBoardWidth - 1)
                    {
                        pMatrix[j * nBoardWidth + i] = 5;
                    }
                    else
                    {
                        pMatrix[j * nBoardWidth + i] = 7;
                    }
                }
                else
                {
                    if (i == 0 || i == nBoardWidth - 1)
                    {
                        pMatrix[j * nBoardWidth + i] = 6;
                    }
                    else
                    {
                        pMatrix[j * nBoardWidth + i] = 0;
                    }
                }
            }
        }

        const vector<char> key = { 'W', 'A', 'S', 'D', 27 };
        bool bKey[5];

        int nCurrentPiece = random(0, 6);
        int nNextPiece = random(0, 6);
        int nCurrentRotation = 0;
        int nCurrentX = nBoardWidth / 2 - 4;
        int nCurrentY = 0;

        int nFrame = 20;
        int nFrameCount = 0;

        bool bForceDown = 0;

        bool bRotateHold = 1;

        int nLevelLimit = 2;
        int nPieceCount = 0;

        int nScore = 0;
        int nScorePosX = 37;
        int nScoreComp = 10;

        int nLine = 0;
        int nLinePosX = 37;
        int nLineComp = 10;
        int nCurrentPiece_color = 0;
        vector<int> vLines;
        bool visible[nScreenWidth * nScreenHeight + 10];
        // Game loop
        while (1)
        {
            // GAME TIMING
            Sleep(40);
            nFrameCount++;
            if (nFrameCount == nFrame)
                bForceDown = 1;
            else
                bForceDown = 0;

            // INPUT
            for (int i = 0; i < key.size(); i++)
            {
                bKey[i] = 0;
                if ((GetKeyState(key.at(i)) & 0x8000) != 0)
                    bKey[i] = 1;
            }

            // GAME LOGIC

            // Game over limit
            int nLimit = 0;
            if (nCurrentPiece == 3)
            {
                nLimit = -1;
            }
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (tetromino.at(nCurrentPiece).at(nCurrentRotation).at(j * 8 + i) != L'.' && nCurrentY + j >= 0)
                    {
                        if (((nCurrentX + i) / 2 + (nCurrentX + i) % 2 + (nCurrentY + j)) % 2 == 0)
                            pColor[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = 8 * 16 + nCurrentPiece_color;
                        else pColor[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = 7 * 16 + nCurrentPiece_color;
                        pBuffer[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = L'▓';
                    }
                }
            }
            // Handling input
            if (bKey[3] == 1 && nCurrentY >= nLimit)
            {
                if (CheckPiece(pMatrix, nCurrentPiece, nCurrentRotation, nCurrentX + 2, nCurrentY) == 1)
                {
                    nCurrentX += 2;
                }
            }

            if (bKey[1] == 1 && nCurrentY >= nLimit && CheckPiece(pMatrix, nCurrentPiece, nCurrentRotation, nCurrentX - 2, nCurrentY) == 1)
            {
                nCurrentX -= 2;
            }

            if (bKey[2] == 1 && nCurrentY >= nLimit)
            {
                int i{};
                while (CheckPiece(pMatrix, nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + i) == 1)
                {
                    i++;
                }
                nCurrentY += i - 1;
            }

            if (bKey[0] == 1 && nCurrentY >= nLimit && bRotateHold == 1 && CheckPiece(pMatrix, nCurrentPiece, (nCurrentRotation + 1) % 4, nCurrentX, nCurrentY) == 1)
            {
                nCurrentRotation++;
                nCurrentRotation %= 4;
                bRotateHold = 0;
            }
            else
            {
                bRotateHold = 1;
            }
            // Pause
            if (bKey[4] == 1)
            {
                int nSelect = 0;
                WORD* pTmpColor = new WORD[nScreenWidth * nScreenHeight];

                for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
                {
                    pTmpColor[i] = pColor[i];
                }

                while (1)
                {
                    Text(pBuffer, pTmpColor, L" ═════ PAUSE ════ ", 10 * 16 + 11, 2, 8);
                    Text(pBuffer, pTmpColor, L"                  ", 10 * 16 + 11, 2, 9);
                    Text(pBuffer, pTmpColor, L" ════════════════ ", 10 * 16 + 11, 2, 12);


                    if (nSelect == 0)
                    {
                        Text(pBuffer, pTmpColor, L" >>  Continue  << ", 10 * 16 + 4, 2, 10);
                        Text(pBuffer, pTmpColor, L"       Quit       ", 10 * 16 + 11, 2, 11);

                        if (GetKeyState('S') & 0x8000)
                        {
                            nSelect++;
                        }
                        else if (GetKeyState(13) & 0x8000)
                        {
                            break;
                        }
                    }
                    else
                    {
                        Text(pBuffer, pTmpColor, L"     Continue     ", 10 * 16 + 11, 2, 10);
                        Text(pBuffer, pTmpColor, L" >>    Quit    << ", 10 * 16 + 6, 2, 11);

                        if (GetKeyState('W') & 0x8000)
                        {
                            nSelect--;
                        }
                        else if (GetKeyState(13) & 0x8000)
                        {
                            return 0;
                        }
                    }

                    for (int j = 0; j < nScreenHeight; j++)
                    {
                        for (int i = 0; i < nScreenWidth; i++)
                        {
                            COORD cPos;
                            cPos.X = i;
                            cPos.Y = j;
                            WriteConsoleOutputAttribute(hConsole, &pTmpColor[j * nScreenWidth + i], 1, cPos, &dwBytesWritten);
                        }
                    }
                    WriteConsoleOutputCharacter(hConsole, pBuffer, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
                }
            }
            // Move down
            if (bForceDown)
            {
                // Update difficulty
                nFrameCount = 0;
                if (nPieceCount == nLevelLimit && nFrame >= 5)
                {
                    nFrame--;
                    nPieceCount = 0;
                    nLevelLimit *= 2;
                }

                // Test if piece can be moved down
                if (CheckPiece(pMatrix, nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                {
                    nCurrentY++;
                }
                else
                {
                    // If the tetromino has not appeared completely, game over
                    if (nCurrentY < nLimit)
                    {
                        break;
                    }
                    else
                    {
                        nPieceCount++;

                        //Fix the tetromino
                        for (int i = 0; i < 8; i++)
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                if (nCurrentY >= 0 && tetromino.at(nCurrentPiece).at(nCurrentRotation).at(j * 8 + i) != L'.')
                                {
                                    pMatrix[(nCurrentY + j) * nBoardWidth + (nCurrentX + i)] = 1;
                                    if (((nCurrentX + i) / 2 + (nCurrentX + i) % 2 + (nCurrentY + j)) % 2 == 0)
                                        pColor[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = 8 * 16 + nCurrentPiece_color;
                                    else pColor[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = 7 * 16 + nCurrentPiece_color;
                                }
                            }
                        }

                        // Check for lines
                        for (int j = 0; j < 4; j++)
                        {
                            if (nCurrentY + j < nBoardHeight - 1)
                            {
                                bool bLine = 1;
                                for (int i = 1; i < nBoardWidth - 1; i++)
                                {
                                    if (pMatrix[(nCurrentY + j) * nBoardWidth + i] == 0)
                                    {
                                        bLine = 0;
                                        break;
                                    }
                                }

                                if (bLine == 1)
                                {
                                    nLine++;

                                    // A little animation
                                    for (int i = 1; i < nBoardWidth - 1; i++)
                                    {
                                        pMatrix[(nCurrentY + j) * nBoardWidth + i] = 3;
                                    }
                                    vLines.push_back(nCurrentY + j);
                                }
                            }
                        }

                        // Increase score
                        nScore += 25;
                        if (!vLines.empty())
                        {
                            nScore += (1 << vLines.size()) * 100;
                        }

                        // Pick new tetromino
                        nCurrentX = nBoardWidth / 2 - 4;
                        nCurrentY = -4;
                        nCurrentRotation = 0;
                        nCurrentPiece = nNextPiece;
                        nCurrentPiece_color = random(0, 6);
                        nNextPiece = random(0, 6);
                    }
                }
            }

            // DISPLAY

            // Frame
            for (int i = 0; i < nBoardWidth; i++)
            {
                for (int j = 0; j < nBoardHeight; j++)
                {
                    pBuffer[j * nScreenWidth + i] = detail[pMatrix[j * nBoardWidth + i]];
                }
            }


            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (tetromino.at(nCurrentPiece).at(nCurrentRotation).at(j * 8 + i) != L'.' && nCurrentY + j >= 0)
                    {
                        pColor[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = 8 * 16 + nCurrentPiece_color;
                        pBuffer[(nCurrentY + j) * nScreenWidth + (nCurrentX + i)] = L'▓';
                    }
                }
            }


            // Score
            if (nScore >= nScoreComp)
            {
                nScorePosX--;
                nScoreComp *= 10;
            }

            // Line
            if (nLine >= nLineComp)
            {
                nLinePosX--;
                nLineComp *= 10;
            }

            // Destroy the lines
            if (!vLines.empty())
            {
                WriteConsoleOutputCharacter(hConsole, pBuffer, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
                Sleep(400);

                for (int l = 0; l < vLines.size(); l++)
                {
                    for (int i = 1; i < nBoardWidth - 1; i++)
                    {
                        for (int j = vLines.at(l); j > 0; j--)
                        {
                            if (j % 2 == 0)
                            {
                                if (i % 4 == 1 || i % 4 == 2)
                                {
                                    pColor[j * nScreenWidth + i] = pColor[(j - 1) * nScreenWidth + i] - 16;
                                }
                                else
                                {
                                    pColor[j * nScreenWidth + i] = pColor[(j - 1) * nScreenWidth + i] + 16;
                                }
                            }
                            else
                            {
                                if (i % 4 == 1 || i % 4 == 2)
                                {
                                    pColor[j * nScreenWidth + i] = pColor[(j - 1) * nScreenWidth + i] + 16;
                                }
                                else
                                {
                                    pColor[j * nScreenWidth + i] = pColor[(j - 1) * nScreenWidth + i] - 16;
                                }
                            }
                            pMatrix[j * nBoardWidth + i] = pMatrix[(j - 1) * nBoardWidth + i];
                        }
                        pMatrix[i] = 0;
                    }
                }
                vLines.clear();
            }

            for (int j = 0; j < nScreenHeight; j++)
            {
                for (int i = 0; i < nScreenWidth; i++)
                {
                    COORD cPos;
                    cPos.X = i;
                    cPos.Y = j;
                    WriteConsoleOutputAttribute(hConsole, &pColor[j * nScreenWidth + i], 1, cPos, &dwBytesWritten);
                }
            }
            WriteConsoleOutputCharacter(hConsole, pBuffer, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
        }

        // Game over
        const vector<wstring> wsGameOver = {
            L"▄──┐┌──▄ ┌─▄─▄ ▄──",
            L"█ ─┐├──█ │ ▀ █ █─ ",
            L"▀──┘┴  ▀ ┴   ▀ ▀──",
            L"▄──┐ ▄  ┬ ▄── ▄──┐",
            L"█  │ █ ┌┘ █─  █─┬┘",
            L"▀──┘ ▀─┘  ▀── ▀ └─"
        };

        for (int i = 0; i < nScreenWidth; i++)
        {
            for (int j = 0; j < nScreenHeight; j++)
            {
                pBuffer[j * nScreenWidth + i] = L' ';
                pColor[j * nScreenWidth + i] = 10 * 16 + 11;
            }
        }

        int nSelect = 0;
        while (1)
        {
            for (int i = 0; i < wsGameOver.size(); i++)
            {
                Text(pBuffer, pColor, wsGameOver.at(i), 10 * 16 + 1, 10, 3 + i);
            }

            Text(pBuffer, pColor, L"════ SCORE ════", 10 * 16 + 3, 12, 11);
            Text(pBuffer, pColor, L"═══════════════", 10 * 16 + 3, 12, 13);

            int nScorePosX = 19;
            int nScoreComp = 100;
            while (nScore >= nScoreComp)
            {
                nScorePosX--;
                nScoreComp *= 100;
            }

            Text(pBuffer, pColor, to_wstring(nScore), 10 * 16 + 3, nScorePosX, 12);

            if (nSelect == 0)
            {
                Text(pBuffer, pColor, L">> Play Again <<", 10 * 16 + 4, 11, 17);
                Text(pBuffer, pColor, L"      Quit      ", 10 * 16 + 11, 11, 18);

                if (GetKeyState('S') & 0x8000)
                {
                    nSelect++;
                }
                if (GetKeyState(13) & 0x8000)
                {
                    break;
                }
            }
            else
            {
                Text(pBuffer, pColor, L"   Play Again   ", 10 * 16 + 11, 11, 17);
                Text(pBuffer, pColor, L">>    Quit    <<", 10 * 16 + 6, 11, 18);

                if (GetKeyState('W') & 0x8000)
                {
                    nSelect--;
                }
                if (GetKeyState(13) & 0x8000)
                {
                    return 0;
                }
            }

            for (int j = 0; j < nScreenHeight; j++)
            {
                for (int i = 0; i < nScreenWidth; i++)
                {
                    COORD cPos;
                    cPos.X = i;
                    cPos.Y = j;
                    WriteConsoleOutputAttribute(hConsole, &pColor[j * nScreenWidth + i], 1, cPos, &dwBytesWritten);
                }
            }
            WriteConsoleOutputCharacter(hConsole, pBuffer, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
        }
    }

    return 0;
}