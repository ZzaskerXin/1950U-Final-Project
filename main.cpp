#include "Engine/window.h"

int main(int argc, char *argv[])
{
    std::cout<<"Hello World"<<std::endl; 

    std::unique_ptr<Window> m_window = std::make_unique<Window>();

    std::cout<<"Start"<<std::endl;
    if(m_window->start() != 0){
        return -1;
    }

    std::cout<<"Loop"<<std::endl;
    m_window->loop();
}