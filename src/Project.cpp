#include "Project.h"

Project::Project()
    : deadline{}, priority(Priority::Low)
{
}

Project::Project(const string& name, const string& description,
                 const Date& deadline, const Priority& priority)
    : name(name), description(description), deadline(deadline),
      priority(priority)
{
}

Project::~Project()
{
}

void Project::addTask(const Task& task)
{
    tasks.push_back(task);
    tasks.back().id = nextId++;
}

bool Project::deleteTask(int id)
{
    for (auto it = tasks.begin(); it != tasks.end(); ++it)
    {
        if (it->getId() == id)
        {
            tasks.erase(it);
            return true;
        }
    }

    return false;
}

Task* Project::findTask(int id)
{
    for (Task& task : tasks)
    {
        if (task.getId() == id)
        {
            return &task;
        }
    }

    return nullptr;
}

bool Project::startTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->start();
}

bool Project::finishTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->finish();
}

bool Project::cancelTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->cancel();
}

bool Project::omitTask(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->omit();
}

bool Project::increaseTaskPriority(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->increasePriority();
}

bool Project::decreaseTaskPriority(int id)
{
    Task* task = findTask(id);
    return task != nullptr && task->decreasePriority();
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

bool Project::increasePriority()
{
    if (priority == Priority::High)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) + 1);
    return true;
}

bool Project::decreasePriority()
{
    if (priority == Priority::Low)
    {
        return false;
    }

    priority = static_cast<Priority>(static_cast<int>(priority) - 1);
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

const Date& Project::getDeadline() const
{
    return deadline;
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
    bool allOmitted = true;
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
        if (task.getStatus() != Status::Omitted)
        {
            allOmitted = false;
        }
        if (task.getStatus() == Status::Canceled)
        {
            hasCanceled = true;
        }
    }

    if (allOmitted)
    {
        return Status::Canceled;
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

const list<Task>& Project::getTasks() const
{
    return tasks;
}
