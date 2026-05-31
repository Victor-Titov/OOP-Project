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

bool Project::cancel()
{
    if (getStatus() == Status::Completed)
    {
        return false;
    }

    for (Task& task : tasks)
    {
        task.cancel();
    }

    return true;
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

    bool allDone = true;
    bool allNotStarted = true;
    bool hasCanceled = false;

    for (const Task& task : tasks)
    {
        if (task.getStatus() != Status::Completed &&
            task.getStatus() != Status::Omitted)
        {
            allDone = false;
        }
        if (task.getStatus() != Status::NotStarted)
        {
            allNotStarted = false;
        }
        if (task.getStatus() == Status::Canceled)
        {
            hasCanceled = true;
        }
    }

    if (allDone)
    {
        return Status::Completed;
    }
    if (hasCanceled)
    {
        return Status::Canceled;
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
