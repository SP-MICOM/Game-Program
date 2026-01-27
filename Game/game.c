#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define Z 122
#define X 120


int screenIndex;
HANDLE screen[2];

void Initialize()
{
	CONSOLE_CURSOR_INFO cursor;

	// 화면 버퍼를 2개 생성합니다.
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

struct Cursor
{
	int x;
	int y;
};

struct obj_1
{
	int x;
	int y;
	int HP;
	int ATK;
	int ACC;
	int MOV;
};

int main()
{
	struct Cursor cursor = { 0, 0 };
	struct obj_1 warrier = { 6, 6, 20, 5, 90, 4 };
	
	char key = 0;

	CONSOLE_SCREEN_BUFFER_INFO console;

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hStdout, &console);

	int width = console.srWindow.Right - console.srWindow.Left - 2;
	int height = console.srWindow.Bottom - console.srWindow.Top;

	Initialize();
	
	Render(cursor.x, cursor.y, "▼");
	Render(warrier.x, warrier.y, "○");

	int select = 0; // 커서가 오브젝트 위에 있는지 여부
	int mover = 0;	// 커서가 오브젝트를 선택했는지 여부

	while (1)
	{
		// 커서 이동
		Flip();
		Clear();
		key = _getch();

		if (key == -32 || key == 0)
		{
			key = _getch();
		}
	
		switch (key)
		{
		case UP: if (cursor.y > 0) { cursor.y--; }
			   break;

		case LEFT: if (cursor.x > 0) { cursor.x -= 2; }
				 break;

		case RIGHT: if (width > cursor.x) { cursor.x += 2; }
				  break;

		case DOWN: if (height > cursor.y) { cursor.y++; }
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

		Render(cursor.x, cursor.y, "▼");

		if ((cursor.x == warrier.x) && (cursor.y == warrier.y))
		{
			if (mover == 1)
			{
				Render(warrier.x, warrier.y, "◎");

				for (int i = warrier.MOV; i > 0; i--)
				{
					Render(warrier.x + i * 2, warrier.y, "□");
					Render(warrier.x - i * 2, warrier.y, "□");
					Render(warrier.x, warrier.y - i, "□");
					Render(warrier.x, warrier.y + i, "□");
				}
			}
			else {Render(warrier.x, warrier.y, "●");}
			select = 1;
		}
		else
		{
			Render(warrier.x, warrier.y, "○");
			select = 0;
		}
	}

	return 0;
}