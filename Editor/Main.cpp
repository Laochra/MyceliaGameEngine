#include "Editor.h"

#include "EditorDebug.h"

#ifdef _DEBUG
int main(int argCount, char* args[])
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, int nCmdShow)
#else
#error No release platform other than windows has been accounted for
#endif
{
	debug = new EditorDebug();

	Application* application = new Editor();
	int exitCode = application->Run();

	delete debug;
	return exitCode;
}
