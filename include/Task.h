#pragma once

#include "defines.h"

class Task
{
public:
    Task();
    Task(const string& title, const string& description,
         Priority priority, Status status);
    ~Task();

    bool start();
    bool finish();
    bool cancel();
    bool omit();

    const string& getTitle() const;
    const string& getDescription() const;
    Priority getPriority() const;
    Status getStatus() const;

private:
    string title;
    string description;
    Priority priority;
    Status status;
};
