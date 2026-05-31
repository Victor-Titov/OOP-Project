#pragma once

#include <string>

using namespace std;

struct Date
{
    int day;
    int month;
    int year;
};

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
