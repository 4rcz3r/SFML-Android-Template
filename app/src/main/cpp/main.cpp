#include "SFML/Graphics.hpp"
#include <iostream>

#include <android/log.h>
#include <pthread.h>
#include <unistd.h>

static int pfd[2];
static pthread_t loggingThread;
static const char *LOG_TAG = "EXAMPLE_DEBUG";

static void *loggingFunction(void *)
{
    ssize_t readSize;
    char buf[128];
    while ((readSize = read(pfd[0], buf, sizeof buf - 1)) > 0)
    {
        if (buf[readSize - 1] == '\n')
            --readSize;
        buf[readSize] = 0;  // add null-terminator
        __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf); // Set any log level you want
    }
    return nullptr;
}

static int runLoggingThread()
{
    setvbuf(stdout, nullptr, _IOLBF, 0); // make stdout line-buffered
    setvbuf(stderr, nullptr, _IONBF, 0); // make stderr unbuffered

    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    pthread_create(&loggingThread, nullptr, loggingFunction, nullptr);
    pthread_detach(loggingThread);
    return 0;
}

int main()
{
    runLoggingThread();

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Example", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::Event event{};
    bool active = true;

    sf::Texture tex;
    tex.loadFromFile("sfml.png");
    sf::Sprite sprite(tex);

    while (window.isOpen())
    {
        while (window.pollEvent(event))
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    //std::cout << "Closing" << std::endl;
                    break;
                case sf::Event::KeyReleased:
                    //std::cout << "Key event" << std::endl;
                    if (event.key.code == sf::Keyboard::Escape) // back key in android
                        window.close();
                    break;
                case sf::Event::MouseEntered:
                case sf::Event::GainedFocus:
                    active = true;
                    window.setActive(true);
                    //std::cout << "Gained focus" << std::endl;
                    break;
                case sf::Event::MouseLeft:
                case sf::Event::LostFocus:
                    active = false;
                    window.setActive(false);
                    //std::cout << "Lost focus" << std::endl;
                    break;
                case sf::Event::Resized:
                    window.setView(sf::View(sf::FloatRect(sf::Vector2f(), sf::Vector2f(
                            static_cast<float>(event.size.width),
                            static_cast<float>(event.size.height)))));
                    //std::cout << "Resize event" << std::endl;
                    break;
                case sf::Event::TouchBegan:
                case sf::Event::TouchMoved:
                case sf::Event::TouchEnded:
                    //std::cout << "Touch event" << std::endl;
                    break;
                default:
                    //std::cout << "Event: " << event.type << std::endl;
                    break;
            }
        if (active)
        {
            window.clear(sf::Color::White);
            window.draw(sprite);
            window.display();
        }
        else
            sf::sleep(sf::milliseconds(100));
    }

    return 0;
}
