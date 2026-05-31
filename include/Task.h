#pragma once

#include "defines.h"

class Task
{
    friend class Project;

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

    int getId() const;
    const string& getTitle() const;
    const string& getDescription() const;
    Priority getPriority() const;
    Status getStatus() const;

private:
    int id = 0;
    const string title;
    const string description;
    Priority priority;
    Status status;
};
