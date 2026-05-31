#pragma once

#include "defines.h"

class Task
{
public:
    Task();
    Task(const string& title, const string& description,
         const Priority& priority, Status status);
    ~Task();

    bool start();
    bool finish();
    bool cancel();
    bool omit();
    bool increasePriority();
    bool decreasePriority();

    const string& getTitle() const;
    const string& getDescription() const;
    Priority getPriority() const;
    Status getStatus() const;

private:
    const string title;
    const string description;
    Priority priority;
    Status status;
};
