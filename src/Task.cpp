#include "Task.h"

Task::Task()
{
}

Task::Task(const string& title, const string& description,
           Priority priority, Status status)
    : title(title), description(description),
      priority(priority), status(status)
{
}

Task::~Task()
{
}

bool Task::start()
{
    if (status != Status::NotStarted)
    {
        return false;
    }

    status = Status::InProgress;
    return true;
}

bool Task::finish()
{
    if (status != Status::InProgress)
    {
        return false;
    }

    status = Status::Completed;
    return true;
}

const string& Task::getTitle() const
{
    return title;
}

const string& Task::getDescription() const
{
    return description;
}

Priority Task::getPriority() const
{
    return priority;
}

Status Task::getStatus() const
{
    return status;
}
