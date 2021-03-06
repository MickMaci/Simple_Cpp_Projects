// Simple tetris game written in c++

#include "stdafx.h"

using namespace std;

// create an assets for represent tetris bblocks
wstring tetromino[7];	

// Playing field 
int nFieldWidth = 12;
int nFieldHeight = 18;
// console size
int nScreenWidth = 80;
int nScreenHeight = 30;
// // // // // 

unsigned char *pField = nullptr;

// Rotation function for current playing field 
int Rotate (int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px;				// 0 degrees
	case 1: return 12 + py - (px * 4);		// 90 degrees
	case 2: return 15 - (py * 4) - px;		// 180 degrees
	case 3: return 3 - py + (px * 4);		// 270 degrees
	}
	return 0;
}

//				 type of block	  type of rotation	top lef x	top lef y fragment of the tetris block
bool DoesPiecesFit(int nTetromino, int nRotation,	int  nPosX, int nPosY)
{
	for(int px = 0; px <4;px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece      //Get index from current tetris Block
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					// Colision detected
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false; // fail on first hit
				}
			}
		}

	return true;
}

int main()
{

#pragma region Tetris BLocks definition
	// create a tetris blocks as a one demention array 
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L"..XX");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");
#pragma endregion

#pragma region Initialized playingField 
	// alocate necessary memory for playing field 
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)					// cloumns 
		for (int y = 0; y < nFieldHeight; y++)				// rows
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
#pragma endregion

#pragma region Screen Initialization
	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenHeight * nScreenWidth; i++) screen[i] = L' ';	// L is a prefix for wchar_t type
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	

#pragma endregion

#pragma region Game Loop
	
	bool bGameOver = false;
	int nCurrentPiece = 2;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;
	bool bKey[4];
	bool bRotateHold = false;
	// we want to force down a piec of tetris block's for every one second 
	// to do that we 
	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	vector<int> vLines;
	int nPieceCount = 0;
	int nScore = 0;

	while (!bGameOver)
	{
		// GAME TIMING
		// one game tick
		this_thread::sleep_for(50ms);
		// increment nSpeedCounter 
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// INPUT
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		// GAME LOGIC
		// If bKey was lef
		nCurrentX -= (bKey[1] && DoesPiecesFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;

		// If bKey was right
		nCurrentX += (bKey[0] && DoesPiecesFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		// If bKey was down
		nCurrentY += (bKey[2] && DoesPiecesFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
			
		// If bKey was z (rotation)
		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPiecesFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;

		// Force down piece
		if (bForceDown)
		{	
			if (DoesPiecesFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{

				// Lock the current piece in the field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							//																	 set the ASCII code eg for 0 it'l be 'A'
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
				// increase a game speed
				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--; 

				// Hawe we got any lines				
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) 
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= ((pField[(nCurrentY + py) * nFieldWidth + px]) != 0);
						if (bLine)
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(py + nCurrentY)*nFieldWidth + px] = 8;
							// If line exist push back the current row to vLines 
							vLines.push_back(nCurrentY + py);
						}						
					}
				// Set Score
				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;
				
				// Choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;					
				// Game Over
				bGameOver = !DoesPiecesFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1);
			}
			nSpeedCounter = 0;
		}

		// RENDER OUTPUT

		// Draw Playground
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					//																	 set the ASCII code eg for 0 it'l be 'A'
					screen[(nCurrentY + py + 2) * nScreenWidth + ( nCurrentX + px + 2)] = nCurrentPiece + 65;
		
		// Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
		
		// Delete the lines
		if (!vLines.empty())
		{
			// Display frame 
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms);
			for(auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1 ; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}
		
		// display Frame
		//							Console handler| canvas|  size of canvas (n charactes)|  where to start| how much was written
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	// Final Score on Screen
	CloseHandle(hConsole);
	cout << "Your score: " << nScore << " Well done.";
	system("pouse");

#pragma endregion
    return 0;
}

