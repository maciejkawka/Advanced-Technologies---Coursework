#pragma once
#include "../../Common/Window.h"
#include"MainGameState.h"

using namespace NCL;
using namespace CSC8503;

int main() {
	Window*w = Window::CreateGameWindow("Coursework!", 1280, 720);
	MainGameState mainGame(w);

	if (!w->HasInitialised()) 
		return -1;
	
	srand(time(0));

	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

	while (w->UpdateWindow()) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		if (!mainGame.Update(dt))
			break;
	}
	Window::DestroyGameWindow();
}