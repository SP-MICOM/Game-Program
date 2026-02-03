#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define Z 122
#define X 120

int screenIndex;
HANDLE screen[2];

struct MAP
{
	int x;
	int y;
	int width;
	int height;
};

struct SYSTEM
{
	int x;
	int y;
	int width;
	int height;
};

struct Cursor
{
	int x;
	int y;
};

struct Warrier
{
	int x;
	int y;
	int HP;
	int ATK;
	int ACC;
	int MOV;
};

struct Enemy1
{
	int x;
	int y;
	int HP;
	int ATK;
	int ACC;
	int MOV;
};


void Initialize()
{
	CONSOLE_CURSOR_INFO cursor;

	screen[0] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	screen[1] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	cursor.bVisible = FALSE;

	SetConsoleCursorInfo(screen[0], &cursor);
	SetConsoleCursorInfo(screen[1], &cursor);
}

void Flip()
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);

	screenIndex = !screenIndex;
}

void Clear()
{
	COORD position = { 0, 0 };

	DWORD dword;

	CONSOLE_SCREEN_BUFFER_INFO buffer;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(console, &buffer);

	int width = buffer.srWindow.Right - buffer.srWindow.Left + 1;
	int height = buffer.srWindow.Bottom - buffer.srWindow.Top + 1;

	FillConsoleOutputCharacter
	(
		screen[screenIndex], ' ', width * height, position, &dword
	);

}

void Release()
{
	CloseHandle(screen[0]);
	CloseHandle(screen[1]);
}

void Render(int x, int y, const char* character)
{
	DWORD dword;
	COORD position = { x, y };

	SetConsoleCursorPosition(screen[screenIndex], position);
	WriteFile(screen[screenIndex], character, strlen(character), &dword, NULL);
}

void RenderMap(int x, int y, int width, int height)
{
	for (int ix = 0; ix <= width; ix += 2)
	{
		Render(x + ix, y, "¡Ø");
		Render(x + ix, y + height, "¡Ø");
	}

	for (int iy = 0; iy <= height; iy++)
	{
		Render(x, iy + y, "¡Ø");
		Render(x + width, iy + y, "¡Ø");
	}
}

void RenderSYSM(int x, int y, int width, int height)
{
	for (int ix = 0; ix <= width; ix++)
	{
		Render(x + ix, y, "¡Ø");
		Render(x + ix, y + height, "¡Ø");

	}

	for (int iy = 0; iy <= height; iy++)
	{
		Render(x, y + iy, "¡Ø");
		Render(x + width, y + iy, "¡Ø");
	}
}

int Where(int x, int y, int ex, int ey, struct MAP map)
{
	if (x == ex && y == ey) return 0;
	if (x <= map.x || x >= map.width) return 0;
	if (y <= map.y || y >= map.height) return 0;
	return 1;
}

int main()
{
	struct Cursor cursor = { 4, 2 };
	struct Warrier warrier = { 10, 6, 20, 5, 90, 3 };
	struct Enemy1 enemy = { 28, 10, 20, 5, 90, 3 };
	struct MAP map = { 2, 1 , 60, 20 };
	// struct SYSTEM sysm = { map.width + 6, map.y, 6, map.height };

	char key = 0;

	CONSOLE_SCREEN_BUFFER_INFO console;

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hStdout, &console);

	int width = console.srWindow.Right - console.srWindow.Left - 2;
	int height = console.srWindow.Bottom - console.srWindow.Top;

	Initialize();

	Render(cursor.x, cursor.y, "¡å");
	Render(warrier.x, warrier.y, "¡Û");
	Render(enemy.x, enemy.y, "¡Ù");

	int select = 0;
	int mover = 0;
	int turn = 0;

	while (1)
	{
		RenderMap(map.x, map.y, map.width, map.height);
		// RenderSYSM(sysm.x, sysm.y, sysm.width, sysm.height);

		Flip();
		Clear();
		key = _getch();

		if (key == -32 || key == 0)
		{
			key = _getch();
		}

		if (mover == 0)
		{
			switch (key)
			{
			case UP: if (cursor.y > map.y + 1) { cursor.y--; }
				   break;

			case LEFT: if (cursor.x > map.x + 2) { cursor.x -= 2; }
					 break;

			case RIGHT: if (cursor.x < map.width) { cursor.x += 2; }
					  break;

			case DOWN: if (cursor.y < map.height) { cursor.y++; }
					 break;

			case Z:
				if ((select == 1) && (mover == 0)) { mover = 1; }
				else if ((select == 1) && (mover == 1))
				{
					select = 0;
					mover = 0;
				}
				break;

			case X: if (mover == 1) { mover = 0; }
				  break;

			default: printf("exception\n");
				break;
			}
		}
		else if (mover == 1)
		{
			int nextX = cursor.x;
			int nextY = cursor.y;

			switch (key)
			{
			case UP: if (cursor.y > map.y + 1) { nextY--; }
				   break;

			case DOWN: if (cursor.y < map.height) { nextY++; }
					 break;

			case LEFT: if (cursor.x > map.x + 2) { nextX -= 2; }
					 break;

			case RIGHT: if (cursor.x < map.width) { nextX += 2; }
					  break;

			case Z:
				mover = 0;
				warrier.x = cursor.x;
				warrier.y = cursor.y;
				turn = 1;
				break;

			case X:
				mover = 0;
				break;
			}

			int dx = abs((nextX - warrier.x) / 2);
			int dy = abs(nextY - warrier.y);

			if ((dx + dy <= warrier.MOV) && (nextX != enemy.x || nextY != enemy.y))
			{
				cursor.x = nextX;
				cursor.y = nextY;
			}
		}

		Render(cursor.x, cursor.y, "¡å");

		if (((cursor.x == warrier.x) && (cursor.y == warrier.y)) && (mover == 0))
		{
			Render(warrier.x, warrier.y, "¡Ü");
			select = 1;
		}
		else if (mover == 1)
		{
			if ((cursor.x == warrier.x) && (cursor.y == warrier.y)) { Render(warrier.x, warrier.y, "¡Ü"); }
			else { Render(warrier.x, warrier.y, "¡Ý"); }

			for (int iy = warrier.MOV; iy >= -warrier.MOV; iy--)
			{
				int remain = warrier.MOV - abs(iy);

				for (int ix = -remain; ix <= remain; ix++)
				{
					if (!((ix == 0) && (iy == 0)))
					{
						if ((warrier.x + ix * 2 > 0) && (warrier.y + iy > 0))
						{
							if ((warrier.x + ix * 2 == cursor.x) && (warrier.y + iy == cursor.y)) { Render(warrier.x + ix * 2, warrier.y + iy, "¡á"); }
							else { Render(warrier.x + ix * 2, warrier.y + iy, "¡à"); }
						}
					}
				}
			}
		}
		else
		{
			Render(warrier.x, warrier.y, "¡Û");
			select = 0;
		}

		if (((cursor.x == enemy.x) && (cursor.y == enemy.y)))
		{
			Render(enemy.x, enemy.y, "¡Ú");
		}
		else
		{
			Render(enemy.x, enemy.y, "¡Ù");
		}
		if (turn == 1)
		{
			int nextX = 0;
			int nextY = 0;

			while (1)
			{
				nextX = rand() % ((enemy.MOV * 2 + 1));
				nextY = rand() % ((enemy.MOV * 2 + 1));

				if (nextX >= enemy.MOV)
				{
					nextX -= enemy.MOV * 2;
				}

				if (nextY >= enemy.MOV)
				{
					nextY -= enemy.MOV * 2;
				}

				if (Where(enemy.x + nextX * 2, enemy.y + nextY, warrier.x, warrier.y, map) == 1)
				{
					enemy.x += nextX * 2;
					enemy.y += nextY;
					turn = 0;
					break;
				}
			}
		}
	}

	return 0;
}