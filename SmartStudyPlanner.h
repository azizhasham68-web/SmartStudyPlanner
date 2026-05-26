#ifndef SMART_STUDY_PLANNER_H                               //include guard to prevent multiple inclusions of the header file
#define SMART_STUDY_PLANNER_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;
                                                                //declarations of all the classes used in the project 
class Subject;
class StudySession;
class Scheduler;
class Analytics;
class Task;
class Date;
class FileManager;
class User;
class Person;
                                                                                //declarations of all the utility functions used in the project
void printBanner();
void userMenu(User& user);
void cpk_line(char fill = '-', int width = 45);
int  getInt(const string& prompt);

#endif                  //SMART_STUDY_PLANNER_H
