#pragma once

#include "defines.h"

class Task
{
public:
    Task();
    Task(const string& title, const string& description, const Date& deadline,
         Priority priority, Status status);
    ~Task();

    bool start();
    bool finish();

    const string& getTitle() const;
    const string& getDescription() const;
    const Date& getDeadline() const;
    Priority getPriority() const;
    Status getStatus() const;

private:
    string title;
    string description;
    Date deadline;
    Priority priority;
    Status status;
};
