#pragma once

#include "defines.h"
#include "Task.h"

#include <vector>

class Project
{
public:
    Project();
    Project(const string& name, const string& description,
            Priority priority);
    ~Project();

    void addTask(const Task& task);

    const string& getName() const;
    const string& getDescription() const;
    Priority getPriority() const;
    Status getStatus() const;
    const vector<Task>& getTasks() const;

private:
    string name;
    string description;
    Priority priority;
    vector<Task> tasks;
};
