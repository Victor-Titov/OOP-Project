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

// Written as the first line of a saved project so the loader knows which
// concrete class to build.
enum class ProjectType
{
    Base = 0,
    School = 1
};

inline string priorityToString(Priority priority)
{
    switch (priority)
    {
    case Priority::Low:    return "Low";
    case Priority::Medium: return "Medium";
    case Priority::High:   return "High";
    }

    return "Unknown";
}

inline string statusToString(Status status)
{
    switch (status)
    {
    case Status::NotStarted: return "Not started";
    case Status::InProgress: return "In progress";
    case Status::Completed:  return "Completed";
    case Status::Canceled:   return "Canceled";
    case Status::Omitted:    return "Omitted";
    }

    return "Unknown";
}
