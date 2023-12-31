#include <iostream>

#include "Editor.h" // Replace with chosen application

int main(char** args)
{
	Application* application = new Editor();

	return application->Run(); // Run returns when the application has closed
}