/* 
Copyright Schulich Racing, FSAE
Written by Justin Tijunelis
*/ 

#include <iostream>
#include <ostream>
#include <telemetry/thing/telemetry_thing.h>
#include <vector>
#include <limits>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void print_instructions() {
  std::cout << "Enter 'thing' to a create a new object that will stream telemetry." << std::endl;
  std::cout << "Enter 'view' to see which 'things' currently exist." << std::endl;
  std::cout << "Enter 'start' to begin telemetry for your 'things'." << std::endl;
  std::cout << "Enter 'q' to quit the program at any time." << std::endl;
}

int main() {
  std::cout << "Welcome to the Schulich Racing Telemetry Simulator!" << std::endl;
  std::cout << "Here are a couple of things you need to know:" << std::endl;
  std::cout << "- This program is in alpha! It has not been thoroughly tested. The program is using various 'hacks'" << std::endl;
  std::cout << "that provide a presentable output that will help you understand what this program does." << std::endl;
  std::cout << "- A 'thing' is an object that we collect data for and can attach our hardware to for telemetry." << std::endl;
  std::cout << "- A 'thing' has arbitrary sensors and an arbitrary number of them. As is stands, this program uses." << std::endl;
  std::cout << "a predefined set of sensors for every 'thing' for testing purposes." << std::endl;
  std::cout << "- In the future, this program will have a GUI made with Qt, and will connect to the SR-Velocty" << std::endl;
  std::cout << "database. This program is a proof of concept but is under active development." << std::endl;

  std::vector<std::unique_ptr<TelemetryThing>> things;
  bool quit = false;
  while (!quit) {
    print_instructions();
    std::string line;
    std::getline(std::cin, line);
    if (line == "thing")
    {
      std::cout << "What is the name of your thing? Enter 'e' to exit, or 'q' to quit." << std::endl;
      std::string thing_name;
      while (1) {
        std::getline(std::cin, thing_name);
        if (thing_name != "q" && thing_name != "e") {
          // Check if already exists
          bool exists = false;
          for (auto& thing: things) {
            if (thing->_name == thing_name) {
              exists = true;
            }
          }
          if (exists) {
            std::cout << "There is already a thing with this name! Please try again." << std::endl;
          } else {
            std::cout << "Great! What is the serial number of " << thing_name << "? Enter 'e' to exit, or 'q' to quit." << std::endl;
            std::string thing_sin;
            while (1) {
              std::getline(std::cin, thing_sin);
              if (thing_sin != "q" && thing_sin != "e") {
                // Check if already exists
                bool exists = false;
                for (auto& thing: things) {
                  if (thing->_serial_number == thing_sin) {
                    exists = true;
                  }
                }
                if (exists) {
                  std::cout << "A thing with this serial number already exists! Please try again." << std::endl;
                } else {
                  things.push_back(std::make_unique<TelemetryThing>(thing_name, thing_sin));
                  std::cout << "Fantastic! You created " << thing_name << " with serial number " << thing_sin << "." << std::endl;
                  break;
                }
              } else {
                if (thing_sin == "q") quit = true;
                break;
              }
            }
            break;
          }
        } else {
          if (thing_name == "q") quit = true;
          break;
        }
      }
    }
    else if (line == "view")
    {
      if (things.size() == 0) {
        std::cout << "You don't have any things yet!" << std::endl;
      } else {
        std::cout << std::endl << "Here are your things:" << std::endl;
        std::cout << "----------------------" << std::endl;
        for (auto it = things.begin(); it != things.end(); it++) {
          std::cout << "Name: " << (*it)->_name << "; " << "Serial Number: " << (*it)->_serial_number << std::endl;
        }
        std::cout << "----------------------" << std::endl;
        // For some reason 
        // std::cout << "If you want to delete a thing, enter it's name. Enter 'e' to exit this view, or 'q' to quit the program." << std::endl;
        // while (1) {
        //   std::string input;
        //   std::getline(std::cin, input);
        //   if (input != "q" && input != "e") {
        //     int index = -1;
        //     for (int i = 0; i < things.size(); i++) {
        //       if (things[i]->_name == input) {
        //         index = i;
        //         break;
        //       }
        //     }
        //     if (index != -1) {
        //       things.erase(things.begin() + index);
        //       std::cout << "Thing " << input << " was removed!" << std::endl;
        //       break;
        //     } else {
        //       std::cout << "Oops, the program couldn't find your thing. Please try again. Enter 'e' to exit this view, or 'q' to quit the program." << std::endl;
        //     }
        //   } else {
        //     if (input == "q") quit = true;
        //     break;
        //   }
        // }
      }
    }
    else if (line == "start")
    {
      if (things.size() == 0) {
        std::cout << "You haven't made any things yet!" << std::endl;
      } else {
        std::cout << "Starting the telemetry session! Note that the console will get overflown." << std::endl;
        std::cout << "Note that multiple thing logs will collide as they run async." << std::endl;
        std::cout << "Enter 'stop' at any time to stop the stream." << std::endl;
        sleep(3);
        for (auto& thing: things) {
          thing->start_telemetry();
        }
        std::string input;
        while (1) {
          std::getline(std::cin, input);
          if (input == "stop") {
            for (auto& thing: things) {
              thing->stop_telemetry();
            }
            break;
          }
        }
      }
    }
    else if (line == "q")
    {
      std::cout << "Thanks for stopping by!" << std::endl;
      quit = true;
    }
    else
    {
      std::cout << "Invalid input, please try again." << std::endl;
    }
  }

  return 0;
}