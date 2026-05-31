#pragma once

#include "defines.h"
#include "Task.h"

#include <vector>

class Project
{
public:
    Project();
    Project(const string& name, const string& description,
            const Date& deadline, const Priority& priority);
    ~Project();

    void addTask(const Task& task);
    bool cancel();
    bool increasePriority();
    bool decreasePriority();

    const string& getName() const;
    const string& getDescription() const;
    const Date& getDeadline() const;
    Priority getPriority() const;
    Status getStatus() const;
    const vector<Task>& getTasks() const;

private:
    const string name;
    const string description;
    const Date deadline;
    Priority priority;
    vector<Task> tasks;
};
