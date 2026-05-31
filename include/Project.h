#pragma once

#include "defines.h"
#include "Task.h"

#include <list>

class Project
{
public:
    Project();
    Project(const string& name, const string& description,
            const Date& deadline, const Priority& priority);
    explicit Project(istream& in);
    ~Project();

    void addTask(const Task& task);
    bool deleteTask(int id);
    bool cancel();
    bool increasePriority();
    bool decreasePriority();

    bool startTask(int id);
    bool finishTask(int id);
    bool cancelTask(int id);
    bool omitTask(int id);
    bool increaseTaskPriority(int id);
    bool decreaseTaskPriority(int id);

    // Writes the project into data/<fileName>, creating the data folder if needed.
    bool saveToFile(const string& fileName) const;

    const string& getName() const;
    const string& getDescription() const;
    const Date& getDeadline() const;
    Priority getPriority() const;
    Status getStatus() const;
    const list<Task>& getTasks() const;

    friend ostream& operator<<(ostream& out, const Project& project);

private:
    Task* findTask(int id);

    const string name;
    const string description;
    const Date deadline;
    Priority priority;
    list<Task> tasks;
    int nextId = 1;
};
