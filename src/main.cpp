#include "Window.hpp"


int main(void) {
    Window window(Window::Settings("CollageMaker2"));

    window.loop();

    return 0;
}
