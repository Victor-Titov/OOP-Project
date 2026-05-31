#pragma once

#include <string>
#include <iostream>
#include<fstream>
#include <limits>

using namespace std;

struct Date
{
    int day;
    int month;
    int year;
};

// Reads a single int and discards the rest of its line, so a following
// getline() starts cleanly on the next line.
inline int readInt(istream& in)
{
    int value = 0;
    in >> value;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

inline string readLine(istream& in)
{
    string line;
    getline(in, line);
    return line;
}

enum class Priority
{
    Low,
    Medium,
    High
};

enum class Status
{
    NotStarted,
    InProgress,
    Completed,
    Canceled,
    Omitted
};
