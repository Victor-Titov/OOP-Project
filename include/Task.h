#pragma once

#include "defines.h"

class Task
{
    friend class Project;

public:
    Task();
    Task(const string& title, const string& description,
         const Priority& priority, Status status);
    explicit Task(istream& in);
    ~Task();

    bool start();
    bool finish();
    bool omit();
    bool restore();   // un-omit: Omitted -> Not started
    bool increasePriority();
    bool decreasePriority();

    int getId() const;
    const string& getTitle() const;
    const string& getDescription() const;
    Priority getPriority() const;
    Status getStatus() const;

    friend ostream& operator<<(ostream& out, const Task& task);

private:
    int id = 0;
    const string title;
    const string description;
    Priority priority;
    Status status;
};
