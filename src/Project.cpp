#include "Project.h"

Project::Project()
{
}

Project::Project(const string& name, const string& description,
                 Priority priority)
    : name(name), description(description),
      priority(priority)
{
}

Project::~Project()
{
}

void Project::addTask(const Task& task)
{
    tasks.push_back(task);
}

const string& Project::getName() const
{
    return name;
}

const string& Project::getDescription() const
{
    return description;
}

Priority Project::getPriority() const
{
    return priority;
}

Status Project::getStatus() const
{
    if (tasks.empty())
    {
        return Status::NotStarted;
    }

    bool allCompleted = true;
    bool allNotStarted = true;

    for (const Task& task : tasks)
    {
        if (task.getStatus() != Status::Completed)
        {
            allCompleted = false;
        }
        if (task.getStatus() != Status::NotStarted)
        {
            allNotStarted = false;
        }
    }

    if (allCompleted)
    {
        return Status::Completed;
    }
    if (allNotStarted)
    {
        return Status::NotStarted;
    }

    return Status::InProgress;
}

const vector<Task>& Project::getTasks() const
{
    return tasks;
}
